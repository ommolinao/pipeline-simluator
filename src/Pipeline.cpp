#include "Pipeline.h"
#include "Solver.h"
#include "Fluid.h"
#include <cmath>

#define DEBUG

// Define PI if not available
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Pipeline::Pipeline(std::string id) : pipelineID(id), numSegments(0) {
    // Pre-allocate space for efficiency
    constexpr size_t RESERVE_SIZE = 100;
    
    // Reserve SoA arrays - Geometry
    segmentIDs.reserve(RESERVE_SIZE);
    x_inlets.reserve(RESERVE_SIZE);
    x_outlets.reserve(RESERVE_SIZE);
    z_inlets.reserve(RESERVE_SIZE);
    z_outlets.reserve(RESERVE_SIZE);
    lengths.reserve(RESERVE_SIZE);
    diameters.reserve(RESERVE_SIZE);
    roughnesses.reserve(RESERVE_SIZE);
    
    // Reserve SoA arrays - State
    inletPressures.reserve(RESERVE_SIZE);
    outletPressures.reserve(RESERVE_SIZE);
    inletTemps.reserve(RESERVE_SIZE);
    outletTemps.reserve(RESERVE_SIZE);
    inletVelocities.reserve(RESERVE_SIZE);
    outletVelocities.reserve(RESERVE_SIZE);
    
    // Reserve SoA arrays - Calculated Fluid Properties
    reynoldsNumbers.reserve(RESERVE_SIZE);
    frictionFactors.reserve(RESERVE_SIZE);
    densities.reserve(RESERVE_SIZE);
    viscosities.reserve(RESERVE_SIZE);
    pressureGradients.reserve(RESERVE_SIZE);
}

Pipeline::~Pipeline() {
    // PHASE 4: No Pipe pointers to clean up - pure SoA storage
}

bool Pipeline::addPipe(Pipe* pipe) {
    if (pipe == nullptr) {
        std::cerr << "ERROR: Cannot add null pipe to pipeline!" << "\n";
        return false;
    }
    
    // PHASE 4: Extract data from Pipe and store in SoA (don't keep pointer)
    return addPipeSegment(
        pipe->getPipeID(),
        pipe->getInletPosition(),
        pipe->getOutletPosition(),
        pipe->getInletElevation(),
        pipe->getOutletElevation(),
        pipe->getDiameter(),
        pipe->getRoughness()
    );
}

bool Pipeline::addPipeSegment(const std::string& id, double x_in, double x_out,
                               double z_in, double z_out, double dia, double rough) {
    // Validate connectivity if not first segment
    if (numSegments > 0) {
        const double tolerance = 1e-6;  // 1 micrometer tolerance
        double dx = std::abs(x_outlets.back() - x_in);
        double dz = std::abs(z_outlets.back() - z_in);
        
        if (dx > tolerance || dz > tolerance) {
            std::cerr << "ERROR: Cannot add pipe segment '" << id 
                      << "' to pipeline '" << pipelineID << "'" << "\n";
            std::cerr << "  Last segment '" << segmentIDs.back() 
                      << "' outlet: x=" << x_outlets.back() 
                      << "m, z=" << z_outlets.back() << "m" << "\n";
            std::cerr << "  New segment '" << id 
                      << "' inlet: x=" << x_in 
                      << "m, z=" << z_in << "m" << "\n";
            std::cerr << "  The segments are not connected!" << "\n";
            return false;
        }
    }
    
    // Calculate length
    double length = std::sqrt(std::pow(x_out - x_in, 2) + std::pow(z_out - z_in, 2));
    
    // Populate SoA arrays directly - Geometry
    segmentIDs.push_back(id);
    x_inlets.push_back(x_in);
    x_outlets.push_back(x_out);
    z_inlets.push_back(z_in);
    z_outlets.push_back(z_out);
    lengths.push_back(length);
    diameters.push_back(dia);
    roughnesses.push_back(rough);
    
    // Initialize state to defaults
    inletPressures.push_back(0.0);
    outletPressures.push_back(0.0);
    inletTemps.push_back(293.15);  // 20°C
    outletTemps.push_back(293.15);
    inletVelocities.push_back(0.0);
    outletVelocities.push_back(0.0);
    
    // Initialize calculated properties to zero (will be populated by solver)
    reynoldsNumbers.push_back(0.0);
    frictionFactors.push_back(0.0);
    densities.push_back(0.0);
    viscosities.push_back(0.0);
    pressureGradients.push_back(0.0);
    
    numSegments++;
    
    #ifdef DEBUG
    std::cout << "Added pipe segment '" << id << "' to pipeline '" 
              << pipelineID << "' (segment " << numSegments << ")" << "\n";
    #endif
    
    return true;
}

