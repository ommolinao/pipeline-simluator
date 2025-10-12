#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <chrono>

// ============================================================================
// APPROACH 1: Structure-of-Arrays (Best Cache Locality for Bulk Operations)
// ============================================================================
class PipelineSoA {
private:
    size_t numSegments;
    
    // Geometric properties (rarely change)
    std::vector<double> x_inlet;
    std::vector<double> x_outlet;
    std::vector<double> z_inlet;
    std::vector<double> z_outlet;
    std::vector<double> lengths;
    std::vector<double> diameters;
    std::vector<double> roughness;
    
    // State variables (change during simulation)
    std::vector<double> inletPressures;
    std::vector<double> outletPressures;
    std::vector<double> inletTemps;
    std::vector<double> outletTemps;
    std::vector<double> inletVelocities;
    std::vector<double> outletVelocities;
    
    std::vector<std::string> pipeIDs;

public:
    PipelineSoA() : numSegments(0) {}
    
    // Add a pipe segment
    void addSegment(const std::string& id,
                   double x_in, double x_out, double z_in, double z_out,
                   double length, double diameter, double rough) {
        // Validate connectivity if not first segment
        if (numSegments > 0) {
            double dx = std::abs(x_outlet.back() - x_in);
            double dz = std::abs(z_outlet.back() - z_in);
            if (dx > 1e-6 || dz > 1e-6) {
                std::cerr << "ERROR: Segment not connected!" << std::endl;
                return;
            }
        }
        
        pipeIDs.push_back(id);
        x_inlet.push_back(x_in);
        x_outlet.push_back(x_out);
        z_inlet.push_back(z_in);
        z_outlet.push_back(z_out);
        lengths.push_back(length);
        diameters.push_back(diameter);
        roughness.push_back(rough);
        
        // Initialize state
        inletPressures.push_back(0.0);
        outletPressures.push_back(0.0);
        inletTemps.push_back(293.15);
        outletTemps.push_back(293.15);
        inletVelocities.push_back(0.0);
        outletVelocities.push_back(0.0);
        
        numSegments++;
    }
    
    // Set inlet conditions for first segment
    void setInletConditions(double P, double T, double V) {
        if (numSegments > 0) {
            inletPressures[0] = P;
            inletTemps[0] = T;
            inletVelocities[0] = V;
        }
    }
    
    // Solve all segments (demonstrates cache-friendly iteration)
    void solveAllSegments(double density, double viscosity) {
        // This loop has EXCELLENT cache locality - all lengths accessed sequentially
        for (size_t i = 0; i < numSegments; i++) {
            double Re = density * inletVelocities[i] * diameters[i] / viscosity;
            double f = 0.02; // Simplified friction factor
            
            // Darcy-Weisbach with elevation
            double velocityHead = 0.5 * density * inletVelocities[i] * inletVelocities[i];
            double frictionLoss = f * (lengths[i] / diameters[i]) * velocityHead;
            double elevationLoss = density * 9.81 * (z_outlet[i] - z_inlet[i]);
            
            outletPressures[i] = inletPressures[i] - frictionLoss - elevationLoss;
            outletTemps[i] = inletTemps[i]; // Simplified - isothermal
            outletVelocities[i] = inletVelocities[i]; // Incompressible
            
            // Propagate outlet to next inlet
            if (i + 1 < numSegments) {
                inletPressures[i + 1] = outletPressures[i];
                inletTemps[i + 1] = outletTemps[i];
                inletVelocities[i + 1] = outletVelocities[i];
            }
        }
    }
    
    // Calculate total pressure drop (EXTREMELY cache friendly!)
    double getTotalPressureDrop() const {
        if (numSegments == 0) return 0.0;
        return inletPressures[0] - outletPressures[numSegments - 1];
    }
    
    void displayInfo() const {
        std::cout << "Pipeline with " << numSegments << " segments\n";
        for (size_t i = 0; i < numSegments; i++) {
            std::cout << "  " << pipeIDs[i] 
                      << ": P_in=" << inletPressures[i]/1000.0 << " kPa"
                      << ", P_out=" << outletPressures[i]/1000.0 << " kPa"
                      << ", ΔP=" << (inletPressures[i] - outletPressures[i])/1000.0 << " kPa\n";
        }
    }
};

// ============================================================================
// APPROACH 2: Array-of-Structures with Value Semantics (Good Compromise)
// ============================================================================
struct PipeSegment {
    std::string id;
    double x_inlet, x_outlet;
    double z_inlet, z_outlet;
    double length, diameter, roughness;
    double P_in, P_out;
    double T_in, T_out;
    double v_in, v_out;
    
