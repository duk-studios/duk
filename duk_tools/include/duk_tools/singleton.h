/// 23/04/2023
/// singleton.h

#ifndef DUK_TOOLS_SINGLETON_H
#define DUK_TOOLS_SINGLETON_H

namespace duk::tools {

template<typename T>
class Singleton {
public:

    static T* instance() {
        assert(s_instance);
        return s_instance;
    }

    template<typename ...Args>
    static void create(Args&&... args) {
        assert(!s_instance);
        s_instance = new T(std::forward<Args>(args)...);
    }

    static void destroy() {
        assert(s_instance);
        delete s_instance;
        s_instance = nullptr;
    }

    virtual ~Singleton() = default;

private:

    static T* s_instance;
};

template<typename T>
T* Singleton<T>::s_instance = nullptr;

}

#endif // DUK_TOOLS_SINGLETON_H
