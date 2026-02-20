#pragma once
#include <vector>
#include "../block/block.hpp"

namespace globals {

extern block::block forwards_mantlet;
extern std::vector<block::block> blocks;
void initialize_blocks();

}