size_t Pipeline::getNumberOfSegments() const {
    return numSegments;
}

void Pipeline::setInletConditions(double P, double T, double V) {
    if (numSegments > 0) {
        inletPressures[0] = P;
        inletTemps[0] = T;
        inletVelocities[0] = V;
    }
}

void Pipeline::solveAll(Solver* solver, Fluid* fluid) {
    if (solver == nullptr || fluid == nullptr) {
        std::cerr << "ERROR: Solver or Fluid is null!" << "\n";
        return;
    }
    if (numSegments == 0) {
        std::cerr << "ERROR: Cannot solve empty pipeline!" << "\n";
        return;
    }
    #ifdef DEBUG
    std::cout << "\nSolving pipeline '" << pipelineID 
              << "' with " << numSegments << " segments using SoA storage..." << "\n";
    std::cout << "Using solver: " << solver->getSolverName() << "\n";
    #endif
    
    // PHASE 4: Pure SoA solving - cache optimized!
    for (size_t i = 0; i < numSegments; i++) {
        // Create temporary Pipe from SoA data
        Pipe tempPipe(segmentIDs[i], 
                     x_inlets[i], x_outlets[i],
                     z_inlets[i], z_outlets[i],
                     diameters[i], roughnesses[i]);
        
        // Set inlet from SoA
        tempPipe.setInletConditions(inletPressures[i], inletTemps[i], inletVelocities[i]);
        
        // Use the actual Solver (respects solver type!)
        solver->solve(&tempPipe, fluid);
        
        // Extract results back to SoA
        outletPressures[i] = tempPipe.getOutletPressure();
        outletTemps[i] = tempPipe.getOutletTemp();
        outletVelocities[i] = tempPipe.getOutletVelocity();
        
        // ===================================================================
        // Calculate and store fluid properties for this segment
        // ===================================================================
        
        // Calculate average conditions for the segment
        double avgPressure = (inletPressures[i] + outletPressures[i]) / 2.0;
        double avgTemperature = (inletTemps[i] + outletTemps[i]) / 2.0;
        
        // Get fluid properties at average conditions
        densities[i] = fluid->getDensity(avgPressure, avgTemperature);
        viscosities[i] = fluid->getViscosity(avgPressure, avgTemperature);
        
        // Calculate Reynolds number for this segment
        reynoldsNumbers[i] = tempPipe.getReynoldsNumber(fluid, avgPressure, avgTemperature);
        
        // Calculate friction factor for this segment
        frictionFactors[i] = tempPipe.getFrictionFactor(fluid, avgPressure, avgTemperature);
        
        // Calculate pressure gradient (dP/dx) for this segment
        double pressureDrop = inletPressures[i] - outletPressures[i];
        pressureGradients[i] = pressureDrop / lengths[i]; // Pa/m
        
        // ===================================================================
        
        // Chain outlet to next inlet with proper continuity enforcement
        if (i + 1 < numSegments) {
            // Pressure and temperature propagate directly
            inletPressures[i + 1] = outletPressures[i];
            inletTemps[i + 1] = outletTemps[i];
            
            // ================================================================
            // CRITICAL: Enforce continuity equation when diameter changes
            // ================================================================
            // For incompressible flow: ρ₁A₁V₁ = ρ₂A₂V₂
            // Assuming constant density: A₁V₁ = A₂V₂
            // Therefore: V₂ = V₁ × (A₁/A₂) = V₁ × (D₁/D₂)²
            
            double D_current = diameters[i];
            double D_next = diameters[i + 1];
            
            // Calculate cross-sectional areas
            double A_current = M_PI * D_current * D_current / 4.0;
            double A_next = M_PI * D_next * D_next / 4.0;
            
            // Apply continuity equation
            inletVelocities[i + 1] = outletVelocities[i] * (A_current / A_next);
            
            #ifdef DEBUG
            // Log diameter changes for debugging
            if (std::abs(D_current - D_next) > 1e-6) {
                std::cout << "  Diameter change at segment " << (i+1) << " -> " << (i+2) << ": "
                          << D_current << "m -> " << D_next << "m" << "\n";
                std::cout << "    Velocity adjusted: " << outletVelocities[i] << " m/s -> "
                          << inletVelocities[i + 1] << " m/s (continuity enforced)" << "\n";
            }
            #endif
        }
    }
    #ifdef DEBUG
    std::cout << "Pipeline solved successfully!" << "\n";
    #endif
}

