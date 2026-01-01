#include "Container.h"
#include "../math/MathUtils.h"
#include <cmath>

Container::Container(const Vector2D& center, float sideLength, float gapSizeFraction, int gapSide)
    : center(center)
    , sideLength(sideLength)
    , gapSizeFraction(gapSizeFraction)
    , gapSide(gapSide)
    , rotationSpeed(360.0f / 10.0f)  // 360° / 10 seconds = 36°/s
    , currentAngleRad(0.0f)
{
}

void Container::update(float deltaTime) {
    // Rotate at 36° per second (full rotation in 10 seconds)
    float deltaAngleDeg = rotationSpeed * deltaTime;
    currentAngleRad += MathUtils::degToRad(deltaAngleDeg);

    // Keep angle in [0, 2π] range
    currentAngleRad = MathUtils::normalizeAngle(currentAngleRad);
}

bool Container::isPointInsideContainer(const Vector2D& point) const {
    // For square: check if point is inside the rotated square
    // Transform point to local space (rotate back by -currentAngleRad)
    Vector2D localPoint = point - center;
    float cosAngle = std::cos(-currentAngleRad);
    float sinAngle = std::sin(-currentAngleRad);
    float localX = localPoint.x * cosAngle - localPoint.y * sinAngle;
    float localY = localPoint.x * sinAngle + localPoint.y * cosAngle;

    float halfSize = sideLength / 2.0f;
    return (std::abs(localX) < halfSize) && (std::abs(localY) < halfSize);
}

bool Container::isPointInGap(const Vector2D& point) const {
    // Check if point is in the gap region
    Vector2D gapStart, gapEnd;
    getGapBoundaries(gapStart, gapEnd);

    // Get the edge that has the gap
    auto edges = getWorldEdges();
    const Edge& gapEdge = edges[gapSide];

    // Project point onto gap edge
    Vector2D toPoint = point - gapEdge.start;
    Vector2D edgeDir = (gapEdge.end - gapEdge.start).normalized();
    float projection = toPoint.dot(edgeDir);
    Vector2D closestPoint = gapEdge.start + edgeDir * projection;

    // Check if closest point is between gap boundaries
    float distToGapStart = closestPoint.distance(gapStart);
    float distToGapEnd = closestPoint.distance(gapEnd);
    float gapLength = gapStart.distance(gapEnd);

    return (distToGapStart + distToGapEnd <= gapLength + 0.01f);
}

std::array<Vector2D, 4> Container::getWorldCorners() const {
    float halfSize = sideLength / 2.0f;

    // Define corners in local space (before rotation)
    // 0=bottom-right, 1=top-right, 2=top-left, 3=bottom-left
    std::array<Vector2D, 4> localCorners = {
        Vector2D(halfSize, -halfSize),   // bottom-right
        Vector2D(halfSize, halfSize),    // top-right
        Vector2D(-halfSize, halfSize),   // top-left
        Vector2D(-halfSize, -halfSize)   // bottom-left
    };

    // Rotate corners and translate to world position
    std::array<Vector2D, 4> worldCorners;
    float cosAngle = std::cos(currentAngleRad);
    float sinAngle = std::sin(currentAngleRad);

    for (size_t i = 0; i < 4; ++i) {
        float rotatedX = localCorners[i].x * cosAngle - localCorners[i].y * sinAngle;
        float rotatedY = localCorners[i].x * sinAngle + localCorners[i].y * cosAngle;
        worldCorners[i] = Vector2D(center.x + rotatedX, center.y + rotatedY);
    }

    return worldCorners;
}

std::array<Edge, 4> Container::getWorldEdges() const {
    auto corners = getWorldCorners();

    std::array<Edge, 4> edges;

    // Create edges connecting corners
    // Edge 0 (right): corner 0 -> corner 1
    // Edge 1 (top): corner 1 -> corner 2
    // Edge 2 (left): corner 2 -> corner 3
    // Edge 3 (bottom): corner 3 -> corner 0
    for (int i = 0; i < 4; ++i) {
        edges[i].start = corners[i];
        edges[i].end = corners[(i + 1) % 4];
        edges[i].sideIndex = i;
        edges[i].hasGap = (i == gapSide);
    }

    return edges;
}

void Container::getGapBoundaries(Vector2D& gapStart, Vector2D& gapEnd) const {
    auto edges = getWorldEdges();
    const Edge& gapEdge = edges[gapSide];

    Vector2D edgeVector = gapEdge.end - gapEdge.start;
    float edgeLength = edgeVector.magnitude();
    float gapLength = edgeLength * gapSizeFraction;

    // Center the gap on the edge
    float gapStartDist = (edgeLength - gapLength) / 2.0f;
    float gapEndDist = gapStartDist + gapLength;

    Vector2D edgeDir = edgeVector.normalized();
    gapStart = gapEdge.start + edgeDir * gapStartDist;
    gapEnd = gapEdge.start + edgeDir * gapEndDist;
}
