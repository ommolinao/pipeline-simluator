# BoundaryConditions Testing & Verification Checklist

## ✅ Pre-Integration Testing

### Phase 1: Standalone Compilation (5 minutes)

```bash
# Copy files to test directory
mkdir bc_test && cd bc_test
cp /path/to/BoundaryConditions.h .
cp /path/to/BoundaryConditions.cpp .
cp /path/to/example_boundary_conditions.cpp .
cp /path/to/Makefile.bc_test .

# Compile
make -f Makefile.bc_test
```

**Expected Result:**
- [ ] Compiles without errors
- [ ] Compiles without warnings
- [ ] Creates executable `test_boundary_conditions`

### Phase 2: Example Execution (5 minutes)

```bash
# Run examples
./test_boundary_conditions
```

**Expected Output - Example 1:**
- [ ] Displays "Example 1: Pressure-Velocity"
- [ ] Shows inlet pressure, temp, velocity
- [ ] Shows outlet pressure
- [ ] Validates as true
- [ ] No errors

**Expected Output - Example 2:**
- [ ] Displays "Example 2: Pressure-Pressure"
- [ ] Identifies as pressure-pressure problem
- [ ] Requires iterative solution = true
- [ ] No errors

**Expected Output - Examples 3-7:**
- [ ] All examples run without crashes
- [ ] Correct validation results
- [ ] Error handling examples catch exceptions
- [ ] Output is clear and readable

### Phase 3: Manual API Testing (10 minutes)

Create test file `test_api.cpp`:

```cpp
#include "BoundaryConditions.h"
#include <iostream>
#include <cassert>

int main() {
    // Test 1: Default construction
    BoundaryConditions bc1;
    assert(!bc1.isValid());  // Should be invalid initially
    
    // Test 2: Pressure-Velocity
    BoundaryConditions bc2;
    bc2.setInletPressureVelocity(1e6, 293.15, 2.0);
    bc2.setOutletPressure(5e5);
    assert(bc2.isValid());
    assert(!bc2.isPressurePressureProblem());
    
    // Test 3: Pressure-Pressure
    BoundaryConditions bc3;
    bc3.setInletPressure(7e6, 288.15);
    bc3.setOutletPressure(5e6);
    assert(bc3.isValid());
    assert(bc3.isPressurePressureProblem());
    assert(bc3.requiresIterativeSolution());
    
    // Test 4: Mass Flow Rate
    BoundaryConditions bc4;
    bc4.setInletMassFlowRate(50.0, 293.15);
    bc4.setOutletPressure(1e5);
    assert(bc4.isValid());
    
    // Test 5: Invalid combination
    BoundaryConditions bc5;
    bc5.setInletMassFlowRate(10.0, 293.15);
    bc5.setOutletMassFlowRate(10.0);
    assert(!bc5.isValid());  // Should be invalid
    
    // Test 6: Conversion utilities
    double v = BoundaryConditions::volFlowRateToVelocity(0.1, 0.05);
    assert(std::abs(v - 2.0) < 1e-6);  // 0.1 / 0.05 = 2.0
    
    std::cout << "✓ All API tests passed!\n";
    return 0;
}
```

Compile and run:
```bash
g++ -std=c++17 -Wall BoundaryConditions.cpp test_api.cpp -o test_api
./test_api
```

**Expected Result:**
- [ ] All assertions pass
- [ ] Prints "All API tests passed!"
- [ ] No crashes or exceptions

## ✅ Integration Testing

### Phase 4: Compilation with Pipeline (15 minutes)

Update your project Makefile:

```makefile
# Add to COMMON_SOURCES
COMMON_SOURCES = ... \
                 $(SRCDIR)/BoundaryConditions.cpp
```

Then compile:
```bash
make clean
make
```

**Expected Result:**
- [ ] BoundaryConditions.cpp compiles
- [ ] No linker errors
- [ ] Executable builds successfully

### Phase 5: Pipeline Integration Test (20 minutes)

Create `test_pipeline_bc.cpp`:

```cpp
#include "Pipeline.h"
#include "BoundaryConditions.h"
#include "Liquid.h"
#include "SimpleSolver.h"
#include <iostream>

int main() {
    // Create fluid and pipeline
    Liquid water("Water", 1000.0, 0.001);
    Pipeline pipeline("Test-BC-Pipeline");
    
    // Add segments
    pipeline.addPipeSegment("Seg-1", 0, 100, 0, 10, 0.3, 0.00015);
    pipeline.addPipeSegment("Seg-2", 100, 200, 10, 5, 0.3, 0.00015);
    
    // Test 1: Traditional method (backwards compatibility)
    std::cout << "Test 1: Traditional setInletConditions\n";
    pipeline.setInletConditions(1e6, 293.15, 2.0);
    
    SimpleSolver solver;
    pipeline.solveAll(&solver, &water);
    
    double dp1 = pipeline.getTotalPressureDrop();
    std::cout << "Pressure drop: " << dp1/1000 << " kPa\n";
    
    // Test 2: BoundaryConditions (if integrated)
    std::cout << "\nTest 2: BoundaryConditions method\n";
    BoundaryConditions bc;
    bc.setInletPressureVelocity(1e6, 293.15, 2.0);
    bc.setOutletPressure(5e5);
    bc.display();
    
    // pipeline.setBoundaryConditions(bc);  // Uncomment after integration
    // pipeline.solveAll(&solver, &water);
    
    std::cout << "✓ Integration test complete\n";
    return 0;
}
```

