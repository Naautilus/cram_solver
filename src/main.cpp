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
    grid_.add_cylinder(point::point(-2, -2, 0), 5, 5);
    grid_.set_block(std::make_shared<block::block>(block::block(
        point::point(0, 0, 0),
        globals::forwards_mantlet.type_,
        globals::forwards_mantlet.faces
    )));

    solver::solver solver_;
    solver_.solution = grid_;

    int iteration = 0;
    while(true) {
        bool print = (iteration % 100 == 0);
        solver_.iterate_solver(1e-4, print);
        if (print) std::cout << "iteration: " << iteration << "\n";
        iteration++;
    }
    
}