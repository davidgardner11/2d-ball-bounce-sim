#include "CollisionDetector.h"
#include "../math/MathUtils.h"
#include <cmath>
#include <cfloat> // Added for FLT_MAX

CollisionInfo CollisionDetector::checkBallCollision(const Ball& ballA, const Ball& ballB) {
    CollisionInfo info;

    // Calculate distance between ball centers
    Vector2D delta = ballB.position - ballA.position;
    float distanceSquared = delta.magnitudeSquared();
    float combinedRadius = ballA.radius + ballB.radius;
    float combinedRadiusSquared = combinedRadius * combinedRadius;

    // Check if balls are overlapping
    if (distanceSquared < combinedRadiusSquared && distanceSquared > 0.0001f) {
        info.hasCollision = true;
        float distance = std::sqrt(distanceSquared);
        info.normal = delta / distance;  // Normalize
        info.penetration = combinedRadius - distance;
    }

    return info;
}

CollisionInfo CollisionDetector::checkContainerCollision(
    const Ball& ball,
    const Container& container)
{
    CollisionInfo bestCollision;
    float minPenetration = FLT_MAX; 

    // Get container edges and gap boundaries
    auto edges = container.getWorldEdges();
    Vector2D gapStart, gapEnd;
    container.getGapBoundaries(gapStart, gapEnd);

    // Check collision with each edge
    for (const auto& edge : edges) {
        // Calculate edge vector and direction
        Vector2D edgeVector = edge.end - edge.start;
        float edgeLength = edgeVector.magnitude();
        if (edgeLength < 0.0001f) continue;  // Skip degenerate edges

        Vector2D edgeDir = edgeVector / edgeLength;

        // Project ball center onto edge
        Vector2D toBall = ball.position - edge.start; 
        float projection = toBall.dot(edgeDir); 
        projection = MathUtils::clamp(projection, 0.0f, edgeLength);

        // Find closest point on edge to ball center
        Vector2D closestPoint = edge.start + edgeDir * projection;

        // Check if closest point is in the gap (if this edge has a gap)
        if (edge.hasGap) {
            float distToGapStart = closestPoint.distance(gapStart);
            float distToGapEnd = closestPoint.distance(gapEnd);
            float gapLength = gapStart.distance(gapEnd);

            // Add small epsilon for precision
            if (distToGapStart + distToGapEnd <= gapLength + 0.5f) {
                // Point is in gap - no collision with this edge
                continue;
            }
        }

        // Calculate distance from edge to ball center
        Vector2D toBallFromEdge = ball.position - closestPoint;
        float distance = toBallFromEdge.magnitude();

        // Calculate the outward normal (perpendicular pointing away from container center)
        Vector2D edgePerp(-edgeDir.y, edgeDir.x);
        Vector2D edgeCenter = (edge.start + edge.end) * 0.5f;
        Vector2D centerToEdge = edgeCenter - container.getCenter();

        // Make sure perpendicular points outward (away from center)
        if (edgePerp.dot(centerToEdge) < 0.0f) {
            edgePerp = edgePerp * -1.0f;
        }

        // Check if ball is on the inside of the edge
        bool ballIsInside = false;
        if (distance > 0.0001f) {
            Vector2D ballDirection = toBallFromEdge / distance;
            // if dot product is negative, they are opposite, meaning ball is inside
            ballIsInside = (ballDirection.dot(edgePerp) < 0.0f);
        } else {
            // Ball center is exactly on the line
            ballIsInside = true; 
        }

        // --- BUG FIX START --- 
        // We now check for collision if:
        // 1. Ball is inside AND overlapping the wall (distance < radius)
        // 2. Ball is outside (has tunneled through)
        
        if ((ballIsInside && distance < ball.radius) || (!ballIsInside)) {
            
            float penetration = 0.0f;
            Vector2D normal; // Must point OUTWARD (Resolver expects Outward normal)

            if (ballIsInside) {
                // Standard collision
                penetration = ball.radius - distance;
                
                // toBallFromEdge points Inward (towards center). 
                // We need Outward. So we invert it.
                if (distance > 0.0001f) {
                    normal = (toBallFromEdge / distance) * -1.0f;
                } else {
                    normal = edgePerp;
                }
            } else {
                // Tunneling case: Ball center is outside
                // We need to push it back distance + radius
                penetration = ball.radius + distance;
                
                // toBallFromEdge points Outward (away from center).
                // This is already the correct direction for the normal.
                if (distance > 0.0001f) {
                    normal = toBallFromEdge / distance;
                } else {
                    normal = edgePerp;
                }
            }

            // Store the deepest collision found so far
            if (penetration < minPenetration) {
                bestCollision.hasCollision = true;
                bestCollision.penetration = penetration;
                bestCollision.normal = normal;
                minPenetration = penetration;
            }
        }
        // --- BUG FIX END ---
    }

    return bestCollision;
}

bool CollisionDetector::isAngleInGap(float angle, float gapStart, float gapEnd) {
    return MathUtils::isAngleInRange(angle, gapStart, gapEnd);
}

float CollisionDetector::getAngleFromCenter(const Vector2D& point, const Vector2D& center) {
    return std::atan2(point.y - center.y, point.x - center.x);
}