    PipeSegment(const std::string& id_, double x_in, double x_out,
                double z_in, double z_out, double len, double dia, double rough)
        : id(id_), x_inlet(x_in), x_outlet(x_out), z_inlet(z_in), z_outlet(z_out),
          length(len), diameter(dia), roughness(rough),
          P_in(0), P_out(0), T_in(293.15), T_out(293.15), v_in(0), v_out(0) {}
};

class PipelineAoS {
private:
    std::vector<PipeSegment> segments;  // Contiguous storage!

public:
    void addSegment(const std::string& id,
                   double x_in, double x_out, double z_in, double z_out,
                   double length, double diameter, double rough) {
        // Validate connectivity
        if (!segments.empty()) {
            const auto& last = segments.back();
            double dx = std::abs(last.x_outlet - x_in);
            double dz = std::abs(last.z_outlet - z_in);
            if (dx > 1e-6 || dz > 1e-6) {
                std::cerr << "ERROR: Segment not connected!" << std::endl;
                return;
            }
        }
        
        segments.emplace_back(id, x_in, x_out, z_in, z_out, length, diameter, rough);
    }
    
    void setInletConditions(double P, double T, double V) {
        if (!segments.empty()) {
            segments[0].P_in = P;
            segments[0].T_in = T;
            segments[0].v_in = V;
        }
    }
    
    void solveAllSegments(double density, double viscosity) {
        for (auto& seg : segments) {
            double Re = density * seg.v_in * seg.diameter / viscosity;
            double f = 0.02;
            
            double velocityHead = 0.5 * density * seg.v_in * seg.v_in;
            double frictionLoss = f * (seg.length / seg.diameter) * velocityHead;
            double elevationLoss = density * 9.81 * (seg.z_outlet - seg.z_inlet);
            
            seg.P_out = seg.P_in - frictionLoss - elevationLoss;
            seg.T_out = seg.T_in;
            seg.v_out = seg.v_in;
        }
        
        // Propagate between segments
        for (size_t i = 1; i < segments.size(); i++) {
            segments[i].P_in = segments[i-1].P_out;
            segments[i].T_in = segments[i-1].T_out;
            segments[i].v_in = segments[i-1].v_out;
        }
    }
    
    double getTotalPressureDrop() const {
        if (segments.empty()) return 0.0;
        return segments.front().P_in - segments.back().P_out;
    }
    
    void displayInfo() const {
        std::cout << "Pipeline with " << segments.size() << " segments\n";
        for (const auto& seg : segments) {
            std::cout << "  " << seg.id 
                      << ": P_in=" << seg.P_in/1000.0 << " kPa"
                      << ", P_out=" << seg.P_out/1000.0 << " kPa"
                      << ", ΔP=" << (seg.P_in - seg.P_out)/1000.0 << " kPa\n";
        }
    }
};

// ============================================================================
// APPROACH 3: Hybrid Hot/Cold Data Separation
// ============================================================================
class PipelineHybrid {
private:
    size_t numSegments;
    
    // COLD DATA: Geometry (rarely accessed in tight loops)
    struct Geometry {
        std::string id;
        double x_inlet, x_outlet;
        double z_inlet, z_outlet;
        double length, diameter, roughness;
    };
    std::vector<Geometry> geometry;
    
    // HOT DATA: State (frequently updated/accessed)
    struct State {
        double P_in, P_out;
        double T_in, T_out;
        double v_in, v_out;
    };
    std::vector<State> state;

public:
    PipelineHybrid() : numSegments(0) {}
    
    void addSegment(const std::string& id,
                   double x_in, double x_out, double z_in, double z_out,
                   double length, double diameter, double rough) {
        geometry.push_back({id, x_in, x_out, z_in, z_out, length, diameter, rough});
        state.push_back({0, 0, 293.15, 293.15, 0, 0});
        numSegments++;
    }
    
    void setInletConditions(double P, double T, double V) {
        if (numSegments > 0) {
            state[0].P_in = P;
            state[0].T_in = T;
            state[0].v_in = V;
        }
    }
    
