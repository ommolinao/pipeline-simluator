#ifndef SIMPLE_SOLVER_H
#define SIMPLE_SOLVER_H

#include "Solver.h"

/**
 * @brief Simple solver using inlet properties only
 * 
 * Single-pass calculation using fluid properties at inlet conditions.
 * Fast but less accurate for compressible fluids or long pipes.
 */
class SimpleSolver : public Solver {
public:
    SimpleSolver();
    
    void solve(Pipe* pipe, const Fluid* fluid) override;
    std::string getSolverName() const override;
};

#endif // SIMPLE_SOLVER_H
