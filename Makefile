# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -I./include
LDFLAGS = 

# Directories
SRCDIR = src
INCDIR = include
OBJDIR = obj
BINDIR = bin

# Target executables
TARGET = $(BINDIR)/pipeline_simulator
TARGET_VIZ = $(BINDIR)/pipeline_viz
TARGET_GAS = $(BINDIR)/pipeline_gas_comparison

# Common source files (exclude all main files)
COMMON_SOURCES = $(filter-out $(SRCDIR)/main.cpp $(SRCDIR)/main_viz.cpp $(SRCDIR)/main_gas_comparison.cpp, $(wildcard $(SRCDIR)/*.cpp))
COMMON_OBJECTS = $(COMMON_SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

# Main-specific files
MAIN_OBJECT = $(OBJDIR)/main.o
MAIN_VIZ_OBJECT = $(OBJDIR)/main_viz.o
MAIN_GAS_OBJECT = $(OBJDIR)/main_gas_comparison.o

# Visualization output
VIZ_OUTPUT = pipeline_visualization.html

# Default target - builds regular simulator
all: $(TARGET)

# Link the regular executable (with main.cpp)
$(TARGET): $(COMMON_OBJECTS) $(MAIN_OBJECT) | $(BINDIR)
	$(CXX) $(COMMON_OBJECTS) $(MAIN_OBJECT) -o $(TARGET) $(LDFLAGS)
	@echo "✓ Build complete: $(TARGET)"

# Link the visualization executable (with main_viz.cpp)
$(TARGET_VIZ): $(COMMON_OBJECTS) $(MAIN_VIZ_OBJECT) | $(BINDIR)
	$(CXX) $(COMMON_OBJECTS) $(MAIN_VIZ_OBJECT) -o $(TARGET_VIZ) $(LDFLAGS)
	@echo "✓ Visualization build complete: $(TARGET_VIZ)"

# Link the gas comparison executable (with main_gas_comparison.cpp)
$(TARGET_GAS): $(COMMON_OBJECTS) $(MAIN_GAS_OBJECT) | $(BINDIR)
	$(CXX) $(COMMON_OBJECTS) $(MAIN_GAS_OBJECT) -o $(TARGET_GAS) $(LDFLAGS)
	@echo "✓ Gas comparison build complete: $(TARGET_GAS)"

# Compile source files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create directories if they don't exist
$(BINDIR):
	mkdir -p $(BINDIR)

$(OBJDIR):
	mkdir -p $(OBJDIR)

# Run the regular simulator
run: $(TARGET)
	@echo "Running pipeline simulator..."
	@./$(TARGET)

# Build, run, and open visualization in browser
viz: $(TARGET_VIZ)
	@echo "Running visualization simulator..."
	@./$(TARGET_VIZ)
	@echo ""
	@if [ -f $(VIZ_OUTPUT) ]; then \
		echo "✓ Visualization generated: $(VIZ_OUTPUT)"; \
		echo "Opening in browser..."; \
		if command -v xdg-open > /dev/null; then \
			xdg-open $(VIZ_OUTPUT); \
		elif command -v open > /dev/null; then \
			open $(VIZ_OUTPUT); \
		elif command -v start > /dev/null; then \
			start $(VIZ_OUTPUT); \
		else \
			echo "Please open $(VIZ_OUTPUT) manually in your browser."; \
		fi; \
	else \
		echo "✗ Visualization file not found. Make sure your code generates $(VIZ_OUTPUT)"; \
	fi

# Build and run gas pipeline comparison
gas: $(TARGET_GAS)
	@echo "Running gas pipeline solver comparison..."
	@./$(TARGET_GAS)
	@echo ""
	@echo "✓ Gas comparison complete!"
	@echo "  Generated 4 HTML files for comparison:"
	@echo "    - gas_pipeline_simple.html (SimpleSolver)"
	@echo "    - gas_pipeline_iterative.html (IterativeSolver)"
	@echo "    - gas_pipeline_adaptive.html (AdaptiveSolver)"
	@echo "    - gas_pipeline_rk4.html (RK4Solver)"

# Quick compile, run, and visualize (useful during development)
quick: clean viz

# Build both regular and visualization executables
both: $(TARGET) $(TARGET_VIZ)
	@echo "✓ Both executables built successfully"

# Build all three executables
all-targets: $(TARGET) $(TARGET_VIZ) $(TARGET_GAS)
	@echo "✓ All executables built successfully"

# Clean build artifacts
clean:
	rm -rf $(OBJDIR) $(BINDIR)
	@echo "✓ Cleaned build artifacts"

# Clean everything including visualization output
cleanall: clean
	rm -f $(VIZ_OUTPUT) *.html
	@echo "✓ Cleaned all generated files"

# Build and run without visualization
test: $(TARGET)
	@echo "Running tests..."
	@./$(TARGET)

# Show help
help:
	@echo "Pipeline Simulator Makefile"
	@echo ""
	@echo "Available targets:"
	@echo "  make          - Build the regular simulator (default)"
	@echo "  make run      - Build and run the regular simulator"
	@echo "  make viz      - Build visualization version, run, and open in browser"
	@echo "  make gas      - Build and run gas pipeline solver comparison"
	@echo "  make both     - Build both regular and visualization executables"
	@echo "  make all-targets - Build all executables (regular, viz, and gas)"
	@echo "  make quick    - Clean build and run visualization (full workflow)"
	@echo "  make test     - Build and run regular version (same as 'make run')"
	@echo "  make clean    - Remove build artifacts (obj/ and bin/)"
	@echo "  make cleanall - Remove everything including HTML visualizations"
	@echo "  make help     - Show this help message"
	@echo ""
	@echo "Quick workflow examples:"
	@echo "  make viz      # Most common: compile, run, and view liquid results"
	@echo "  make gas      # Compare all 4 solvers on gas pipeline"
	@echo "  make quick    # Fresh build + visualization"

# Show current configuration
info:
	@echo "Compiler: $(CXX)"
	@echo "Flags: $(CXXFLAGS)"
	@echo "Common sources: $(COMMON_SOURCES)"
	@echo "Common objects: $(COMMON_OBJECTS)"
	@echo "Regular target: $(TARGET)"
	@echo "Viz target: $(TARGET_VIZ)"
	@echo "Gas target: $(TARGET_GAS)"
	@echo "Visualization: $(VIZ_OUTPUT)"

# Phony targets
.PHONY: all run viz gas quick both all-targets clean cleanall test help info
