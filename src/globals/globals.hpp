#pragma once
#include <vector>
#include <random>
#include "../block/block.hpp"

namespace globals {

extern block::block forwards_mantlet;
extern std::vector<block::block> blocks;
extern std::mt19937 rng;
void initialize_blocks();

}