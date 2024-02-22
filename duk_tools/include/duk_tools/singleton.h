/// 23/04/2023
/// singleton.h

#ifndef DUK_TOOLS_SINGLETON_H
#define DUK_TOOLS_SINGLETON_H

namespace duk::tools {

template<typename T>
class Singleton {
public:
    static bool valid() {
        return s_instance != nullptr;
    }

    static T* instance(bool createIfInvalid = false) {
        if (createIfInvalid && !valid()) {
            create();
        }
        assert(valid());
        return s_instance;
    }

    static void create() {
        if (valid()) {
            destroy();
        }
        s_instance = new T();
    }

    static void destroy() {
        assert(valid());
        delete s_instance;
        s_instance = nullptr;
    }

    virtual ~Singleton() = default;

private:
    static T* s_instance;
};

template<typename T>
T* Singleton<T>::s_instance = nullptr;

}// namespace duk::tools

#endif// DUK_TOOLS_SINGLETON_H
