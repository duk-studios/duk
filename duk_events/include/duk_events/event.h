//
// Created by Ricardo on 15/04/2023.
//

#ifndef DUK_EVENTS_EVENT_H
#define DUK_EVENTS_EVENT_H

#include <functional>
#include <cassert>
#include <vector>
#include <memory>

namespace duk::events {

class Event {
private:
    friend class EventListener;
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

    virtual void unsubscribe(size_t listenerId) = 0;

private:
    std::shared_ptr<void> m_controlBlock;
};

template<typename ...Args>
class EventT : public Event {
public:

    using Callback = std::function<void(Args...)>;

    inline void operator()(const Args& ... args) {
        emit(args...);
    }

    void emit(const Args& ... args) {
        assert(!m_emitting && "Attempted to emit a event that was already being emitted.");
        m_emitting = true;
        for (auto& callback: m_callbacks) {
            callback(args...);
        }
        m_emitting = false;
    }

    void subscribe(Callback&& callback, size_t listenerId) {
        assert(!m_emitting && "Attempted to subscribe on a event that is currently emitting");
        assert(std::find_if(m_listeners.begin(), m_listeners.end(), [listenerId](size_t listener) {
            return listener == listenerId;
        }) == m_listeners.end() && "Tried to subscribe to an event with a listener that was already subscribed.");

        m_listeners.emplace_back(listenerId);
        m_callbacks.emplace_back(std::move(callback));
    }

    void unsubscribe(size_t listenerId) override {
        assert(!m_emitting && "Attempted to unsubscribe from an event that is currently emitting");
        auto it = std::find_if(m_listeners.begin(), m_listeners.end(), [listenerId](size_t listener) {
            return listener == listenerId;
        });

        assert(it != m_listeners.end() && "Attempted to unsubscribe from an event with a listener that is not subscribed.");

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

class EventListener {
public:

    EventListener();

    ~EventListener();

    template<typename TEvent>
    void listen(TEvent& event, typename TEvent::Callback&& callback) {
        event.subscribe(std::move(callback), m_id);
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

}

#endif //DUK_EVENTS_EVENT_H
