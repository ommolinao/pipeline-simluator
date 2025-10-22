# Updated Makefile Guide - BoundaryConditions Integration

## 🎯 What Changed

Your Makefile has been updated to include support for the new BoundaryConditions class!

### New Features Added

1. **New Target: `bc-test`** - Test BoundaryConditions standalone
2. **New Target: `all-with-bc`** - Build everything including BC test
3. **Auto-compilation** - BoundaryConditions.cpp automatically included in common sources
4. **Updated Help** - New targets listed in help menu

## 📋 Changes Summary

### New Targets

```makefile
TARGET_BC_TEST = $(BINDIR)/boundary_conditions_test
```

### New Build Rules

```makefile
# Link the BoundaryConditions test executable
$(TARGET_BC_TEST): $(OBJDIR)/BoundaryConditions.o $(MAIN_BC_TEST_OBJECT) | $(BINDIR)
	$(CXX) $(OBJDIR)/BoundaryConditions.o $(MAIN_BC_TEST_OBJECT) -o $(TARGET_BC_TEST) $(LDFLAGS)
	@echo "✓ BoundaryConditions test build complete: $(TARGET_BC_TEST)"
```

### New Make Commands

1. **`make bc-test`** - Build and run BoundaryConditions tests
2. **`make all-with-bc`** - Build all executables including BC test

## 🚀 How to Use

### Step 1: Copy Files to Your Project

```bash
# Copy header to include directory
cp BoundaryConditions.h include/

# Copy implementation to src directory
cp BoundaryConditions.cpp src/

# Copy test/example file to src directory
cp example_boundary_conditions.cpp src/

# Replace your Makefile
cp Makefile ./
```

### Step 2: Test the BoundaryConditions Class

```bash
# Build and run BC tests
make bc-test
```

**Expected Output:**
```
Running BoundaryConditions tests...
=== BoundaryConditions Class Examples ===

Example 1: Pressure-Velocity (Current Default)
...
Example 7: Error Handling
...

✓ BoundaryConditions test complete!
```

### Step 3: Verify Your Existing Targets Still Work

```bash
# Test regular build
make clean
make

# Test visualization
make viz

# Test gas comparison
make gas
```

All your existing functionality should work exactly as before! ✅

## 📚 New Makefile Targets

### Original Targets (Unchanged)
- `make` or `make all` - Build regular simulator
- `make run` - Build and run regular simulator
- `make viz` - Build, run, and open visualization
- `make gas` - Run gas pipeline solver comparison
- `make both` - Build regular and viz executables
- `make all-targets` - Build regular, viz, and gas
- `make quick` - Clean build + visualization
- `make test` - Build and run regular version
- `make clean` - Remove build artifacts
- `make cleanall` - Remove everything including HTML
- `make help` - Show help message
- `make info` - Show configuration

### New Targets
- **`make bc-test`** ⭐ - Build and run BoundaryConditions tests
- **`make all-with-bc`** ⭐ - Build all four executables

## 🔧 Technical Details

### What Gets Compiled

When you run any build command, the Makefile now automatically includes:

**Common objects (used by all executables):**
- All your existing source files (Fluid.cpp, Pipe.cpp, etc.)
- **BoundaryConditions.cpp** (NEW! - automatically included)

**Main executables:**
1. `pipeline_simulator` - Your regular simulator
2. `pipeline_viz` - Visualization version
3. `pipeline_gas_comparison` - Gas solver comparison
4. `boundary_conditions_test` (NEW!) - BC standalone test

### File Structure Expected

```
your-project/
├── Makefile (updated)
├── include/
│   ├── BoundaryConditions.h (NEW!)
│   ├── Fluid.h
│   ├── Pipe.h
│   └── ... (all your existing headers)
├── src/
│   ├── BoundaryConditions.cpp (NEW!)
│   ├── example_boundary_conditions.cpp (NEW!)
│   ├── main.cpp
│   ├── main_viz.cpp
│   ├── main_gas_comparison.cpp
│   └── ... (all your existing sources)
├── bin/ (created by make)
└── obj/ (created by make)
```

