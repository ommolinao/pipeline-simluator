#ifndef ADAPTIVE_SOLVER_H
#define ADAPTIVE_SOLVER_H

#include "Solver.h"
#include <stdexcept>

/**
 * @brief Adaptive solver with automatic mesh refinement
 * 
 * Recursively subdivides pipe segments where pressure drop exceeds
 * specified threshold. Ensures accuracy without manual meshing.
 * Includes validation for ill-posed problems.
 */
class AdaptiveSolver : public Solver {
private:
    double maxPressureDrop;  // Pa - maximum allowed pressure drop per segment
    int maxSubdivisions;     // Maximum recursion depth
    bool verbose;            // Print subdivision info
    
    /**
     * @brief Recursive function to solve segment with adaptive refinement
     */
    void solveSegment(const Fluid* fluid, 
                     double x_start, double x_end,
                     double z_start, double z_end,
                     double P_in, double T_in, double V_in,
                     double diameter, double roughness,
                     double& P_out, double& V_out,
                     int depth = 0) const;
    
    /**
     * @brief Calculate pressure drop for a segment defined by parameters
     */
    double calculatePressureDropForSegment(const Fluid* fluid,
                                          double length, double diameter, double roughness,
                                          double elevation_change,
                                          double P, double T, double velocity) const;

public:
    AdaptiveSolver(double maxDP, int maxIter = 20, double tol = 1.0, 
                   int maxSubdiv = 10, bool verb = false);
    
    void solve(Pipe* pipe, const Fluid* fluid) override;
    std::string getSolverName() const override;
    
    // Getters and setters
    double getMaxPressureDrop() const;
    void setMaxPressureDrop(double maxDP);
    void setVerbose(bool verb);
};

#endif // ADAPTIVE_SOLVER_H
