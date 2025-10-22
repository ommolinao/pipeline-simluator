# BoundaryConditions Integration Guide

## Overview

The `BoundaryConditions` class provides a flexible way to specify inlet and outlet conditions for your pipeline simulator. This guide shows how to integrate it with your existing `Pipeline` class.

## Features

### Supported Boundary Condition Types

**Inlet:**
- **Pressure + Velocity** (current default) - You specify P, T, V
- **Pressure only** - Flow rate will be calculated
- **Mass flow rate** - You specify ṁ, T
- **Volumetric flow rate** - You specify Q, T

**Outlet:**
- **Pressure** - Most common (pressure-driven flow)
- **Velocity** - Specify exit velocity
- **Mass flow rate** - Specify exit mass flow
- **Volumetric flow rate** - Specify exit volumetric flow

### Valid Combinations

✅ **Pressure-Velocity inlet + Pressure outlet** (current default)
✅ **Pressure inlet + Pressure outlet** (find flow rate)
✅ **Mass flow inlet + Pressure outlet**
✅ **Volumetric flow inlet + Pressure outlet**
✅ **Pressure-Velocity inlet + Velocity outlet** (for specific cases)

❌ **Mass flow inlet + Mass flow outlet** (over-specified)
❌ **Velocity inlet + Velocity outlet** (over-specified for single pipe)

## Integration with Pipeline Class

### Option 1: Minimal Changes (Backwards Compatible)

Keep your existing `Pipeline::setInletConditions()` method, but add support for `BoundaryConditions`:

```cpp
// In Pipeline.h
class Pipeline {
private:
    // ... existing members ...
    
public:
    // Existing method (keep for backwards compatibility)
    void setInletConditions(double P, double T, double V);
    
    // New method using BoundaryConditions
    void setBoundaryConditions(const BoundaryConditions& bc);
    
    // Get current boundary conditions
    BoundaryConditions getBoundaryConditions() const;
};
```

```cpp
// In Pipeline.cpp
void Pipeline::setBoundaryConditions(const BoundaryConditions& bc) {
    if (!bc.isValid()) {
        throw std::invalid_argument("Invalid boundary conditions");
    }
    
    // Handle inlet based on type
    if (numSegments > 0) {
        inletTemps[0] = bc.getInletTemperature();
        
        switch (bc.getInletType()) {
            case BCType::PRESSURE:
                inletPressures[0] = bc.getInletPressure();
                // Velocity will be calculated during solve
                break;
                
            case BCType::VELOCITY:
                inletPressures[0] = bc.getInletPressure();
                inletVelocities[0] = bc.getInletVelocity();
                break;
                
            case BCType::MASS_FLOW_RATE:
                // Convert to velocity using first segment diameter
                double area = M_PI * diameters[0] * diameters[0] / 4.0;
                // Need fluid density - this requires modification to accept Fluid*
                // Or store BC and apply during solve when fluid is known
                break;
                
            case BCType::VOLUMETRIC_FLOW_RATE:
                double area = M_PI * diameters[0] * diameters[0] / 4.0;
                inletVelocities[0] = bc.getInletVolFlowRate() / area;
                break;
        }
    }
}
```

### Option 2: Enhanced Pipeline with BC Support

For full integration, modify `Pipeline` to store and use `BoundaryConditions`:

```cpp
// In Pipeline.h
#include "BoundaryConditions.h"

class Pipeline {
private:
    // ... existing members ...
    BoundaryConditions boundaryConditions;
    
public:
    // New interface
    void setBoundaryConditions(const BoundaryConditions& bc);
    const BoundaryConditions& getBoundaryConditions() const;
    
    // Modified solve to respect boundary conditions
    void solveAll(Solver* solver, Fluid* fluid);
    
    // Keep old interface for backwards compatibility
    void setInletConditions(double P, double T, double V);
};
```

```cpp
// In Pipeline.cpp
void Pipeline::setBoundaryConditions(const BoundaryConditions& bc) {
    if (!bc.isValid()) {
        throw std::invalid_argument("Invalid boundary conditions");
    }
    boundaryConditions = bc;
}

void Pipeline::solveAll(Solver* solver, Fluid* fluid) {
    // Apply inlet boundary conditions
    applyInletBC(fluid);
    
    // ... existing solve loop ...
    
    // Apply outlet boundary conditions (if needed)
    applyOutletBC(fluid);
}

void Pipeline::applyInletBC(Fluid* fluid) {
    if (numSegments == 0) return;
    
    inletTemps[0] = boundaryConditions.getInletTemperature();
    
    switch (boundaryConditions.getInletType()) {
        case BCType::PRESSURE:
            inletPressures[0] = boundaryConditions.getInletPressure();
            // Velocity unknown - will be calculated iteratively
            break;
            
        case BCType::VELOCITY:
            inletPressures[0] = boundaryConditions.getInletPressure();
            inletVelocities[0] = boundaryConditions.getInletVelocity();
            break;
            
        case BCType::MASS_FLOW_RATE: {
            double mdot = boundaryConditions.getInletMassFlowRate();
            double area = M_PI * diameters[0] * diameters[0] / 4.0;
            double rho = fluid->getDensity(inletPressures[0], inletTemps[0]);
            inletVelocities[0] = mdot / (rho * area);
            break;
        }
            
        case BCType::VOLUMETRIC_FLOW_RATE: {
            double Q = boundaryConditions.getInletVolFlowRate();
            double area = M_PI * diameters[0] * diameters[0] / 4.0;
            inletVelocities[0] = Q / area;
            break;
        }
    }
}
```

