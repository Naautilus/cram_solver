//#include "globals/globals.hpp"
#include <stdio.h>
#include <iostream>
#include "block/block.hpp"
#include "globals/globals.hpp"
#include "solver/solver.hpp"

// x forward, y right, z down

int main(int argc, char* argv[]) {
    globals::initialize_blocks();
    for (block::block block_ : globals::blocks) {
        std::cout << block_.to_string() << "\n";
    }

    block::grid grid_;
    grid_.add_box(point::point(-1, -1, -1), point::point(3, 3, 3));
    grid_.set_block(std::make_shared<block::block>(block::block(
        point::point(0, 0, -1),
        globals::forwards_mantlet.type_,
        globals::forwards_mantlet.faces
    )));

    solver::solver solver_;
    solver_.solution = grid_;

    int iteration = 0;
    while(true) {
        solver_.iterate_solver(0.0);
        iteration++;
        if (iteration % 1000 == 0) std::cout << "iteration: " << iteration << "\n";
    }
    
}