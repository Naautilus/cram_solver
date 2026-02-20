#include <iostream>
#include "solver.hpp"
#include "../globals/globals.hpp"

namespace solver {

solver::solver(block::grid solution_) {
    solution = solution_;
    /*
    for (std::shared_ptr<block::block> block_ : solution_.blocks) {
        solution.set_block(std::make_shared<block::block>(block::block(*block_)));
    }
    */
}


double solver::iterate_solver(double randomness, bool print) {
    block::grid original_state = solution;
    double original_score = score_current_solution();
    for (int i = 0; i < 1; i++) modify_solution();
    double modified_score = score_current_solution();
    bool make_change = (modified_score > original_score || (globals::rng() * 1.0 / globals::rng.max()) < randomness);
    
    if (print) {
        auto metrics_list = get_all_cram_cannon_metrics();
        if (metrics_list.size() != 0) {
            cannon::metrics metrics_ = get_all_cram_cannon_metrics()[0];
            std::cout << "metrics_.block_count: " << metrics_.block_count << "\n";
            std::cout << "metrics_.pellet_connections: " << metrics_.pellet_connections << "\n";
            std::cout << "metrics_.compactor_connections: " << metrics_.compactor_connections << "\n";
        }
    }
    if (print) {
        for (std::shared_ptr<block::block> block_ : solution.blocks) {
            //std::cout << block_->to_string() << ", " << block_->position.transpose() << "\n";
        }
    }
    if (print) std::cout << "original_score: " << original_score << "\n";
    if (print) std::cout << "modified_score: " << modified_score << "\n";

    if (make_change) {
        // keep modified state
        if (print) std::cout << "keeping modified state\n";
        return modified_score;
    } else {
        solution = original_state;
        if (print) std::cout << "keeping original state\n";
        return original_score;
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
        cannon::cannon cannon_ = get_full_cram_cannon(mantlet.value());
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
        if (!extend_cram_cannon(cannon_))
            return cannon_;
    };
}

std::vector<std::shared_ptr<block::block>> solver::find_connectible_adjacent_blocks(std::shared_ptr<block::block> block_) {
    std::vector<std::shared_ptr<block::block>> output;
    for (point::point adjacent_block_position : point::points_on_faces(block_->position)) {
        auto optional_adjacent_block = find_connectible_adjacent_block_at_position(block_, adjacent_block_position);
        if (!optional_adjacent_block.has_value()) continue;
        std::shared_ptr<block::block> adjacent_block = optional_adjacent_block.value();
        output.push_back(adjacent_block);
    }
    return output;
}

std::optional<std::shared_ptr<block::block>> solver::find_any_connectible_adjacent_block(std::shared_ptr<block::block> block_) {
    std::vector<std::shared_ptr<block::block>> output;
    for (point::point adjacent_block_position : point::points_on_faces(block_->position)) {
        auto optional_adjacent_block = find_connectible_adjacent_block_at_position(block_, adjacent_block_position);
        if (!optional_adjacent_block.has_value()) continue;
        std::shared_ptr<block::block> adjacent_block = optional_adjacent_block.value();
        return adjacent_block;
    }
    return std::nullopt;
}

std::optional<std::shared_ptr<block::block>> solver::find_connectible_adjacent_block_at_position(std::shared_ptr<block::block> block_, point::point adjacent_block_position) {
    auto optional_adjacent_block = solution.get_block(adjacent_block_position);
    if (!optional_adjacent_block.has_value()) return std::nullopt;
    std::shared_ptr<block::block> adjacent_block = optional_adjacent_block.value();
    if (!block_->check_for_relation(adjacent_block, face::connection)) return std::nullopt;
    return adjacent_block;
}

bool solver::extend_cram_cannon(cannon::cannon& cannon_) {
    bool any_new_blocks = false;
    // 7 here represents how many times bigger the vector can grow in a single cycle.
    // This is an easy way to make sure the vector never gets moved mid-loop.
    cannon_.blocks.reserve(cannon_.blocks.size()*7); 
    for (std::shared_ptr<block::block> block_ : cannon_.blocks) {
        std::vector<std::shared_ptr<block::block>> connectible_adjacent_blocks = find_connectible_adjacent_blocks(block_);
        for (std::shared_ptr<block::block> adjacent_block : connectible_adjacent_blocks) {
            if (std::find(cannon_.blocks.begin(), cannon_.blocks.end(), adjacent_block) != cannon_.blocks.end()) continue;
            cannon_.blocks.push_back(adjacent_block);
            any_new_blocks = true;
        }
    }
    return any_new_blocks;
}

void solver::modify_solution() {
    std::shared_ptr<block::block> new_block;
    int grid_index;
    do {
        grid_index = globals::rng() % solution.blocks.size();
        int blocks_index;
        do {
            blocks_index = globals::rng() % globals::blocks.size();
        } while (globals::blocks[blocks_index].type_ == block::air
              || globals::blocks[blocks_index].type_ == block::mantlet);
        new_block = std::make_shared<block::block>(block::block(
            solution.blocks[grid_index]->position,
            globals::blocks[blocks_index].type_,
            globals::blocks[blocks_index].faces
        ));
    } while (!find_any_connectible_adjacent_block(new_block).has_value()
          || solution.blocks[grid_index]->type_ == block::mantlet);
    solution.set_block(new_block);
}



}
