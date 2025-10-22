#include "BoundaryConditions.h"
#include <iostream>

/**
 * @brief Example demonstrating various boundary condition configurations
 */
int main() {
    std::cout << "=== BoundaryConditions Class Examples ===\n" << std::endl;
    
    // ========================================================================
    // Example 1: Pressure-Velocity (Current Default)
    // ========================================================================
    std::cout << "Example 1: Pressure-Velocity (Current Default)\n";
    std::cout << "Use case: You know inlet pressure, velocity, and temperature" << std::endl;
    
    BoundaryConditions bc1;
    bc1.setInletPressureVelocity(1000000.0, 293.15, 2.0);  // 1 MPa, 20°C, 2 m/s
    bc1.setOutletPressure(500000.0);                        // 0.5 MPa
    
    bc1.display();
    
    // ========================================================================
    // Example 2: Pressure-Pressure (Find Flow Rate)
    // ========================================================================
    std::cout << "\nExample 2: Pressure-Pressure (Flow Rate Unknown)\n";
    std::cout << "Use case: You know inlet and outlet pressures, want to find flow rate" << std::endl;
    
    BoundaryConditions bc2;
    bc2.setInletPressure(2000000.0, 293.15);   // 2 MPa, 20°C
    bc2.setOutletPressure(1500000.0);          // 1.5 MPa
    
    bc2.display();
    
    if (bc2.isPressurePressureProblem()) {
        std::cout << "NOTE: This configuration requires iterative solution to find flow rate!\n";
    }
    
    // ========================================================================
    // Example 3: Mass Flow Rate Specification
    // ========================================================================
    std::cout << "\nExample 3: Mass Flow Rate at Inlet\n";
    std::cout << "Use case: You know the mass flow rate (e.g., from pump specification)" << std::endl;
    
    BoundaryConditions bc3;
    bc3.setInletMassFlowRate(10.0, 293.15);    // 10 kg/s, 20°C
    bc3.setOutletPressure(100000.0);           // 100 kPa
    
    bc3.display();
    
    // ========================================================================
    // Example 4: Volumetric Flow Rate Specification
    // ========================================================================
    std::cout << "\nExample 4: Volumetric Flow Rate at Inlet\n";
    std::cout << "Use case: Flow meter measures volumetric flow" << std::endl;
    
    BoundaryConditions bc4;
    bc4.setInletVolumetricFlowRate(0.05, 293.15);  // 50 L/s = 0.05 m³/s, 20°C
    bc4.setOutletPressure(200000.0);               // 200 kPa
    
    bc4.display();
    
    // ========================================================================
    // Example 5: Conversion Utilities
    // ========================================================================
    std::cout << "\nExample 5: Using Conversion Utilities\n";
    std::cout << "Converting between flow rate representations..." << std::endl;
    
    double diameter = 0.3;  // 300mm pipe
    double area = M_PI * diameter * diameter / 4.0;
    double rho = 1000.0;    // Water density
    double velocity = 2.0;  // 2 m/s
    
    double mdot = BoundaryConditions::velocityToMassFlowRate(velocity, rho, area);
    double Q = BoundaryConditions::velocityToVolFlowRate(velocity, area);
    
    std::cout << "Given: D = " << diameter << " m, V = " << velocity << " m/s, ρ = " 
              << rho << " kg/m³\n";
    std::cout << "Calculated:\n";
    std::cout << "  Area: " << area << " m²\n";
    std::cout << "  Mass flow rate: " << mdot << " kg/s\n";
    std::cout << "  Volumetric flow rate: " << Q << " m³/s (" << Q*1000 << " L/s)\n";
    
    // Convert back
    double v_from_mdot = BoundaryConditions::massFlowRateToVelocity(mdot, rho, area);
    double v_from_Q = BoundaryConditions::volFlowRateToVelocity(Q, area);
    
    std::cout << "Verification:\n";
    std::cout << "  Velocity from mdot: " << v_from_mdot << " m/s\n";
    std::cout << "  Velocity from Q: " << v_from_Q << " m/s\n";
    
    // ========================================================================
    // Example 6: Invalid Configurations (will be caught)
    // ========================================================================
    std::cout << "\n\nExample 6: Invalid Configurations\n";
    std::cout << "Testing validation...\n" << std::endl;
    
    // Over-specified: flow rate at both ends
    BoundaryConditions bc_invalid1;
    bc_invalid1.setInletMassFlowRate(10.0, 293.15);
    bc_invalid1.setOutletMassFlowRate(10.0);
    
    std::cout << "Configuration: Mass flow rate at both inlet and outlet\n";
    std::cout << "Valid: " << (bc_invalid1.isValid() ? "Yes" : "No") 
              << " (Expected: No - over-specified)\n";
    
    // Valid configuration for comparison
    BoundaryConditions bc_valid;
    bc_valid.setInletPressureVelocity(1000000.0, 293.15, 2.0);
    bc_valid.setOutletPressure(500000.0);
    
    std::cout << "\nConfiguration: Pressure-Velocity inlet, Pressure outlet\n";
    std::cout << "Valid: " << (bc_valid.isValid() ? "Yes" : "No") 
              << " (Expected: Yes)\n";
    
    // ========================================================================
    // Example 7: Error Handling
    // ========================================================================
    std::cout << "\n\nExample 7: Error Handling\n";
    std::cout << "Testing exception handling...\n" << std::endl;
    
    try {
        BoundaryConditions bc_error;
        bc_error.setInletPressureVelocity(-1000000.0, 293.15, 2.0);  // Negative pressure
    } catch (const std::invalid_argument& e) {
        std::cout << "✓ Caught expected error: " << e.what() << std::endl;
    }
    
    try {
        BoundaryConditions bc_error;
        bc_error.setInletMassFlowRate(-5.0, 293.15);  // Negative flow rate
    } catch (const std::invalid_argument& e) {
        std::cout << "✓ Caught expected error: " << e.what() << std::endl;
    }
    
    try {
        BoundaryConditions bc_error;
        bc_error.setInletPressure(1000000.0, -100.0);  // Negative temperature
    } catch (const std::invalid_argument& e) {
        std::cout << "✓ Caught expected error: " << e.what() << std::endl;
    }
    
    std::cout << "\n=== All Examples Complete ===" << std::endl;
    
    return 0;
}
