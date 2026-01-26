#pragma once

#include <vector>
#include <iostream>
#include <fstream>
#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3dskeleton.h>
#include <t3d/t3danim.h>
#include <t3d/t3ddebug.h>

typedef struct {
  float min[2];
  float max[2];
} AABB;

typedef struct {
  T3DVec3 min;
  T3DVec3 max;
} AABB_3D;

typedef struct {
  T3DVec3 v0, v1, v2;
} Triangle;

//from tiny3d example 05_splitscreen
static void resolve_aabb_collision(const AABB *aabb, T3DVec3 *pos) {

  // Check if the point is inside the AABB
  if(pos->v[0] >= aabb->min[0] && pos->v[0] <= aabb->max[0]
  && pos->v[2] >= aabb->min[1] && pos->v[2] <= aabb->max[1])
  {
    // find the closest edge and move the point to it
    // keep in mind that that is a rather simplistic approach to keep the example simple
    float dxMin = fabsf(pos->v[0] - aabb->min[0]);
    float dxMax = fabsf(pos->v[0] - aabb->max[0]);
    float dzMin = fabsf(pos->v[2] - aabb->min[1]);
    float dzMax = fabsf(pos->v[2] - aabb->max[1]);

    float minDist = fminf(fminf(dxMin, dxMax), fminf(dzMin, dzMax));

    if(minDist == dxMin){ pos->v[0] = aabb->min[0]; return; }
    if(minDist == dxMax){ pos->v[0] = aabb->max[0]; return; }
    if(minDist == dzMin){ pos->v[2] = aabb->min[1]; return; }
    if(minDist == dzMax){ pos->v[2] = aabb->max[1]; return; }
  }
}

static void check_map_collision(T3DVec3 *pos, const AABB* mapColl, int collMatSize) {
  // in a real-world scenario, you would use a more sophisticated collision logic
  // for example, by having a broad-phase to pre-filter AABBs
  for(int i=0; i<collMatSize; ++i) {
    // to simplify even further, we only check each AABB in order once
    // usually one AABB resolver could move the point into another AABB, that is ignored here
    resolve_aabb_collision(&mapColl[i], pos);
  }
}

namespace collision{
  void scaleTriangles(std::vector<Triangle>* triangles, float scalar);
  std::vector<Triangle> loadCollTriangles(const std::string& binaryFilename);

  //check ground
  bool rayIntersectsTriangle(const T3DVec3& rayOrigin, const T3DVec3& rayDir, const Triangle& tri, float& outT, T3DVec3& outIntersection);
  T3DVec3 findGroundIntersection(const std::vector<Triangle>& triangles, const T3DVec3& rayOrigin);
  T3DVec3 findGroundIntersection(const std::vector<Triangle>& triangles, const T3DVec3& rayOrigin, Triangle& triangleOut);
  T3DVec3 triangleNormal(const Triangle& tri);
  bool raycastGround(const T3DVec3& playerPos, const std::vector<Triangle>& triangles, float maxSlopeAngle, T3DVec3& outGroundPos, T3DVec3& outSlideDirection);

  //check wall
  T3DVec3 closestPointOnSegment(const T3DVec3& p, const T3DVec3& lineA, const T3DVec3& lineB, T3DVec3 best);
  bool pointInTriangle(const T3DVec3& p, const Triangle& tri);
  T3DVec3 closestPointOnTriangle(const T3DVec3& p, const Triangle& tri);
  bool sphereIntersectsTriangle(const T3DVec3& sphereCenter, float sphereRadius, const Triangle& tri);
  T3DVec3 resolveWallCollision(const T3DVec3& playerPos, float playerRadius, const std::vector<Triangle>& triangles, float maxSlopeAngle, const T3DVec3& movementDir, bool* grounded);
  T3DVec3 projectVectorOntoPlaneKeepMagnitude(const T3DVec3& V, const T3DVec3& N);
  T3DVec3 resolveWallCollisionLoop(const T3DVec3& playerPos, float playerRadius, const std::vector<Triangle>& triangles, float maxSlopeAngle, const T3DVec3& movementDir, bool* grounded);
}