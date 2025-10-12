#include "Solver.h"
#include <cmath>

Solver::Solver(int maxIter, double tol) 
    : maxIterations(maxIter), tolerance(tol) {}

Solver::~Solver() {}

int Solver::getMaxIterations() const {
    return maxIterations;
}

double Solver::getTolerance() const {
    return tolerance;
}

void Solver::setMaxIterations(int maxIter) {
    maxIterations = maxIter;
}

void Solver::setTolerance(double tol) {
    tolerance = tol;
}

double Solver::calculatePressureDrop(const Pipe* pipe, const Fluid* fluid, 
                                     double P, double T, double velocity) const {
    // Calculate friction factor
    double rho = fluid->getDensity(P, T);
    double mu = fluid->getViscosity(P, T);
    double Re = (rho * velocity * pipe->getDiameter()) / mu;
    double eD = pipe->getRoughness() / pipe->getDiameter();
    
    double f;
    if (Re < 2300) {
        // Laminar flow
        f = 64.0 / Re;
    } else {
        // Turbulent flow - Swamee-Jain approximation
        double term1 = eD / 3.7;
        double term2 = 5.74 / pow(Re, 0.9);
        f = 0.25 / pow(log10(term1 + term2), 2);
    }
    
    // Darcy-Weisbach equation: ΔP_friction = f * (L/D) * (ρ*V²/2)
    double frictionLoss = f * (pipe->getLength() / pipe->getDiameter()) * (rho * velocity * velocity / 2.0);
    
    // Hydrostatic pressure change: ΔP_hydrostatic = ρ * g * Δz
    double elevationChange = pipe->getElevationChange();
    double hydrostaticLoss = rho * g * elevationChange;
    
    // Total pressure drop
    return frictionLoss + hydrostaticLoss;
}
