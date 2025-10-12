#ifndef PIPE_H
#define PIPE_H

#include <string>
#include <iostream>

// Forward declaration
class Fluid;

/**
 * @brief Pipe class representing a pipeline segment
 * 
 * Stores geometric properties, position, elevation, and flow conditions.
 * Uses coordinate-based positioning (x, z) for inlet and outlet.
 */
class Pipe {
private:
    // Geometric properties
    std::string pipeID;
    double length;           // m (calculated from coordinates)
    double diameter;         // m
    double roughness;        // m (absolute roughness)
    double inletPosition;    // m (horizontal position)
    double outletPosition;   // m (horizontal position)
    double inletElevation;   // m (elevation above datum)
    double outletElevation;  // m (elevation above datum)
    
    // Inlet conditions
    double inletPressure;    // Pa
    double inletTemp;        // K
    double inletVelocity;    // m/s
    
    // Outlet conditions
    double outletPressure;   // Pa
    double outletTemp;       // K
    double outletVelocity;   // m/s

public:
    /**
     * @brief Constructor with coordinate-based positioning
     * @param id Pipe identifier
     * @param x_ini Inlet horizontal position (m)
     * @param x_end Outlet horizontal position (m)
     * @param z_ini Inlet elevation (m)
     * @param z_end Outlet elevation (m)
     * @param dia Diameter (m)
     * @param rough Absolute roughness (m)
     */
    Pipe(std::string id, double x_ini, double x_end, double z_ini, double z_end, 
         double dia, double rough);
    
    // Setters
    void setInletConditions(double P, double T, double V);
    void setOutletConditions(double P, double T, double V);
    
    // Getters - Geometry
    double getLength() const;
    double getDiameter() const;
    double getRoughness() const;
    double getInletPosition() const;
    double getOutletPosition() const;
    double getInletElevation() const;
    double getOutletElevation() const;
    double getElevationChange() const;
    std::string getPipeID() const;
    
    // Getters - Inlet conditions
    double getInletPressure() const;
    double getInletTemp() const;
    double getInletVelocity() const;
    
    // Getters - Outlet conditions
    double getOutletPressure() const;
    double getOutletTemp() const;
    double getOutletVelocity() const;
    
    // Flow calculations
    double getReynoldsNumber(const Fluid* fluid, double P, double T) const;
    double getRelativeRoughness() const;
    double getFrictionFactor(const Fluid* fluid, double P, double T) const;
    
    // Display
    void displayInfo(const Fluid* fluid) const;
};

#endif // PIPE_H
