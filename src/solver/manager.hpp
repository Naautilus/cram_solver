#include "solver.hpp"

namespace solver {

class manager {
    std::vector<solver> solvers;
    public:
    manager(int solver_count, block::grid initial_state);
    block::grid iterate_solvers(double randomness, bool print);
};

}