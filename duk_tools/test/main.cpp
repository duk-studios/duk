//
// Created by Ricardo on 09/07/2023.
//

#include <duk_tools/bit_block.h>
#include <duk_tools/globals.h>
#include <iostream>

class Global1 {
public:
    void foo() {
        std::cout << "Global1::foo()" << std::endl;
    }
};

class Global2 {
public:

    Global2(int a)
        : m_a(a) {

    }

    void foo() {
        std::cout << "Global2::foo() " << m_a << std::endl;
    }

private:
    int m_a;
};

int test_globals() {
    duk::tools::Globals globals;

    auto global1 = globals.add<Global1>();

    global1->foo();

    auto global2 = globals.add<Global2>(42);

    global2->foo();

    return 0;
}

int test_bit_block() {
    duk::tools::BitBlock<128> bitBlock;
    for (int i = 0; i < 64; i++) {
        bitBlock.set(i);
    }

    bitBlock = ~bitBlock;

    std::cout << "BitBlock: " << bitBlock.to_string() << std::endl;
    std::cout << "Set bit indices: ";
    for (auto bit: bitBlock.bits<true>()) {
        std::cout << bit << ',';
    }
    std::cout << std::endl;
    std::cout << "Unset bit indices: ";
    duk::tools::for_each_bit<false>(bitBlock, [](size_t bitIndex) {
        std::cout << bitIndex << ',';
    });
    std::cout << std::endl;
    return 0;
}

int main() {
    test_globals();
    test_bit_block();
    return 0;
}