**Expected Result:**
- [ ] Compiles without errors
- [ ] Traditional method still works
- [ ] BoundaryConditions display works
- [ ] (After full integration) New method works

### Phase 6: Solver Compatibility (15 minutes)

Test with all solver types:

```cpp
#include "Pipeline.h"
#include "BoundaryConditions.h"
#include "Liquid.h"
#include "SimpleSolver.h"
#include "IterativeSolver.h"
#include "AdaptiveSolver.h"
#include "RK4Solver.h"

int main() {
    Liquid water("Water", 1000.0, 0.001);
    
    // Create 4 identical pipelines
    Pipeline p1("Simple"), p2("Iterative"), p3("Adaptive"), p4("RK4");
    
    for (auto* p : {&p1, &p2, &p3, &p4}) {
        p->addPipeSegment("Seg-1", 0, 100, 0, 10, 0.3, 0.00015);
        p->setInletConditions(1e6, 293.15, 2.0);
    }
    
    // Test all solvers
    SimpleSolver s1;
    IterativeSolver s2(20, 1.0);
    AdaptiveSolver s3(50000, 20, 1.0, 10, false);
    RK4Solver s4(100);
    
    p1.solveAll(&s1, &water);
    p2.solveAll(&s2, &water);
    p3.solveAll(&s3, &water);
    p4.solveAll(&s4, &water);
    
    std::cout << "SimpleSolver: " << p1.getTotalPressureDrop()/1000 << " kPa\n";
    std::cout << "IterativeSolver: " << p2.getTotalPressureDrop()/1000 << " kPa\n";
    std::cout << "AdaptiveSolver: " << p3.getTotalPressureDrop()/1000 << " kPa\n";
    std::cout << "RK4Solver: " << p4.getTotalPressureDrop()/1000 << " kPa\n";
    
    return 0;
}
```

**Expected Result:**
- [ ] All solvers work with BC class
- [ ] Results are reasonable
- [ ] No crashes or errors
- [ ] Similar pressure drops across solvers

## ✅ Validation Testing

### Phase 7: Error Handling (10 minutes)

Test error cases:

```cpp
#include "BoundaryConditions.h"
#include <iostream>

void testErrorHandling() {
    int errors_caught = 0;
    
    // Test 1: Negative pressure
    try {
        BoundaryConditions bc;
        bc.setInletPressure(-1e6, 293.15);
    } catch (const std::invalid_argument&) {
        errors_caught++;
        std::cout << "✓ Caught negative pressure\n";
    }
    
    // Test 2: Negative temperature
    try {
        BoundaryConditions bc;
        bc.setInletPressure(1e6, -100);
    } catch (const std::invalid_argument&) {
        errors_caught++;
        std::cout << "✓ Caught negative temperature\n";
    }
    
    // Test 3: Negative flow rate
    try {
        BoundaryConditions bc;
        bc.setInletMassFlowRate(-10.0, 293.15);
    } catch (const std::invalid_argument&) {
        errors_caught++;
        std::cout << "✓ Caught negative flow rate\n";
    }
    
    // Test 4: Invalid combination
    BoundaryConditions bc;
    bc.setInletMassFlowRate(10.0, 293.15);
    bc.setOutletMassFlowRate(10.0);
    if (!bc.isValid()) {
        errors_caught++;
        std::cout << "✓ Detected invalid combination\n";
    }
    
    std::cout << "\nErrors caught: " << errors_caught << "/4\n";
}
```

**Expected Result:**
- [ ] All 4 errors caught
- [ ] Appropriate error messages
- [ ] No crashes

### Phase 8: Boundary Cases (10 minutes)

Test edge cases:

```cpp
void testBoundaryCases() {
    // Test 1: Zero velocity (stagnant flow)
    BoundaryConditions bc1;
    bc1.setInletPressureVelocity(1e6, 293.15, 0.0);  // Should work
    assert(bc1.isValid());
    
    // Test 2: Very high pressure
    BoundaryConditions bc2;
    bc2.setInletPressure(1e8, 293.15);  // 100 MPa
    assert(bc2.isValid());
    
    // Test 3: Near-zero flow rate
    BoundaryConditions bc3;
    bc3.setInletMassFlowRate(1e-6, 293.15);  // Very small
    assert(bc3.isValid());
    
    // Test 4: High temperature
    BoundaryConditions bc4;
    bc4.setInletPressure(1e6, 500.0);  // 227°C
    assert(bc4.isValid());
    
    std::cout << "✓ All boundary cases handled\n";
}
```

