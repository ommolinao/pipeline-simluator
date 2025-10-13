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
    
    // Calculated fluid properties (populated after solving)
    std::vector<double> reynoldsNumbers;    // Reynolds number for each segment
    std::vector<double> frictionFactors;    // Darcy friction factor
    std::vector<double> densities;          // Fluid density at segment conditions
    std::vector<double> viscosities;        // Fluid viscosity at segment conditions
    std::vector<double> pressureGradients;  // dP/dx for each segment (Pa/m)

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
    
    // ============================================================================
    // Getter methods for Visualizer (read-only access to SoA arrays)
    // ============================================================================
    
    /**
     * @brief Get number of pipe segments (alias for compatibility)
     */
    size_t getNumSegments() const {
        return numSegments;
    }
    
    /**
     * @brief Get pipe ID for segment i
     */
    std::string getPipeID(size_t i) const {
        if (i >= numSegments) throw std::out_of_range("Segment index out of range");
        return segmentIDs[i];
    }
    
    /**
     * @brief Get length of segment i
     */
    double getLength(size_t i) const {
        if (i >= numSegments) throw std::out_of_range("Segment index out of range");
        return lengths[i];
    }
    
    /**
     * @brief Get diameter of segment i
     */
    double getDiameter(size_t i) const {
        if (i >= numSegments) throw std::out_of_range("Segment index out of range");
        return diameters[i];
    }
    
    /**
     * @brief Get roughness of segment i
     */
    double getRoughness(size_t i) const {
        if (i >= numSegments) throw std::out_of_range("Segment index out of range");
        return roughnesses[i];
    }
    
    /**
     * @brief Get inlet X position of segment i
     */
    double getXInlet(size_t i) const {
        if (i >= numSegments) throw std::out_of_range("Segment index out of range");
        return x_inlets[i];
    }
    
    /**
     * @brief Get outlet X position of segment i
     */
    double getXOutlet(size_t i) const {
        if (i >= numSegments) throw std::out_of_range("Segment index out of range");
        return x_outlets[i];
    }
    
    /**
     * @brief Get inlet Z elevation of segment i
     */
    double getZInlet(size_t i) const {
        if (i >= numSegments) throw std::out_of_range("Segment index out of range");
        return z_inlets[i];
    }
    
    /**
     * @brief Get outlet Z elevation of segment i
     */
    double getZOutlet(size_t i) const {
        if (i >= numSegments) throw std::out_of_range("Segment index out of range");
        return z_outlets[i];
    }
    
    /**
     * @brief Get inlet pressure of segment i
     */
    double getInletPressure(size_t i) const {
        if (i >= numSegments) throw std::out_of_range("Segment index out of range");
        return inletPressures[i];
    }
    
    /**
     * @brief Get outlet pressure of segment i
     */
    double getOutletPressure(size_t i) const {
        if (i >= numSegments) throw std::out_of_range("Segment index out of range");
        return outletPressures[i];
    }
    
    /**
     * @brief Get inlet temperature of segment i
     */
    double getInletTemperature(size_t i) const {
        if (i >= numSegments) throw std::out_of_range("Segment index out of range");
        return inletTemps[i];
    }
    
    /**
     * @brief Get outlet temperature of segment i
     */
    double getOutletTemperature(size_t i) const {
        if (i >= numSegments) throw std::out_of_range("Segment index out of range");
        return outletTemps[i];
    }
    
    /**
     * @brief Get inlet velocity of segment i
     */
    double getInletVelocity(size_t i) const {
        if (i >= numSegments) throw std::out_of_range("Segment index out of range");
        return inletVelocities[i];
    }
    
    /**
     * @brief Get outlet velocity of segment i
     */
    double getOutletVelocity(size_t i) const {
        if (i >= numSegments) throw std::out_of_range("Segment index out of range");
        return outletVelocities[i];
    }
    
    // ============================================================================
    // Getters for calculated fluid properties
    // ============================================================================
    
    /**
     * @brief Get Reynolds number for segment i
     */
    double getReynoldsNumber(size_t i) const {
        if (i >= numSegments) throw std::out_of_range("Segment index out of range");
        return reynoldsNumbers[i];
    }
    
    /**
     * @brief Get friction factor for segment i
     */
    double getFrictionFactor(size_t i) const {
        if (i >= numSegments) throw std::out_of_range("Segment index out of range");
        return frictionFactors[i];
    }
    
    /**
     * @brief Get density for segment i
     */
    double getDensity(size_t i) const {
        if (i >= numSegments) throw std::out_of_range("Segment index out of range");
        return densities[i];
    }
    
    /**
     * @brief Get viscosity for segment i
     */
    double getViscosity(size_t i) const {
        if (i >= numSegments) throw std::out_of_range("Segment index out of range");
        return viscosities[i];
    }
    
    /**
     * @brief Get pressure gradient for segment i
     */
    double getPressureGradient(size_t i) const {
        if (i >= numSegments) throw std::out_of_range("Segment index out of range");
        return pressureGradients[i];
    }
};

#endif // PIPELINE_H
