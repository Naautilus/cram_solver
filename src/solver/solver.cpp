#include <iostream>
#include <random>
#include "solver.hpp"
#include "../globals/globals.hpp"

namespace solver {

void solver::iterate_solver(double randomness) {
    std::mt19937 rng(time(nullptr));
    block::grid original_state = solution;
    double original_score = score_current_solution();
    for (int i = 0; i < 3; i++) modify_solution();
    double modified_score = score_current_solution();
    bool print = (modified_score > original_score);
    
    if (print) {
        for (std::shared_ptr<block::block> block_ : solution.blocks) {
            std::cout << block_->to_string() << ", " << block_->position.transpose() << "\n";
        }
    }
    if (print) std::cout << "original_score: " << original_score << "\n";
    if (print) std::cout << "modified_score: " << modified_score << "\n";

    if (modified_score > original_score || (rng() / rng.max()) > randomness) {
        // keep modified state
        if (print) std::cout << "keeping modified state\n";
    } else {
        solution = original_state;
        if (print) std::cout << "keeping original state\n";
    }
}

namespace {

// value function visualiation available at https://www.desmos.com/3d/xdjscswtnt

double value_function(double blocks, double pellets, double compactors) {
    double value = 0;
    value += 1.0 * sqrt(blocks);
    value += 0.15 * sqrt(pellets + compactors);
    value += -0.001 * fmin(1.0/sqrt(blocks), 1.0) * pow(2.4*pellets - 20.0 - compactors, 2.0);
    return value;
}

}

double solver::score_current_solution() {
    std::vector<cannon::metrics> all_metrics = get_all_cram_cannon_metrics();
    double score = 0;
    for (cannon::metrics metrics_ : all_metrics) {
        score += value_function(metrics_.block_count, metrics_.pellet_connections, metrics_.compactor_connections);
    }
    return score;
}

std::vector<cannon::metrics> solver::get_all_cram_cannon_metrics() {
    std::vector<cannon::metrics> output;
    block::grid original_solution_state = solution;

    while(true) {
        std::optional<std::shared_ptr<block::block>> mantlet = solution.find_first_block_of_type(block::mantlet);
        if (!mantlet.has_value()) {
            solution = original_solution_state;
            return output;
        }
        //std::cout << "etnering get_full_cram_cannon\n";
        cannon::cannon cannon_ = get_full_cram_cannon(mantlet.value());
        //std::cout << "exiting get_full_cram_cannon\n";
        output.push_back(cannon_.get_metrics());
        for (std::shared_ptr<block::block> block_ : cannon_.blocks) {
            if (!solution.erase_block(block_)) {
                std::cerr << "fatal error in solver::count_valid_cram_cannons; aborting\n";
                std::abort();
            }
        }
    }
}

cannon::cannon solver::get_full_cram_cannon(std::shared_ptr<block::block> origin) {
    cannon::cannon cannon_{{origin}};
    while(true) {
        //std::cout << "entering extend_cram_cannon\n";
        if (!extend_cram_cannon(cannon_))
            return cannon_;
        //std::cout << "exiting extend_cram_cannon\n";
    };
}

bool solver::extend_cram_cannon(cannon::cannon& cannon_) {
    bool any_new_blocks = false;
    // 7 here represents how many times bigger the vector can grow in a single cycle.
    // This is a lazy way to make sure the vector never gets moved mid-loop.
    cannon_.blocks.reserve(cannon_.blocks.size()*7); 
    for (std::shared_ptr<block::block> block_ : cannon_.blocks) {
        for (point::point adjacent_block_position : point::points_on_faces(block_->position)) {

            auto optional_adjacent_block = solution.get_block(adjacent_block_position);
            if (!optional_adjacent_block.has_value()) continue;
            std::shared_ptr<block::block> adjacent_block = optional_adjacent_block.value();
            
            if (std::find(cannon_.blocks.begin(), cannon_.blocks.end(), adjacent_block) != cannon_.blocks.end()) continue;

            if (!block_->check_for_relation(adjacent_block, face::connection)) continue;
            
            cannon_.blocks.push_back(adjacent_block);
            any_new_blocks = true;
            
        }
    }
    return any_new_blocks;
}
void solver::modify_solution() {
    std::mt19937 rng(time(nullptr));
    int grid_index = rng() % solution.blocks.size();
    int blocks_index = rng() % globals::blocks.size();
    std::shared_ptr<block::block> new_block = std::make_shared<block::block>(block::block(
        solution.blocks[grid_index]->position,
        globals::blocks[blocks_index].type_,
        globals::blocks[blocks_index].faces
    ));
    solution.set_block(new_block);
}



}
