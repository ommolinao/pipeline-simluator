#include "Pipeline.h"
#include "Solver.h"
#include "Fluid.h"
#include <cmath>

Pipeline::Pipeline(std::string id) : pipelineID(id) {}

void Pipeline::addPipe(Pipe* pipe) {
    if (pipe == nullptr) {
        std::cerr << "Warning: Attempted to add null pipe to pipeline " << pipelineID << std::endl;
        return;
    }
    
    // If pipeline is not empty, validate connectivity
    if (!pipes.empty()) {
        Pipe* lastPipe = pipes.back();
        
        // Check if outlet position of last pipe matches inlet position of new pipe
        double lastOutletX = lastPipe->getOutletPosition();
        double lastOutletZ = lastPipe->getOutletElevation();
        double newInletX = pipe->getInletPosition();
        double newInletZ = pipe->getInletElevation();
        
        // Use small tolerance for floating point comparison
        const double tolerance = 1e-6;
        
        bool positionMatch = (fabs(lastOutletX - newInletX) < tolerance);
        bool elevationMatch = (fabs(lastOutletZ - newInletZ) < tolerance);
        
        if (!positionMatch || !elevationMatch) {
            std::cerr << "ERROR: Cannot add pipe '" << pipe->getPipeID() 
                      << "' to pipeline '" << pipelineID << "'" << std::endl;
            std::cerr << "  Last pipe '" << lastPipe->getPipeID() << "' outlet: "
                      << "x=" << lastOutletX << "m, z=" << lastOutletZ << "m" << std::endl;
            std::cerr << "  New pipe '" << pipe->getPipeID() << "' inlet: "
                      << "x=" << newInletX << "m, z=" << newInletZ << "m" << std::endl;
            std::cerr << "  The pipes are not connected!" << std::endl;
            return;
        }
    }
    
    // Validation passed, add the pipe
    pipes.push_back(pipe);
    std::cout << "Added pipe '" << pipe->getPipeID() << "' to pipeline '" 
              << pipelineID << "' (segment " << pipes.size() << ")" << std::endl;
}

size_t Pipeline::getNumberOfPipes() const {
    return pipes.size();
}

Pipe* Pipeline::getPipe(size_t index) const {
    if (index < pipes.size()) {
        return pipes[index];
    }
    return nullptr;
}

void Pipeline::solveSequential(Solver* solver, const Fluid* fluid) {
    if (pipes.empty()) {
        std::cerr << "Error: Cannot solve empty pipeline " << pipelineID << std::endl;
        return;
    }
    
    if (solver == nullptr || fluid == nullptr) {
        std::cerr << "Error: Solver or fluid is null" << std::endl;
        return;
    }
    
    std::cout << "Solving pipeline: " << pipelineID << " (" << pipes.size() << " segments)" << std::endl;
    
    // Solve each pipe in sequence
    for (size_t i = 0; i < pipes.size(); i++) {
        std::cout << "  Solving segment " << (i+1) << "/" << pipes.size() 
                  << ": " << pipes[i]->getPipeID() << std::endl;
        
        // Solve current pipe
        solver->solve(pipes[i], fluid);
        
        // Chain to next pipe: outlet of pipe i → inlet of pipe i+1
        if (i < pipes.size() - 1) {
            pipes[i+1]->setInletConditions(
                pipes[i]->getOutletPressure(),
                pipes[i]->getOutletTemp(),
                pipes[i]->getOutletVelocity()
            );
        }
    }
    
    std::cout << "Pipeline solution complete." << std::endl;
}

void Pipeline::displayResults(const Fluid* fluid) const {
    std::cout << "\n========================================" << std::endl;
    std::cout << "PIPELINE RESULTS: " << pipelineID << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    for (size_t i = 0; i < pipes.size(); i++) {
        pipes[i]->displayInfo(fluid);
    }
}

double Pipeline::getTotalLength() const {
    double totalLength = 0.0;
    for (const auto& pipe : pipes) {
        totalLength += pipe->getLength();
    }
    return totalLength;
}

