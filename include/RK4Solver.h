#ifndef RK4SOLVER_H
#define RK4SOLVER_H

#include "Solver.h"
#include "Pipe.h"
#include "Fluid.h"

/**
 * @brief 4th-order Runge-Kutta solver for pipe flow equations
 * 
 * Solves the coupled differential equations for pressure, temperature, and velocity
 * along a pipe segment using the classical RK4 method.
 * 
 * The RK4 method provides 4th-order accuracy: O(h^4) global error
 * Much more accurate than simple Euler methods for the same step size.
 * 
 * Governing equations integrated:
 * - Momentum: dP/dx = -f * (ρV²)/(2D) - ρg*sin(θ)
 * - Energy: dT/dx = heat transfer and friction effects
 * - Continuity: ρAV = constant (enforced at boundaries)
 */
class RK4Solver : public Solver {
private:
    int numSteps;           // Number of integration steps
    
    /**
     * @brief State vector for RK4 integration
     */
    struct State {
        double pressure;     // Pa
        double temperature;  // K
        double velocity;     // m/s
        double x;            // Position along pipe (m)
    };
    
    /**
     * @brief Calculate derivatives dState/dx
     * @param state Current state
     * @param pipe Pipe geometry
     * @param fluid Fluid properties
     * @return Derivatives of state variables
     */
    State calculateDerivatives(const State& state, const Pipe* pipe, const Fluid* fluid) const;
    
    /**
     * @brief Perform one RK4 integration step
     * @param state Current state
     * @param h Step size
     * @param pipe Pipe geometry
     * @param fluid Fluid properties
     * @return New state after step
     */
    State rk4Step(const State& state, double h, const Pipe* pipe, const Fluid* fluid) const;

public:
    /**
     * @brief Constructor
     * @param steps Number of integration steps (more steps = more accurate)
     * @param tol Convergence tolerance (default 1.0)
     * @param maxIter Maximum iterations for convergence (default 20)
     */
    RK4Solver(int steps = 100, double tol = 1.0, int maxIter = 20);
    
    /**
     * @brief Solve pipe flow using RK4 integration
     * @param pipe Pipe to solve
     * @param fluid Fluid being transported
     */
    void solve(Pipe* pipe, const Fluid* fluid) override;
    
    /**
     * @brief Set number of integration steps
     */
    void setNumSteps(int steps);
    
    /**
     * @brief Get number of integration steps
     */
    int getNumSteps() const;
    
    /**
     * @brief Get solver name
     */
    std::string getSolverName() const override;
};

#endif // RK4SOLVER_H
