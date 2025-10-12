#ifndef FLUID_H
#define FLUID_H

#include <string>
#include <iostream>

/**
 * @brief Abstract base class for fluids
 * 
 * Provides interface for calculating fluid properties at given pressure
 * and temperature conditions.
 */
class Fluid {
protected:
    std::string fluidName;

public:
    Fluid(std::string name);
    virtual ~Fluid();
    
    // Pure virtual functions - must be implemented by derived classes
    virtual double getDensity(double P, double T) const = 0;
    virtual double getViscosity(double P, double T) const = 0;
    
    // Common methods
    std::string getName() const;
    virtual void displayProperties(double P, double T) const;
};

#endif // FLUID_H