double Pipeline::getTotalPressureDrop() const {
    if (numSegments == 0) return 0.0;
    return inletPressures[0] - outletPressures[numSegments - 1];
}

void Pipeline::displayAll(const Fluid* fluid) const {
    std::cout << "\n=== Pipeline: " << pipelineID << " ===" << "\n";
    std::cout << "Number of segments: " << numSegments << "\n";
    std::cout << "Total pressure drop: " << getTotalPressureDrop() / 1000.0 << " kPa" << "\n";
    
    for (size_t i = 0; i < numSegments; i++) {
        // Create temporary Pipe for display
        Pipe tempPipe(segmentIDs[i],
                     x_inlets[i], x_outlets[i],
                     z_inlets[i], z_outlets[i],
                     diameters[i], roughnesses[i]);
        tempPipe.setInletConditions(inletPressures[i], inletTemps[i], inletVelocities[i]);
        tempPipe.setOutletConditions(outletPressures[i], outletTemps[i], outletVelocities[i]);
        
        std::cout << "\n--- Segment " << (i + 1) << " ---" << "\n";
        tempPipe.displayInfo(fluid);
    }
}

void Pipeline::displaySummary() const {
    std::cout << "\n=== Pipeline Summary: " << pipelineID << " ===" << "\n";
    std::cout << "Segments: " << numSegments << "\n";
    std::cout << "Total Pressure Drop: " << getTotalPressureDrop() / 1000.0 << " kPa" << "\n";
}

void Pipeline::displaySoADebug() const {
    std::cout << "\n=== SoA Arrays (Phase 4: Pure SoA Storage) ===" << "\n";
    std::cout << "Number of segments: " << numSegments << "\n";
    
    for (size_t i = 0; i < numSegments; i++) {
        std::cout << "\nSegment " << (i + 1) << ": " << segmentIDs[i] << "\n";
        std::cout << "  Geometry: x[" << x_inlets[i] << " -> " << x_outlets[i] 
                  << "], z[" << z_inlets[i] << " -> " << z_outlets[i] << "]" << "\n";
        std::cout << "  Length: " << lengths[i] << " m, Diameter: " << diameters[i] << " m" << "\n";
        std::cout << "  Roughness: " << roughnesses[i] << " m" << "\n";
        
        std::cout << "  State: P[" << inletPressures[i]/1000.0 << " -> " 
                  << outletPressures[i]/1000.0 << "] kPa" << "\n";
        std::cout << "  Velocity: [" << inletVelocities[i] << " -> " 
                  << outletVelocities[i] << "] m/s" << "\n";
        std::cout << "  Temperature: [" << inletTemps[i] << " -> " 
                  << outletTemps[i] << "] K" << "\n";
        
        // Display calculated fluid properties
        std::cout << "  Calculated Properties:" << "\n";
        std::cout << "    Reynolds Number: " << reynoldsNumbers[i] << "\n";
        std::cout << "    Friction Factor: " << frictionFactors[i] << "\n";
        std::cout << "    Density: " << densities[i] << " kg/m³" << "\n";
        std::cout << "    Viscosity: " << viscosities[i] << " Pa·s" << "\n";
        std::cout << "    Pressure Gradient: " << pressureGradients[i] << " Pa/m" << "\n";
    }
}
