/// 21/02/2024
/// types.cpp

#include <duk_tools/types.h>
#ifdef __GNUG__
#include <cxxabi.h>
#include <memory>
#endif

namespace duk::tools {

#ifdef __GNUG__

std::string demangle(const char* value) {
    //https://stackoverflow.com/questions/281818/unmangling-the-result-of-stdtype-infoname
    int status = -4;
    std::unique_ptr<char, void (*)(void*)> res{abi::__cxa_demangle(value, NULL, NULL, &status), std::free};

    return (status == 0) ? res.get() : value;
}

std::string type_name_of(const std::type_info& info) {
    return demangle(info.name());
}

#else

std::string demangle(const char* _value) {
    return _value;
}

std::string type_name_of(const type_info& _info) {
    std::string name = _info.name();
    return name.substr(6);// Remove class prefix
}

#endif
}// namespace duk::tools