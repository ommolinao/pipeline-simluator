#ifndef ITERATIVE_SOLVER_H
#define ITERATIVE_SOLVER_H

#include "Solver.h"

/**
 * @brief Iterative solver using average properties
 * 
 * Uses iterative method with fluid properties evaluated at average
 * pressure and temperature. More accurate than SimpleSolver for
 * compressible fluids and long pipes.
 */
class IterativeSolver : public Solver {
public:
    IterativeSolver(int maxIter = 20, double tol = 1.0);
    
    void solve(Pipe* pipe, const Fluid* fluid) override;
    std::string getSolverName() const override;
};

#endif // ITERATIVE_SOLVER_H
