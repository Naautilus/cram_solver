#pragma once
#include <vector>
#include "../block/block.hpp"

namespace globals {

extern std::vector<block::block> blocks;
void initialize_blocks();

}