#ifndef SOLVER_H
#define SOLVER_H

#include <string>
#include "Pipe.h"
#include "Fluid.h"

/**
 * @brief Abstract base class for pipeline solvers
 * 
 * Implements Strategy pattern for different solution algorithms.
 * Provides common pressure drop calculation utilities.
 */
class Solver {
protected:
    int maxIterations;
    double tolerance;
    static constexpr double g = 9.81;  // Gravitational acceleration (m/s²)
    
public:
    Solver(int maxIter = 20, double tol = 1.0);
    virtual ~Solver();
    
    // Pure virtual - must be implemented by derived classes
    virtual void solve(Pipe* pipe, const Fluid* fluid) = 0;
    virtual std::string getSolverName() const = 0;
    
    // Getters and setters
    int getMaxIterations() const;
    double getTolerance() const;
    void setMaxIterations(int maxIter);
    void setTolerance(double tol);
    
protected:
    /**
     * @brief Calculate pressure drop for a pipe using Darcy-Weisbach equation
     * @param pipe Pipe object with geometry
     * @param fluid Fluid object for properties
     * @param P Pressure for property evaluation (Pa)
     * @param T Temperature for property evaluation (K)
     * @param velocity Flow velocity (m/s)
     * @return Total pressure drop (Pa) including friction and hydrostatic
     */
    double calculatePressureDrop(const Pipe* pipe, const Fluid* fluid, 
                                 double P, double T, double velocity) const;
};

#endif // SOLVER_H
