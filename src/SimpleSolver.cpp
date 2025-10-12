#include "SimpleSolver.h"

SimpleSolver::SimpleSolver() : Solver(1, 0.0) {}

void SimpleSolver::solve(Pipe* pipe, const Fluid* fluid) {
    double P_in = pipe->getInletPressure();
    double T_in = pipe->getInletTemp();
    double V_in = pipe->getInletVelocity();
    
    // Calculate pressure drop using inlet properties
    double dP = calculatePressureDrop(pipe, fluid, P_in, T_in, V_in);
    double P_out = P_in - dP;
    
    // Calculate outlet velocity using continuity equation
    double rho_in = fluid->getDensity(P_in, T_in);
    double rho_out = fluid->getDensity(P_out, T_in);
    double V_out = V_in * (rho_in / rho_out);
    
    // Set outlet conditions
    pipe->setOutletConditions(P_out, T_in, V_out);
}

std::string SimpleSolver::getSolverName() const {
    return "Simple Solver (Inlet Properties)";
}
