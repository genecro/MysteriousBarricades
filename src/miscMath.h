#pragma once

#include <t3d/t3d.h>
#include <math.h>

namespace math{
    float normalizeAngle(float angle)
    {
        return std::remainder(angle, 2.0f * T3D_PI);
    }

    float angleDelta(float from, float to)
    {
        return std::remainder(to - from, 2.0f * T3D_PI);
    }

    bool isAngleInRange(float angle, float minAngle, float maxAngle)
    {
        angle    = normalizeAngle(angle);
        minAngle = normalizeAngle(minAngle);
        maxAngle = normalizeAngle(maxAngle);

        // Range does NOT wrap
        if (minAngle <= maxAngle)
        {
            return angle >= minAngle && angle <= maxAngle;
        }
        // Range wraps around +/- pi
        else
        {
            return angle >= minAngle || angle <= maxAngle;
        }
    }

    float angleBetweenPointsClampedWrapped(T3DVec3 p1, T3DVec3 p2, float minAngle, float maxAngle)
    {
        float angle = normalizeAngle(atan2f(p2.z - p1.z, p2.x - p1.x) - T3D_PI/2.0f);
        minAngle = normalizeAngle(minAngle);
        maxAngle = normalizeAngle(maxAngle);

        if (isAngleInRange(angle, minAngle, maxAngle))
            return angle;

        // Find which boundary is closer (shortest circular distance)
        float distToMin = fabs(angleDelta(angle, minAngle));
        float distToMax = fabs(angleDelta(angle, maxAngle));

        return (distToMin < distToMax) ? minAngle : maxAngle;
    }

    struct Timer
  {
    float value{};
    float target{};

    void update(float deltaTime) {
      if(target < value) {
        value = fmaxf(value - deltaTime, target);
      } else {
        value = fminf(value + deltaTime, target);
      }
    }
  };
}