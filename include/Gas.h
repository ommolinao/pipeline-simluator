#ifndef GAS_H
#define GAS_H

#include "Fluid.h"

/**
 * @brief Gas class - compressible fluid
 * 
 * Models ideal gas using equation of state: ρ = (P*MW)/(R*T)
 */
class Gas : public Fluid {
private:
    double molecularWeight;  // kg/kmol
    double viscosity;        // Pa·s (assumed constant)
    static constexpr double R = 8314.46; // Universal gas constant J/(kmol·K)

public:
    Gas(std::string name, double MW, double mu);
    
    // Ideal gas law: ρ = (P*MW)/(R*T)
    double getDensity(double P, double T) const override;
    double getViscosity(double P, double T) const override;
    
    // Getters
    double getMolecularWeight() const;
    
    void setViscosityValue(double mu);
    void displayProperties(double P, double T) const override;
};

#endif // GAS_H