## Usage Examples

### Example 1: Current Default (No Changes Needed)

```cpp
Pipeline pipeline("MyPipeline");
// ... add segments ...

// Old way (still works)
pipeline.setInletConditions(1000000.0, 293.15, 2.0);

// New way (equivalent)
BoundaryConditions bc;
bc.setInletPressureVelocity(1000000.0, 293.15, 2.0);
bc.setOutletPressure(500000.0);  // Optional - can be calculated
pipeline.setBoundaryConditions(bc);
```

### Example 2: Pressure-Pressure Problem

```cpp
Pipeline pipeline("TransmissionLine");
// ... add segments ...

BoundaryConditions bc;
bc.setInletPressure(7000000.0, 288.15);   // 7 MPa, 15°C
bc.setOutletPressure(5000000.0);          // 5 MPa

pipeline.setBoundaryConditions(bc);

// Will require iterative solver to find flow rate
IterativeSolver solver(100, 0.1);
pipeline.solveAll(&solver, &naturalGas);

// After solving, can query results
double flowRate = pipeline.getInletVelocity(0) * area * density;
```

### Example 3: Mass Flow Rate Specification

```cpp
Pipeline pipeline("PumpedSystem");
// ... add segments ...

BoundaryConditions bc;
bc.setInletMassFlowRate(50.0, 293.15);   // 50 kg/s from pump
bc.setOutletPressure(101325.0);          // Atmospheric outlet

pipeline.setBoundaryConditions(bc);
pipeline.solveAll(&solver, &water);
```

### Example 4: Volumetric Flow Rate

```cpp
Pipeline pipeline("MeasuredFlow");
// ... add segments ...

BoundaryConditions bc;
bc.setInletVolumetricFlowRate(0.1, 293.15);  // 100 L/s from flow meter
bc.setOutletPressure(200000.0);              // 200 kPa discharge

pipeline.setBoundaryConditions(bc);
pipeline.solveAll(&solver, &liquid);
```

## Solver Requirements

Different boundary condition types may require different solvers:

### Pressure-Velocity → Pressure
- **SimpleSolver**: ✓ Works
- **IterativeSolver**: ✓ Better accuracy
- **AdaptiveSolver**: ✓ Best accuracy
- **RK4Solver**: ✓ Highest accuracy

### Pressure → Pressure (find flow rate)
- **SimpleSolver**: ❌ Not suitable (needs iteration)
- **IterativeSolver**: ✓ Required
- **AdaptiveSolver**: ✓ Recommended
- **RK4Solver**: ✓ Can be used with outer iteration

### Flow Rate → Pressure
- **SimpleSolver**: ✓ Works (flow rate converts to velocity)
- **IterativeSolver**: ✓ Better for compressible flow
- **AdaptiveSolver**: ✓ Best accuracy
- **RK4Solver**: ✓ Highest accuracy

## Migration Path

### Phase 1: Add BoundaryConditions class
1. ✅ Add `BoundaryConditions.h` and `.cpp` to project
2. ✅ Update Makefile to compile new files
3. ✅ Test standalone with `example_boundary_conditions.cpp`

### Phase 2: Pipeline Integration (Optional)
1. Add `#include "BoundaryConditions.h"` to `Pipeline.h`
2. Add `BoundaryConditions boundaryConditions;` member
3. Add `setBoundaryConditions()` method
4. Keep existing `setInletConditions()` for backwards compatibility

### Phase 3: Solver Enhancement (Future)
1. Modify solvers to handle pressure-pressure problems
2. Add iterative outer loop for unknown flow rates
3. Implement convergence criteria for flow rate iteration

## Benefits

✅ **Flexibility**: Support multiple boundary condition types
✅ **Validation**: Automatic checking of valid configurations
✅ **Clarity**: Clear specification of what's known vs unknown
✅ **Future-proof**: Easy to add new BC types
✅ **Backwards compatible**: Old code still works

## API Reference

See `BoundaryConditions.h` for complete API documentation.

Key methods:
- `setInletPressureVelocity()` - Current default
- `setInletPressure()` - For pressure-pressure problems
- `setInletMassFlowRate()` - From pump specifications
- `setInletVolumetricFlowRate()` - From flow meters
- `setOutletPressure()` - Most common outlet BC
- `isValid()` - Check configuration validity
- `isPressurePressureProblem()` - Requires iteration
- `display()` - Print configuration

## Next Steps

1. Compile and test `BoundaryConditions` standalone
2. Run `example_boundary_conditions` to understand usage
3. Decide on integration strategy (Option 1 or 2)
4. Update Pipeline class incrementally
5. Add tests for different BC combinations
6. Update documentation and README

## Questions?

The class is designed to be intuitive and self-documenting. Check the examples and try different configurations!
