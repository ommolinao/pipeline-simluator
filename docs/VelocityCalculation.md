# Understanding Velocity Behavior: RK4 vs SimpleSolver

## The Observation

You noticed that:
- **SimpleSolver**: Velocity varies slightly along the pipeline
- **RK4Solver**: Velocity remains constant within each segment

## Why This Happens

### Within a Single Pipe Segment

Both solvers work on **one segment at a time**, where:
- Diameter is constant (D₁ = D₂)
- Area is constant (A₁ = A₂)
- For incompressible flow: **V must be constant**

```
Continuity: A₁V₁ = A₂V₂
If A₁ = A₂, then V₁ = V₂  ✓
```

### The Correct Behavior

**RK4Solver is more accurate!** Within a segment, velocity should be constant for incompressible flow.

**SimpleSolver shows small variations** due to numerical approximations and possibly some compressibility handling.

### Between Segments (Diameter Changes)

When diameter changes between segments:
```
Segment i:   D = 0.25m, V = 2.0 m/s
Segment i+1: D = 0.20m, V = ???
```

Continuity requires:
```
A₁V₁ = A₂V₂
π(0.25/2)²(2.0) = π(0.20/2)²(V₂)
V₂ = 2.0 × (0.25/0.20)² = 3.125 m/s
```

## The Fix

The issue is in `Pipeline::solveAll()` - it needs to enforce continuity at segment boundaries!

### Current Code (INCORRECT)
```cpp
// Simply copies outlet velocity to next inlet
inletVelocities[i + 1] = outletVelocities[i];
```

This doesn't account for area changes!

### Fixed Code (CORRECT)
```cpp
// Calculate areas
double A_current = π * D_current² / 4
double A_next = π * D_next² / 4

// Apply continuity equation
inletVelocities[i + 1] = outletVelocities[i] * (A_current / A_next)
```

## Implementation

Replace your `solveAll()` method in `Pipeline.cpp` with the updated version that includes:

```cpp
// Apply continuity: V_next = V_current * (A_current / A_next)
inletVelocities[i + 1] = outletVelocities[i] * (A_current / A_next);
```

This ensures that when diameter changes:
- Mass flow rate is conserved (ṁ = ρAV = constant)
- Velocity adjusts appropriately
- Works correctly for both SimpleSolver and RK4Solver

## Expected Results After Fix

For your pipeline with diameter changes (0.25m → 0.20m → 0.25m):

| Section | Diameter | Expected Velocity |
|---------|----------|-------------------|
| Segments 1-5 | 0.25m | ~2.00 m/s |
| Segments 6-10 | 0.20m | ~3.13 m/s (56% increase) |
| Segments 11-15 | 0.22m | ~2.58 m/s |
| Segments 16-20 | 0.25m | ~2.00 m/s |

The velocity profile should show clear steps at diameter changes!

## Why SimpleSolver Showed Variations

SimpleSolver might have been:
1. Using approximate continuity enforcement
2. Including small compressibility effects
3. Accumulating numerical errors

RK4's constant velocity within segments is actually **more physically correct** for incompressible flow!

## Summary

✅ **RK4's constant velocity within segments is CORRECT**
✅ **Velocity SHOULD change between segments when diameter changes**
✅ **The fix belongs in Pipeline::solveAll(), not in RK4Solver**
✅ **SimpleSolver should also benefit from this fix**

After applying the fix, both solvers will show proper velocity changes at diameter transitions!
