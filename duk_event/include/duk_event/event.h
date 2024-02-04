//
// Created by Ricardo on 15/04/2023.
//

#ifndef DUK_EVENT_EVENT_H
#define DUK_EVENT_EVENT_H

#include <functional>
#include <cassert>
#include <vector>
#include <memory>

namespace duk::event {

class Event {
private:
    friend class Listener;
    class Handle {
    public:
        explicit Handle(Event& owner);

        void unsubscribe(size_t id);

        Handle& operator=(const Handle& other);

        bool operator==(Handle& other) const noexcept;

    private:
        Event& m_owner;

        //used to check if the event is still alive when unsubscribing
        std::weak_ptr<void> m_controlBlock;
    };

public:

    Event();

    virtual ~Event();

    virtual void remove_listener(size_t listenerId) = 0;

private:
    std::shared_ptr<void> m_controlBlock;
};

template<typename ...Args>
class EventT : public Event {
public:

    using Callback = std::function<void(Args...)>;

    inline void operator()(const Args&... args) {
        emit(args...);
    }

    void emit(const Args&... args) {
        assert(!m_emitting && "Attempted to emit a event that was already being emitted.");
        m_emitting = true;
        for (auto& callback: m_callbacks) {
            callback(args...);
        }
        m_emitting = false;
    }

    void add_listener(Callback&& callback, size_t listenerId) {
        assert(!m_emitting && "Attempted to add_listener on a event that is currently emitting");
        assert(std::find_if(m_listeners.begin(), m_listeners.end(), [listenerId](size_t listener) {
            return listener == listenerId;
        }) == m_listeners.end() && "Tried to add_listener to an event with a listener that was already subscribed.");

        m_listeners.emplace_back(listenerId);
        m_callbacks.emplace_back(std::move(callback));
    }

    void remove_listener(size_t listenerId) override {
        assert(!m_emitting && "Attempted to remove_listener from an event that is currently emitting");
        auto it = std::find_if(m_listeners.begin(), m_listeners.end(), [listenerId](size_t listener) {
            return listener == listenerId;
        });

        assert(it != m_listeners.end() && "Attempted to remove_listener from an event with a listener that is not subscribed.");

        auto callbackIt = std::next(m_callbacks.begin(), std::distance(m_listeners.begin(), it));
        m_listeners.erase(it);
        m_callbacks.erase(callbackIt);
    }

protected:
    std::vector<Callback> m_callbacks;
    std::vector<size_t> m_listeners;
    bool m_emitting = false;
};

using EventVoid = EventT<>;

class Listener {
public:

    Listener();

    ~Listener();

    template<typename TEvent>
    void listen(TEvent& event, typename TEvent::Callback&& callback) {
        event.add_listener(std::move(callback), m_id);
        m_events.emplace_back(event);
    }

    template<typename TEvent>
    void ignore(TEvent& event) {
        Event::Handle handle(event);
        handle.unsubscribe(m_id);
        auto it = std::find(m_events.begin(), m_events.end(), handle);
        m_events.erase(it);
    }

private:

    size_t m_id;
    std::vector<Event::Handle> m_events;
};


class Dispatcher {
public:

    template<typename T>
    void emit(const T& ev) {
        const auto idx = index<T>();
        if (m_events.size() <= idx) {
            // no one is listening to this event, ignore it
            return;
        }

        const auto event = dynamic_cast<EventT<T>*>(m_events[idx].get());
        event->emit(ev);
    }

    template<typename T>
    void add_listener(Listener& listener, typename EventT<T>::Callback&& callback) {
        const auto idx = index<T>();
        if (m_events.size() <= idx) {
            m_events.resize(idx + 1);
            m_events[idx] = std::make_unique<EventT<T>>();
        }

        const auto event = dynamic_cast<EventT<T>*>(m_events[idx].get());
        listener.listen(*event, std::move(callback));
    }

    template<typename T>
    void remove_listener(Listener& listener) {
        const auto idx = index<T>();
        assert(m_events.size() > idx); //it would mean that this was never even listened to
        const auto event = dynamic_cast<EventT<T>*>(m_events[idx].get());
        listener.ignore(*event);
    }

private:

    template<typename T>
    static size_t index();


private:
    static size_t s_indexCounter;
    std::vector<std::unique_ptr<Event>> m_events;
};

template<typename T>
size_t Dispatcher::index() {
    static size_t index = s_indexCounter++;
    return index;
}

}

#endif //DUK_EVENT_EVENT_H
