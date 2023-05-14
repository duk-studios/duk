/// 23/04/2023
/// singleton.h

#ifndef DUK_TOOLS_SINGLETON_H
#define DUK_TOOLS_SINGLETON_H

namespace duk::tools {

template<typename T>
class Singleton {
public:
    static T* instance() {
        static T* s_instance = new T();
        return s_instance;
    }
};

}

#endif // DUK_TOOLS_SINGLETON_H
