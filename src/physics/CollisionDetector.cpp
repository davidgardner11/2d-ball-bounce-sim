#include "CollisionDetector.h"
#include "../math/MathUtils.h"
#include <cmath>

CollisionInfo CollisionDetector::checkBallCollision(const Ball& a, const Ball& b) {
    CollisionInfo info;

    // Calculate distance between ball centers
    Vector2D delta = b.position - a.position;
    float distanceSquared = delta.magnitudeSquared();
    float combinedRadius = a.radius + b.radius;
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
    CollisionInfo info;

    // Calculate distance from ball center to container center
    Vector2D delta = ball.position - container.getCenter();
    float distance = delta.magnitude();

    // Calculate the collision angle
    float angle = std::atan2(delta.y, delta.x);
    angle = MathUtils::normalizeAngle(angle);

    // Get gap boundaries
    float gapStart = container.getGapStartAngle();
    float gapEnd = container.getGapEndAngle();
    bool isInGap = MathUtils::isAngleInRange(angle, gapStart, gapEnd);

    // Skip collision if in gap area
    if (isInGap) {
        return info;
    }

    // Determine which side of the container the ball is on
    float containerInnerRadius = container.getRadius() - ball.radius;
    float containerOuterRadius = container.getRadius() + ball.radius;

    // Check for collision with inner wall (ball pushing out from inside)
    if (distance > containerInnerRadius && distance <= container.getRadius()) {
        // Ball is penetrating inner wall from inside
        info.hasCollision = true;
        info.normal = delta.normalized();  // Normal points outward from center
        info.penetration = distance - containerInnerRadius;
    }
    // Check for collision with outer wall (ball bouncing off from outside)
    else if (distance > container.getRadius() && distance < containerOuterRadius) {
        // Ball is penetrating outer wall from outside
        info.hasCollision = true;
        info.normal = delta.normalized() * -1.0f;  // Normal points inward toward center
        info.penetration = containerOuterRadius - distance;
    }

    return info;
}

bool CollisionDetector::isAngleInGap(float angle, float gapStart, float gapEnd) {
    return MathUtils::isAngleInRange(angle, gapStart, gapEnd);
}

float CollisionDetector::getAngleFromCenter(const Vector2D& point, const Vector2D& center) {
    return std::atan2(point.y - center.y, point.x - center.x);
}
