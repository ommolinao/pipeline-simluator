#include "AdaptiveSolver.h"
#include <iostream>
#include <cmath>
#include <string>

AdaptiveSolver::AdaptiveSolver(double maxDP, int maxIter, double tol, 
                               int maxSubdiv, bool verb)
    : Solver(maxIter, tol), maxPressureDrop(maxDP), 
      maxSubdivisions(maxSubdiv), verbose(verb) {}

void AdaptiveSolver::solve(Pipe* pipe, const Fluid* fluid) {
    double P_in = pipe->getInletPressure();
    double T_in = pipe->getInletTemp();
    double V_in = pipe->getInletVelocity();
    
    if (verbose) {
        std::cout << "Starting adaptive solver:" << std::endl;
        std::cout << "  Inlet P = " << P_in/1000.0 << " kPa" << std::endl;
        std::cout << "  Max ΔP per segment = " << maxPressureDrop/1000.0 << " kPa" << std::endl;
    }
    
    double P_out, V_out;
    
    try {
        // Start adaptive refinement
        solveSegment(fluid,
                    pipe->getInletPosition(), pipe->getOutletPosition(),
                    pipe->getInletElevation(), pipe->getOutletElevation(),
                    P_in, T_in, V_in,
                    pipe->getDiameter(), pipe->getRoughness(),
                    P_out, V_out);
        
        if (verbose) {
            std::cout << "  Final outlet P = " << P_out/1000.0 << " kPa" << std::endl;
        }
        
        pipe->setOutletConditions(P_out, T_in, V_out);
    } catch (const std::runtime_error& e) {
        std::cerr << "\nAdaptive solver failed: " << e.what() << std::endl;
        // Set outlet conditions to indicate failure
        pipe->setOutletConditions(-1.0, T_in, 0.0);
    }
}

