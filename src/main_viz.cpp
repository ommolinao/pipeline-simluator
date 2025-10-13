// Example of how to use the Visualizer in your main.cpp

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
    
    // Add pipe segments (example: 5 segments with varying elevations)
    // pipeline.addPipeSegment("DirectSeg-1", 0.0, 500.0, 0.0, 10.0, 0.3, 0.000045);
    pipeline.addPipeSegment("Pipe-1", 0.0, 100.0, 0.0, 5.0, 0.2, 0.00015); // Uphill
    pipeline.addPipeSegment("Pipe-2", 100.0, 200.0, 5.0, 5.0, 0.2, 0.00015); // Level
    pipeline.addPipeSegment("Pipe-3", 200.0, 300.0, 5.0, 2.0, 0.2, 0.00015); // Downhill
    pipeline.addPipeSegment("Pipe-4", 300.0, 400.0, 2.0, 0.0, 0.2, 0.00015); // Downhill
    pipeline.addPipeSegment("Pipe-5", 400.0, 500.0, 0.0, -3.0, 0.2, 0.00015); // Downhill

    // Set inlet conditions directly
    pipeline.setInletConditions(1000000.0, 293.15, 2.0);

    // Solve the pipeline
    std::cout << "Solving pipeline...\n";
    pipeline.solveAll(&solver, &water);
    
    // Display results to console
    std::cout << "\n=== Pipeline Solution ===" << std::endl;
    pipeline.displaySummary();
    
    // Generate visualization
    std::cout << "\nGenerating visualization...\n";
    Visualizer viz("my_pipeline_results.html");
    
    if (viz.generate(pipeline, water, "My Pipeline Analysis")) {
        std::cout << "\n✓ Success! Open 'my_pipeline_results.html' in your browser.\n";
    } else {
        std::cerr << "\n✗ Failed to generate visualization.\n";
    }
    
    return 0;
}
