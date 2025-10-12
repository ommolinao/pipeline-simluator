# Compiler settings
CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -Iinclude
LDFLAGS = -lm

# Directories
SRCDIR = src
INCDIR = include
OBJDIR = obj
BINDIR = bin

# Target executable
TARGET = $(BINDIR)/pipeline_simulator

# Source files
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SOURCES))

# Header files (for dependency tracking)
HEADERS = $(wildcard $(INCDIR)/*.h)

# Default target
all: directories $(TARGET)

# Create necessary directories
directories:
	@mkdir -p $(OBJDIR)
	@mkdir -p $(BINDIR)

# Link object files to create executable
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)
	@echo "Build complete: $(TARGET)"

# Compile source files to object files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -rf $(OBJDIR) $(BINDIR)
	@echo "Clean complete"

# Run the program
run: $(TARGET)
	./$(TARGET)

# Debug build
debug: CXXFLAGS += -g -O0
debug: clean all

# Release build with optimizations
release: CXXFLAGS += -O3 -DNDEBUG
release: clean all

# Show help
help:
	@echo "Available targets:"
	@echo "  all      - Build the project (default)"
	@echo "  clean    - Remove build artifacts"
	@echo "  run      - Build and run the program"
	@echo "  debug    - Build with debug symbols"
	@echo "  release  - Build with optimizations"
	@echo "  help     - Show this help message"

.PHONY: all clean run debug release help directories
