#include "Liquid.h"

Liquid::Liquid(std::string name, double rho_ref, double mu,
               double T_ref, double P_ref, double beta_T, double alpha_P)
    : Fluid(name), referenceDensity(rho_ref), 
      viscosity(mu), referenceTemperature(T_ref), referencePressure(P_ref),
      isothermalCompressibility(beta_T), isobaricExpansivity(alpha_P) {}

Liquid::Liquid(std::string name, double rho_ref, double mu)
    : Fluid(name), referenceDensity(rho_ref),
      viscosity(mu), referenceTemperature(293.15), referencePressure(101325.0),
      isothermalCompressibility(4.5e-10), isobaricExpansivity(2.07e-4) {}

double Liquid::getDensity(double P, double T) const {
    double dP = P - referencePressure;
    double dT = T - referenceTemperature;
    double densityFactor = 1.0 + isothermalCompressibility * dP - isobaricExpansivity * dT;
    return referenceDensity * densityFactor;
}

double Liquid::getViscosity(double /*P*/, double /*T*/) const {
    return viscosity;
}

double Liquid::getIsothermalCompressibility() const {
    return isothermalCompressibility;
}

double Liquid::getIsobaricExpansivity() const {
    return isobaricExpansivity;
}

double Liquid::getReferenceDensity() const {
    return referenceDensity;
}

void Liquid::setViscosityValue(double mu) {
    viscosity = mu;
}

void Liquid::displayProperties(double P, double T) const {
    Fluid::displayProperties(P, T);
    std::cout << "Reference Density: " << referenceDensity << " kg/m³" << std::endl;
    std::cout << "Isothermal Compressibility: " << isothermalCompressibility << " 1/Pa" << std::endl;
    std::cout << "Isobaric Expansivity: " << isobaricExpansivity << " 1/K" << std::endl;
}
