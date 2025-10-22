# BoundaryConditions Class - Complete Package

## 📦 What's Included

This package provides a flexible boundary conditions system for your C++ pipeline simulator.

### Files
1. **BoundaryConditions.h** - Header file with class declaration
2. **BoundaryConditions.cpp** - Implementation
3. **example_boundary_conditions.cpp** - 7 comprehensive examples
4. **INTEGRATION_GUIDE.md** - Detailed integration instructions
5. **Makefile.bc_test** - Standalone test Makefile
6. **README.md** - This file

## 🚀 Quick Start

### 1. Test Standalone (Recommended First Step)

```bash
# Copy files to your project directory
cp BoundaryConditions.h BoundaryConditions.cpp example_boundary_conditions.cpp .

# Compile and test
make -f Makefile.bc_test run
```

This will demonstrate all boundary condition types and validate the implementation.

### 2. Basic Usage

```cpp
#include "BoundaryConditions.h"

// Create boundary conditions
BoundaryConditions bc;

// Set inlet: pressure + velocity + temperature
bc.setInletPressureVelocity(1000000.0, 293.15, 2.0);  // 1 MPa, 20°C, 2 m/s

// Set outlet: pressure
bc.setOutletPressure(500000.0);  // 0.5 MPa

// Validate
if (bc.isValid()) {
    bc.display();  // Print configuration
}
```

## 🎯 Supported Boundary Condition Types

### Inlet Types
✅ **Pressure + Velocity** - Your current default  
✅ **Pressure only** - Flow rate to be calculated  
✅ **Mass flow rate** - From pump specifications (kg/s)  
✅ **Volumetric flow rate** - From flow meters (m³/s)  

### Outlet Types
✅ **Pressure** - Most common (atmospheric or specified)  
✅ **Velocity** - Specified exit velocity  
✅ **Mass flow rate** - Specified exit mass flow  
✅ **Volumetric flow rate** - Specified exit volumetric flow  

## 📊 Configuration Examples

### Example 1: Current Default (Backwards Compatible)
```cpp
bc.setInletPressureVelocity(1000000.0, 293.15, 2.0);  // P, T, V
bc.setOutletPressure(500000.0);
// This matches your existing Pipeline::setInletConditions()
```

### Example 2: Pressure-Pressure (Find Flow Rate)
```cpp
bc.setInletPressure(7000000.0, 288.15);   // 7 MPa, 15°C
bc.setOutletPressure(5000000.0);          // 5 MPa
// Flow rate unknown - requires iterative solver
```

### Example 3: Mass Flow Rate
```cpp
bc.setInletMassFlowRate(50.0, 293.15);    // 50 kg/s from pump
bc.setOutletPressure(101325.0);           // Atmospheric
```

### Example 4: Volumetric Flow Rate
```cpp
bc.setInletVolumetricFlowRate(0.1, 293.15);  // 100 L/s
bc.setOutletPressure(200000.0);              // 200 kPa
```

## ✅ Validation Rules

The class automatically validates configurations:

**Valid Combinations:**
- ✅ Pressure-Velocity inlet + Pressure outlet (current)
- ✅ Pressure inlet + Pressure outlet (find flow)
- ✅ Mass/Vol flow inlet + Pressure outlet
- ✅ Pressure-Velocity inlet + Velocity outlet (special cases)

**Invalid Combinations:**
- ❌ Mass flow inlet + Mass flow outlet (over-specified)
- ❌ Velocity inlet + Velocity outlet (over-specified)
- ❌ Negative pressures, temperatures, or flow rates

## 🔧 Integration Options

### Option 1: Minimal (Backwards Compatible)
Keep your existing `Pipeline::setInletConditions()` and add:
```cpp
void Pipeline::setBoundaryConditions(const BoundaryConditions& bc);
```

### Option 2: Full Integration
Replace inlet condition handling with BoundaryConditions system.

See **INTEGRATION_GUIDE.md** for detailed instructions.

## 📝 Key Features

1. **Type Safety** - Enum-based boundary condition types
2. **Validation** - Automatic checking of valid configurations
3. **Conversion Utilities** - Convert between velocity/flow rate representations
4. **Error Handling** - Exceptions for invalid inputs
5. **Display Methods** - Pretty-print configurations
6. **Detection Methods** - Identify special cases (e.g., pressure-pressure problems)

## 🔍 API Highlights

### Setting Boundary Conditions
```cpp
// Inlet
bc.setInletPressureVelocity(P, T, V);
bc.setInletPressure(P, T);
bc.setInletMassFlowRate(mdot, T);
bc.setInletVolumetricFlowRate(Q, T);

// Outlet
bc.setOutletPressure(P);
bc.setOutletVelocity(V);
bc.setOutletMassFlowRate(mdot);
bc.setOutletVolumetricFlowRate(Q);
```

### Querying Configuration
```cpp
bool bc.isValid();                      // Check if valid
bool bc.isPressurePressureProblem();    // Requires iteration?
bool bc.requiresIterativeSolution();    // Needs outer iteration?
std::string bc.getConfigurationString(); // Get description
```

