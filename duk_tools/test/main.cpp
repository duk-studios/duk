//
// Created by Ricardo on 09/07/2023.
//

#include <duk_tools/bit_block.h>
#include <iostream>
#include <bitset>
#include <bit>

int main() {

    std::bitset<128> bitset;

    duk::tools::BitBlock<128> bitBlock;
    for (int i = 0; i < 64; i++) {
        bitBlock.set(i);
    }

    bitBlock = ~bitBlock;

    std::cout << "BitBlock: " << bitBlock.to_string() << std::endl;
    std::cout << "Set bit indices: ";
    duk::tools::for_each_bit<false>(bitBlock, [](size_t bitIndex) {
        std::cout << bitIndex << ',';
    });
    std::cout << std::endl;

    return 0;
}