#include "Pipe.h"
#include "Fluid.h"
#include <cmath>

Pipe::Pipe(std::string id, double x_ini, double x_end, double z_ini, double z_end, 
           double dia, double rough)
    : pipeID(id), length(0.0), diameter(dia), roughness(rough),
      inletPosition(x_ini), outletPosition(x_end),
      inletElevation(z_ini), outletElevation(z_end),
      inletPressure(0.0), inletTemp(293.15), inletVelocity(0.0),
      outletPressure(0.0), outletTemp(293.15), outletVelocity(0.0) {
    length = fabs(outletPosition - inletPosition);
    if (length < 1e-6) {
        std::cerr << "Warning: Pipe " << pipeID << " has near-zero length!" << std::endl;
    }
}

void Pipe::setInletConditions(double P, double T, double V) {
    inletPressure = P;
    inletTemp = T;
    inletVelocity = V;
}

void Pipe::setOutletConditions(double P, double T, double V) {
    outletPressure = P;
    outletTemp = T;
    outletVelocity = V;
}

double Pipe::getLength() const { return length; }
double Pipe::getDiameter() const { return diameter; }
double Pipe::getRoughness() const { return roughness; }
double Pipe::getInletPosition() const { return inletPosition; }
double Pipe::getOutletPosition() const { return outletPosition; }
double Pipe::getInletElevation() const { return inletElevation; }
double Pipe::getOutletElevation() const { return outletElevation; }
double Pipe::getElevationChange() const { return outletElevation - inletElevation; }
std::string Pipe::getPipeID() const { return pipeID; }

double Pipe::getInletPressure() const { return inletPressure; }
double Pipe::getInletTemp() const { return inletTemp; }
double Pipe::getInletVelocity() const { return inletVelocity; }

double Pipe::getOutletPressure() const { return outletPressure; }
double Pipe::getOutletTemp() const { return outletTemp; }
double Pipe::getOutletVelocity() const { return outletVelocity; }

double Pipe::getReynoldsNumber(const Fluid* fluid, double P, double T) const {
    double rho = fluid->getDensity(P, T);
    double mu = fluid->getViscosity(P, T);
    return (rho * inletVelocity * diameter) / mu;
}

double Pipe::getRelativeRoughness() const {
    return roughness / diameter;
}

double Pipe::getFrictionFactor(const Fluid* fluid, double P, double T) const {
    double Re = getReynoldsNumber(fluid, P, T);
    double eD = getRelativeRoughness();
    
    if (Re < 2300) {
        return 64.0 / Re;
    } else {
        double term1 = eD / 3.7;
        double term2 = 5.74 / pow(Re, 0.9);
        return 0.25 / pow(log10(term1 + term2), 2);
    }
}

void Pipe::displayInfo(const Fluid* fluid) const {
    std::cout << "=== Pipe Segment: " << pipeID << " ===" << std::endl;
    std::cout << "Horizontal Position: " << inletPosition << " m → " << outletPosition << " m" << std::endl;
    std::cout << "Length: " << length << " m" << std::endl;
    std::cout << "Diameter: " << diameter << " m" << std::endl;
    std::cout << "Roughness: " << roughness << " m" << std::endl;
    std::cout << "Elevation: " << inletElevation << " m → " << outletElevation << " m" << std::endl;
    std::cout << "Elevation Change: " << getElevationChange() << " m" << std::endl;
    std::cout << "Reynolds Number: " << getReynoldsNumber(fluid, inletPressure, inletTemp) << std::endl;
    std::cout << "Friction Factor: " << getFrictionFactor(fluid, inletPressure, inletTemp) << std::endl;
    std::cout << "\nInlet Conditions:" << std::endl;
    std::cout << "  Pressure: " << inletPressure/1000 << " kPa" << std::endl;
    std::cout << "  Temperature: " << inletTemp << " K" << std::endl;
    std::cout << "  Velocity: " << inletVelocity << " m/s" << std::endl;
    std::cout << "  Density: " << fluid->getDensity(inletPressure, inletTemp) << " kg/m³" << std::endl;
    std::cout << "\nOutlet Conditions:" << std::endl;
    std::cout << "  Pressure: " << outletPressure/1000 << " kPa" << std::endl;
    std::cout << "  Temperature: " << outletTemp << " K" << std::endl;
    std::cout << "  Velocity: " << outletVelocity << " m/s" << std::endl;
    std::cout << "  Density: " << fluid->getDensity(outletPressure, outletTemp) << " kg/m³" << std::endl;
    std::cout << "================================\n" << std::endl;
}
