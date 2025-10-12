#include "IterativeSolver.h"
#include <iostream>
#include <cmath>

IterativeSolver::IterativeSolver(int maxIter, double tol) 
    : Solver(maxIter, tol) {}

void IterativeSolver::solve(Pipe* pipe, const Fluid* fluid) {
    double P_in = pipe->getInletPressure();
    double T_in = pipe->getInletTemp();
    double V_in = pipe->getInletVelocity();
    
    double P_out = P_in - calculatePressureDrop(pipe, fluid, P_in, T_in, V_in);
    double P_out_old;
    int iter = 0;
    
    do {
        P_out_old = P_out;
        double P_avg = (P_in + P_out) / 2.0;
        double T_avg = T_in;
        double dP = calculatePressureDrop(pipe, fluid, P_avg, T_avg, V_in);
        P_out = P_in - dP;
        iter++;
        
        if (fabs(P_out - P_out_old) < tolerance) {
            break;
        }
    } while (iter < maxIterations);
    
    if (iter >= maxIterations) {
        std::cout << "Warning: Iterative solver did not converge after " 
                  << maxIterations << " iterations" << std::endl;
    }
    
    double rho_in = fluid->getDensity(P_in, T_in);
    double rho_out = fluid->getDensity(P_out, T_in);
    double V_out = V_in * (rho_in / rho_out);
    
    pipe->setOutletConditions(P_out, T_in, V_out);
}

std::string IterativeSolver::getSolverName() const {
    return "Iterative Solver (Average Properties)";
}