## ✅ Testing Checklist

After updating your Makefile:

### Phase 1: BoundaryConditions Test
```bash
make bc-test
```
- [ ] Compiles without errors
- [ ] Shows 7 example outputs
- [ ] All examples pass
- [ ] No crashes

### Phase 2: Existing Functionality
```bash
make clean
make run
```
- [ ] Regular simulator still compiles
- [ ] Regular simulator still runs
- [ ] No new warnings or errors

### Phase 3: Visualization
```bash
make viz
```
- [ ] Visualization compiles
- [ ] Visualization runs
- [ ] HTML file generated
- [ ] Opens in browser

### Phase 4: Gas Comparison
```bash
make gas
```
- [ ] Gas comparison compiles
- [ ] Gas comparison runs
- [ ] 4 HTML files generated

### Phase 5: All Targets
```bash
make all-with-bc
```
- [ ] All four executables build successfully
- [ ] No compilation errors

## 🎯 Quick Reference

### Test BoundaryConditions
```bash
make bc-test
```

### Build Everything (including BC test)
```bash
make all-with-bc
```

### Your Normal Workflow (unchanged)
```bash
make viz           # Most common
make gas           # Gas pipeline comparison
make quick         # Clean build + viz
```

## 🐛 Troubleshooting

### Issue: "BoundaryConditions.h: No such file or directory"
**Solution:** Copy BoundaryConditions.h to `include/` directory

### Issue: "undefined reference to BoundaryConditions::..."
**Solution:** Copy BoundaryConditions.cpp to `src/` directory

### Issue: "example_boundary_conditions.cpp: No such file"
**Solution:** Copy example_boundary_conditions.cpp to `src/` directory

### Issue: Compilation errors in existing code
**Solution:** The new code shouldn't affect existing files. Check that:
- BoundaryConditions files are in correct directories
- No name conflicts with existing code
- C++17/20 flag is set in CXXFLAGS

## 💡 Pro Tips

1. **Test BC separately first**: Run `make bc-test` before integrating with Pipeline
2. **Keep existing workflow**: All your old commands still work
3. **Build everything**: Use `make all-with-bc` to verify everything compiles
4. **Clean builds**: Use `make clean` if you have issues

## 📊 Makefile Comparison

### Before (Original)
- 3 executables: simulator, viz, gas
- Common sources auto-detected
- Clean targets

### After (Updated)
- **4 executables**: simulator, viz, gas, **bc-test** ⭐
- Common sources auto-detected (includes BoundaryConditions.cpp)
- Clean targets (unchanged)
- **2 new targets**: `bc-test`, `all-with-bc` ⭐
- Updated help menu

## 🎓 Integration Path

### Step 1: Test Standalone (Now!)
```bash
# Copy files
cp BoundaryConditions.h include/
cp BoundaryConditions.cpp src/
cp example_boundary_conditions.cpp src/

# Test
make bc-test
```

### Step 2: Verify No Regressions
```bash
make clean
make run
make viz
make gas
```

### Step 3: Integrate with Pipeline (When Ready)
Follow INTEGRATION_GUIDE.md to add BoundaryConditions support to your Pipeline class.

## 🔄 Backwards Compatibility

✅ **All existing targets work exactly as before**
✅ **No changes to your existing code required**
✅ **New targets are optional**
✅ **Can test BC standalone before integrating**

## 🎉 Summary

Your Makefile now:
- ✅ Supports BoundaryConditions class
- ✅ Can test BC independently with `make bc-test`
- ✅ Maintains all existing functionality
- ✅ Automatically compiles BoundaryConditions.cpp
- ✅ Includes updated help menu
- ✅ Is ready for Pipeline integration

**Next Step:** Run `make bc-test` to verify everything works! 🚀

---

**Questions?** See the INTEGRATION_GUIDE.md for more details on integrating BoundaryConditions with your Pipeline class.
