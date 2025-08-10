# Black Hole Simulator

A real-time black hole simulation featuring gravitational lensing, accretion disk visualization, and orbiting planets. Available in three implementations:

- **HTML/WebGL** - Browser-based version with interactive controls
- **Windows** - Native C++ application using OpenGL
- **Linux** - Cross-platform C++ implementation

![Black Hole Simulation Preview](https://via.placeholder.com/800x400/000000/FFFFFF?text=Black+Hole+Simulator)

## Features

### Core Simulation
- **Gravitational Lensing**: Real-time light bending around the black hole using curved geodesic approximation
- **Accretion Disk**: Dynamic, rotating disk with spiral arms, temperature gradients, and Doppler effects
- **Orbiting Planets**: Two planets with realistic orbital mechanics and lighting
- **Procedural Starfield**: High-quality background with twinkling stars and nebula effects
- **Interactive Controls**: Real-time parameter adjustment and camera controls
- **Adaptive Performance**: Dynamic quality adjustment based on framerate

### Visual Effects
- **Ray Marching**: Accurate light transport simulation
- **Keplerian Orbits**: Realistic orbital motion (ω ∝ r^-1.5)
- **Doppler Shifts**: Color changes based on orbital velocity
- **Bloom/Glow**: Enhanced visual appeal
- **Gamma Correction**: Proper color space handling

## Platforms

### 🌐 HTML/WebGL Version

The web version runs directly in your browser using WebGL.

**Requirements:**
- Modern web browser with WebGL support
- Hardware-accelerated graphics

**Features:**
- Zero installation required
- Cross-platform compatibility
- Mobile device support (touch controls)
- Real-time parameter sliders

**Access:**
Open `index.html` in your web browser or visit the live demo.

**Controls:**
- **Mouse**: Drag to orbit camera, scroll to zoom
- **Touch**: Single finger drag to orbit, pinch to zoom
- **UI Sliders**: Adjust mass and disk size
- **Checkboxes**: Toggle features on/off

### 🪟 Windows Version

Native C++ implementation optimized for Windows.

**Requirements:**
- Windows 10/11
- Visual Studio 2019/2022
- OpenGL 3.3+ compatible graphics card
- DirectX compatible hardware

**Build Instructions:**
1. Open `Windows/Blackhole.sln` in Visual Studio
2. Ensure dependencies are in place:
   - GLFW library: `Windows/Libraries/lib/glfw3.lib`
   - Headers: `Windows/Libraries/include/`
3. Build the solution (F7)
4. Run the executable

**Features:**
- Optimized performance
- Full keyboard shortcuts
- Real-time FPS monitoring
- Advanced OpenGL features

### 🐧 Linux Version

Cross-platform C++ implementation using CMake.

**Requirements:**
- GCC 8+ or Clang 10+
- CMake 3.16+
- OpenGL 3.3+ support
- GLFW 3.x development libraries

**Build Instructions:**
```bash
# Install dependencies (Ubuntu/Debian)
sudo apt update
sudo apt install build-essential cmake libglfw3-dev libgl1-mesa-dev

# Clone and build
cd cpp/
chmod +x run.sh
./run.sh
```

**Manual Build:**
```bash
cd cpp/
mkdir build
cd build
cmake ..
make
./blackhole
```

**Features:**
- Cross-platform compatibility
- Optimized for Unix-like systems
- Terminal-based status output
- Lightweight dependencies

## Controls

### Universal Controls (All Platforms)

#### Camera Movement
- **Mouse Drag** / **Touch Drag**: Orbit camera around black hole
- **Mouse Wheel** / **Pinch**: Zoom in/out

#### Feature Toggles
- **1**: Toggle starfield background
- **2**: Toggle orbiting planets
- **3**: Toggle accretion disk
- **4**: Toggle gravitational lensing

#### Parameter Adjustment
- **Q** / **+**: Increase black hole mass
- **A** / **-**: Decrease black hole mass
- **W** / **]**: Increase disk outer radius
- **S** / **[**: Decrease disk outer radius
- **R**: Reset to default parameters
- **ESC**: Exit application

### Platform-Specific Controls

#### HTML/WebGL
- **GUI Sliders**: Real-time mass and disk size adjustment
- **Checkboxes**: Visual feature toggles
- **Touch Support**: Mobile-friendly controls

#### Windows/Linux
- **Terminal Output**: Real-time parameter display
- **Keyboard Shortcuts**: Full control via hotkeys
- **Performance Metrics**: FPS and optimization info

## Technical Details

### Physics Simulation
- **Schwarzschild Metric**: Simplified gravitational field
- **Ray Marching**: Per-pixel light transport
- **Adaptive Step Sizes**: Performance optimization
- **Event Horizon**: Proper light absorption

### Visual Techniques
- **Procedural Generation**: Starfield and disk patterns
- **Noise Functions**: Turbulence and detail
- **Color Temperature**: Realistic disk coloration
- **Anti-aliasing**: Smooth visual quality

### Performance Features
- **Dynamic Quality**: FPS-based optimization
- **Early Ray Termination**: Efficiency improvements
- **Conditional Rendering**: Skip disabled features
- **Memory Optimization**: Minimal GPU usage

## Parameters

### Black Hole Properties
- **Mass**: 0.1 - 5.0 (affects lensing strength)
- **Schwarzschild Radius**: Rs = 2GM/c² (auto-calculated)
- **Event Horizon**: Light absorption boundary

### Accretion Disk
- **Inner Radius**: 1.5 × Rs (innermost stable orbit)
- **Outer Radius**: 2.0 - 30.0 units (user adjustable)
- **Temperature**: Blue-white (inner) → Orange-red (outer)
- **Spiral Arms**: 3-arm logarithmic pattern

### Planets
- **Planet 1**: Rocky, 18 units orbit, 0.4 radius
- **Planet 2**: Gas giant, 30 units orbit, 1.0 radius
- **Lighting**: Illuminated by accretion disk

## Performance Optimization

### Automatic Adjustments
- **Step Size**: Larger when far from geometry
- **Max Steps**: Reduced for better performance
- **Feature LOD**: Dynamic quality based on distance
- **FPS Targeting**: Maintain 30+ FPS

### Manual Optimization
- Disable lensing for 2x performance boost
- Turn off planets for minor improvement
- Reduce window size for mobile devices
- Close other applications for more GPU resources

## Troubleshooting

### Common Issues

#### Performance Problems
- **Low FPS**: Disable gravitational lensing, reduce window size
- **Stuttering**: Update graphics drivers, close background apps
- **Overheating**: Lower quality settings, improve ventilation

#### Visual Issues
- **Black Screen**: Check OpenGL/WebGL support
- **Artifacts**: Update graphics drivers
- **Wrong Colors**: Verify monitor color profile

#### Build Problems
- **Linux**: Install development packages, check CMake version
- **Windows**: Verify Visual Studio C++ tools, check library paths
- **WebGL**: Use modern browser, enable hardware acceleration

### System Requirements

#### Minimum
- **GPU**: OpenGL 3.3 / WebGL 2.0 support
- **RAM**: 2GB available
- **CPU**: Dual-core 2GHz+

#### Recommended
- **GPU**: Dedicated graphics card with 1GB+ VRAM
- **RAM**: 4GB+ available
- **CPU**: Quad-core 3GHz+
- **Display**: 1920×1080 or higher

## Educational Value

This simulation demonstrates several important physics and computer graphics concepts:

### Physics
- **General Relativity**: Spacetime curvature effects
- **Orbital Mechanics**: Keplerian motion laws
- **Astrophysics**: Black hole accretion processes
- **Optics**: Light bending and gravitational lensing

### Computer Graphics
- **Ray Marching**: Alternative to rasterization
- **Procedural Generation**: Algorithmic content creation
- **Shader Programming**: GPU-based computation
- **Real-time Rendering**: Interactive graphics techniques

## Development

### Project Structure
```
/
├── index.html              # WebGL version
├── Windows/                # Visual Studio project
│   ├── Blackhole.sln      # Solution file
│   ├── *.cpp, *.h         # Source code
│   └── Libraries/         # Dependencies
├── cpp/                   # Linux/cross-platform
│   ├── CMakeLists.txt     # Build configuration
│   ├── src/               # Source code
│   └── shaders/           # GLSL shaders
└── README.md              # This file
```

### Contributing
1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test on multiple platforms
5. Submit a pull request

### Future Enhancements
- **Ray Tracing**: More accurate light simulation
- **Relativistic Effects**: Proper general relativity
- **Particle Systems**: Dynamic debris and jets
- **VR Support**: Immersive experience
- **Educational Mode**: Interactive physics lessons

## Credits and References

### Scientific Basis
- Einstein's General Theory of Relativity
- Schwarzschild solution for black hole spacetime
- Astrophysical journal papers on accretion disks
- NASA/ESA black hole observations

### Technical References
- Real-time rendering techniques
- GPU shader programming guides
- Ray marching algorithms
- OpenGL/WebGL specifications

### Libraries and Tools
- **GLFW**: Window management (Windows/Linux)
- **GLAD**: OpenGL loading (Windows/Linux)
- **WebGL**: Browser graphics API
- **CMake**: Cross-platform build system

## License

This project is released under the MIT License. See LICENSE file for details.

Educational and demonstration use encouraged. Commercial use permitted with attribution.

---

*Experience the fascinating physics of black holes in real-time!*
