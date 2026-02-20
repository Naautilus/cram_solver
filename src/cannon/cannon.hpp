#pragma once
#include "../block/grid.hpp"
#include "metrics.hpp"

namespace cannon {

struct cannon {
    std::vector<std::shared_ptr<block::block>> blocks;
    metrics get_metrics();
};
    
}