# Marble Bouncing - Project Memory

## Project Overview

A C++17 physics simulation game featuring balls bouncing inside a rotating circular container with a gap. Balls fall under gravity, collide elastically, and can escape through the gap. The project demonstrates real-time physics simulation, spatial optimization, and interactive parameter tuning.

**Technology Stack:**
- Language: C++17
- Build System: CMake 3.15+
- Graphics: SDL2, SDL2_ttf
- Architecture: Custom physics engine (no external physics libraries)

---

## Build Commands

### macOS
```bash
brew install cmake sdl2 sdl2_ttf
mkdir build && cd build
cmake ..
cmake --build .
./MarbleBouncing
```

### Linux (Ubuntu/Debian)
```bash
sudo apt-get install cmake libsdl2-dev libsdl2-ttf-dev
mkdir build && cd build
cmake ..
cmake --build .
./MarbleBouncing
```

### Rebuild after changes
```bash
cd build
cmake --build .
```

---

## Project Structure

```
src/
├── main.cpp                      # Entry point (minimal bootstrapper)
├── core/                         # Application framework
│   ├── Application.h/cpp         # Main game loop, fixed timestep (120Hz)
│   ├── Config.h                  # Global constants and configuration
│   └── Time.h/cpp                # Frame timing utilities
├── math/                         # Vector mathematics
│   ├── Vector2D.h/cpp            # 2D vector with full operator support
│   └── MathUtils.h/cpp           # Math helper functions
├── physics/                      # Physics simulation engine
│   ├── PhysicsEngine.h/cpp       # Physics update orchestrator
│   ├── CollisionDetector.h/cpp   # Collision detection algorithms
│   ├── CollisionResolver.h/cpp   # Collision response calculation
│   └── SpatialGrid.h/cpp         # Spatial hashing optimization
├── entities/                     # Game objects
│   ├── Ball.h/cpp                # Ball physics body
│   └── Container.h/cpp           # Rotating circular container
├── game/                         # Game logic
│   ├── GameState.h/cpp           # Game state coordinator
│   └── BallManager.h/cpp         # Ball lifecycle management
├── rendering/                    # Rendering subsystem
│   ├── Renderer.h/cpp            # SDL2 wrapper, frame management
│   ├── CircleRenderer.h/cpp      # Midpoint circle algorithm
│   ├── CircleTextureCache.h/cpp  # Pre-rendered circle textures
│   └── TextRenderer.h/cpp        # UI text rendering
└── ui/                           # User interface
    ├── Slider.h/cpp              # Interactive sliders
    └── Button.h/cpp              # Interactive buttons
```

---

## Key Architecture Patterns

### Fixed Timestep Physics Loop
Physics updates run at fixed 120Hz (8.33ms per step) regardless of frame rate:
- Accumulator pattern in [Application.cpp](src/core/Application.cpp)
- Max 5 physics steps per frame to prevent spiral of death
- Rendering interpolates between physics states

### Component-Based Architecture
- **Entities** (Ball, Container) hold state
- **Systems** (PhysicsEngine, BallManager, Renderer) operate on entities
- **GameState** coordinates all systems

### Spatial Optimization
- [SpatialGrid.h/cpp](src/physics/SpatialGrid.h) implements spatial hashing
- Reduces collision detection from O(n²) to ~O(n)
- Critical for performance with many balls

### Collision Physics
- **Detection**: [CollisionDetector](src/physics/CollisionDetector.cpp) finds overlaps
- **Resolution**: [CollisionResolver](src/physics/CollisionResolver.cpp) calculates elastic response
- Ball-ball: Circle-circle distance checking
- Ball-container: Angle-based gap detection accounting for rotation

---

## Key Configuration Constants

Located in [src/core/Config.h](src/core/Config.h):

```cpp
// Window
constexpr int WINDOW_WIDTH = 1024;
constexpr int WINDOW_HEIGHT = 768;

// Container
constexpr float CONTAINER_RADIUS = 300.0f;
constexpr float CONTAINER_GAP_PERCENTAGE = 0.125f;  // 12.5% gap
constexpr float CONTAINER_ROTATION_SPEED = 36.0f;   // degrees/sec

// Physics
constexpr float GRAVITY = 980.0f;                   // px/s²
constexpr float RESTITUTION = 1.0f;                 // Perfect elasticity
constexpr float FIXED_TIMESTEP = 1.0f / 120.0f;    // 120Hz

// Ball defaults
constexpr float BALL_RADIUS = 7.5f;
```