### Conversion Utilities (Static Methods)
```cpp
double v = BoundaryConditions::massFlowRateToVelocity(mdot, rho, area);
double v = BoundaryConditions::volFlowRateToVelocity(Q, area);
double mdot = BoundaryConditions::velocityToMassFlowRate(v, rho, area);
double Q = BoundaryConditions::velocityToVolFlowRate(v, area);
```

## 🧪 Testing

Run the comprehensive example:
```bash
make -f Makefile.bc_test run
```

This demonstrates:
1. Pressure-Velocity configuration
2. Pressure-Pressure configuration
3. Mass flow rate specification
4. Volumetric flow rate specification
5. Conversion utilities
6. Invalid configuration detection
7. Error handling

## 🎓 When to Use Each Type

### Pressure-Velocity Inlet (Current Default)
**Use when:** You know inlet pressure, temperature, and velocity  
**Good for:** Laboratory setups, controlled experiments  
**Solver:** SimpleSolver, IterativeSolver, RK4Solver all work  

### Pressure-Pressure
**Use when:** You know inlet and outlet pressures, want to find flow rate  
**Good for:** Gravity-driven flow, transmission pipelines  
**Solver:** Requires IterativeSolver or AdaptiveSolver  

### Mass Flow Rate Inlet
**Use when:** Pump delivers specified mass flow rate  
**Good for:** Pumped systems, process control  
**Solver:** Any solver works (converts to velocity)  

### Volumetric Flow Rate Inlet
**Use when:** Flow meter measures volumetric flow  
**Good for:** Systems with flow measurement  
**Solver:** Any solver works (converts to velocity)  

## 🔄 Integration with Your Pipeline

Your current code:
```cpp
Pipeline pipeline("MyPipeline");
pipeline.addPipeSegment(...);
pipeline.setInletConditions(1000000.0, 293.15, 2.0);
pipeline.solveAll(&solver, &fluid);
```

With BoundaryConditions (backwards compatible):
```cpp
Pipeline pipeline("MyPipeline");
pipeline.addPipeSegment(...);

// Option A: Keep using old method (still works)
pipeline.setInletConditions(1000000.0, 293.15, 2.0);

// Option B: Use new BoundaryConditions (more flexible)
BoundaryConditions bc;
bc.setInletPressureVelocity(1000000.0, 293.15, 2.0);
bc.setOutletPressure(500000.0);  // Can specify outlet
pipeline.setBoundaryConditions(bc);

pipeline.solveAll(&solver, &fluid);
```

## 📚 Documentation

- **INTEGRATION_GUIDE.md** - Step-by-step integration instructions
- **Header comments** - Complete API documentation in BoundaryConditions.h
- **Example code** - 7 examples in example_boundary_conditions.cpp

## 🛠️ Requirements

- C++17 or later
- Standard library only (no external dependencies)
- Compatible with your existing codebase

## 📦 Adding to Your Project

### For Your Existing Makefile

Add to COMMON_SOURCES:
```makefile
COMMON_SOURCES = ... \
                 $(SRCDIR)/BoundaryConditions.cpp
```

Then use in your code:
```cpp
#include "BoundaryConditions.h"
```

### Standalone Testing First
```bash
# Test independently before integrating
g++ -std=c++17 -Wall -Wextra BoundaryConditions.cpp example_boundary_conditions.cpp -o test_bc
./test_bc
```

## 🎯 Benefits

1. **Flexibility** - Support multiple problem types
2. **Future-proof** - Easy to add new BC types
3. **Self-documenting** - Clear API, automatic validation
4. **Backwards compatible** - Old code still works
5. **Robust** - Input validation and error handling
6. **Production-ready** - Thoroughly tested and documented

## 🔮 Future Enhancements

The class is designed to be extensible:

- **Heat flux boundaries** - For non-isothermal problems
- **Mixed boundaries** - Combinations at single location
- **Time-dependent BCs** - For transient problems
- **Network BCs** - Multiple inlets/outlets
- **Control strategies** - PID controllers, etc.

## 💡 Pro Tips

1. **Always validate** - Call `bc.isValid()` before using
2. **Check solver requirements** - Use `bc.requiresIterativeSolution()`
3. **Display for debugging** - Use `bc.display()` to inspect configuration
4. **Use conversion utilities** - Don't reinvent flow rate calculations
5. **Test standalone first** - Run examples before integrating

## 🤝 Contributing

This is part of your pipeline simulator project. Feel free to extend the class with additional boundary condition types as needed!

## 📞 Support

Refer to:
- INTEGRATION_GUIDE.md for detailed integration steps
- example_boundary_conditions.cpp for usage patterns
- BoundaryConditions.h for complete API reference

## 🎉 Summary

You now have a production-ready boundary conditions system that:
- ✅ Supports 4 inlet types and 4 outlet types
- ✅ Validates configurations automatically
- ✅ Integrates cleanly with your existing code
- ✅ Provides clear error messages
- ✅ Is fully documented and tested
- ✅ Is extensible for future needs

**Next step:** Run `make -f Makefile.bc_test run` to see it in action! 🚀
