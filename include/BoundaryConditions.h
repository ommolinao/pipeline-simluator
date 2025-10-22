#ifndef BOUNDARY_CONDITIONS_H
#define BOUNDARY_CONDITIONS_H

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include <string>
#include <stdexcept>
#include <iostream>

/**
 * @brief Enumeration of boundary condition types
 */
enum class BCType {
    PRESSURE,           // Specified pressure
    VELOCITY,           // Specified velocity
    MASS_FLOW_RATE,     // Specified mass flow rate
    VOLUMETRIC_FLOW_RATE, // Specified volumetric flow rate
    NONE                // No boundary condition (used for internal calculations)
};

/**
 * @brief BoundaryConditions class for flexible pipeline inlet/outlet conditions
 * 
 * Supports multiple types of boundary conditions:
 * - Pressure-Pressure (find flow rate)
 * - Pressure-Velocity (current default)
 * - Velocity-Pressure
 * - Mass flow rate specifications
 * - Volumetric flow rate specifications
 * 
 * Temperature is always specified at inlet.
 */
class BoundaryConditions {
private:
    // Inlet conditions
    BCType inletType;
    double inletPressure;        // Pa
    double inletVelocity;        // m/s
    double inletMassFlowRate;    // kg/s
    double inletVolFlowRate;     // m³/s
    double inletTemperature;     // K
    
    // Outlet conditions
    BCType outletType;
    double outletPressure;       // Pa
    double outletVelocity;       // m/s
    double outletMassFlowRate;   // kg/s
    double outletVolFlowRate;    // m³/s
    
    // Validation flags
    bool inletSet;
    bool outletSet;

public:
    /**
     * @brief Default constructor - initializes to unset state
     */
    BoundaryConditions();
    
    // ========================================================================
    // INLET BOUNDARY CONDITIONS
    // ========================================================================
    
    /**
     * @brief Set inlet conditions with pressure and velocity (current default)
     * @param P Inlet pressure (Pa)
     * @param T Inlet temperature (K)
     * @param V Inlet velocity (m/s)
     */
    void setInletPressureVelocity(double P, double T, double V);
    
    /**
     * @brief Set inlet conditions with only pressure (velocity to be calculated)
     * @param P Inlet pressure (Pa)
     * @param T Inlet temperature (K)
     */
    void setInletPressure(double P, double T);
    
    /**
     * @brief Set inlet conditions with mass flow rate
     * @param mdot Mass flow rate (kg/s)
     * @param T Inlet temperature (K)
     */
    void setInletMassFlowRate(double mdot, double T);
    
    /**
     * @brief Set inlet conditions with volumetric flow rate
     * @param Q Volumetric flow rate (m³/s)
     * @param T Inlet temperature (K)
     */
    void setInletVolumetricFlowRate(double Q, double T);
    
    // ========================================================================
    // OUTLET BOUNDARY CONDITIONS
    // ========================================================================
    
    /**
     * @brief Set outlet pressure (most common for pressure-driven flow)
     * @param P Outlet pressure (Pa)
     */
    void setOutletPressure(double P);
    
    /**
     * @brief Set outlet velocity
     * @param V Outlet velocity (m/s)
     */
    void setOutletVelocity(double V);
    
    /**
     * @brief Set outlet mass flow rate
     * @param mdot Mass flow rate (kg/s)
     */
    void setOutletMassFlowRate(double mdot);
    
    /**
     * @brief Set outlet volumetric flow rate
     * @param Q Volumetric flow rate (m³/s)
     */
    void setOutletVolumetricFlowRate(double Q);
    
    // ========================================================================
    // GETTERS
    // ========================================================================
    
    BCType getInletType() const { return inletType; }
    BCType getOutletType() const { return outletType; }
    
    double getInletPressure() const { return inletPressure; }
    double getInletVelocity() const { return inletVelocity; }
    double getInletMassFlowRate() const { return inletMassFlowRate; }
    double getInletVolFlowRate() const { return inletVolFlowRate; }
    double getInletTemperature() const { return inletTemperature; }
    
    double getOutletPressure() const { return outletPressure; }
    double getOutletVelocity() const { return outletVelocity; }
    double getOutletMassFlowRate() const { return outletMassFlowRate; }
    double getOutletVolFlowRate() const { return outletVolFlowRate; }
    
    bool isInletSet() const { return inletSet; }
    bool isOutletSet() const { return outletSet; }
    
    // ========================================================================
    // VALIDATION AND UTILITIES
    // ========================================================================
    
    /**
     * @brief Check if boundary conditions are fully specified and valid
     * @return true if valid, false otherwise
     */
    bool isValid() const;
    
    /**
     * @brief Get a string describing the boundary condition configuration
     * @return Description string
     */
    std::string getConfigurationString() const;
    
    /**
     * @brief Check if this is a pressure-pressure problem (flow rate unknown)
     * @return true if both inlet and outlet are pressure BCs
     */
    bool isPressurePressureProblem() const;
    
    /**
     * @brief Check if this requires iterative solution
     * @return true if problem requires iteration to find unknown BC
     */
    bool requiresIterativeSolution() const;
    
    /**
     * @brief Display boundary conditions
     */
    void display() const;
    
    /**
     * @brief Convert BC type to string
     */
    static std::string bcTypeToString(BCType type);
    
    // ========================================================================
    // CONVERSION UTILITIES (for use by solvers)
    // ========================================================================
    
    /**
     * @brief Calculate velocity from mass flow rate
     * @param mdot Mass flow rate (kg/s)
     * @param rho Density (kg/m³)
     * @param area Cross-sectional area (m²)
     * @return Velocity (m/s)
     */
    static double massFlowRateToVelocity(double mdot, double rho, double area);
    
    /**
     * @brief Calculate velocity from volumetric flow rate
     * @param Q Volumetric flow rate (m³/s)
     * @param area Cross-sectional area (m²)
     * @return Velocity (m/s)
     */
    static double volFlowRateToVelocity(double Q, double area);
    
    /**
     * @brief Calculate mass flow rate from velocity
     * @param V Velocity (m/s)
     * @param rho Density (kg/m³)
     * @param area Cross-sectional area (m²)
     * @return Mass flow rate (kg/s)
     */
    static double velocityToMassFlowRate(double V, double rho, double area);
    
    /**
     * @brief Calculate volumetric flow rate from velocity
     * @param V Velocity (m/s)
     * @param area Cross-sectional area (m²)
     * @return Volumetric flow rate (m³/s)
     */
    static double velocityToVolFlowRate(double V, double area);
};

#endif // BOUNDARY_CONDITIONS_H
