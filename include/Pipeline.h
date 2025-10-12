#ifndef PIPELINE_H
#define PIPELINE_H

#include <vector>
#include <string>
#include <iostream>
#include "Pipe.h"

// Forward declarations
class Solver;
class Fluid;

/**
 * @brief Pipeline class for managing a series of connected pipe segments
 * 
 * PHASE 4: Pure SoA (Structure-of-Arrays) storage for optimal cache performance.
 * All pipe data stored in contiguous arrays for excellent memory locality.
 * 
 * Handles sequential pipe chains where the outlet of segment N becomes
 * the inlet of segment N+1. Provides methods to solve the entire pipeline
 * and display results.
 */
class Pipeline {
private:
    std::string pipelineID;    // Identifier for the pipeline
    size_t numSegments;        // Number of segments
    
    // Geometry arrays (cold data - rarely accessed during iteration)
    std::vector<std::string> segmentIDs;
    std::vector<double> x_inlets;
    std::vector<double> x_outlets;
    std::vector<double> z_inlets;
    std::vector<double> z_outlets;
    std::vector<double> lengths;
    std::vector<double> diameters;
    std::vector<double> roughnesses;
    
    // State arrays (hot data - frequently accessed during solving)
    std::vector<double> inletPressures;
    std::vector<double> outletPressures;
    std::vector<double> inletTemps;
    std::vector<double> outletTemps;
    std::vector<double> inletVelocities;
    std::vector<double> outletVelocities;

public:
    /**
     * @brief Constructor
     * @param id Pipeline identifier
     */
    Pipeline(std::string id);
    
    /**
     * @brief Destructor
     */
    ~Pipeline();
    
    /**
     * @brief Add a pipe to the pipeline by extracting its properties
     * @param pipe Pointer to pipe segment (data extracted, pointer not stored)
     * @return true if pipe was added successfully, false if validation failed
     * 
     * Convenience method that extracts geometry from a Pipe object and stores
     * it in SoA arrays. The Pipe pointer is NOT stored - only data is copied.
     * PHASE 4: Pure SoA storage - no Pipe pointers kept.
     */
    bool addPipe(Pipe* pipe);
    
    /**
     * @brief Add a pipe segment directly by specifying parameters
     * @param id Pipe identifier
     * @param x_in Inlet horizontal position (m)
     * @param x_out Outlet horizontal position (m)
     * @param z_in Inlet elevation (m)
     * @param z_out Outlet elevation (m)
     * @param dia Diameter (m)
     * @param rough Absolute roughness (m)
     * @return true if pipe was added successfully, false if validation failed
     * 
     * Direct method that populates SoA arrays without creating a Pipe object.
     * More efficient for pipeline-only usage.
     */
    bool addPipeSegment(const std::string& id, double x_in, double x_out,
                        double z_in, double z_out, double dia, double rough);
    
    /**
     * @brief Get number of segments in pipeline
     * @return Number of segments
     */
    size_t getNumberOfSegments() const;
    
    /**
     * @brief Set inlet conditions for the first segment
     * @param P Inlet pressure (Pa)
     * @param T Inlet temperature (K)
     * @param V Inlet velocity (m/s)
     */
    void setInletConditions(double P, double T, double V);
    
    /**
     * @brief Solve the entire pipeline using SoA storage
     * @param solver Solver to use for each pipe segment
     * @param fluid Fluid being transported
     * 
     * PHASE 4: Uses pure SoA storage with cache-optimized solving.
     * Creates temporary Pipe objects on-the-fly to interface with Solver.
     */
    void solveAll(Solver* solver, Fluid* fluid);
    
    /**
     * @brief Get total pressure drop across the pipeline
     * @return Total pressure drop (Pa)
     */
    double getTotalPressureDrop() const;
    
    /**
     * @brief Display information for all pipes in the pipeline
     * @param fluid Fluid for property calculations
     */
    void displayAll(const Fluid* fluid) const;
    
    /**
     * @brief Display summary of pipeline
     */
    void displaySummary() const;
    
    /**
     * @brief Debug: Display SoA array contents
     */
    void displaySoADebug() const;
};

#endif // PIPELINE_H
