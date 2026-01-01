#pragma once

#include "../math/Vector2D.h"
#include <array>

// Edge structure for square container
struct Edge {
    Vector2D start;
    Vector2D end;
    int sideIndex;  // 0=right, 1=top, 2=left, 3=bottom (in local space)
    bool hasGap;
};

class Container {
public:
    Container(const Vector2D& center, float sideLength, float gapSizeFraction, int gapSide = 0);

    // Update rotation
    void update(float deltaTime);

    // Collision detection helpers (legacy methods - may not be used for square)
    bool isPointInsideContainer(const Vector2D& point) const;
    bool isPointInGap(const Vector2D& point) const;

    // Square geometry methods
    std::array<Vector2D, 4> getWorldCorners() const;
    std::array<Edge, 4> getWorldEdges() const;
    void getGapBoundaries(Vector2D& gapStart, Vector2D& gapEnd) const;

    // Rendering info
    Vector2D getCenter() const { return center; }
    float getSideLength() const { return sideLength; }
    float getCurrentRotation() const { return currentAngleRad; }
    float getGapSizeFraction() const { return gapSizeFraction; }
    int getGapSide() const { return gapSide; }

    // Configuration
    void setGapSizeFraction(float fraction) { gapSizeFraction = fraction; }
    void setSideLength(float newSideLength) { sideLength = newSideLength; }

private:
    Vector2D center;
    float sideLength;          // Side length of the square
    float gapSizeFraction;     // Gap size as fraction of side (0.0-1.0)
    int gapSide;               // Which side has the gap (0=right, 1=top, 2=left, 3=bottom)
    float rotationSpeed;       // Degrees per second
    float currentAngleRad;     // Current rotation angle in radians
};
