// Example comparing SimpleSolver vs RK4Solver
#include <iostream>
#include <chrono>

#include "Pipeline.h"
#include "Liquid.h"
#include "SimpleSolver.h"
#include "RK4Solver.h"
#include "Visualizer.h"

int main() {
    // Create fluid (water)
    Liquid water("Water", 1000.0, 0.001);
    
    // Create two identical pipelines for comparison
    Pipeline pipelineSimple("Simple-Solver-Pipeline");
    Pipeline pipelineRK4("RK4-Solver-Pipeline");
    
    // Add identical segments to both pipelines
    std::cout << "Building pipelines...\n";
    
    // Complex pipeline with significant elevation changes
    for (int i = 0; i < 20; i++) {
        double x_in = i * 50.0;
        double x_out = (i + 1) * 50.0;
        
        // Varying elevation profile
        double z_in, z_out;
        if (i < 5) {
            // Uphill
            z_in = i * 5.0;
            z_out = (i + 1) * 5.0;
        } else if (i < 10) {
            // Plateau
            z_in = 25.0;
            z_out = 25.0;
        } else if (i < 15) {
            // Downhill
            z_in = 25.0 - (i - 10) * 5.0;
            z_out = 25.0 - (i - 9) * 5.0;
        } else {
            // Valley and climb
            z_in = (i - 15) * 2.0;
            z_out = (i - 14) * 2.0;
        }
        
        // Varying diameter
        double diameter = (i >= 5 && i < 10) ? 0.2 : 0.25;
        
        std::string pipeID = "Pipe-" + std::to_string(i + 1);
        
        pipelineSimple.addPipeSegment(pipeID, x_in, x_out, z_in, z_out, 
                                      diameter, 0.00015);
        pipelineRK4.addPipeSegment(pipeID, x_in, x_out, z_in, z_out, 
                                   diameter, 0.00015);
    }
    
    // Set identical inlet conditions
    double inletPressure = 2000000.0;  // 2 MPa
    double inletTemp = 293.15;         // 20°C
    double inletVelocity = 2.0;        // 2 m/s
    
    pipelineSimple.setInletConditions(inletPressure, inletTemp, inletVelocity);
    pipelineRK4.setInletConditions(inletPressure, inletTemp, inletVelocity);
    
    // Create solvers
    SimpleSolver simpleSolver;
    RK4Solver rk4Solver(100);  // 100 integration steps per segment
    
    // Solve with SimpleSolver
    std::cout << "\n=== Solving with SimpleSolver ===\n";
    auto start1 = std::chrono::high_resolution_clock::now();
    pipelineSimple.solveAll(&simpleSolver, &water);
    auto end1 = std::chrono::high_resolution_clock::now();
    auto duration1 = std::chrono::duration_cast<std::chrono::microseconds>(end1 - start1);
    
    // Solve with RK4Solver
    std::cout << "\n=== Solving with RK4Solver ===\n";
    auto start2 = std::chrono::high_resolution_clock::now();
    pipelineRK4.solveAll(&rk4Solver, &water);
    auto end2 = std::chrono::high_resolution_clock::now();
    auto duration2 = std::chrono::duration_cast<std::chrono::microseconds>(end2 - start2);
    
    // Compare results
    std::cout << "\n=== Comparison Results ===\n";
    std::cout << "SimpleSolver computation time: " << duration1.count() << " μs\n";
    std::cout << "RK4Solver computation time: " << duration2.count() << " μs\n";
    std::cout << "RK4 overhead factor: " << static_cast<double>(duration2.count()) / duration1.count() << "x\n";
    
    std::cout << "\nPressure drops:\n";
    std::cout << "  SimpleSolver: " << pipelineSimple.getTotalPressureDrop() / 1000.0 << " kPa\n";
    std::cout << "  RK4Solver: " << pipelineRK4.getTotalPressureDrop() / 1000.0 << " kPa\n";
    std::cout << "  Difference: " << std::abs(pipelineSimple.getTotalPressureDrop() - 
                                               pipelineRK4.getTotalPressureDrop()) / 1000.0 << " kPa\n";
    
    // Display summaries
    std::cout << "\n";
    pipelineSimple.displaySummary();
    std::cout << "\n";
    pipelineRK4.displaySummary();
    
    // Generate visualizations for both
    std::cout << "\n=== Generating Visualizations ===\n";
    
    Visualizer vizSimple("pipeline_simple_solver.html");
    if (vizSimple.generate(pipelineSimple, water, "Pipeline Analysis - SimpleSolver")) {
        std::cout << "✓ SimpleSolver visualization: pipeline_simple_solver.html\n";
    }
    
    Visualizer vizRK4("pipeline_rk4_solver.html");
    if (vizRK4.generate(pipelineRK4, water, "Pipeline Analysis - RK4Solver")) {
        std::cout << "✓ RK4Solver visualization: pipeline_rk4_solver.html\n";
    }
    
    std::cout << "\nOpen both HTML files to compare results!\n";
    std::cout << "\nKey differences to look for:\n";
    std::cout << "  - RK4 provides smoother pressure profiles\n";
    std::cout << "  - RK4 captures gradual changes better\n";
    std::cout << "  - RK4 is more accurate for long pipes with varying conditions\n";
    
    return 0;
}

