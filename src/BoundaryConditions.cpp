#include "BoundaryConditions.h"
#include <cmath>
#include <iomanip>

BoundaryConditions::BoundaryConditions()
    : inletType(BCType::NONE),
      inletPressure(0.0),
      inletVelocity(0.0),
      inletMassFlowRate(0.0),
      inletVolFlowRate(0.0),
      inletTemperature(293.15),  // Default to 20°C
      outletType(BCType::NONE),
      outletPressure(0.0),
      outletVelocity(0.0),
      outletMassFlowRate(0.0),
      outletVolFlowRate(0.0),
      inletSet(false),
      outletSet(false) {}

// ============================================================================
// INLET BOUNDARY CONDITIONS
// ============================================================================

void BoundaryConditions::setInletPressureVelocity(double P, double T, double V) {
    if (P <= 0.0) {
        throw std::invalid_argument("Inlet pressure must be positive");
    }
    if (T <= 0.0) {
        throw std::invalid_argument("Temperature must be positive (in Kelvin)");
    }
    if (V < 0.0) {
        throw std::invalid_argument("Velocity cannot be negative");
    }
    
    inletType = BCType::VELOCITY;
    inletPressure = P;
    inletTemperature = T;
    inletVelocity = V;
    inletSet = true;
}

void BoundaryConditions::setInletPressure(double P, double T) {
    if (P <= 0.0) {
        throw std::invalid_argument("Inlet pressure must be positive");
    }
    if (T <= 0.0) {
        throw std::invalid_argument("Temperature must be positive (in Kelvin)");
    }
    
    inletType = BCType::PRESSURE;
    inletPressure = P;
    inletTemperature = T;
    inletSet = true;
}

void BoundaryConditions::setInletMassFlowRate(double mdot, double T) {
    if (mdot < 0.0) {
        throw std::invalid_argument("Mass flow rate cannot be negative");
    }
    if (T <= 0.0) {
        throw std::invalid_argument("Temperature must be positive (in Kelvin)");
    }
    
    inletType = BCType::MASS_FLOW_RATE;
    inletMassFlowRate = mdot;
    inletTemperature = T;
    inletSet = true;
}

void BoundaryConditions::setInletVolumetricFlowRate(double Q, double T) {
    if (Q < 0.0) {
        throw std::invalid_argument("Volumetric flow rate cannot be negative");
    }
    if (T <= 0.0) {
        throw std::invalid_argument("Temperature must be positive (in Kelvin)");
    }
    
    inletType = BCType::VOLUMETRIC_FLOW_RATE;
    inletVolFlowRate = Q;
    inletTemperature = T;
    inletSet = true;
}

// ============================================================================
// OUTLET BOUNDARY CONDITIONS
// ============================================================================

void BoundaryConditions::setOutletPressure(double P) {
    if (P <= 0.0) {
        throw std::invalid_argument("Outlet pressure must be positive");
    }
    
    outletType = BCType::PRESSURE;
    outletPressure = P;
    outletSet = true;
}

void BoundaryConditions::setOutletVelocity(double V) {
    if (V < 0.0) {
        throw std::invalid_argument("Velocity cannot be negative");
    }
    
    outletType = BCType::VELOCITY;
    outletVelocity = V;
    outletSet = true;
}

void BoundaryConditions::setOutletMassFlowRate(double mdot) {
    if (mdot < 0.0) {
        throw std::invalid_argument("Mass flow rate cannot be negative");
    }
    
    outletType = BCType::MASS_FLOW_RATE;
    outletMassFlowRate = mdot;
    outletSet = true;
}

void BoundaryConditions::setOutletVolumetricFlowRate(double Q) {
    if (Q < 0.0) {
        throw std::invalid_argument("Volumetric flow rate cannot be negative");
    }
    
    outletType = BCType::VOLUMETRIC_FLOW_RATE;
    outletVolFlowRate = Q;
    outletSet = true;
}

// ============================================================================
// VALIDATION AND UTILITIES
// ============================================================================

bool BoundaryConditions::isValid() const {
    // Both inlet and outlet must be set
    if (!inletSet || !outletSet) {
        return false;
    }
    
    // Check for invalid combinations
    // Cannot have both inlet and outlet as flow rates (over-specified)
    if ((inletType == BCType::MASS_FLOW_RATE || inletType == BCType::VOLUMETRIC_FLOW_RATE) &&
        (outletType == BCType::MASS_FLOW_RATE || outletType == BCType::VOLUMETRIC_FLOW_RATE)) {
        return false;
    }
    
    // Velocity-velocity is also over-specified for single pipe
    if (inletType == BCType::VELOCITY && outletType == BCType::VELOCITY) {
        return false;
    }
    
    return true;
}

std::string BoundaryConditions::getConfigurationString() const {
    std::string config = "BC Configuration: ";
    config += "Inlet(" + bcTypeToString(inletType) + ") → ";
    config += "Outlet(" + bcTypeToString(outletType) + ")";
    return config;
}

