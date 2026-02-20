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
    std::vector<std::shared_ptr<block::block>> find_connectible_adjacent_blocks(std::shared_ptr<block::block> block_);
    std::optional<std::shared_ptr<block::block>> find_any_connectible_adjacent_block(std::shared_ptr<block::block> block_);
    std::optional<std::shared_ptr<block::block>> find_connectible_adjacent_block_at_position(std::shared_ptr<block::block> block_, point::point adjacent_block_position);
    bool extend_cram_cannon(cannon::cannon& cannon_);
    void modify_solution();
};

}

