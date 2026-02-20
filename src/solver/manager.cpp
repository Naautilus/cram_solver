#include "manager.hpp"

namespace solver {

manager::manager(int solver_count, block::grid initial_state) {
    solvers = std::vector<solver>(solver_count, solver(initial_state));
}

block::grid manager::iterate_solvers(double randomness, bool print) {
    block::grid best_solution;
    double best_score;
    for (solver& solver_ : solvers) {
        double score = solver_.iterate_solver(randomness, print);
        if (score > best_score) {
            best_solution = solver_.solution;
            best_score = score;
        }
    }
    return best_solution;
}

}