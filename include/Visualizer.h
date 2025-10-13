#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "Pipeline.h"
#include "Fluid.h"

/**
 * @brief Visualizer class for generating interactive HTML visualizations
 * 
 * Creates HTML files with Plotly.js charts showing:
 * - Pressure profile along pipeline
 * - Elevation profile
 * - Velocity profile
 * - Temperature profile
 * - 2D pipeline schematic
 */
class Visualizer {
private:
    std::string outputFilename;
    
    /**
     * @brief Escapes special characters for JSON strings
     */
    std::string escapeJson(const std::string& str) const {
        std::string escaped;
        for (char c : str) {
            if (c == '"') escaped += "\\\"";
            else if (c == '\\') escaped += "\\\\";
            else escaped += c;
        }
        return escaped;
    }
    
    /**
     * @brief Converts vector of doubles to JSON array string
     */
    std::string vectorToJson(const std::vector<double>& vec) const {
        std::ostringstream oss;
        oss << "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            oss << vec[i];
            if (i < vec.size() - 1) oss << ",";
        }
        oss << "]";
        return oss.str();
    }
    
    /**
     * @brief Converts vector of strings to JSON array string
     */
    std::string vectorToJson(const std::vector<std::string>& vec) const {
        std::ostringstream oss;
        oss << "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            oss << "\"" << escapeJson(vec[i]) << "\"";
            if (i < vec.size() - 1) oss << ",";
        }
        oss << "]";
        return oss.str();
    }
    
    /**
     * @brief Calculate cumulative distance along pipeline
     */
    std::vector<double> calculateCumulativeDistance(const Pipeline& pipeline) const {
        std::vector<double> distances;
        double cumulative = 0.0;
        
        for (size_t i = 0; i < pipeline.getNumSegments(); ++i) {
            distances.push_back(cumulative);
            cumulative += pipeline.getLength(i);
        }
        distances.push_back(cumulative); // Add final point
        
        return distances;
    }

public:
    /**
     * @brief Constructor
     * @param filename Output HTML filename (default: "pipeline_visualization.html")
     */
    Visualizer(const std::string& filename = "pipeline_visualization.html")
        : outputFilename(filename) {}
    
    /**
     * @brief Generate interactive HTML visualization
     * @param pipeline Pipeline object to visualize
     * @param fluid Fluid object for additional information
     * @param title Title for the visualization
     * @return true if successful, false otherwise
     */
    bool generate(const Pipeline& pipeline, const Fluid& fluid, 
                  const std::string& title = "Pipeline Analysis Results") {
        
        size_t n = pipeline.getNumSegments();
        if (n == 0) {
            std::cerr << "Error: Pipeline has no segments to visualize\n";
            return false;
        }
        
        // Calculate cumulative distances
        std::vector<double> distances = calculateCumulativeDistance(pipeline);
        
        // Collect data from pipeline (using SoA arrays)
        std::vector<double> pressures_inlet, pressures_outlet;
        std::vector<double> elevations_inlet, elevations_outlet;
        std::vector<double> velocities_inlet, velocities_outlet;
        std::vector<double> temps_inlet, temps_outlet;
        std::vector<std::string> pipeIDs;
        
        for (size_t i = 0; i < n; ++i) {
            pressures_inlet.push_back(pipeline.getInletPressure(i));
            pressures_outlet.push_back(pipeline.getOutletPressure(i));
            elevations_inlet.push_back(pipeline.getZInlet(i));
            elevations_outlet.push_back(pipeline.getZOutlet(i));
            velocities_inlet.push_back(pipeline.getInletVelocity(i));
            velocities_outlet.push_back(pipeline.getOutletVelocity(i));
            temps_inlet.push_back(pipeline.getInletTemperature(i));
            temps_outlet.push_back(pipeline.getOutletTemperature(i));
            pipeIDs.push_back(pipeline.getPipeID(i));
        }
        
        // Build continuous profiles (inlet of segment i, then add outlet of last segment)
        std::vector<double> pressure_profile, elevation_profile, velocity_profile, temp_profile;
        for (size_t i = 0; i < n; ++i) {
            pressure_profile.push_back(pressures_inlet[i]);
            elevation_profile.push_back(elevations_inlet[i]);
            velocity_profile.push_back(velocities_inlet[i]);
            temp_profile.push_back(temps_inlet[i]);
        }
        // Add final outlet values
        pressure_profile.push_back(pressures_outlet[n-1]);
        elevation_profile.push_back(elevations_outlet[n-1]);
        velocity_profile.push_back(velocities_outlet[n-1]);
        temp_profile.push_back(temps_outlet[n-1]);
        
        // Open output file
        std::ofstream outFile(outputFilename);
        if (!outFile) {
            std::cerr << "Error: Could not open output file: " << outputFilename << "\n";
            return false;
        }
        
        // Write HTML with embedded Plotly.js
        outFile << R"(<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>)" << escapeJson(title) << R"(</title>
    <script src="https://cdn.plot.ly/plotly-2.27.0.min.js"></script>
    <style>
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            margin: 20px;
            background-color: #f5f5f5;
        }
        h1 {
            color: #333;
            text-align: center;
        }
        .info-box {
            background-color: white;
            padding: 15px;
            margin: 20px auto;
            max-width: 800px;
            border-radius: 8px;
            box-shadow: 0 2px 4px rgba(0,0,0,0.1);
        }
        .plot-container {
            background-color: white;
            margin: 20px auto;
            padding: 20px;
            border-radius: 8px;
            box-shadow: 0 2px 4px rgba(0,0,0,0.1);
        }
        .grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
            gap: 10px;
            margin: 10px 0;
        }
        .info-item {
            padding: 8px;
            background-color: #f9f9f9;
            border-left: 3px solid #4CAF50;
        }
    </style>
