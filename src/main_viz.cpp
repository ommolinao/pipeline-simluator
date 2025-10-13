// Enhanced visualization example with more pipe segments
#include "Pipeline.h"
#include "Liquid.h"
#include "SimpleSolver.h"
#include "Visualizer.h"

int main() {
    // Create fluid (water)
    Liquid water("Water", 1000.0, 0.001);
    
    // Create pipeline
    Pipeline pipeline("Example-Pipeline");
    
    // Create solver
    SimpleSolver solver;
    
    // Add 20 pipe segments with varying elevations and diameters
    // This creates a more interesting and realistic pipeline profile
    
    // Section 1: Uphill climb (segments 1-5)
    pipeline.addPipeSegment("Pipe-01", 0.0, 50.0, 0.0, 5.0, 0.25, 0.00015);
    pipeline.addPipeSegment("Pipe-02", 50.0, 100.0, 5.0, 12.0, 0.25, 0.00015);
    pipeline.addPipeSegment("Pipe-03", 100.0, 150.0, 12.0, 20.0, 0.25, 0.00015);
    pipeline.addPipeSegment("Pipe-04", 150.0, 200.0, 20.0, 25.0, 0.25, 0.00015);
    pipeline.addPipeSegment("Pipe-05", 200.0, 250.0, 25.0, 28.0, 0.25, 0.00015);
    
    // Section 2: High plateau with diameter change (segments 6-10)
    pipeline.addPipeSegment("Pipe-06", 250.0, 300.0, 28.0, 28.0, 0.20, 0.00015); // Smaller diameter
    pipeline.addPipeSegment("Pipe-07", 300.0, 350.0, 28.0, 29.0, 0.20, 0.00015);
    pipeline.addPipeSegment("Pipe-08", 350.0, 400.0, 29.0, 29.0, 0.20, 0.00015);
    pipeline.addPipeSegment("Pipe-09", 400.0, 450.0, 29.0, 28.0, 0.20, 0.00015);
    pipeline.addPipeSegment("Pipe-10", 450.0, 500.0, 28.0, 27.0, 0.20, 0.00015);
    
    // Section 3: Gradual descent (segments 11-15)
    pipeline.addPipeSegment("Pipe-11", 500.0, 550.0, 27.0, 23.0, 0.22, 0.00015);
    pipeline.addPipeSegment("Pipe-12", 550.0, 600.0, 23.0, 18.0, 0.22, 0.00015);
    pipeline.addPipeSegment("Pipe-13", 600.0, 650.0, 18.0, 12.0, 0.22, 0.00015);
    pipeline.addPipeSegment("Pipe-14", 650.0, 700.0, 12.0, 8.0, 0.22, 0.00015);
    pipeline.addPipeSegment("Pipe-15", 700.0, 750.0, 8.0, 5.0, 0.22, 0.00015);
    
    // Section 4: Valley and final climb (segments 16-20)
    pipeline.addPipeSegment("Pipe-16", 750.0, 800.0, 5.0, 0.0, 0.25, 0.00015); // Back to larger diameter
    pipeline.addPipeSegment("Pipe-17", 800.0, 850.0, 0.0, -3.0, 0.25, 0.00015); // Into valley
    pipeline.addPipeSegment("Pipe-18", 850.0, 900.0, -3.0, -2.0, 0.25, 0.00015);
    pipeline.addPipeSegment("Pipe-19", 900.0, 950.0, -2.0, 3.0, 0.25, 0.00015); // Climb out
    pipeline.addPipeSegment("Pipe-20", 950.0, 1000.0, 3.0, 8.0, 0.25, 0.00015);
    
    // Set inlet conditions: High pressure inlet, ambient temperature, 2 m/s velocity
    pipeline.setInletConditions(2000000.0, 293.15, 2.0);
    
    // Solve the pipeline
    std::cout << "Solving pipeline with " << pipeline.getNumSegments() << " segments...\n";
    pipeline.solveAll(&solver, &water);
    
    // Display results to console
    std::cout << "\n=== Pipeline Solution ===" << std::endl;
    pipeline.displaySummary();
    
    // Generate visualization
    std::cout << "\nGenerating enhanced visualization...\n";
    Visualizer viz("my_pipeline_results.html");
    
    if (viz.generate(pipeline, water, "Enhanced Pipeline Analysis - 20 Segments")) {
        std::cout << "\n✓ Success! Open 'my_pipeline_results.html' in your browser.\n";
        std::cout << "  The visualization includes:\n";
        std::cout << "    - Pressure profile\n";
        std::cout << "    - Elevation profile\n";
        std::cout << "    - Velocity profile\n";
        std::cout << "    - Temperature profile\n";
        std::cout << "    - Reynolds number\n";
        std::cout << "    - Friction factor\n";
        std::cout << "    - Pressure gradient\n";
        std::cout << "    - 2D pipeline schematic\n";
    } else {
        std::cerr << "\n✗ Failed to generate visualization.\n";
    }
    
    return 0;
}
