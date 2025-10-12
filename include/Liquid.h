#ifndef LIQUID_H
#define LIQUID_H

#include "Fluid.h"

/**
 * @brief Liquid class - slightly compressible fluid
 * 
 * Models liquid with pressure and temperature-dependent density using
 * isothermal compressibility and isobaric expansivity.
 */
class Liquid : public Fluid {
private:
    double referenceDensity;           // kg/m³ at reference conditions
    double viscosity;                  // Pa·s (assumed constant)
    double referenceTemperature;       // K
    double referencePressure;          // Pa
    double isothermalCompressibility;  // 1/Pa (βT)
    double isobaricExpansivity;        // 1/K (αP)

public:
    // Full constructor
    Liquid(std::string name, double rho_ref, double mu,
           double T_ref, double P_ref, double beta_T, double alpha_P);
    
    // Simplified constructor with default compressibility (water at 20°C, 1 atm)
    Liquid(std::string name, double rho_ref, double mu);
    
    // Density calculation: ρ(P,T) = ρ_ref * [1 + βT*(P - P_ref) - αP*(T - T_ref)]
    double getDensity(double P, double T) const override;
    double getViscosity(double P, double T) const override;
    
    // Getters
    double getIsothermalCompressibility() const;
    double getIsobaricExpansivity() const;
    double getReferenceDensity() const;
    
    void setViscosityValue(double mu);
    void displayProperties(double P, double T) const override;
};

#endif // LIQUID_H
