cmake_minimum_required(VERSION 3.15)
project(BallBouncing VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for IDE support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Use pkg-config to find SDL2 and SDL2_ttf
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)

# Source files
set(SOURCES
    src/main.cpp
    src/math/Vector2D.cpp
    src/math/MathUtils.cpp
    src/physics/PhysicsEngine.cpp
    src/physics/CollisionDetector.cpp
    src/physics/CollisionResolver.cpp
    src/physics/SpatialGrid.cpp
    src/entities/Ball.cpp
    src/entities/Container.cpp
    src/game/GameState.cpp
    src/game/BallManager.cpp
    src/rendering/Renderer.cpp
    src/rendering/CircleRenderer.cpp
    src/rendering/CircleTextureCache.cpp
    src/rendering/TextRenderer.cpp
    src/ui/Slider.cpp
    src/ui/Button.cpp
    src/core/Application.cpp
    src/core/Time.cpp
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Include directories
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_TTF_INCLUDE_DIRS}
)

# Link directories and libraries
target_link_directories(${PROJECT_NAME}
    PRIVATE
        ${SDL2_LIBRARY_DIRS}
        ${SDL2_TTF_LIBRARY_DIRS}
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        ${SDL2_LIBRARIES}
        ${SDL2_TTF_LIBRARIES}
)

# Platform-specific settings
if(APPLE)
    target_compile_definitions(${PROJECT_NAME} PRIVATE __APPLE__)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
elseif(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _WIN32)
elseif(UNIX)
    target_compile_definitions(${PROJECT_NAME} PRIVATE __linux__)
endif()

# Debug/Release configurations
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Installation rules
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)
