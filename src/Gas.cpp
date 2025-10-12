#include "Gas.h"

Gas::Gas(std::string name, double MW, double mu)
    : Fluid(name), molecularWeight(MW), viscosity(mu) {}

double Gas::getDensity(double P, double T) const {
    return (P * molecularWeight) / (R * T);
}

double Gas::getViscosity(double P, double T) const {
    // For simplicity, viscosity is assumed constant
    // Could be extended to include temperature dependence (e.g., Sutherland's law)
    (void)P; // Suppress unused parameter warning
    (void)T;
    return viscosity;
}

double Gas::getMolecularWeight() const {
    return molecularWeight;
}

void Gas::setViscosityValue(double mu) {
    viscosity = mu;
}

void Gas::displayProperties(double P, double T) const {
    Fluid::displayProperties(P, T);
    std::cout << "Molecular Weight: " << molecularWeight << " kg/kmol" << std::endl;
    std::cout << "Compressibility Factor: Z ≈ 1.0 (ideal gas)" << std::endl;
    std::cout << "Viscosity: " << viscosity << " Pa·s" << std::endl;
}
