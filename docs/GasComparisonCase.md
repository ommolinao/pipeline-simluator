# Gas Pipeline Solver Comparison Guide

## Overview

This comparison demonstrates how different solvers handle **compressible gas flow** through a transmission pipeline. Gas pipelines are much more challenging than liquid pipelines due to:

- **High compressibility** - Density varies significantly with pressure
- **Pressure-dependent properties** - Viscosity and density change along the pipe
- **Long distances** - Cumulative errors matter more
- **High pressures** - 70 bar (7 MPa) transmission pressures

## The Test Case

### Pipeline Configuration
- **10 segments** × 500m each = 5 km total
- **600mm diameter** - Typical transmission pipeline
- **Varying terrain** - Hills, mountain plateau, valley descent
- **Total elevation change** - 150m climb, then descent

### Gas Properties (Natural Gas)
- **Density**: 0.717 kg/m³ (at standard conditions)
- **Viscosity**: 1.08×10⁻⁵ Pa·s
- **Highly compressible** - Density varies with pressure

### Inlet Conditions
- **Pressure**: 7 MPa (70 bar)
- **Temperature**: 15°C (288 K)
- **Velocity**: 10 m/s

## Solver Comparison

### 1. SimpleSolver
**Algorithm**: Single-pass Euler method

**Pros:**
- ⚡ Extremely fast
- Simple implementation
- Good for quick estimates

**Cons:**
- ❌ Poor accuracy for gases
- ❌ Doesn't iterate for pressure-dependent properties
- ❌ Ignores compressibility effects

**Expected Error**: 5-15% for gas pipelines

---

### 2. IterativeSolver
**Algorithm**: Iterative refinement with convergence check

**Pros:**
- ✓ Much better accuracy
- ✓ Handles pressure-dependent properties correctly
- ✓ Converges to consistent solution
- ⚡ Moderate speed (2-5× slower than Simple)

**Cons:**
- May require many iterations for convergence
- Fixed iteration scheme

**Expected Error**: 1-3% for gas pipelines

---

### 3. AdaptiveSolver
**Algorithm**: Adaptive refinement based on local conditions

**Pros:**
- ✓ Excellent accuracy
- ✓ Adapts to varying conditions
- ✓ Good for complex geometries
- ✓ Smart about where to refine

**Cons:**
- ~ Slower than Iterative (5-10× slower than Simple)
- More complex implementation

**Expected Error**: 0.5-1% for gas pipelines

---

### 4. RK4Solver
**Algorithm**: 4th-order Runge-Kutta integration

**Pros:**
- ✓✓ Highest accuracy (4th order)
- ✓✓ Smooth integration of all effects
- ✓✓ Best for validation and verification
- ✓ Captures gradual density changes perfectly

**Cons:**
- 🐌 Slowest (100-400× slower than Simple)
- Computationally expensive

**Expected Error**: < 0.1% for gas pipelines

## What to Look For

### 1. Pressure Profiles
Compare the pressure curves:
- **SimpleSolver**: May show discontinuities
- **IterativeSolver**: Smooth but may have slight deviations
- **AdaptiveSolver**: Very smooth, well-refined
- **RK4Solver**: Smoothest, most physically accurate

### 2. Velocity Variations
For gas (compressible flow), velocity changes even with constant diameter!

**Why?** Continuity: ρAV = constant
- As pressure drops → density drops → velocity increases

**SimpleSolver**: May not capture this correctly
**RK4Solver**: Shows smooth velocity increase

### 3. Density Variations
Gas density changes significantly:
- Inlet (7 MPa): ~50 kg/m³
- Outlet (~5 MPa): ~35 kg/m³

Check how each solver captures this 30% density change!

### 4. Reynolds Number
Reynolds number varies because:
- Re = ρVD/μ
- Both ρ and V change along the pipe

More advanced solvers capture this better.

### 5. Pressure Gradients
Look at the bar chart:
- Steeper in uphill sections (gravity + friction)
- Less steep in downhill sections (gravity helps)
- RK4 shows smoothest transitions

## Running the Comparison

### Quick Start
```bash
make gas
```

This will:
1. Compile the gas comparison code
2. Run all 4 solvers
3. Generate 4 HTML files
4. Display performance comparison

### Manual Approach
```bash
# Compile
make clean
make all-targets

# Run comparison
./bin/pipeline_gas_comparison

# Open the HTML files to compare
```

### Expected Output Files
- `gas_pipeline_simple.html` - SimpleSolver results
- `gas_pipeline_iterative.html` - IterativeSolver results
- `gas_pipeline_adaptive.html` - AdaptiveSolver results
- `gas_pipeline_rk4.html` - RK4Solver results

## Interpreting Results

### Computation Time
You'll see something like:
```
SimpleSolver:    500 μs (baseline)
IterativeSolver: 2,500 μs (5x)
AdaptiveSolver:  5,000 μs (10x)
RK4Solver:       50,000 μs (100x)
```

### Pressure Drop Differences
```
SimpleSolver:    1.8 MPa
IterativeSolver: 1.95 MPa (Δ = 150 kPa)
AdaptiveSolver:  1.98 MPa (Δ = 180 kPa)
RK4Solver:       2.0 MPa (Δ = 200 kPa)
```

The differences show how much accuracy you gain!

## Recommendations

### For Production Use (Gas Pipelines)
**Best**: AdaptiveSolver
- Good balance of accuracy and speed
- Handles complex conditions well
- Reliable for real-world scenarios

### For Validation/Research
**Best**: RK4Solver
- Highest accuracy
- Gold standard for verification
- Use to validate other solvers

### For Quick Estimates
**Acceptable**: IterativeSolver
- Much better than SimpleSolver for gases
- Fast enough for iteration
- Good starting point

### Avoid for Gases
**SimpleSolver** - Not accurate enough for compressible flow!

## Physics Insights

### Why RK4 is Better for Gases

1. **Smooth density integration**
   - RK4 captures gradual density changes
   - SimpleSolver treats each segment as uniform

2. **Accurate velocity prediction**
   - Continuity enforced at every integration step
   - Not just at segment boundaries

3. **Pressure-dependent properties**
   - RK4 evaluates properties at multiple points
   - Better representation of property variations

4. **4th-order accuracy**
   - Error scales as h⁴ (very small!)
   - SimpleSolver error scales as h (much larger)

## Conclusion

For **gas pipelines**, the solver choice matters significantly!

**Key Takeaways:**
- ✓ Use IterativeSolver or better for gas
- ✓ SimpleSolver is inadequate for compressible flow
- ✓ RK4Solver provides benchmark accuracy
- ✓ AdaptiveSolver offers best practicality

The visualization comparison will make these differences crystal clear! 🎯
