# BoundaryConditions Quick Reference Card

## 🎯 Common Patterns

### Pattern 1: Standard Pressure-Velocity (Current Default)
```cpp
BoundaryConditions bc;
bc.setInletPressureVelocity(1e6, 293.15, 2.0);  // 1 MPa, 20°C, 2 m/s
bc.setOutletPressure(5e5);                       // 0.5 MPa
```

### Pattern 2: Find Flow Rate (Pressure-Pressure)
```cpp
BoundaryConditions bc;
bc.setInletPressure(7e6, 288.15);    // 7 MPa, 15°C
bc.setOutletPressure(5e6);           // 5 MPa
// Requires IterativeSolver or AdaptiveSolver
```

### Pattern 3: Pump with Known Mass Flow
```cpp
BoundaryConditions bc;
bc.setInletMassFlowRate(50.0, 293.15);  // 50 kg/s
bc.setOutletPressure(1e5);              // Atmospheric
```

### Pattern 4: Flow Meter at Inlet
```cpp
BoundaryConditions bc;
bc.setInletVolumetricFlowRate(0.1, 293.15);  // 100 L/s
bc.setOutletPressure(2e5);                   // 200 kPa
```

## 📋 API Cheat Sheet

### Inlet Methods
| Method | Parameters | Use Case |
|--------|-----------|----------|
| `setInletPressureVelocity()` | P, T, V | Standard (current default) |
| `setInletPressure()` | P, T | Flow rate unknown |
| `setInletMassFlowRate()` | ṁ, T | Pump specification |
| `setInletVolumetricFlowRate()` | Q, T | Flow meter reading |

### Outlet Methods
| Method | Parameter | Use Case |
|--------|-----------|----------|
| `setOutletPressure()` | P | Most common |
| `setOutletVelocity()` | V | Specified exit |
| `setOutletMassFlowRate()` | ṁ | Mass flow control |
| `setOutletVolumetricFlowRate()` | Q | Volume flow control |

### Query Methods
| Method | Returns | Purpose |
|--------|---------|---------|
| `isValid()` | bool | Check if config is valid |
| `isPressurePressureProblem()` | bool | Is flow rate unknown? |
| `requiresIterativeSolution()` | bool | Need outer iteration? |
| `getConfigurationString()` | string | Get description |
| `display()` | void | Print full config |

### Conversion Utilities (Static)
```cpp
// To velocity
double v = BoundaryConditions::massFlowRateToVelocity(mdot, rho, area);
double v = BoundaryConditions::volFlowRateToVelocity(Q, area);

// From velocity
double mdot = BoundaryConditions::velocityToMassFlowRate(v, rho, area);
double Q = BoundaryConditions::velocityToVolFlowRate(v, area);
```

## ✅ Valid Combinations Matrix

| Inlet Type | Outlet Type | Valid? | Notes |
|------------|-------------|--------|-------|
| Pressure+Vel | Pressure | ✅ | Current default |
| Pressure | Pressure | ✅ | Find flow rate (needs iteration) |
| Mass Flow | Pressure | ✅ | Convert to velocity |
| Vol Flow | Pressure | ✅ | Convert to velocity |
| Pressure+Vel | Velocity | ✅ | Special cases only |
| Mass Flow | Mass Flow | ❌ | Over-specified |
| Vol Flow | Vol Flow | ❌ | Over-specified |
| Velocity | Velocity | ❌ | Over-specified |

## 🔧 Solver Requirements

| BC Type | SimpleSolver | IterativeSolver | AdaptiveSolver | RK4Solver |
|---------|--------------|-----------------|----------------|-----------|
| Press+Vel → Press | ✅ | ✅ | ✅ | ✅ |
| Press → Press | ❌ | ✅ | ✅ | ✅* |
| Flow → Press | ✅ | ✅ | ✅ | ✅ |

*RK4Solver needs outer iteration loop for pressure-pressure

## 🎓 Decision Tree

```
Do you know inlet velocity?
├─ YES → Use setInletPressureVelocity()
│        (current default)
│
└─ NO → Do you know mass flow rate?
         ├─ YES → Use setInletMassFlowRate()
         │
         └─ NO → Do you know volumetric flow rate?
                  ├─ YES → Use setInletVolumetricFlowRate()
                  │
                  └─ NO → Use setInletPressure()
                           (flow rate will be calculated)
```

## 💻 Code Template

