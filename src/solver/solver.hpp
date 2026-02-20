#pragma once
#include <memory>
#include "../block/grid.hpp"
#include "../cannon/cannon.hpp"

namespace solver {

struct solver {
    block::grid solution;
    solver(){};
    void iterate_solver(double randomness);
    double score_current_solution();
    std::vector<cannon::metrics> get_all_cram_cannon_metrics();
    cannon::cannon get_full_cram_cannon(std::shared_ptr<block::block> origin);
    bool extend_cram_cannon(cannon::cannon& cannon_);
};

}

