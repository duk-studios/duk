//
// Created by Ricardo on 09/07/2023.
//

#include <duk_tools/bit_block.h>
#include <iostream>

int main() {
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