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
    std::cout << "=== PIPELINE SIMULATOR ===" << std::endl;
    std::cout << "Testing sequential pipeline functionality\n" << std::endl;
    
    // Create fluid
    auto water = std::make_unique<Liquid>("Water", 1000.0, 0.001, 
                                          293.15, 101325.0,
                                          4.5e-10, 2.07e-4);
    
    // Create solver
    auto iterativeSolver = std::make_unique<IterativeSolver>(20, 1.0);
    
    std::cout << "\n=== EXAMPLE 1: THREE-SEGMENT PIPELINE ===" << std::endl;
    
    // Create three pipe segments
    // Segment 1: 0→500m, elevation 0→10m
    Pipe pipe1("Segment-1", 0.0, 500.0, 0.0, 10.0, 0.3, 0.000045);
    
    // Segment 2: 500→1200m, elevation 10→25m  
    Pipe pipe2("Segment-2", 500.0, 1200.0, 10.0, 25.0, 0.3, 0.000045);
    
    // Segment 3: 1200→2000m, elevation 25→15m (downhill)
    Pipe pipe3("Segment-3", 1200.0, 2000.0, 25.0, 15.0, 0.3, 0.000045);
    
    // Set inlet conditions for first segment
    pipe1.setInletConditions(1000000.0, 293.15, 2.5);
    
    // Create pipeline and add segments
    Pipeline network("MainLine-A");
    network.addPipe(&pipe1);
    network.addPipe(&pipe2);
    network.addPipe(&pipe3);
    
    // Solve the entire pipeline
    network.solveSequential(iterativeSolver.get(), water.get());
    
    // Display summary
    network.displaySummary(water.get());
    
    // Display detailed results for each segment
    network.displayResults(water.get());
    
    std::cout << "\n=== EXAMPLE 2: LONG PIPELINE WITH MULTIPLE SEGMENTS ===" << std::endl;
    
    // Create a longer pipeline with 5 segments
    Pipe seg1("Seg-1", 0.0, 1000.0, 0.0, 5.0, 0.4, 0.000045);
    Pipe seg2("Seg-2", 1000.0, 2000.0, 5.0, 15.0, 0.4, 0.000045);
    Pipe seg3("Seg-3", 2000.0, 3000.0, 15.0, 30.0, 0.35, 0.000045);
    Pipe seg4("Seg-4", 3000.0, 4000.0, 30.0, 35.0, 0.35, 0.000045);
    Pipe seg5("Seg-5", 4000.0, 5000.0, 35.0, 20.0, 0.3, 0.000045);
    
    // Set inlet conditions
    seg1.setInletConditions(1500000.0, 293.15, 3.0);
    
    // Create new pipeline
    Pipeline longPipeline("MainLine-B");
    longPipeline.addPipe(&seg1);
    longPipeline.addPipe(&seg2);
    longPipeline.addPipe(&seg3);
    longPipeline.addPipe(&seg4);
    longPipeline.addPipe(&seg5);
    
    // Solve
    longPipeline.solveSequential(iterativeSolver.get(), water.get());
    
    // Display only summary
    longPipeline.displaySummary(water.get());
    
    std::cout << "\n=== EXAMPLE 3: GAS PIPELINE ===" << std::endl;
    
    auto naturalGas = std::make_unique<Gas>("Natural Gas", 16.04, 1.1e-5);
    
    // Gas pipeline segments
    Pipe gasPipe1("Gas-Seg-1", 0.0, 2000.0, 0.0, 20.0, 0.5, 0.000045);
    Pipe gasPipe2("Gas-Seg-2", 2000.0, 4000.0, 20.0, 40.0, 0.5, 0.000045);
    Pipe gasPipe3("Gas-Seg-3", 4000.0, 6000.0, 40.0, 30.0, 0.45, 0.000045);
    
    gasPipe1.setInletConditions(5000000.0, 293.15, 15.0);
    
    Pipeline gasPipeline("Gas-MainLine");
    gasPipeline.addPipe(&gasPipe1);
    gasPipeline.addPipe(&gasPipe2);
    gasPipeline.addPipe(&gasPipe3);
    
    // Use adaptive solver for gas pipeline
    auto adaptiveSolver = std::make_unique<AdaptiveSolver>(100000.0, 20, 1.0, 10, false);
    gasPipeline.solveSequential(adaptiveSolver.get(), naturalGas.get());
    
    gasPipeline.displaySummary(naturalGas.get());
    
    std::cout << "\n=== SIMULATION COMPLETE ===" << std::endl;
    
    return 0;
}
