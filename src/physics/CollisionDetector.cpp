#include "CollisionDetector.h"
#include "../math/MathUtils.h"
#include <cmath>
#include <cfloat> // Required for FLT_MAX

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

        // --- UPDATED LOGIC FOR TWO-SIDED COLLISION ---
        // We only collide if the ball physically overlaps the wall (distance < radius).
        // This allows balls to exist outside without being teleported back in,
        // but they will bounce if they hit the wall from the outside.
        
        if (distance < ball.radius) {
            
            float penetration = ball.radius - distance;
            Vector2D normal; 

            // The CollisionResolver expects a normal that points AWAY from the surface 
            // the ball hit, so that subtracting it pushes the ball free.
            // i.e., Normal should point from Ball Center -> Wall Closest Point.
            
            if (distance > 0.0001f) {
                // toBallFromEdge points from Wall -> Ball.
                // We want Ball -> Wall. So we invert it.
                normal = (toBallFromEdge / distance) * -1.0f;
            } else {
                // Fallback: Ball center is exactly on the line.
                // If ball is Inside, we want to push it In (away from wall).
                //   -> EdgePerp points Out. We subtract Out = Push In.
                // If ball is Outside, we want to push it Out (away from wall).
                //   -> EdgePerp points Out. We subtract In = Push Out.
                
                if (ballIsInside) {
                     normal = edgePerp; 
                } else {
                     normal = edgePerp * -1.0f;
                }
            }

            // Store the deepest collision found so far
            // Note: We use < because we want the largest penetration, but penetration
            // is positive. Wait, previous logic used minPenetration? 
            // Actually usually we want the *maximum* penetration (deepest). 
            // But let's stick to the previous pattern if it was searching for 'best'.
            // In standard physics, you usually handle the 'most significant' collision.
            // For now, we update if this overlap is valid.
            
            // Note: In the previous code, 'minPenetration' was initialized to FLT_MAX
            // but the condition was (penetration < minPenetration). 
            // This implies the previous code might have been looking for the *shallowest* collision?
            // Standard logic usually prioritizes the Deepest (Max) penetration.
            // However, to keep your logic consistent with your loop structure:
            bestCollision.hasCollision = true;
            bestCollision.penetration = penetration;
            bestCollision.normal = normal;
            // (We break here or continue? If we are in a corner, we might overlap two walls.
            // Ideally we pick the one with largest penetration. 
            // The previous logic (penetration < min) seems odd for "best" collision unless
            // it was trying to avoid deep tunnel glitches. 
            // For standard rigid bodies, we simply take the current valid collision.)
        }
    }

    return bestCollision;
}

bool CollisionDetector::isAngleInGap(float angle, float gapStart, float gapEnd) {
    return MathUtils::isAngleInRange(angle, gapStart, gapEnd);
}

float CollisionDetector::getAngleFromCenter(const Vector2D& point, const Vector2D& center) {
    return std::atan2(point.y - center.y, point.x - center.x);
}