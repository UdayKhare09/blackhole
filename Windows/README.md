# Black Hole Simulator

A real-time black hole simulation implemented in C++ using OpenGL, featuring gravitational lensing, accretion disk visualization, and orbiting planets.

## Features

- **Gravitational Lensing**: Real-time simulation of light bending around a black hole using curved geodesic approximation
- **Accretion Disk**: Dynamic, rotating accretion disk with spiral arms and temperature-based coloring
- **Orbiting Planets**: Two planets orbiting the black hole with realistic lighting
- **Procedural Starfield**: High-quality background starfield with twinkling stars and nebula effects
- **Interactive Controls**: Mouse/keyboard controls for camera movement and parameter adjustment
- **Adaptive Performance**: Dynamic step size adjustment based on scene complexity and framerate

## Requirements

- Windows 10/11
- Visual Studio 2019/2022
- OpenGL 3.3+ compatible graphics card
- GLFW 3.x
- GLAD OpenGL loader

## Controls

### Mouse Controls
- **Left Mouse Drag**: Orbit camera around the black hole
- **Mouse Wheel**: Zoom in/out

### Keyboard Controls
- **1**: Toggle starfield on/off
- **2**: Toggle planets on/off
- **3**: Toggle accretion disk on/off
- **4**: Toggle gravitational lensing on/off
- **Q**: Increase black hole mass
- **A**: Decrease black hole mass
- **W**: Increase disk size
- **S**: Decrease disk size
- **R**: Reset to default parameters
- **ESC**: Exit simulation

## Build Instructions

1. Ensure all dependencies are properly installed:
   - GLFW 3.x library in `Libraries/lib/glfw3.lib`
   - GLFW headers in `Libraries/include/GLFW/`
   - GLAD headers in `Libraries/include/glad/`

2. Open `Blackhole.vcxproj` in Visual Studio

3. Build the project (F7 or Build > Build Solution)

4. Run the executable

## Project Structure

```
Blackhole/
??? Main.cpp                    # Application entry point and GLFW setup
??? BlackHoleSimulation.h       # Main simulation class header
??? BlackHoleSimulation.cpp     # Simulation implementation with shaders
??? glad.c                      # GLAD OpenGL loader implementation
??? Libraries/
?   ??? include/
?   ?   ??? GLFW/              # GLFW headers
?   ?   ??? glad/              # GLAD headers
?   ?   ??? KHR/               # Khronos headers
?   ??? lib/
?       ??? glfw3.lib          # GLFW static library
??? shaders/                   # Shader files (for reference)
    ??? particle.vert
    ??? particle.frag
```

## Technical Details

### Rendering Technique
The simulation uses a ray marching approach implemented entirely in the fragment shader. Each pixel traces a ray through the scene, accumulating color contributions from:

1. **Black Hole Event Horizon**: Rays that get too close are absorbed
2. **Gravitational Lensing**: Ray directions are bent by simulated gravitational acceleration
3. **Accretion Disk**: Semi-transparent disk with spiral patterns and Doppler effects
4. **Planets**: Solid spheres with basic Lambertian lighting
5. **Starfield**: Procedural background with realistic star distribution

### Performance Optimizations
- **Adaptive Step Sizes**: Larger steps when far from interesting geometry
- **Early Ray Termination**: Stop tracing when rays escape or hit opaque objects
- **Dynamic Quality**: Adjust rendering complexity based on current framerate
- **Conditional Features**: Skip expensive calculations when features are disabled

### Mathematical Models
- **Schwarzschild Metric**: Simplified gravitational field approximation
- **Keplerian Orbits**: Realistic orbital motion for planets and disk material
- **Doppler Shifts**: Color shifts in the accretion disk based on orbital velocity
- **Noise Functions**: Procedural patterns for disk turbulence and starfield

## Parameters

### Black Hole
- **Mass**: Controls the strength of gravitational lensing (0.1 - 5.0)
- **Schwarzschild Radius**: Event horizon size (automatically calculated from mass)

### Accretion Disk
- **Inner Radius**: 1.5 × Schwarzschild radius (automatically calculated)
- **Outer Radius**: User adjustable (2.0 - 30.0 units)
- **Rotation**: Keplerian velocity profile (? ? r^-1.5)
- **Temperature Gradient**: Blue-white (inner) to orange-red (outer)

### Planets
- **Planet 1**: Rocky planet at 18 units, 0.4 unit radius
- **Planet 2**: Gas giant at 30 units, 1.0 unit radius
- **Orbital Periods**: Realistic based on distance from center

## Troubleshooting

### Performance Issues
- Disable gravitational lensing (key 4) for better performance
- Reduce window size
- Update graphics drivers

### Visual Artifacts
- Ensure graphics card supports OpenGL 3.3+
- Check for driver updates
- Try running in compatibility mode

### Build Errors
- Verify library paths in project settings
- Ensure GLFW and GLAD are properly installed
- Check Visual Studio C++ toolset version

## Future Enhancements

- **Ray Tracing**: More accurate light transport simulation
- **Relativistic Effects**: Proper general relativity calculations
- **Particle Systems**: Dynamic debris and gas flows
- **Multiple Black Holes**: Binary systems and interactions
- **VR Support**: Immersive virtual reality experience

## License

This project is for educational and demonstration purposes. The WebGL implementation it's based on can be found in the original HTML file.

## Credits

- Based on WebGL black hole simulator concepts
- Uses GLFW for window management
- Uses GLAD for OpenGL loading
- Mathematical foundations from general relativity and astrophysics literature