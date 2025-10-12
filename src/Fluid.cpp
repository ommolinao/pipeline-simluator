#include "Fluid.h"

Fluid::Fluid(std::string name) : fluidName(name) {}

Fluid::~Fluid() {}

std::string Fluid::getName() const {
    return fluidName;
}

void Fluid::displayProperties(double P, double T) const {
    std::cout << "Fluid: " << fluidName << std::endl;
    std::cout << "Temperature: " << T << " K" << std::endl;
    std::cout << "Pressure: " << P/1000.0 << " kPa" << std::endl;
    std::cout << "Density: " << getDensity(P, T) << " kg/m³" << std::endl;
    std::cout << "Viscosity: " << getViscosity(P, T) << " Pa·s" << std::endl;
}