**Expected Result:**
- [ ] All edge cases work
- [ ] No crashes or unexpected behavior

## ✅ Performance Testing

### Phase 9: Performance Check (5 minutes)

Test object creation overhead:

```cpp
#include <chrono>

void testPerformance() {
    const int N = 1000000;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < N; i++) {
        BoundaryConditions bc;
        bc.setInletPressureVelocity(1e6, 293.15, 2.0);
        bc.setOutletPressure(5e5);
        bc.isValid();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    double time_per_op = duration.count() / (double)N;
    std::cout << "Time per BC operation: " << time_per_op << " μs\n";
    std::cout << "Ops per second: " << 1.0e6 / time_per_op << "\n";
}
```

**Expected Result:**
- [ ] < 1 μs per operation
- [ ] > 1 million ops/second
- [ ] No memory leaks

## ✅ Documentation Testing

### Phase 10: Documentation Review (15 minutes)

Check all documentation:

- [ ] README.md is clear and complete
- [ ] INTEGRATION_GUIDE.md has working code examples
- [ ] QUICK_REFERENCE.md is accurate
- [ ] ARCHITECTURE.md diagrams are correct
- [ ] All code examples compile
- [ ] No broken references

### Phase 11: API Completeness (10 minutes)

Verify all advertised features work:

- [ ] All 4 inlet types work
- [ ] All 4 outlet types work
- [ ] Validation works correctly
- [ ] Error messages are helpful
- [ ] Conversion utilities work
- [ ] Display methods work
- [ ] Query methods work

## 📊 Testing Summary

### Pre-Integration Checklist
- [ ] Standalone compilation successful
- [ ] All examples run correctly
- [ ] Manual API tests pass
- [ ] Error handling works
- [ ] Boundary cases handled

### Integration Checklist
- [ ] Compiles with Pipeline
- [ ] Traditional methods still work
- [ ] New BC methods work (if integrated)
- [ ] All solvers compatible
- [ ] No regressions

### Quality Checklist
- [ ] No compiler warnings
- [ ] No memory leaks
- [ ] Good performance
- [ ] Clear error messages
- [ ] Complete documentation

## 🎯 Success Criteria

All tests pass when:
- ✅ Code compiles cleanly
- ✅ Examples run without errors
- ✅ API tests pass
- ✅ Error handling works
- ✅ Integration successful
- ✅ Documentation accurate
- ✅ Performance acceptable

## 🐛 Troubleshooting

### Issue: Won't compile
**Check:**
- C++17 support enabled
- All files copied correctly
- Makefile updated correctly

### Issue: Linker errors
**Check:**
- BoundaryConditions.cpp in source list
- Correct include paths
- No duplicate definitions

### Issue: Tests fail
**Check:**
- Input values are reasonable
- Units are correct (Pa, K, m/s)
- Configuration is valid

### Issue: Poor performance
**Check:**
- Compiler optimizations enabled (-O2 or -O3)
- Not creating unnecessary copies
- Using by-reference where appropriate

## 📝 Test Log Template

```
Date: ___________
Tester: ___________

Phase 1: Standalone Compilation
  Result: [ ] Pass [ ] Fail
  Notes: ___________

Phase 2: Example Execution
  Result: [ ] Pass [ ] Fail
  Notes: ___________

Phase 3: Manual API Testing
  Result: [ ] Pass [ ] Fail
  Notes: ___________

Phase 4: Compilation with Pipeline
  Result: [ ] Pass [ ] Fail
  Notes: ___________

Phase 5: Pipeline Integration Test
  Result: [ ] Pass [ ] Fail
  Notes: ___________

Phase 6: Solver Compatibility
  Result: [ ] Pass [ ] Fail
  Notes: ___________

Phase 7: Error Handling
  Result: [ ] Pass [ ] Fail
  Notes: ___________

Phase 8: Boundary Cases
  Result: [ ] Pass [ ] Fail
  Notes: ___________

Phase 9: Performance Check
  Result: [ ] Pass [ ] Fail
  Time per op: _____ μs
  
Phase 10: Documentation Review
  Result: [ ] Pass [ ] Fail
  Notes: ___________

Phase 11: API Completeness
  Result: [ ] Pass [ ] Fail
  Notes: ___________

Overall: [ ] PASS [ ] FAIL
```

## 🎉 Completion

When all tests pass:
1. ✅ Package is verified working
2. ✅ Ready for production use
3. ✅ Safe to integrate with your project
4. ✅ Documentation is accurate

**Congratulations!** Your BoundaryConditions system is fully tested and ready to use. 🚀
