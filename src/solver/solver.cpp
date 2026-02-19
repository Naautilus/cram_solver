#include "solver.hpp"

namespace solver {

void solver::iterate_solver(double randomness) {

}

void solver::score_solution() {

}

int solver::count_valid_cram_cannons() {
    int count = 0;
    block::grid original_solution_state = solution;

    while(true) {
        std::optional<std::shared_ptr<block::block>> mantlet = solution.find_first_block_of_type(block::mantlet);
        if (!mantlet.has_value()) {
            solution = original_solution_state;
            return count;
        }
        count++;
        std::vector<std::shared_ptr<block::block>> blocks_in_cram_cannon = get_full_cram_cannon(mantlet.value());
        for (std::shared_ptr<block::block> block_ : blocks_in_cram_cannon) {
            if (!solution.delete_block(block_)) std::abort();
        }
    }
}

std::vector<std::shared_ptr<block::block>> solver::get_full_cram_cannon(std::shared_ptr<block::block> origin) {
    std::vector<std::shared_ptr<block::block>> cannon_blocks = {origin};
    while(true) {
        if (!extend_cram_cannon(cannon_blocks))
            return cannon_blocks;
    };
}

bool solver::extend_cram_cannon(std::vector<std::shared_ptr<block::block>>& cannon_blocks) {
    bool any_new_blocks = false;
    for (std::shared_ptr<block::block> block_ : cannon_blocks) {
        for (point::point adjacent_block_positions : point::points_on_faces(block_->position)) {
            if (!solution.block_location_cache.contains(adjacent_block_positions)) continue;
            std::shared_ptr<block::block> adjacent_block = solution.block_location_cache[adjacent_block_positions];
            if (std::find(cannon_blocks.begin(), cannon_blocks.end(), adjacent_block) != cannon_blocks.end()) continue;
            cannon_blocks.push_back(adjacent_block);
            any_new_blocks = true;
        }
    }
    return any_new_blocks;
}



}
