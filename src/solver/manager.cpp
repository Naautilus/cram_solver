#include <iostream>
#include "manager.hpp"

namespace solver {

manager::manager(int solver_count, block::grid initial_state) {
    solvers = std::vector<solver>(solver_count, solver(initial_state));
}

block::grid manager::iterate_solvers(double randomness, bool print) {
    solver& best_solver = solvers[0];
    double best_score;
    for (solver& solver_ : solvers) {
        double score = solver_.iterate_solver(randomness, false);
        if (score > best_score) {
            best_solver = solver_;
            best_score = score;
        }
    }
    
    block::grid best_solution = best_solver.solution;
    if (print) std::cout << best_solution.to_string() << "\n";
    if (print) std::cout << best_solver.get_metrics_string() << "\n";
    if (print) std::cout << "score: " << best_score << "\n";

    return best_solution;
}

}