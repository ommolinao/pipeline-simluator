#!/bin/bash

# Simple build script for pipeline simulator
# Alternative to using Makefile

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Configuration
CXX=g++
CXXFLAGS="-std=c++20 -Wall -Wextra -Iinclude"
SRCDIR="src"
OBJDIR="obj"
BINDIR="bin"
TARGET="$BINDIR/pipeline_simulator"

# Create directories
echo -e "${YELLOW}Creating build directories...${NC}"
mkdir -p $OBJDIR
mkdir -p $BINDIR

# Source files
SOURCES=(
    "Fluid.cpp"
    "Liquid.cpp"
    "Gas.cpp"
    "Pipe.cpp"
    "Pipeline.cpp"
    "Solver.cpp"
    "SimpleSolver.cpp"
    "IterativeSolver.cpp"
    "AdaptiveSolver.cpp"
    "main.cpp"
)

# Compile each source file
echo -e "${YELLOW}Compiling source files...${NC}"
for src in "${SOURCES[@]}"; do
    obj="${src%.cpp}.o"
    echo "  Compiling $src..."
    $CXX $CXXFLAGS -c "$SRCDIR/$src" -o "$OBJDIR/$obj"
    if [ $? -ne 0 ]; then
        echo -e "${RED}Error compiling $src${NC}"
        exit 1
    fi
done

# Link object files
echo -e "${YELLOW}Linking...${NC}"
OBJECTS=""
for src in "${SOURCES[@]}"; do
    obj="${src%.cpp}.o"
    OBJECTS="$OBJECTS $OBJDIR/$obj"
done

$CXX $OBJECTS -o $TARGET -lm
if [ $? -ne 0 ]; then
    echo -e "${RED}Error linking${NC}"
    exit 1
fi

echo -e "${GREEN}Build successful!${NC}"
echo -e "${GREEN}Executable: $TARGET${NC}"
echo ""
echo "Run with: ./$TARGET"
