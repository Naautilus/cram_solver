#pragma once
#include <memory>
#include "../block/grid.hpp"

namespace solver {

struct solver {
    block::grid solution;
    solver(){};
    void iterate_solver(double randomness);
    double score_solution();
    int count_valid_cram_cannons();
    std::vector<std::shared_ptr<block::block>> get_full_cram_cannon(std::shared_ptr<block::block> origin);
    bool extend_cram_cannon(std::vector<std::shared_ptr<block::block>>& cannon_blocks);
};

}