// // Enhanced visualization example with more pipe segments
// #include "Pipeline.h"
// #include "Liquid.h"
// #include "SimpleSolver.h"
// #include "Visualizer.h"

// int main() {
//     // Create fluid (water)
//     Liquid water("Water", 1000.0, 0.001);
    
//     // Create pipeline
//     Pipeline pipeline("Example-Pipeline");
    
//     // Create solver
//     SimpleSolver solver;
    
//     // Add 20 pipe segments with varying elevations and diameters
//     // This creates a more interesting and realistic pipeline profile
    
//     // Section 1: Uphill climb (segments 1-5)
//     pipeline.addPipeSegment("Pipe-01", 0.0, 50.0, 0.0, 5.0, 0.25, 0.00015);
//     pipeline.addPipeSegment("Pipe-02", 50.0, 100.0, 5.0, 12.0, 0.25, 0.00015);
//     pipeline.addPipeSegment("Pipe-03", 100.0, 150.0, 12.0, 20.0, 0.25, 0.00015);
//     pipeline.addPipeSegment("Pipe-04", 150.0, 200.0, 20.0, 25.0, 0.25, 0.00015);
//     pipeline.addPipeSegment("Pipe-05", 200.0, 250.0, 25.0, 28.0, 0.25, 0.00015);
    
//     // Section 2: High plateau with diameter change (segments 6-10)
//     pipeline.addPipeSegment("Pipe-06", 250.0, 300.0, 28.0, 28.0, 0.20, 0.00015); // Smaller diameter
//     pipeline.addPipeSegment("Pipe-07", 300.0, 350.0, 28.0, 29.0, 0.20, 0.00015);
//     pipeline.addPipeSegment("Pipe-08", 350.0, 400.0, 29.0, 29.0, 0.20, 0.00015);
//     pipeline.addPipeSegment("Pipe-09", 400.0, 450.0, 29.0, 28.0, 0.20, 0.00015);
//     pipeline.addPipeSegment("Pipe-10", 450.0, 500.0, 28.0, 27.0, 0.20, 0.00015);
    
//     // Section 3: Gradual descent (segments 11-15)
//     pipeline.addPipeSegment("Pipe-11", 500.0, 550.0, 27.0, 23.0, 0.22, 0.00015);
//     pipeline.addPipeSegment("Pipe-12", 550.0, 600.0, 23.0, 18.0, 0.22, 0.00015);
//     pipeline.addPipeSegment("Pipe-13", 600.0, 650.0, 18.0, 12.0, 0.22, 0.00015);
//     pipeline.addPipeSegment("Pipe-14", 650.0, 700.0, 12.0, 8.0, 0.22, 0.00015);
//     pipeline.addPipeSegment("Pipe-15", 700.0, 750.0, 8.0, 5.0, 0.22, 0.00015);
    
//     // Section 4: Valley and final climb (segments 16-20)
//     pipeline.addPipeSegment("Pipe-16", 750.0, 800.0, 5.0, 0.0, 0.25, 0.00015); // Back to larger diameter
//     pipeline.addPipeSegment("Pipe-17", 800.0, 850.0, 0.0, -3.0, 0.25, 0.00015); // Into valley
//     pipeline.addPipeSegment("Pipe-18", 850.0, 900.0, -3.0, -2.0, 0.25, 0.00015);
//     pipeline.addPipeSegment("Pipe-19", 900.0, 950.0, -2.0, 3.0, 0.25, 0.00015); // Climb out
//     pipeline.addPipeSegment("Pipe-20", 950.0, 1000.0, 3.0, 8.0, 0.25, 0.00015);
    
//     // Set inlet conditions: High pressure inlet, ambient temperature, 2 m/s velocity
//     pipeline.setInletConditions(2000000.0, 293.15, 2.0);
    
//     // Solve the pipeline
//     std::cout << "Solving pipeline with " << pipeline.getNumSegments() << " segments...\n";
//     pipeline.solveAll(&solver, &water);
    
//     // Display results to console
//     std::cout << "\n=== Pipeline Solution ===" << std::endl;
//     pipeline.displaySummary();
    
//     // Generate visualization
//     std::cout << "\nGenerating enhanced visualization...\n";
//     Visualizer viz("my_pipeline_results.html");
    
//     if (viz.generate(pipeline, water, "Enhanced Pipeline Analysis - 20 Segments")) {
//         std::cout << "\n✓ Success! Open 'my_pipeline_results.html' in your browser.\n";
//         std::cout << "  The visualization includes:\n";
//         std::cout << "    - Pressure profile\n";
//         std::cout << "    - Elevation profile\n";
//         std::cout << "    - Velocity profile\n";
//         std::cout << "    - Temperature profile\n";
//         std::cout << "    - Reynolds number\n";
//         std::cout << "    - Friction factor\n";
//         std::cout << "    - Pressure gradient\n";
//         std::cout << "    - 2D pipeline schematic\n";
//     } else {
//         std::cerr << "\n✗ Failed to generate visualization.\n";
//     }
    
//     return 0;
// }
