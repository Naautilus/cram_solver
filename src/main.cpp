//#include "globals/globals.hpp"
#include <stdio.h>
#include <iostream>
#include "block/block.hpp"
#include "globals/globals.hpp"

// x forward, y right, z down

int main(int argc, char* argv[]) {
    globals::initialize_blocks();
    for (block::block block_ : globals::blocks) {
        std::cout << block_.to_string() << "\n";
    }
}