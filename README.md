# Marble Bouncing Simulator

A 2D physics simulator built with C++ and SDL2 featuring balls bouncing inside a rotating circular container with realistic elastic collisions and gravity.

## Features

- **Realistic Physics**: Elastic collisions between balls conserving momentum and kinetic energy
- **Gravity Simulation**: Balls fall realistically with Earth-like gravity (9.8 m/s²)
- **Rotating Container**: 600px diameter circular container with a 5% gap that rotates every 10 seconds
- **Dynamic Spawning**: Starts with 1 ball; when a ball exits through the bottom, 2 new balls spawn
- **Visual Feedback**:
  - FPS counter
  - Ball count display
  - Color-coded balls (random vibrant colors)

## Requirements

- CMake 3.15 or higher
- C++17 compiler
- SDL2
- SDL2_ttf

## Installation

### macOS

```bash
# Install dependencies
brew install cmake sdl2 sdl2_ttf

# Build the project
mkdir build && cd build
cmake ..
cmake --build .

# Run the simulator
./MarbleBouncing
```

### Linux

```bash
# Install dependencies (Ubuntu/Debian)
sudo apt-get install cmake libsdl2-dev libsdl2-ttf-dev

# Build and run
mkdir build && cd build
cmake ..
cmake --build .
./MarbleBouncing
```

## Controls

- **ESC**: Quit the application
- **Close Window**: Also quits the application

## Physics Details

### Collision Physics
- **Ball-Ball Collisions**: Perfectly elastic collisions using momentum and energy conservation
- **Ball-Wall Collisions**: 100% bounce response (no energy loss)
- **Gravity**: 980 px/s² (scaled for pixel-based simulation)

### Container
- **Diameter**: 600 pixels (300px radius)
- **Gap Size**: 5% of circumference (approximately 18 degrees)
- **Rotation Speed**: 36 degrees per second (full rotation in 10 seconds)

### Balls
- **Diameter**: 25 pixels (12.5px radius)
- **Initial Velocity**: Random direction, 50-200 px/s speed
- **Mass**: Proportional to area (π × r²)

## Project Structure

```
marble-bouncing/
├── CMakeLists.txt
├── README.md
└── src/
    ├── main.cpp
    ├── math/           # Vector math and utilities
    ├── physics/        # Collision detection and resolution
    ├── entities/       # Ball and Container classes
    ├── game/           # Game logic and ball management
    ├── rendering/      # SDL2 rendering wrappers
    └── core/           # Application framework and config
```

## Implementation Highlights

- **Fixed Timestep Physics**: 120Hz physics updates for stable simulation
- **Midpoint Circle Algorithm**: Efficient circle rendering
- **Spatial Math**: Custom 2D vector class with rotation and collision support
- **Gap Detection**: Angle-based detection accounting for rotation wrap-around

## License

This project is provided as-is for educational purposes. 