void AdaptiveSolver::solveSegment(const Fluid* fluid, 
                                  double x_start, double x_end,
                                  double z_start, double z_end,
                                  double P_in, double T_in, double V_in,
                                  double diameter, double roughness,
                                  double& P_out, double& V_out,
                                  int depth) const {
    
    double segment_length = fabs(x_end - x_start);
    double elevation_change = z_end - z_start;
    
    if (verbose && depth < 3) {
        std::cout << "  " << std::string(depth*2, ' ') 
                  << "Depth " << depth << ": Length=" << segment_length 
                  << "m, Elevation change=" << elevation_change << "m" << std::endl;
    }
    
    // Initial guess for pressure drop
    double dP_initial = calculatePressureDropForSegment(
        fluid, segment_length, diameter, roughness,
        elevation_change, P_in, T_in, V_in);
    
    double P_out_initial = P_in - dP_initial;
    
    // Check for ill-posed problem on first iteration
    if (P_out_initial < 0 && depth == 0) {
        std::cerr << "\nERROR: Ill-posed problem detected!" << std::endl;
        std::cerr << "Initial calculation yields negative outlet pressure: " 
                  << P_out_initial/1000.0 << " kPa" << std::endl;
        std::cerr << "Inlet pressure: " << P_in/1000.0 << " kPa" << std::endl;
        std::cerr << "Estimated pressure drop: " << dP_initial/1000.0 << " kPa" << std::endl;
        std::cerr << "  - Hydrostatic component: " 
                  << (fluid->getDensity(P_in, T_in) * g * elevation_change)/1000.0 
                  << " kPa" << std::endl;
        std::cerr << "\nSuggestions:" << std::endl;
        std::cerr << "  1. Increase inlet pressure" << std::endl;
        std::cerr << "  2. Reduce elevation gain" << std::endl;
        std::cerr << "  3. Reduce pipe length or flow velocity" << std::endl;
        throw std::runtime_error("Negative outlet pressure - inlet conditions insufficient");
    }
    
    // Iterative solution for this segment
    double P_out_guess = P_out_initial;
    double P_out_old;
    int iter = 0;
    
    do {
        P_out_old = P_out_guess;
        double P_avg = (P_in + P_out_guess) / 2.0;
        double T_avg = T_in;
        
        double dP = calculatePressureDropForSegment(
            fluid, segment_length, diameter, roughness,
            elevation_change, P_avg, T_avg, V_in);
        
        P_out_guess = P_in - dP;
        
        // Check for negative pressure during iteration
        if (P_out_guess < 0) {
            if (depth == 0) {
                std::cerr << "\nERROR: Negative outlet pressure during iteration!" << std::endl;
                std::cerr << "P_out = " << P_out_guess/1000.0 << " kPa at iteration " << iter << std::endl;
                throw std::runtime_error("Negative outlet pressure - problem cannot be solved");
            }
            // For subdivisions, just accept the result and let it propagate
            break;
        }
        
        iter++;
        
        if (fabs(P_out_guess - P_out_old) < tolerance) {
            break;
        }
    } while (iter < maxIterations);
    
    double dP = P_in - P_out_guess;
    
    if (verbose && depth < 3) {
        std::cout << "  " << std::string(depth*2, ' ') 
                  << "ΔP = " << dP/1000.0 << " kPa, P_out = " 
                  << P_out_guess/1000.0 << " kPa" << std::endl;
    }
    
    // Check if subdivision is needed
    if (fabs(dP) > maxPressureDrop && depth < maxSubdivisions && P_out_guess > 0) {
        if (verbose && depth < 3) {
            std::cout << "  " << std::string(depth*2, ' ') 
                      << "Subdividing (ΔP > " << maxPressureDrop/1000.0 << " kPa)" << std::endl;
        }
        
        // Subdivide: split the segment in half
        double x_mid = (x_start + x_end) / 2.0;
        double z_mid = (z_start + z_end) / 2.0;
        
        // Solve first half
        double P_mid, V_mid;
        solveSegment(fluid, x_start, x_mid, z_start, z_mid,
                    P_in, T_in, V_in, diameter, roughness,
                    P_mid, V_mid, depth + 1);
        
        // Check if first half resulted in negative pressure
        if (P_mid < 0) {
            std::cerr << "\nERROR: Negative pressure encountered during subdivision!" << std::endl;
            std::cerr << "Midpoint pressure: " << P_mid/1000.0 << " kPa" << std::endl;
            throw std::runtime_error("Negative pressure in pipe segment");
        }
        
        // Solve second half using outlet of first half as inlet
        solveSegment(fluid, x_mid, x_end, z_mid, z_end,
                    P_mid, T_in, V_mid, diameter, roughness,
                    P_out, V_out, depth + 1);
    } else {
        // No subdivision needed, accept this result
        P_out = P_out_guess;
        
        // Calculate outlet velocity using continuity
        double rho_in = fluid->getDensity(P_in, T_in);
        double rho_out = fluid->getDensity(P_out, T_in);
        V_out = V_in * (rho_in / rho_out);
        
        if (verbose && depth < 3) {
            std::cout << "  " << std::string(depth*2, ' ') 
                      << "No subdivision needed" << std::endl;
        }
    }
}

double AdaptiveSolver::calculatePressureDropForSegment(const Fluid* fluid,
                                                       double length, double diameter, double roughness,
                                                       double elevation_change,
                                                       double P, double T, double velocity) const {
    double rho = fluid->getDensity(P, T);
    double mu = fluid->getViscosity(P, T);
    double Re = (rho * velocity * diameter) / mu;
    double eD = roughness / diameter;
    
    double f;
    if (Re < 2300) {
        f = 64.0 / Re;
    } else {
        double term1 = eD / 3.7;
        double term2 = 5.74 / pow(Re, 0.9);
        f = 0.25 / pow(log10(term1 + term2), 2);
    }
    
    double frictionLoss = f * (length / diameter) * (rho * velocity * velocity / 2.0);
    double hydrostaticLoss = rho * g * elevation_change;
    
    return frictionLoss + hydrostaticLoss;
}

std::string AdaptiveSolver::getSolverName() const {
    return "Adaptive Solver (Max ΔP = " + std::to_string(maxPressureDrop/1000.0) + " kPa)";
}

double AdaptiveSolver::getMaxPressureDrop() const {
    return maxPressureDrop;
}

void AdaptiveSolver::setMaxPressureDrop(double maxDP) {
    maxPressureDrop = maxDP;
}

void AdaptiveSolver::setVerbose(bool verb) {
    verbose = verb;
}
