#include "RK4Solver.h"
#include <cmath>
#include <iostream>

RK4Solver::RK4Solver(int steps, double tol, int maxIter)
    : Solver(maxIter, tol), numSteps(steps) {
    
    if (numSteps < 10) {
        std::cerr << "Warning: RK4Solver with < 10 steps may be inaccurate. Using 10 steps.\n";
        numSteps = 10;
    }
}

RK4Solver::State RK4Solver::calculateDerivatives(const State& state, const Pipe* pipe, const Fluid* fluid) const {
    State derivatives;
    
    // Get pipe properties
    double D = pipe->getDiameter();
    double roughness = pipe->getRoughness();
    double L = pipe->getLength();
    double z_in = pipe->getInletElevation();
    double z_out = pipe->getOutletElevation();
    
    // Get fluid properties at current conditions
    double rho = fluid->getDensity(state.pressure, state.temperature);
    double mu = fluid->getViscosity(state.pressure, state.temperature);
    
    // Calculate Reynolds number
    double Re = (rho * state.velocity * D) / mu;
    
    // Calculate friction factor using Colebrook-White (Swamee-Jain approximation)
    double f;
    if (Re < 2300) {
        // Laminar flow
        f = 64.0 / Re;
    } else {
        // Turbulent flow - Swamee-Jain explicit approximation
        double relRough = roughness / D;
        double term1 = relRough / 3.7;
        double term2 = 5.74 / std::pow(Re, 0.9);
        f = 0.25 / std::pow(std::log10(term1 + term2), 2);
    }
    
    // Calculate elevation angle
    double dz_dx = (z_out - z_in) / L;
    
    // Momentum equation: dP/dx = -friction - gravity
    double frictionTerm = -f * (rho * state.velocity * state.velocity) / (2.0 * D);
    double gravityTerm = -rho * 9.81 * dz_dx;
    derivatives.pressure = frictionTerm + gravityTerm;
    
    // Energy equation: For isothermal flow, dT/dx ≈ 0
    // For more complex thermal models, add heat transfer here
    derivatives.temperature = 0.0;
    
    // Continuity equation: ρAV = constant
    // For liquids (nearly incompressible): AV = constant
    // Since diameter is constant within a segment, dV/dx = 0 within each segment
    // Velocity changes occur at diameter transitions (handled by pipeline boundary conditions)
    derivatives.velocity = 0.0;
    
    // Note: This assumes uniform diameter within each pipe segment.
    // Velocity adjustments for diameter changes and compressibility happen between segments
    // via continuity enforcement in Pipeline::solveAll()
    
    // Position derivative
    derivatives.x = 1.0;  // dx/dx = 1
    
    return derivatives;
}

RK4Solver::State RK4Solver::rk4Step(const State& state, double h, const Pipe* pipe, const Fluid* fluid) const {
    // Calculate k1
    State k1 = calculateDerivatives(state, pipe, fluid);
    
    // Calculate k2
    State state2;
    state2.pressure = state.pressure + 0.5 * h * k1.pressure;
    state2.temperature = state.temperature + 0.5 * h * k1.temperature;
    state2.velocity = state.velocity + 0.5 * h * k1.velocity;
    state2.x = state.x + 0.5 * h;
    State k2 = calculateDerivatives(state2, pipe, fluid);
    
    // Calculate k3
    State state3;
    state3.pressure = state.pressure + 0.5 * h * k2.pressure;
    state3.temperature = state.temperature + 0.5 * h * k2.temperature;
    state3.velocity = state.velocity + 0.5 * h * k2.velocity;
    state3.x = state.x + 0.5 * h;
    State k3 = calculateDerivatives(state3, pipe, fluid);
    
    // Calculate k4
    State state4;
    state4.pressure = state.pressure + h * k3.pressure;
    state4.temperature = state.temperature + h * k3.temperature;
    state4.velocity = state.velocity + h * k3.velocity;
    state4.x = state.x + h;
    State k4 = calculateDerivatives(state4, pipe, fluid);
    
    // Combine to get new state
    State newState;
    newState.pressure = state.pressure + (h / 6.0) * 
                       (k1.pressure + 2*k2.pressure + 2*k3.pressure + k4.pressure);
    newState.temperature = state.temperature + (h / 6.0) * 
                          (k1.temperature + 2*k2.temperature + 2*k3.temperature + k4.temperature);
    newState.velocity = state.velocity + (h / 6.0) * 
                       (k1.velocity + 2*k2.velocity + 2*k3.velocity + k4.velocity);
    newState.x = state.x + h;
    
    return newState;
}

void RK4Solver::solve(Pipe* pipe, const Fluid* fluid) {
    if (pipe == nullptr || fluid == nullptr) {
        std::cerr << "ERROR: Pipe or Fluid is null in RK4Solver!\n";
        return;
    }
    
    // Initialize state from inlet conditions
    State state;
    state.pressure = pipe->getInletPressure();
    state.temperature = pipe->getInletTemp();
    state.velocity = pipe->getInletVelocity();
    state.x = 0.0;
    
    // Calculate step size
    double L = pipe->getLength();
    double h = L / static_cast<double>(numSteps);
    
    // Integrate along the pipe using RK4
    for (int i = 0; i < numSteps; ++i) {
        state = rk4Step(state, h, pipe, fluid);
    }
    
    // Apply compressible continuity correction for outlet velocity
    // For compressible flow: ρ_in × A × V_in = ρ_out × A × V_out
    // Therefore: V_out = V_in × (ρ_in / ρ_out)
    double rho_in = fluid->getDensity(pipe->getInletPressure(), pipe->getInletTemp());
    double rho_out = fluid->getDensity(state.pressure, state.temperature);
    double V_out_corrected = pipe->getInletVelocity() * (rho_in / rho_out);
    
    // Set outlet conditions (using the compressibility-corrected velocity)
    pipe->setOutletConditions(state.pressure, state.temperature, V_out_corrected);
}

void RK4Solver::setNumSteps(int steps) {
    if (steps < 10) {
        std::cerr << "Warning: Minimum 10 steps recommended. Setting to 10.\n";
        numSteps = 10;
    } else {
        numSteps = steps;
    }
}

int RK4Solver::getNumSteps() const {
    return numSteps;
}

std::string RK4Solver::getSolverName() const {
    return "RK4Solver";
}
