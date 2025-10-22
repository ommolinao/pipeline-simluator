# BoundaryConditions Class Architecture

## Class Structure

```
┌─────────────────────────────────────────────────────────────────┐
│                    BoundaryConditions                            │
├─────────────────────────────────────────────────────────────────┤
│ Private Members:                                                 │
│   - BCType inletType                                            │
│   - BCType outletType                                           │
│   - double inletPressure                                        │
│   - double inletVelocity                                        │
│   - double inletMassFlowRate                                    │
│   - double inletVolFlowRate                                     │
│   - double inletTemperature                                     │
│   - double outletPressure                                       │
│   - double outletVelocity                                       │
│   - double outletMassFlowRate                                   │
│   - double outletVolFlowRate                                    │
│   - bool inletSet                                               │
│   - bool outletSet                                              │
├─────────────────────────────────────────────────────────────────┤
│ Public Methods:                                                  │
│   Inlet Setters:                                                │
│     + setInletPressureVelocity(P, T, V)                        │
│     + setInletPressure(P, T)                                   │
│     + setInletMassFlowRate(mdot, T)                            │
│     + setInletVolumetricFlowRate(Q, T)                         │
│   Outlet Setters:                                               │
│     + setOutletPressure(P)                                     │
│     + setOutletVelocity(V)                                     │
│     + setOutletMassFlowRate(mdot)                              │
│     + setOutletVolumetricFlowRate(Q)                           │
│   Getters: [12 methods for all properties]                     │
│   Validation:                                                    │
│     + isValid() : bool                                          │
│     + isPressurePressureProblem() : bool                       │
│     + requiresIterativeSolution() : bool                       │
│   Utilities:                                                     │
│     + display()                                                 │
│     + getConfigurationString() : string                        │
│   Static Conversion Methods:                                    │
│     + massFlowRateToVelocity(mdot, rho, A) : double           │
│     + volFlowRateToVelocity(Q, A) : double                    │
│     + velocityToMassFlowRate(V, rho, A) : double              │
│     + velocityToVolFlowRate(V, A) : double                    │
└─────────────────────────────────────────────────────────────────┘
```

## BCType Enumeration

```
enum class BCType {
    PRESSURE,              // Specified pressure
    VELOCITY,              // Specified velocity  
    MASS_FLOW_RATE,        // Specified mass flow rate
    VOLUMETRIC_FLOW_RATE,  // Specified volumetric flow rate
    NONE                   // Not set
}
```

## Usage Flow Diagram

```
┌──────────────┐
│ Create BC    │
│ Object       │
└──────┬───────┘
       │
       v
┌──────────────┐     ┌──────────────────────────────────────┐
│ Set Inlet    │────→│ Choose one:                          │
│ Conditions   │     │ - setInletPressureVelocity(P, T, V) │
└──────┬───────┘     │ - setInletPressure(P, T)            │
       │             │ - setInletMassFlowRate(mdot, T)     │
       │             │ - setInletVolumetricFlowRate(Q, T)  │
       │             └──────────────────────────────────────┘
       v
┌──────────────┐     ┌──────────────────────────────────────┐
│ Set Outlet   │────→│ Choose one:                          │
│ Conditions   │     │ - setOutletPressure(P)              │
└──────┬───────┘     │ - setOutletVelocity(V)              │
       │             │ - setOutletMassFlowRate(mdot)       │
       │             │ - setOutletVolumetricFlowRate(Q)    │
       │             └──────────────────────────────────────┘
       v
┌──────────────┐
│ Validate     │
│ isValid()?   │
└──────┬───────┘
       │
       ├─ No ──→ [Error: Invalid configuration]
       │
       v Yes
┌──────────────┐
│ Check Type   │
│ requiresIter?│
└──────┬───────┘
       │
       ├─ Yes ──→ [Use IterativeSolver/AdaptiveSolver]
       │
       v No
┌──────────────┐
│ Use with     │
│ Pipeline     │
└──────────────┘
```

## Integration with Pipeline