bool BoundaryConditions::isPressurePressureProblem() const {
    return (inletType == BCType::PRESSURE && outletType == BCType::PRESSURE);
}

bool BoundaryConditions::requiresIterativeSolution() const {
    // Pressure-pressure problems require iteration to find flow rate
    if (isPressurePressureProblem()) {
        return true;
    }
    
    // Any configuration where we need to find inlet/outlet velocity
    // from pressure BCs may require iteration for compressible flow
    if (inletType == BCType::PRESSURE || outletType == BCType::PRESSURE) {
        return true;
    }
    
    return false;
}

void BoundaryConditions::display() const {
    std::cout << "\n=== Boundary Conditions ===" << std::endl;
    std::cout << std::fixed << std::setprecision(4);
    
    // Inlet
    std::cout << "\nInlet:" << std::endl;
    std::cout << "  Type: " << bcTypeToString(inletType) << std::endl;
    std::cout << "  Temperature: " << inletTemperature << " K ("
              << (inletTemperature - 273.15) << " °C)" << std::endl;
    
    switch (inletType) {
        case BCType::PRESSURE:
            std::cout << "  Pressure: " << inletPressure / 1000.0 << " kPa" << std::endl;
            break;
        case BCType::VELOCITY:
            std::cout << "  Pressure: " << inletPressure / 1000.0 << " kPa" << std::endl;
            std::cout << "  Velocity: " << inletVelocity << " m/s" << std::endl;
            break;
        case BCType::MASS_FLOW_RATE:
            std::cout << "  Mass Flow Rate: " << inletMassFlowRate << " kg/s" << std::endl;
            break;
        case BCType::VOLUMETRIC_FLOW_RATE:
            std::cout << "  Volumetric Flow Rate: " << inletVolFlowRate << " m³/s" << std::endl;
            break;
        case BCType::NONE:
            std::cout << "  [Not Set]" << std::endl;
            break;
    }
    
    // Outlet
    std::cout << "\nOutlet:" << std::endl;
    std::cout << "  Type: " << bcTypeToString(outletType) << std::endl;
    
    switch (outletType) {
        case BCType::PRESSURE:
            std::cout << "  Pressure: " << outletPressure / 1000.0 << " kPa" << std::endl;
            break;
        case BCType::VELOCITY:
            std::cout << "  Velocity: " << outletVelocity << " m/s" << std::endl;
            break;
        case BCType::MASS_FLOW_RATE:
            std::cout << "  Mass Flow Rate: " << outletMassFlowRate << " kg/s" << std::endl;
            break;
        case BCType::VOLUMETRIC_FLOW_RATE:
            std::cout << "  Volumetric Flow Rate: " << outletVolFlowRate << " m³/s" << std::endl;
            break;
        case BCType::NONE:
            std::cout << "  [Not Set]" << std::endl;
            break;
    }
    
    // Validation status
    std::cout << "\nValidation:" << std::endl;
    std::cout << "  Configuration: " << getConfigurationString() << std::endl;
    std::cout << "  Valid: " << (isValid() ? "Yes" : "No") << std::endl;
    std::cout << "  Requires Iteration: " << (requiresIterativeSolution() ? "Yes" : "No") << std::endl;
    
    if (isPressurePressureProblem()) {
        std::cout << "  Note: Pressure-Pressure problem - flow rate will be calculated" << std::endl;
    }
    
    std::cout << "===========================\n" << std::endl;
}

std::string BoundaryConditions::bcTypeToString(BCType type) {
    switch (type) {
        case BCType::PRESSURE: return "Pressure";
        case BCType::VELOCITY: return "Velocity";
        case BCType::MASS_FLOW_RATE: return "Mass Flow Rate";
        case BCType::VOLUMETRIC_FLOW_RATE: return "Volumetric Flow Rate";
        case BCType::NONE: return "None";
        default: return "Unknown";
    }
}

// ============================================================================
// CONVERSION UTILITIES
// ============================================================================

double BoundaryConditions::massFlowRateToVelocity(double mdot, double rho, double area) {
    if (rho <= 0.0) {
        throw std::invalid_argument("Density must be positive");
    }
    if (area <= 0.0) {
        throw std::invalid_argument("Area must be positive");
    }
    return mdot / (rho * area);
}

double BoundaryConditions::volFlowRateToVelocity(double Q, double area) {
    if (area <= 0.0) {
        throw std::invalid_argument("Area must be positive");
    }
    return Q / area;
}

double BoundaryConditions::velocityToMassFlowRate(double V, double rho, double area) {
    if (rho <= 0.0) {
        throw std::invalid_argument("Density must be positive");
    }
    if (area <= 0.0) {
        throw std::invalid_argument("Area must be positive");
    }
    return rho * area * V;
}

double BoundaryConditions::velocityToVolFlowRate(double V, double area) {
    if (area <= 0.0) {
        throw std::invalid_argument("Area must be positive");
    }
    return area * V;
}