```cpp
#include "BoundaryConditions.h"

// Create and configure
BoundaryConditions bc;

// Set inlet (choose one)
bc.setInletPressureVelocity(P, T, V);  // Option A
// bc.setInletPressure(P, T);          // Option B
// bc.setInletMassFlowRate(mdot, T);   // Option C
// bc.setInletVolumetricFlowRate(Q, T); // Option D

// Set outlet
bc.setOutletPressure(P_out);

// Validate
if (!bc.isValid()) {
    std::cerr << "Invalid BC configuration!\n";
    return;
}

// Check requirements
if (bc.requiresIterativeSolution()) {
    std::cout << "Note: Requires iterative solver\n";
}

// Display configuration
bc.display();

// Use with Pipeline (after integration)
pipeline.setBoundaryConditions(bc);
```

## 🚨 Common Errors

### Error 1: Negative Pressure
```cpp
bc.setInletPressure(-1000.0, 293.15);  // ❌ THROWS!
```
**Fix:** Use positive pressure (Pa)

### Error 2: Negative Temperature
```cpp
bc.setInletPressure(1e6, -100.0);  // ❌ THROWS!
```
**Fix:** Use Kelvin (not Celsius!)

### Error 3: Over-specified
```cpp
bc.setInletMassFlowRate(10.0, 293.15);
bc.setOutletMassFlowRate(10.0);  // ❌ Over-specified!
```
**Fix:** Use pressure outlet instead

### Error 4: Not Validated
```cpp
BoundaryConditions bc;
// ... set conditions ...
pipeline.setBoundaryConditions(bc);  // May fail if invalid
```
**Fix:** Check `bc.isValid()` first

## 📐 Unit Reference

| Quantity | SI Unit | Typical Range |
|----------|---------|---------------|
| Pressure | Pa | 1e5 - 1e7 (0.1-10 MPa) |
| Temperature | K | 273-373 (0-100°C) |
| Velocity | m/s | 0.5-10 |
| Mass Flow | kg/s | 1-1000 |
| Vol Flow | m³/s | 0.001-1 |
| Area | m² | πD²/4 |
| Density | kg/m³ | 1-1000 |

## 🔄 Conversion Examples

### Example 1: Flow Meter Reading
You have: 180 m³/hr  
Convert to: 0.05 m³/s  
```cpp
double Q = 180.0 / 3600.0;  // Convert to m³/s
bc.setInletVolumetricFlowRate(Q, 293.15);
```

### Example 2: Pump Specification
You have: 50 kg/s at 20°C  
```cpp
bc.setInletMassFlowRate(50.0, 293.15);
```

### Example 3: Pressure in bar
You have: 70 bar  
Convert to: 7e6 Pa  
```cpp
double P = 70.0 * 1e5;  // bar to Pa
bc.setInletPressure(P, 288.15);
```

## 📊 Typical Values

### Water System
```cpp
bc.setInletPressureVelocity(5e5, 293.15, 2.0);  // 500 kPa, 20°C, 2 m/s
bc.setOutletPressure(1e5);                       // Atmospheric
```

### Gas Transmission
```cpp
bc.setInletPressure(7e6, 288.15);   // 70 bar, 15°C
bc.setOutletPressure(5e6);           // 50 bar
```

### Pumped System
```cpp
bc.setInletMassFlowRate(100.0, 293.15);  // 100 kg/s
bc.setOutletPressure(2e5);                // 200 kPa
```

## 🎯 Integration Checklist

- [ ] Copy BoundaryConditions.h and .cpp to project
- [ ] Update Makefile to compile new files
- [ ] Test standalone with examples
- [ ] Add setBoundaryConditions() to Pipeline
- [ ] Keep setInletConditions() for backwards compatibility
- [ ] Update documentation
- [ ] Add unit tests
- [ ] Update README.md

## 📱 Quick Test

```bash
# Compile standalone test
g++ -std=c++17 BoundaryConditions.cpp example_boundary_conditions.cpp -o test
./test

# Or use provided Makefile
make -f Makefile.bc_test run
```

## 🎓 Best Practices

1. ✅ Always call `isValid()` before use
2. ✅ Check `requiresIterativeSolution()` for solver choice
3. ✅ Use static conversion methods for flow rate conversions
4. ✅ Display configuration with `display()` for debugging
5. ✅ Handle exceptions for invalid inputs
6. ✅ Use descriptive variable names
7. ✅ Document BC choices in code comments

## 📚 Further Reading

- README.md - Complete documentation
- INTEGRATION_GUIDE.md - Integration with Pipeline
- BoundaryConditions.h - Full API reference
- example_boundary_conditions.cpp - 7 examples

---

**Remember:** This reference card is for quick lookup. See full documentation for detailed explanations!
