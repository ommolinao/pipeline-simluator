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
 * Handles sequential pipe chains where the outlet of pipe N becomes
 * the inlet of pipe N+1. Provides methods to solve the entire pipeline
 * and display results.
 */
class Pipeline {
private:
    std::vector<Pipe*> pipes;  // Collection of pipe segments
    std::string pipelineID;    // Identifier for the pipeline
    
public:
    /**
     * @brief Constructor
     * @param id Pipeline identifier
     */
    Pipeline(std::string id);
    
    /**
     * @brief Add a pipe to the pipeline
     * @param pipe Pointer to pipe segment
     * 
     * Pipes are added in order and will be solved sequentially.
     */
    void addPipe(Pipe* pipe);
    
    /**
     * @brief Get number of pipes in the pipeline
     */
    size_t getNumberOfPipes() const;
    
    /**
     * @brief Get a specific pipe by index
     * @param index Index of the pipe (0-based)
     * @return Pointer to the pipe, or nullptr if index is out of bounds
     */
    Pipe* getPipe(size_t index) const;
    
    /**
     * @brief Solve the entire pipeline sequentially
     * @param solver Solver to use for each pipe segment
     * @param fluid Fluid being transported
     * 
     * Solves each pipe in order, chaining outlet conditions to the next inlet.
     * Assumes the first pipe's inlet conditions are already set.
     */
    void solveSequential(Solver* solver, const Fluid* fluid);
    
    /**
     * @brief Display results for all pipes in the pipeline
     * @param fluid Fluid for property calculations
     */
    void displayResults(const Fluid* fluid) const;
    
    /**
     * @brief Get total length of the pipeline
     */
    double getTotalLength() const;
    
    /**
     * @brief Get total elevation change (outlet - inlet)
     */
    double getTotalElevationChange() const;
    
    /**
     * @brief Get total pressure drop across the pipeline
     */
    double getTotalPressureDrop() const;
    
    /**
     * @brief Display summary statistics for the pipeline
     */
    void displaySummary(const Fluid* fluid) const;
    
    /**
     * @brief Clear all pipes from the pipeline
     */
    void clear();
    
    /**
     * @brief Get pipeline ID
     */
    std::string getPipelineID() const;
};

#endif // PIPELINE_H
