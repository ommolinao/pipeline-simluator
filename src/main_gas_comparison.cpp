// Comprehensive gas pipeline solver comparison
// Compares SimpleSolver, IterativeSolver, AdaptiveSolver, and RK4Solver
#include "Pipeline.h"
#include "Gas.h"
#include "SimpleSolver.h"
#include "IterativeSolver.h"
#include "AdaptiveSolver.h"
#include "RK4Solver.h"
#include "Visualizer.h"
#include <iostream>
#include <chrono>
#include <iomanip>

#define P_STANDARD 101325.0  // Standard atmospheric pressure in Pa
#define T_STANDARD 293.15    // Standard temperature in K (20°C)

int main() {
    std::cout << "=== Gas Pipeline Solver Comparison ===\n";
    std::cout << "Testing 4 solvers on a natural gas transmission pipeline\n\n";
    
    // Create natural gas (highly compressible!)
    // Typical natural gas properties at standard conditions
    Gas naturalGas("Natural Gas", 0.717, 1.08e-5);  // Lower density than air, low viscosity
    
    std::cout << "Fluid: Natural Gas\n";
    std::cout << "  Base density: " << naturalGas.getDensity(P_STANDARD, T_STANDARD) << " kg/m³\n";
    std::cout << "  Base viscosity: " << naturalGas.getViscosity(P_STANDARD, T_STANDARD) << " Pa·s\n\n";
    
    // Create four identical pipelines for comparison
    Pipeline pipelineSimple("SimpleSolver-Pipeline");
    Pipeline pipelineIterative("IterativeSolver-Pipeline");
    Pipeline pipelineAdaptive("AdaptiveSolver-Pipeline");
    Pipeline pipelineRK4("RK4Solver-Pipeline");
    
    // Build a realistic gas transmission pipeline
    // Long distances, high pressures, significant elevation changes
    std::cout << "Building gas transmission pipeline (10 segments, 500m each)...\n";
    
    for (int i = 0; i < 10; i++) {
        double x_in = i * 500.0;
        double x_out = (i + 1) * 500.0;
        
        // Varying terrain - hills and valleys
        double z_in, z_out;
        if (i < 3) {
            // Uphill to mountain pass
            z_in = i * 50.0;
            z_out = (i + 1) * 50.0;
        } else if (i < 6) {
            // Crossing mountain plateau
            z_in = 150.0 - (i - 3) * 20.0;
            z_out = 150.0 - (i - 2) * 20.0;
        } else {
            // Descending to valley
            z_in = 90.0 - (i - 6) * 30.0;
            z_out = 90.0 - (i - 5) * 30.0;
        }
        
        // Large diameter gas pipeline
        double diameter = 0.6;  // 600mm typical for transmission
        double roughness = 0.000045;  // Smooth steel
        
        std::string pipeID = "Segment-" + std::to_string(i + 1);
        
        pipelineSimple.addPipeSegment(pipeID, x_in, x_out, z_in, z_out, diameter, roughness);
        pipelineIterative.addPipeSegment(pipeID, x_in, x_out, z_in, z_out, diameter, roughness);
        pipelineAdaptive.addPipeSegment(pipeID, x_in, x_out, z_in, z_out, diameter, roughness);
        pipelineRK4.addPipeSegment(pipeID, x_in, x_out, z_in, z_out, diameter, roughness);
    }
    
    // High pressure gas transmission conditions
    double inletPressure = 7000000.0;    // 7 MPa (70 bar) - typical transmission pressure
    double inletTemp = 288.15;           // 15°C
    double inletVelocity = 10.0;         // 10 m/s - typical gas velocity
    
    std::cout << "\nInlet conditions:\n";
    std::cout << "  Pressure: " << inletPressure / 1e6 << " MPa\n";
    std::cout << "  Temperature: " << inletTemp << " K (" << (inletTemp - 273.15) << "°C)\n";
    std::cout << "  Velocity: " << inletVelocity << " m/s\n\n";
    
    pipelineSimple.setInletConditions(inletPressure, inletTemp, inletVelocity);
    pipelineIterative.setInletConditions(inletPressure, inletTemp, inletVelocity);
    pipelineAdaptive.setInletConditions(inletPressure, inletTemp, inletVelocity);
    pipelineRK4.setInletConditions(inletPressure, inletTemp, inletVelocity);
    
    // Create solvers with appropriate settings
    SimpleSolver simpleSolver;
    IterativeSolver iterativeSolver(50, 0.1);      // 50 iterations, 0.1 Pa tolerance
    AdaptiveSolver adaptiveSolver(50, 0.1);        // 50 iterations, 0.1 Pa tolerance
    RK4Solver rk4Solver(200);                      // 200 integration steps per segment
    
    std::cout << "Solver configurations:\n";
    std::cout << "  SimpleSolver: Single-pass calculation\n";
    std::cout << "  IterativeSolver: " << iterativeSolver.getMaxIterations() << " max iterations\n";
    std::cout << "  AdaptiveSolver: " << adaptiveSolver.getMaxIterations() << " max iterations\n";
    std::cout << "  RK4Solver: " << rk4Solver.getNumSteps() << " integration steps per segment\n\n";
    
    // Solve with each solver and measure performance
    std::cout << "=== Solving Pipelines ===\n\n";
    
    // SimpleSolver
    std::cout << "1. SimpleSolver...\n";
    auto start1 = std::chrono::high_resolution_clock::now();
    pipelineSimple.solveAll(&simpleSolver, &naturalGas);
    auto end1 = std::chrono::high_resolution_clock::now();
    auto duration1 = std::chrono::duration_cast<std::chrono::microseconds>(end1 - start1);
    
    // IterativeSolver
    std::cout << "\n2. IterativeSolver...\n";
    auto start2 = std::chrono::high_resolution_clock::now();
    pipelineIterative.solveAll(&iterativeSolver, &naturalGas);
    auto end2 = std::chrono::high_resolution_clock::now();
    auto duration2 = std::chrono::duration_cast<std::chrono::microseconds>(end2 - start2);
    
    // AdaptiveSolver
    std::cout << "\n3. AdaptiveSolver...\n";
    auto start3 = std::chrono::high_resolution_clock::now();
    pipelineAdaptive.solveAll(&adaptiveSolver, &naturalGas);
    auto end3 = std::chrono::high_resolution_clock::now();
    auto duration3 = std::chrono::duration_cast<std::chrono::microseconds>(end3 - start3);
    
    // RK4Solver
    std::cout << "\n4. RK4Solver...\n";
    auto start4 = std::chrono::high_resolution_clock::now();
    pipelineRK4.solveAll(&rk4Solver, &naturalGas);
    auto end4 = std::chrono::high_resolution_clock::now();
    auto duration4 = std::chrono::duration_cast<std::chrono::microseconds>(end4 - start4);
    
    // Performance comparison
    std::cout << "\n=== Performance Comparison ===\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "\nComputation Time:\n";
    std::cout << "  SimpleSolver:    " << std::setw(8) << duration1.count() << " μs (baseline)\n";
    std::cout << "  IterativeSolver: " << std::setw(8) << duration2.count() << " μs (" 
              << (double)duration2.count() / duration1.count() << "x)\n";
    std::cout << "  AdaptiveSolver:  " << std::setw(8) << duration3.count() << " μs (" 
              << (double)duration3.count() / duration1.count() << "x)\n";
    std::cout << "  RK4Solver:       " << std::setw(8) << duration4.count() << " μs (" 
              << (double)duration4.count() / duration1.count() << "x)\n";
    
    // Results comparison
    std::cout << "\n=== Results Comparison ===\n";
    std::cout << std::setprecision(4);
    
    double pDrop1 = pipelineSimple.getTotalPressureDrop();
    double pDrop2 = pipelineIterative.getTotalPressureDrop();
    double pDrop3 = pipelineAdaptive.getTotalPressureDrop();
    double pDrop4 = pipelineRK4.getTotalPressureDrop();
    
    std::cout << "\nTotal Pressure Drop:\n";
    std::cout << "  SimpleSolver:    " << pDrop1 / 1e6 << " MPa\n";
    std::cout << "  IterativeSolver: " << pDrop2 / 1e6 << " MPa (Δ = " 
              << std::abs(pDrop2 - pDrop1) / 1e3 << " kPa)\n";
    std::cout << "  AdaptiveSolver:  " << pDrop3 / 1e6 << " MPa (Δ = " 
              << std::abs(pDrop3 - pDrop1) / 1e3 << " kPa)\n";
    std::cout << "  RK4Solver:       " << pDrop4 / 1e6 << " MPa (Δ = " 
              << std::abs(pDrop4 - pDrop1) / 1e3 << " kPa)\n";
    
    // Generate visualizations
    std::cout << "\n=== Generating Visualizations ===\n";
    
    Visualizer viz1("gas_pipeline_simple.html");
    if (viz1.generate(pipelineSimple, naturalGas, "Gas Pipeline - SimpleSolver")) {
        std::cout << "✓ SimpleSolver:    gas_pipeline_simple.html\n";
    }
    
    Visualizer viz2("gas_pipeline_iterative.html");
    if (viz2.generate(pipelineIterative, naturalGas, "Gas Pipeline - IterativeSolver")) {
        std::cout << "✓ IterativeSolver: gas_pipeline_iterative.html\n";
    }
    
    Visualizer viz3("gas_pipeline_adaptive.html");
    if (viz3.generate(pipelineAdaptive, naturalGas, "Gas Pipeline - AdaptiveSolver")) {
        std::cout << "✓ AdaptiveSolver:  gas_pipeline_adaptive.html\n";
    }
    
    Visualizer viz4("gas_pipeline_rk4.html");
    if (viz4.generate(pipelineRK4, naturalGas, "Gas Pipeline - RK4Solver")) {
        std::cout << "✓ RK4Solver:       gas_pipeline_rk4.html\n";
    }
    
    std::cout << "\n=== Analysis Summary ===\n";
    std::cout << "\nFor gas pipelines with high compressibility:\n\n";
    std::cout << "SimpleSolver:\n";
    std::cout << "  + Fastest computation\n";
    std::cout << "  - Poor accuracy for compressible flow\n";
    std::cout << "  - Doesn't capture density variations well\n\n";
    
    std::cout << "IterativeSolver:\n";
    std::cout << "  + Good accuracy through iteration\n";
    std::cout << "  + Handles pressure-dependent properties\n";
    std::cout << "  ~ Moderate computational cost\n\n";
    
    std::cout << "AdaptiveSolver:\n";
    std::cout << "  + Excellent accuracy with adaptive refinement\n";
    std::cout << "  + Adjusts to varying conditions\n";
    std::cout << "  ~ Higher computational cost\n\n";
    
    std::cout << "RK4Solver:\n";
    std::cout << "  + Highest accuracy (4th order)\n";
    std::cout << "  + Smooth integration of all effects\n";
    std::cout << "  + Best for validation/verification\n";
    std::cout << "  - Highest computational cost\n\n";
    
    std::cout << "Key Differences to Observe:\n";
    std::cout << "  1. Pressure profiles - RK4 shows smoothest curves\n";
    std::cout << "  2. Velocity variations - Compressibility causes velocity changes\n";
    std::cout << "  3. Density variations - Gas density drops as pressure drops\n";
    std::cout << "  4. Reynolds number changes - Varies with density\n\n";
    
    std::cout << "Open the HTML files to compare the solver results visually!\n";
    
    return 0;
}