</head>
<body>
    <h1>)" << escapeJson(title) << R"(</h1>
    
    <div class="info-box">
        <h2>Pipeline Information</h2>
        <div class="grid">
            <div class="info-item"><strong>Number of Segments:</strong> )" << n << R"(</div>
            <div class="info-item"><strong>Total Length:</strong> )" << distances.back() << R"( m</div>
            <div class="info-item"><strong>Fluid Type:</strong> )" << escapeJson(fluid.getName()) << R"(</div>
        </div>
    </div>
    
    <div class="plot-container">
        <div id="pressurePlot"></div>
    </div>
    
    <div class="plot-container">
        <div id="elevationPlot"></div>
    </div>
    
    <div class="plot-container">
        <div id="velocityPlot"></div>
    </div>
    
    <div class="plot-container">
        <div id="temperaturePlot"></div>
    </div>
    
    <div class="plot-container">
        <div id="schematicPlot"></div>
    </div>

    <script>
        // Data from C++
        const distances = )" << vectorToJson(distances) << R"(;
        const pressures = )" << vectorToJson(pressure_profile) << R"(;
        const elevations = )" << vectorToJson(elevation_profile) << R"(;
        const velocities = )" << vectorToJson(velocity_profile) << R"(;
        const temperatures = )" << vectorToJson(temp_profile) << R"(;
        const pipeIDs = )" << vectorToJson(pipeIDs) << R"(;
        
        // Common layout settings
        const commonLayout = {
            font: { family: 'Segoe UI, sans-serif' },
            plot_bgcolor: '#fafafa',
            paper_bgcolor: 'white',
            hovermode: 'x unified'
        };
        
        // Pressure Profile
        const pressureTrace = {
            x: distances,
            y: pressures,
            type: 'scatter',
            mode: 'lines+markers',
            name: 'Pressure',
            line: { color: '#2196F3', width: 3 },
            marker: { size: 8 }
        };
        
        const pressureLayout = {
            ...commonLayout,
            title: 'Pressure Profile',
            xaxis: { title: 'Distance along pipeline (m)' },
            yaxis: { title: 'Pressure (Pa)' }
        };
        
        Plotly.newPlot('pressurePlot', [pressureTrace], pressureLayout, {responsive: true});
        
        // Elevation Profile
        const elevationTrace = {
            x: distances,
            y: elevations,
            type: 'scatter',
            mode: 'lines+markers',
            name: 'Elevation',
            fill: 'tozeroy',
            line: { color: '#4CAF50', width: 3 },
            marker: { size: 8 }
        };
        
        const elevationLayout = {
            ...commonLayout,
            title: 'Elevation Profile',
            xaxis: { title: 'Distance along pipeline (m)' },
            yaxis: { title: 'Elevation (m)' }
        };
        
        Plotly.newPlot('elevationPlot', [elevationTrace], elevationLayout, {responsive: true});
        
        // Velocity Profile
        const velocityTrace = {
            x: distances,
            y: velocities,
            type: 'scatter',
            mode: 'lines+markers',
            name: 'Velocity',
            line: { color: '#FF9800', width: 3 },
            marker: { size: 8 }
        };
        
        const velocityLayout = {
            ...commonLayout,
            title: 'Velocity Profile',
            xaxis: { title: 'Distance along pipeline (m)' },
            yaxis: { title: 'Velocity (m/s)' }
        };
        
        Plotly.newPlot('velocityPlot', [velocityTrace], velocityLayout, {responsive: true});
        
        // Temperature Profile
        const temperatureTrace = {
            x: distances,
            y: temperatures,
            type: 'scatter',
            mode: 'lines+markers',
            name: 'Temperature',
            line: { color: '#F44336', width: 3 },
            marker: { size: 8 }
        };
        
        const temperatureLayout = {
            ...commonLayout,
            title: 'Temperature Profile',
            xaxis: { title: 'Distance along pipeline (m)' },
            yaxis: { title: 'Temperature (K)' }
        };
        
        Plotly.newPlot('temperaturePlot', [temperatureTrace], temperatureLayout, {responsive: true});
        
        // Pipeline Schematic (2D layout)
        // Build x and z positions for schematic (inlet of each segment + final outlet)
        const xSchematic = [];
        const zSchematic = [];
        
        // Add inlet positions of all segments
        for (let i = 0; i < )" << n << R"(; i++) {
            xSchematic.push(distances[i]);
            zSchematic.push(elevations[i]);
        }
        // Add final outlet position
        xSchematic.push(distances[)" << n << R"(]);
        zSchematic.push(elevations[)" << n << R"(]);
        
        const schematicTrace = {
            x: xSchematic,
            y: zSchematic,
            type: 'scatter',
            mode: 'lines+markers',
            name: 'Pipeline Layout',
            line: { color: '#9C27B0', width: 4 },
            marker: { size: 10 }
        };
        
        const schematicLayout = {
            ...commonLayout,
            title: '2D Pipeline Schematic',
            xaxis: { title: 'Distance along pipeline (m)' },
            yaxis: { title: 'Elevation (m)', scaleanchor: 'x' }
        };
        
        Plotly.newPlot('schematicPlot', [schematicTrace], schematicLayout, {responsive: true});
    </script>
</body>
</html>
)";
        
        outFile.close();
        std::cout << "✓ Visualization generated: " << outputFilename << "\n";
        std::cout << "  Open this file in a web browser to view interactive plots.\n";
        
        return true;
    }
    
    /**
     * @brief Set output filename
     */
    void setOutputFilename(const std::string& filename) {
        outputFilename = filename;
    }
    
    /**
     * @brief Get current output filename
     */
    std::string getOutputFilename() const {
        return outputFilename;
    }
};

#endif // VISUALIZER_H