    void solveAllSegments(double density, double viscosity) {
        // Tight loop over HOT data with minimal geometry access
        for (size_t i = 0; i < numSegments; i++) {
            const auto& geom = geometry[i];
            auto& st = state[i];
            
            double Re = density * st.v_in * geom.diameter / viscosity;
            double f = 0.02;
            
            double velocityHead = 0.5 * density * st.v_in * st.v_in;
            double frictionLoss = f * (geom.length / geom.diameter) * velocityHead;
            double elevationLoss = density * 9.81 * (geom.z_outlet - geom.z_inlet);
            
            st.P_out = st.P_in - frictionLoss - elevationLoss;
            st.T_out = st.T_in;
            st.v_out = st.v_in;
            
            if (i + 1 < numSegments) {
                state[i+1].P_in = st.P_out;
                state[i+1].T_in = st.T_out;
                state[i+1].v_in = st.v_out;
            }
        }
    }
    
    double getTotalPressureDrop() const {
        if (numSegments == 0) return 0.0;
        return state[0].P_in - state[numSegments-1].P_out;
    }
    
    void displayInfo() const {
        std::cout << "Pipeline with " << numSegments << " segments\n";
        for (size_t i = 0; i < numSegments; i++) {
            std::cout << "  " << geometry[i].id 
                      << ": P_in=" << state[i].P_in/1000.0 << " kPa"
                      << ", P_out=" << state[i].P_out/1000.0 << " kPa"
                      << ", ΔP=" << (state[i].P_in - state[i].P_out)/1000.0 << " kPa\n";
        }
    }
};

// ============================================================================
// BENCHMARK
// ============================================================================
int main() {
    const int N = 10000;  // Number of segments for benchmarking
    const int ITERATIONS = 1000;
    
    std::cout << "=== CACHE LOCALITY COMPARISON ===\n\n";
    
    // Build test pipelines
    PipelineSoA pipelineSoA;
    PipelineAoS pipelineAoS;
    PipelineHybrid pipelineHybrid;
    
    std::cout << "Building pipelines with " << N << " segments...\n";
    
    for (int i = 0; i < N; i++) {
        double x_in = i * 100.0;
        double x_out = (i + 1) * 100.0;
        double z_in = i * 0.1;
        double z_out = (i + 1) * 0.1;
        
        pipelineSoA.addSegment("Seg-" + std::to_string(i),
                               x_in, x_out, z_in, z_out,
                               100.0, 0.3, 0.000045);
        pipelineAoS.addSegment("Seg-" + std::to_string(i),
                               x_in, x_out, z_in, z_out,
                               100.0, 0.3, 0.000045);
        pipelineHybrid.addSegment("Seg-" + std::to_string(i),
                                  x_in, x_out, z_in, z_out,
                                  100.0, 0.3, 0.000045);
    }
    
    pipelineSoA.setInletConditions(1000000.0, 293.15, 2.0);
    pipelineAoS.setInletConditions(1000000.0, 293.15, 2.0);
    pipelineHybrid.setInletConditions(1000000.0, 293.15, 2.0);
    
    double density = 1000.0;
    double viscosity = 0.001;
    
    // Benchmark SoA
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < ITERATIONS; i++) {
        pipelineSoA.solveAllSegments(density, viscosity);
    }
    auto end = std::chrono::high_resolution_clock::now();
    double timeSoA = std::chrono::duration<double>(end - start).count();
    
    // Benchmark AoS
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < ITERATIONS; i++) {
        pipelineAoS.solveAllSegments(density, viscosity);
    }
    end = std::chrono::high_resolution_clock::now();
    double timeAoS = std::chrono::duration<double>(end - start).count();
    
    // Benchmark Hybrid
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < ITERATIONS; i++) {
        pipelineHybrid.solveAllSegments(density, viscosity);
    }
    end = std::chrono::high_resolution_clock::now();
    double timeHybrid = std::chrono::duration<double>(end - start).count();
    
    std::cout << "\n=== BENCHMARK RESULTS (" << ITERATIONS << " iterations) ===\n";
    std::cout << "Structure-of-Arrays:  " << timeSoA << " sec\n";
    std::cout << "Array-of-Structures:  " << timeAoS << " sec\n";
    std::cout << "Hybrid Hot/Cold:      " << timeHybrid << " sec\n";
    
    std::cout << "\nSpeedup vs AoS:\n";
    std::cout << "  SoA:    " << timeAoS / timeSoA << "x\n";
    std::cout << "  Hybrid: " << timeAoS / timeHybrid << "x\n";
    
    // std::cout << "\n=== EXAMPLE OUTPUT ===\n";
    // pipelineHybrid.displayInfo();
    
    return 0;
}
