#include "Pipeline.h"
#include "Solver.h"
#include "Fluid.h"
#include <cmath>

Pipeline::Pipeline(std::string id) : pipelineID(id), numSegments(0) {
    // Pre-allocate space for efficiency
    constexpr size_t RESERVE_SIZE = 100;
    
    // Reserve SoA arrays
    segmentIDs.reserve(RESERVE_SIZE);
    x_inlets.reserve(RESERVE_SIZE);
    x_outlets.reserve(RESERVE_SIZE);
    z_inlets.reserve(RESERVE_SIZE);
    z_outlets.reserve(RESERVE_SIZE);
    lengths.reserve(RESERVE_SIZE);
    diameters.reserve(RESERVE_SIZE);
    roughnesses.reserve(RESERVE_SIZE);
    
    inletPressures.reserve(RESERVE_SIZE);
    outletPressures.reserve(RESERVE_SIZE);
    inletTemps.reserve(RESERVE_SIZE);
    outletTemps.reserve(RESERVE_SIZE);
    inletVelocities.reserve(RESERVE_SIZE);
    outletVelocities.reserve(RESERVE_SIZE);
}

Pipeline::~Pipeline() {
    // PHASE 4: No Pipe pointers to clean up - pure SoA storage
}

bool Pipeline::addPipe(Pipe* pipe) {
    if (pipe == nullptr) {
        std::cerr << "ERROR: Cannot add null pipe to pipeline!" << std::endl;
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
                      << "' to pipeline '" << pipelineID << "'" << std::endl;
            std::cerr << "  Last segment '" << segmentIDs.back() 
                      << "' outlet: x=" << x_outlets.back() 
                      << "m, z=" << z_outlets.back() << "m" << std::endl;
            std::cerr << "  New segment '" << id 
                      << "' inlet: x=" << x_in 
                      << "m, z=" << z_in << "m" << std::endl;
            std::cerr << "  The segments are not connected!" << std::endl;
            return false;
        }
    }
    
    // Calculate length
    double length = std::sqrt(std::pow(x_out - x_in, 2) + std::pow(z_out - z_in, 2));
    
    // Populate SoA arrays directly
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
    
    numSegments++;
    
    std::cout << "Added pipe segment '" << id << "' to pipeline '" 
              << pipelineID << "' (segment " << numSegments << ")" << std::endl;
    
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
    if (numSegments == 0) {
        std::cerr << "ERROR: Cannot solve empty pipeline!" << std::endl;
        return;
    }
    
    std::cout << "\nSolving pipeline '" << pipelineID 
              << "' with " << numSegments << " segments using SoA storage..." << std::endl;
    
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
        
        // Chain outlet to next inlet (excellent cache locality!)
        if (i + 1 < numSegments) {
            inletPressures[i + 1] = outletPressures[i];
            inletTemps[i + 1] = outletTemps[i];
            inletVelocities[i + 1] = outletVelocities[i];
        }
    }
    
    std::cout << "Pipeline solved successfully!" << std::endl;
}

double Pipeline::getTotalPressureDrop() const {
    if (numSegments == 0) return 0.0;
    return inletPressures[0] - outletPressures[numSegments - 1];
}

void Pipeline::displayAll(const Fluid* fluid) const {
    std::cout << "\n=== Pipeline: " << pipelineID << " ===" << std::endl;
    std::cout << "Number of segments: " << numSegments << std::endl;
    std::cout << "Total pressure drop: " << getTotalPressureDrop() / 1000.0 << " kPa" << std::endl;
    
    for (size_t i = 0; i < numSegments; i++) {
        // Create temporary Pipe for display
        Pipe tempPipe(segmentIDs[i],
                     x_inlets[i], x_outlets[i],
                     z_inlets[i], z_outlets[i],
                     diameters[i], roughnesses[i]);
        tempPipe.setInletConditions(inletPressures[i], inletTemps[i], inletVelocities[i]);
        tempPipe.setOutletConditions(outletPressures[i], outletTemps[i], outletVelocities[i]);
        
        std::cout << "\n--- Segment " << (i + 1) << " ---" << std::endl;
        tempPipe.displayInfo(fluid);
    }
}

void Pipeline::displaySummary() const {
    std::cout << "\n=== Pipeline Summary: " << pipelineID << " ===" << std::endl;
    std::cout << "Segments: " << numSegments << std::endl;
    std::cout << "Total Pressure Drop: " << getTotalPressureDrop() / 1000.0 << " kPa" << std::endl;
}

void Pipeline::displaySoADebug() const {
    std::cout << "\n=== SoA Arrays (Phase 4: Pure SoA Storage) ===" << std::endl;
    std::cout << "Number of segments: " << numSegments << std::endl;
    
    for (size_t i = 0; i < numSegments; i++) {
        std::cout << "\nSegment " << (i + 1) << ": " << segmentIDs[i] << std::endl;
        std::cout << "  Geometry: x[" << x_inlets[i] << " -> " << x_outlets[i] 
                  << "], z[" << z_inlets[i] << " -> " << z_outlets[i] << "]" << std::endl;
        std::cout << "  Length: " << lengths[i] << " m, Diameter: " << diameters[i] << " m" << std::endl;
        std::cout << "  State: P[" << inletPressures[i]/1000.0 << " -> " 
                  << outletPressures[i]/1000.0 << "] kPa" << std::endl;
    }
}