```
┌─────────────────────────────────────────────────────────────┐
│                         Pipeline                             │
├─────────────────────────────────────────────────────────────┤
│ Private:                                                     │
│   - BoundaryConditions boundaryConditions                   │
│   - [... existing SoA arrays ...]                           │
├─────────────────────────────────────────────────────────────┤
│ Public:                                                      │
│   + setBoundaryConditions(const BC& bc)  [NEW]             │
│   + setInletConditions(P, T, V)          [LEGACY]          │
│   + solveAll(solver, fluid)              [MODIFIED]        │
│   - applyInletBC(fluid)                  [NEW PRIVATE]     │
│   - applyOutletBC(fluid)                 [NEW PRIVATE]     │
└─────────────────────────────────────────────────────────────┘
```

## Solver Compatibility Matrix

```
                    ┌──────────┬──────────────┬───────────────┬──────────┐
                    │  Simple  │  Iterative   │   Adaptive    │   RK4    │
                    │  Solver  │   Solver     │    Solver     │  Solver  │
┌───────────────────┼──────────┼──────────────┼───────────────┼──────────┤
│ Press+Vel→Press   │    ✅    │      ✅      │      ✅       │    ✅    │
├───────────────────┼──────────┼──────────────┼───────────────┼──────────┤
│ Press→Press       │    ❌    │      ✅      │      ✅       │    ✅*   │
├───────────────────┼──────────┼──────────────┼───────────────┼──────────┤
│ MassFlow→Press    │    ✅    │      ✅      │      ✅       │    ✅    │
├───────────────────┼──────────┼──────────────┼───────────────┼──────────┤
│ VolFlow→Press     │    ✅    │      ✅      │      ✅       │    ✅    │
└───────────────────┴──────────┴──────────────┴───────────────┴──────────┘
* Requires outer iteration loop
```

## Data Flow: Pressure-Velocity (Current Default)

```
User Input
   ↓
┌──────────────────────────────────────┐
│ bc.setInletPressureVelocity()       │
│   - stores P, T, V                   │
│   - sets inletType = VELOCITY        │
└──────────────┬───────────────────────┘
               ↓
┌──────────────────────────────────────┐
│ bc.setOutletPressure()               │
│   - stores P_out                     │
│   - sets outletType = PRESSURE       │
└──────────────┬───────────────────────┘
               ↓
┌──────────────────────────────────────┐
│ pipeline.setBoundaryConditions(bc)   │
│   - validates configuration          │
│   - stores BC object                 │
└──────────────┬───────────────────────┘
               ↓
┌──────────────────────────────────────┐
│ pipeline.solveAll(solver, fluid)     │
│   - calls applyInletBC()             │
│   - sets inlet P, T, V in SoA        │
│   - solves each segment              │
│   - calls applyOutletBC() (optional) │
└──────────────┬───────────────────────┘
               ↓
          Results
```

## Data Flow: Pressure-Pressure (Find Flow Rate)

```
User Input
   ↓
┌──────────────────────────────────────┐
│ bc.setInletPressure()                │
│   - stores P_in, T                   │
│   - sets inletType = PRESSURE        │
│   - velocity UNKNOWN                 │
└──────────────┬───────────────────────┘
               ↓
┌──────────────────────────────────────┐
│ bc.setOutletPressure()               │
│   - stores P_out                     │
│   - sets outletType = PRESSURE       │
└──────────────┬───────────────────────┘
               ↓
┌──────────────────────────────────────┐
│ bc.isPressurePressureProblem() = YES │
│ bc.requiresIterativeSolution() = YES │
└──────────────┬───────────────────────┘
               ↓
┌──────────────────────────────────────┐
│ pipeline.setBoundaryConditions(bc)   │
│   - validates configuration          │
│   - detects P-P problem              │
└──────────────┬───────────────────────┘
               ↓
┌──────────────────────────────────────┐
│ Outer iteration loop:                │
│   1. Guess initial velocity          │
│   2. pipeline.solveAll()             │
│   3. Check if outlet P matches       │
│   4. Adjust velocity and repeat      │
└──────────────┬───────────────────────┘
               ↓
          Results
     (including flow rate)
```

