#include <iostream>
#include <memory>
#include "Liquid.h"
#include "Gas.h"
#include "Pipe.h"
#include "Pipeline.h"
#include "SimpleSolver.h"
#include "IterativeSolver.h"
#include "AdaptiveSolver.h"

int main() {
    std::cout << "=== LIQUID FLOW EXAMPLE (WITH ELEVATION) ===" << std::endl;
    
    auto water = std::make_unique<Liquid>("Water", 1000.0, 0.001, 
                                          293.15, 101325.0,
                                          4.5e-10, 2.07e-4);
    
    Pipe pipe1("Pipe-001", 0.0, 100.0, 0.0, 20.0, 0.3, 0.000045);
    pipe1.setInletConditions(500000.0, 293.15, 2.0);
    
    std::cout << "\nFluid Properties at Inlet:" << std::endl;
    water->displayProperties(500000.0, 293.15);
    std::cout << std::endl;
    
    auto simpleSolver = std::make_unique<SimpleSolver>();
    auto iterativeSolver = std::make_unique<IterativeSolver>(20, 1.0);
    auto adaptiveSolver = std::make_unique<AdaptiveSolver>(50000.0, 20, 1.0, 10, false);
    
    std::cout << "--- Using Iterative Solver (Single Pipe) ---" << std::endl;
    iterativeSolver->solve(&pipe1, water.get());  // Correct: solver operates on pipe
    pipe1.displayInfo(water.get());
    
    std::cout << "\n=== PIPELINE EXAMPLE ===" << std::endl;
    
    // Method 1: Create a pipeline using Pipe objects
    Pipeline mainPipeline("MainLine-A");
    
    // Create pipe objects
    Pipe seg1("Segment-1", 0.0, 500.0, 0.0, 10.0, 0.3, 0.000045);
    Pipe seg2("Segment-2", 500.0, 1200.0, 10.0, 25.0, 0.3, 0.000045);
    Pipe seg3("Segment-3", 1200.0, 2000.0, 25.0, 30.0, 0.25, 0.000045);
    
    // Add segments
    mainPipeline.addPipe(&seg1);
    mainPipeline.addPipe(&seg2);
    mainPipeline.addPipe(&seg3);
    
    // Set inlet conditions
    mainPipeline.setInletConditions(1000000.0, 293.15, 2.0);
    
    // Solve entire pipeline - you can use any solver!
    std::cout << "Using: " << adaptiveSolver->getSolverName() << std::endl;
    mainPipeline.solveAll(adaptiveSolver.get(), water.get());
    
    // Display results
    mainPipeline.displaySummary();
    
    // PHASE 2: Verify SoA arrays are populated correctly
    std::cout << "\n=== PHASE 2 VERIFICATION (Method 1: Pipe Objects) ===" << std::endl;
    mainPipeline.displaySoADebug();
    
    std::cout << "\n=== PIPELINE EXAMPLE (Method 2: Direct SoA) ===" << std::endl;
    
    // Method 2: Add segments directly without creating Pipe objects
    Pipeline directPipeline("DirectLine-B");
    
    // Add segments directly - more efficient for pipeline-only usage
    directPipeline.addPipeSegment("DirectSeg-1", 0.0, 500.0, 0.0, 10.0, 0.3, 0.000045);
    directPipeline.addPipeSegment("DirectSeg-2", 500.0, 1200.0, 10.0, 25.0, 0.3, 0.000045);
    directPipeline.addPipeSegment("DirectSeg-3", 1200.0, 2000.0, 25.0, 30.0, 0.25, 0.000045);
    
    // Set inlet conditions directly
    directPipeline.setInletConditions(1000000.0, 293.15, 2.0);
    
    // PHASE 4: Can use ANY solver type with SoA storage!
    std::cout << "Using: " << iterativeSolver->getSolverName() << std::endl;
    directPipeline.solveAll(iterativeSolver.get(), water.get());
    
    // Display results
    directPipeline.displaySummary();
    
    std::cout << "\n=== PHASE 4 VERIFICATION (Method 2: Direct SoA Solved!) ===" << std::endl;
    directPipeline.displaySoADebug();
    
    // Compare results between methods
    std::cout << "\n=== COMPARING BOTH METHODS ===" << std::endl;
    std::cout << "Method 1 (addPipe):        ΔP = " 
              << mainPipeline.getTotalPressureDrop() / 1000.0 << " kPa" << std::endl;
    std::cout << "Method 2 (addPipeSegment): ΔP = " 
              << directPipeline.getTotalPressureDrop() / 1000.0 << " kPa" << std::endl;
    std::cout << "Both use pure SoA storage internally!" << std::endl;
    
    std::cout << "\n=== TESTING CONNECTIVITY VALIDATION ===" << std::endl;
    Pipeline testPipeline("Test-Pipeline");
    Pipe conn1("Connected-1", 0.0, 100.0, 0.0, 5.0, 0.3, 0.000045);
    Pipe conn2("Connected-2", 100.0, 200.0, 5.0, 10.0, 0.3, 0.000045);
    Pipe disconn("Disconnected", 300.0, 400.0, 20.0, 25.0, 0.3, 0.000045);
    
    testPipeline.addPipe(&conn1);
    testPipeline.addPipe(&conn2);
    std::cout << "\nAttempting to add disconnected pipe (should fail):" << std::endl;
    testPipeline.addPipe(&disconn);
    
    std::cout << "\n=== GAS FLOW EXAMPLE (WITH ELEVATION) ===" << std::endl;
    
    // Gas constructor: Gas(name, molecularWeight, viscosity)
    // Natural gas: MW ≈ 16-20 kg/kmol, using 16.04 for methane
    auto naturalGas = std::make_unique<Gas>("Natural Gas", 16.04, 0.000011);
    
    Pipe pipe2("Pipe-002", 0.0, 100.0, 0.0, 20.0, 0.3, 0.000045);
    pipe2.setInletConditions(500000.0, 293.15, 5.0);
    
    std::cout << "\nFluid Properties at Inlet:" << std::endl;
    naturalGas->displayProperties(500000.0, 293.15);
    std::cout << std::endl;
    
    auto adaptiveSolverGas = std::make_unique<AdaptiveSolver>(50000.0, 20, 1.0, 15, true);
    std::cout << "--- Using Adaptive Solver ---" << std::endl;
    adaptiveSolverGas->solve(&pipe2, naturalGas.get());  // Correct pattern
    pipe2.displayInfo(naturalGas.get());
    
    std::cout << "\n=== REASONABLE LONG PIPE EXAMPLE ===" << std::endl;
    
    Pipe pipe3("Pipe-003", 0.0, 1000.0, 0.0, 20.0, 0.3, 0.000045);
    pipe3.setInletConditions(1000000.0, 293.15, 2.0);
    
    std::cout << "\nLong pipe (1000m, 20m elevation, 1000 kPa inlet):" << std::endl;
    auto adaptiveSolverReasonable = std::make_unique<AdaptiveSolver>(50000.0, 20, 1.0, 15, true);
    adaptiveSolverReasonable->solve(&pipe3, water.get());
    pipe3.displayInfo(water.get());
    
    return 0;
}
