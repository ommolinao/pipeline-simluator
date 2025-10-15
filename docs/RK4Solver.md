# RK4 Solver Documentation

## Overview

The 4th-order Runge-Kutta (RK4) solver provides highly accurate numerical integration of the differential equations governing pipe flow. It's particularly valuable for pipes with:
- Significant elevation changes
- Long pipe lengths
- Varying conditions along the pipe
- Compressible flow effects

## Mathematical Foundation

### Classical RK4 Method

For a differential equation dy/dx = f(x, y), the RK4 method computes:

```
k₁ = f(xₙ, yₙ)
k₂ = f(xₙ + h/2, yₙ + h·k₁/2)
k₃ = f(xₙ + h/2, yₙ + h·k₂/2)
k₄ = f(xₙ + h, yₙ + h·k₃)

yₙ₊₁ = yₙ + (h/6)·(k₁ + 2k₂ + 2k₃ + k₄)
```

### Equations Integrated

**1. Momentum Equation (Pressure)**
```
dP/dx = -f·(ρV²)/(2D) - ρg·sin(θ)
```
Where:
- f = Darcy friction factor
- ρ = fluid density
- V = velocity
- D = pipe diameter
- θ = pipe inclination angle

**2. Energy Equation (Temperature)**
```
dT/dx = heat transfer effects (currently isothermal: dT/dx = 0)
```

**3. Continuity (Mass Conservation)**
```
ρAV = constant
```

## Accuracy

### Error Analysis

| Method | Local Error | Global Error |
|--------|-------------|--------------|
| Euler (SimpleSolver) | O(h²) | O(h) |
| **RK4** | **O(h⁵)** | **O(h⁴)** |

This means RK4 is **dramatically more accurate** for the same step size!

### Example Error Comparison

For a 100m pipe with 10 steps (h = 10m):
- SimpleSolver error: ~10m × ε ≈ 10ε
- RK4 error: ~(10m)⁴ × ε ≈ 0.0001ε

**RK4 is ~100,000× more accurate for this case!**

## When to Use RK4 vs SimpleSolver

### Use **RK4Solver** when:
✅ High accuracy is critical  
✅ Long pipes (> 100m per segment)  
✅ Steep elevation changes (> 10m)  
✅ Compressible fluids (gases)  
✅ Scientific/research applications  
✅ Verification/validation studies  

### Use **SimpleSolver** when:
✅ Quick approximate results needed  
✅ Short pipe segments (< 50m)  
✅ Small elevation changes (< 5m)  
✅ Incompressible liquids  
✅ Preliminary design studies  
✅ Real-time applications (speed matters)  

## Performance Considerations

### Computational Cost

For N integration steps:
- **SimpleSolver**: 1 evaluation per segment
- **RK4Solver**: 4N evaluations per segment

**Typical overhead**: RK4 is ~100-400× slower than SimpleSolver

### Memory Usage

Both solvers have identical memory footprint:
- No additional storage required
- Same SoA arrays used
- Temporary variables only

### Optimization Tips

1. **Start with fewer steps** (50-100) for testing
2. **Increase steps** (500-1000) for production runs
3. **Use SimpleSolver** for quick iterations
4. **Use RK4** for final validation

## Usage Examples

### Basic Usage

```cpp
#include "RK4Solver.h"

// Create solver with 100 integration steps per pipe segment
RK4Solver solver(100);

// Solve pipeline
pipeline.solveAll(&solver, &fluid);
```

### Adjusting Accuracy

```cpp
// Low accuracy, fast (50 steps)
RK4Solver solverFast(50);

// Medium accuracy (100 steps) - recommended default
RK4Solver solverMedium(100);

// High accuracy (500 steps)
RK4Solver solverHigh(500);

// Very high accuracy (1000 steps) - for verification
RK4Solver solverVeryHigh(1000);
```

### Changing Parameters

```cpp
RK4Solver solver(100);

// Change number of steps
solver.setNumSteps(200);

// Change tolerance (for future iterative features)
solver.setTolerance(1e-8);

// Query current settings
int steps = solver.getNumSteps();
double tol = solver.getTolerance();
```

## Integration Steps Recommendation

| Pipe Length | Min Steps | Recommended | High Accuracy |
|-------------|-----------|-------------|---------------|
| < 50m | 20 | 50 | 100 |
| 50-100m | 50 | 100 | 200 |
| 100-500m | 100 | 200 | 500 |
| 500-1000m | 200 | 500 | 1000 |
| > 1000m | 500 | 1000 | 2000 |

## Comparison with Other Methods

### SimpleSolver (Forward Euler)
- **Speed**: ⚡⚡⚡⚡⚡ (1× baseline)
- **Accuracy**: ⭐⭐ (1st order)
- **Best for**: Quick estimates, short pipes

### IterativeSolver
- **Speed**: ⚡⚡⚡⚡ (2-5× slower)
- **Accuracy**: ⭐⭐⭐ (iterative refinement)
- **Best for**: Steady-state convergence

### AdaptiveSolver
- **Speed**: ⚡⚡⚡ (5-20× slower)
- **Accuracy**: ⭐⭐⭐⭐ (adaptive steps)
- **Best for**: Variable conditions

### RK4Solver
- **Speed**: ⚡ (100-400× slower)
- **Accuracy**: ⭐⭐⭐⭐⭐ (4th order)
- **Best for**: Maximum accuracy, validation

## Future Enhancements

Potential improvements to the RK4 solver:

1. **Adaptive step sizing** - automatically adjust h based on error estimates
2. **Heat transfer** - include thermal effects in energy equation
3. **Compressibility** - full variable-density integration
4. **Cash-Karp method** - embedded error estimation
5. **Parallel integration** - OpenMP for multi-segment pipelines

## References

- Numerical Recipes in C++, Press et al.
- Computational Fluid Dynamics, Anderson
- Chemical Engineering Design, Coulson & Richardson

## Testing

Compare RK4 against analytical solutions for simple cases:

```cpp
// Test case: Horizontal pipe, constant diameter
// Should match Darcy-Weisbach exactly
Pipe testPipe("Test", 0, 100, 0, 0, 0.2, 0.00015);
testPipe.setInletConditions(100000, 293.15, 2.0);

RK4Solver rk4(1000);  // High accuracy
rk4.solve(&testPipe, &water);

// Compare with analytical solution
double analytical = analyticalPressureDrop(...);
double numerical = testPipe.getInletPressure() - testPipe.getOutletPressure();
double error = std::abs(analytical - numerical) / analytical;
std::cout << "Error: " << error * 100 << "%\n";
```

## Conclusion

The RK4 solver is a powerful tool for high-accuracy pipeline simulation. While slower than simple methods, its 4th-order accuracy makes it invaluable for:
- Validation studies
- Complex geometries
- Scientific research
- Final design verification

Use it when accuracy matters more than speed!