## Data Flow: Mass Flow Rate

```
User Input
   ↓
┌──────────────────────────────────────┐
│ bc.setInletMassFlowRate()            │
│   - stores mdot, T                   │
│   - sets inletType = MASS_FLOW_RATE  │
└──────────────┬───────────────────────┘
               ↓
┌──────────────────────────────────────┐
│ pipeline.setBoundaryConditions(bc)   │
└──────────────┬───────────────────────┘
               ↓
┌──────────────────────────────────────┐
│ pipeline.applyInletBC(fluid):        │
│   1. Get mdot from BC                │
│   2. Calculate area from diameter    │
│   3. Get density from fluid          │
│   4. V = mdot / (rho * A)           │
│   5. Set inlet velocity              │
└──────────────┬───────────────────────┘
               ↓
┌──────────────────────────────────────┐
│ Continue with normal solve           │
└──────────────────────────────────────┘
```

## Validation Logic

```
┌─────────────────────────┐
│ isValid() called        │
└───────────┬─────────────┘
            │
            v
┌─────────────────────────┐
│ Both inlet & outlet     │
│ set?                    │
└───┬──────────────┬──────┘
    │ No           │ Yes
    v              v
  FALSE    ┌──────────────────────┐
           │ Check combinations:  │
           │ - Flow + Flow? → No  │
           │ - Vel + Vel? → No    │
           │ - Otherwise → Yes    │
           └───────┬──────────────┘
                   v
                 TRUE/FALSE
```

## File Dependencies

```
BoundaryConditions.h
    ↓
    ├─── <string>
    ├─── <stdexcept>
    └─── <iostream>

BoundaryConditions.cpp
    ↓
    ├─── BoundaryConditions.h
    ├─── <cmath>
    └─── <iomanip>

example_boundary_conditions.cpp
    ↓
    ├─── BoundaryConditions.h
    └─── <iostream>

Pipeline.h (after integration)
    ↓
    ├─── BoundaryConditions.h
    └─── [... other includes ...]
```

## Memory Layout

```
BoundaryConditions object size ≈ 120 bytes

┌─────────────────────────────────────┐
│ inletType           (4 bytes)       │  BCType enum
├─────────────────────────────────────┤
│ outletType          (4 bytes)       │  BCType enum
├─────────────────────────────────────┤
│ inletPressure       (8 bytes)       │  double
├─────────────────────────────────────┤
│ inletVelocity       (8 bytes)       │  double
├─────────────────────────────────────┤
│ inletMassFlowRate   (8 bytes)       │  double
├─────────────────────────────────────┤
│ inletVolFlowRate    (8 bytes)       │  double
├─────────────────────────────────────┤
│ inletTemperature    (8 bytes)       │  double
├─────────────────────────────────────┤
│ outletPressure      (8 bytes)       │  double
├─────────────────────────────────────┤
│ outletVelocity      (8 bytes)       │  double
├─────────────────────────────────────┤
│ outletMassFlowRate  (8 bytes)       │  double
├─────────────────────────────────────┤
│ outletVolFlowRate   (8 bytes)       │  double
├─────────────────────────────────────┤
│ inletSet            (1 byte)        │  bool
├─────────────────────────────────────┤
│ outletSet           (1 byte)        │  bool
├─────────────────────────────────────┤
│ [padding]           (6 bytes)       │  alignment
└─────────────────────────────────────┘
Total: ~120 bytes (small, efficient)
```

## Summary

The BoundaryConditions class provides:

1. **Encapsulation** - All BC data in one object
2. **Type Safety** - Enum-based types prevent errors
3. **Validation** - Automatic checking of configurations
4. **Flexibility** - Support for multiple BC types
5. **Utility** - Conversion methods and queries
6. **Integration** - Clean interface with Pipeline
7. **Documentation** - Self-describing API

Perfect for extending your pipeline simulator! 🚀