---

## Code Style Guidelines

### File Organization
- One class per file with matching .h/.cpp pair
- Header guards: Use `#pragma once`
- Directory names: lowercase (physics/, rendering/)
- Class names: PascalCase (PhysicsEngine, BallManager)

### Class Structure
- Public section first, then private
- Member initialization in constructor initializer lists
- Const-correctness for getters
- Inline simple getters/setters in headers

### Naming Conventions
- Classes: PascalCase
- Functions/methods: camelCase
- Member variables: camelCase (no prefix)
- Constants: UPPER_SNAKE_CASE
- Namespaces: lowercase (Config, MathUtils)

### Physics Implementation
- Use Vector2D for all 2D math operations
- Position in pixels, velocity in px/s, acceleration in px/s²
- Angles internally in radians, public API can use degrees
- Mass calculated from area: π × radius²

---

## Interactive Controls

The application includes 6 runtime-adjustable sliders:
1. **Bounciness** - Restitution coefficient (0.0-1.0)
2. **Ball Size** - Ball radius
3. **Hole Size** - Container gap size
4. **Respawn Count** - Balls spawned per escaped ball
5. **Gravity** - Gravitational acceleration
6. **Container Diameter** - Container size

Plus **Reset** and **Pause** buttons.

All UI elements in [Application.cpp](src/core/Application.cpp) initialization.

---

## Common Development Tasks

### Adding a new physics feature
1. Modify [PhysicsEngine.cpp](src/physics/PhysicsEngine.cpp) update loop
2. Update [Config.h](src/core/Config.h) if new constants needed
3. Add slider in [Application.cpp](src/core/Application.cpp) if runtime-tunable

### Adding a new entity type
1. Create class in `src/entities/` with .h/.cpp pair
2. Add to [GameState](src/game/GameState.h) if needs management
3. Add rendering logic to [Application.cpp](src/core/Application.cpp) render loop

### Modifying collision behavior
1. **Detection**: Update [CollisionDetector.cpp](src/physics/CollisionDetector.cpp)
2. **Response**: Update [CollisionResolver.cpp](src/physics/CollisionResolver.cpp)
3. Both return `CollisionInfo` struct with normal and penetration depth

### Performance profiling
- Check ball count via on-screen counter
- Spatial grid effectiveness visible with high ball counts (>100)
- Profile physics vs rendering time separately

---

## Physics Implementation Details

### Elastic Collision Formula
Implemented in [CollisionResolver.cpp](src/physics/CollisionResolver.cpp):
- Conserves momentum and kinetic energy
- Velocity impulse: `j = (2 * relativeVelocity · normal) / (1/m1 + 1/m2)`
- Applied along collision normal

### Container Gap Detection
Special handling in [CollisionDetector.cpp](src/physics/CollisionDetector.cpp):
- Converts ball position to angle relative to container center
- Accounts for container rotation (angle wrapping)
- Ball escapes if angle within gap range

### Spatial Grid Parameters
- Cell size auto-calculated from max ball radius
- Hash function: `(x * 73856093) ^ (y * 19349663)`
- Rebuilt every frame (could be optimized for static scenarios)

---

## Known Patterns & Conventions

1. **Window coordinates**: Origin (0,0) at top-left, Y increases downward
2. **Container center**: Always at window center (WINDOW_WIDTH/2, WINDOW_HEIGHT/2)
3. **Initial ball spawn**: Center of container with zero velocity
4. **Ball removal**: When position exceeds 2× window dimensions
5. **Color generation**: Random RGB with full alpha
6. **Texture caching**: Circles pre-rendered for sizes 5-50px

---

## Dependencies & Compatibility

**Required Libraries:**
- SDL2 (2D graphics, window management, events)
- SDL2_ttf (TrueType font rendering)

**Supported Platforms:**
- macOS (primary development platform)
- Linux (Ubuntu/Debian tested)
- Windows (CMake configured, not extensively tested)

**Compiler Requirements:**
- C++17 support required
- Tested with Clang (macOS) and GCC (Linux)
- Warning flags: `-Wall -Wextra -pedantic`

---

## Git Workflow

**Main branches:**
- `main` - Stable releases
- `working_circle-container` - Current development branch

**Clean git status** - All changes committed