double Pipeline::getTotalElevationChange() const {
    if (pipes.empty()) return 0.0;
    
    // Total elevation change = last outlet elevation - first inlet elevation
    double firstInletElevation = pipes.front()->getInletElevation();
    double lastOutletElevation = pipes.back()->getOutletElevation();
    
    return lastOutletElevation - firstInletElevation;
}

double Pipeline::getTotalPressureDrop() const {
    if (pipes.empty()) return 0.0;
    
    // Total pressure drop = first inlet pressure - last outlet pressure
    double firstInletPressure = pipes.front()->getInletPressure();
    double lastOutletPressure = pipes.back()->getOutletPressure();
    
    return firstInletPressure - lastOutletPressure;
}

void Pipeline::displaySummary(const Fluid* fluid) const {
    const int boxWidth = 60;
    std::string title = "PIPELINE SUMMARY: " + pipelineID;
    
    std::cout << "\n╔";
    for (int i = 0; i < boxWidth; i++) std::cout << "═";
    std::cout << "╗" << std::endl;
    
    // Center the title
    int totalPadding = boxWidth - title.length();
    int leftPad = totalPadding / 2;
    int rightPad = totalPadding - leftPad;
    
    std::cout << "║";
    for (int i = 0; i < leftPad; i++) std::cout << " ";
    std::cout << title;
    for (int i = 0; i < rightPad; i++) std::cout << " ";
    std::cout << "║" << std::endl;
    
    std::cout << "╚";
    for (int i = 0; i < boxWidth; i++) std::cout << "═";
    std::cout << "╝" << std::endl;
    
    if (pipes.empty()) {
        std::cout << "Pipeline is empty." << std::endl;
        return;
    }
    
    // Get first and last pipe info
    Pipe* firstPipe = pipes.front();
    Pipe* lastPipe = pipes.back();
    
    std::cout << "\nPipeline Configuration:" << std::endl;
    std::cout << "  Number of segments: " << pipes.size() << std::endl;
    std::cout << "  Total length: " << getTotalLength() << " m" << std::endl;
    std::cout << "  Total elevation change: " << getTotalElevationChange() << " m" << std::endl;
    
    std::cout << "\nInlet Conditions (Segment 1):" << std::endl;
    std::cout << "  Pressure: " << firstPipe->getInletPressure()/1000.0 << " kPa" << std::endl;
    std::cout << "  Temperature: " << firstPipe->getInletTemp() << " K" << std::endl;
    std::cout << "  Velocity: " << firstPipe->getInletVelocity() << " m/s" << std::endl;
    std::cout << "  Density: " << fluid->getDensity(firstPipe->getInletPressure(), 
                                                     firstPipe->getInletTemp()) << " kg/m³" << std::endl;
    
    std::cout << "\nOutlet Conditions (Segment " << pipes.size() << "):" << std::endl;
    std::cout << "  Pressure: " << lastPipe->getOutletPressure()/1000.0 << " kPa" << std::endl;
    std::cout << "  Temperature: " << lastPipe->getOutletTemp() << " K" << std::endl;
    std::cout << "  Velocity: " << lastPipe->getOutletVelocity() << " m/s" << std::endl;
    std::cout << "  Density: " << fluid->getDensity(lastPipe->getOutletPressure(), 
                                                     lastPipe->getOutletTemp()) << " kg/m³" << std::endl;
    
    std::cout << "\nOverall Performance:" << std::endl;
    std::cout << "  Total pressure drop: " << getTotalPressureDrop()/1000.0 << " kPa" << std::endl;
    std::cout << "  Pressure ratio: " << lastPipe->getOutletPressure()/firstPipe->getInletPressure() << std::endl;
    
    // Calculate average pressure drop per meter
    double avgPressureDropPerMeter = getTotalPressureDrop() / getTotalLength();
    std::cout << "  Avg. pressure drop per meter: " << avgPressureDropPerMeter << " Pa/m" << std::endl;
    
    std::cout << "\n════════════════════════════════════════════════════════════\n" << std::endl;
}

void Pipeline::clear() {
    pipes.clear();
}

std::string Pipeline::getPipelineID() const {
    return pipelineID;
}
