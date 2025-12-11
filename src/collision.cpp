#include "collision.h"
#include "globals.h"
#include "Fonts.h"

void debugPrintVec(T3DVec3 vec) {
    debugf("x: %.2f, y: %.2f, z:%.2f\n", vec.x, vec.y, vec.z);
}

void debugDrawLine(uint16_t *fb, int px0, int py0, int px1, int py1, uint16_t color)
{
  uint32_t width = display_get_width();
  uint32_t height = display_get_height();
  if((px0 > width + 200) || (px1 > width + 200) ||
     (py0 > height + 200) || (py1 > height + 200)) {
    return;
  }

  float pos[2] = {px0, py0};
  int dx = px1 - px0;
  int dy = py1 - py0;
  int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
  if(steps <= 0)return;
  float xInc = dx / (float)steps;
  float yInc = dy / (float)steps;

  for(int i=0; i<steps; ++i)
  {
    if(pos[1] >= 0 && pos[1] < height && pos[0] >= 0 && pos[0] < width) {
      fb[(int)pos[1] * width + (int)pos[0]] = color;
    }
    pos[0] += xInc;
    pos[1] += yInc;
  }
}

void debugDrawLineVec3(uint16_t *fb, const T3DVec3 *p0, const T3DVec3 *p1, uint16_t color)
{
  if(p0->v[2] < 1.0 && p1->v[2] < 1.0) {
    debugDrawLine(fb, (int)p0->v[0], (int)p0->v[1], (int)p1->v[0], (int)p1->v[1], color);
  }
}

void collision::scaleTriangles(std::vector<Triangle>* triangles, float scalar) {
    for (auto &tri : *triangles) {
        tri.v0 *= scalar;
        tri.v1 *= scalar;
        tri.v2 *= scalar;
    }
}

std::vector<Triangle> collision::loadCollTriangles(const std::string& binaryFilename) {
    std::vector<Triangle> triangles;
  
    std::ifstream inFile(binaryFilename, std::ios::binary);
  
    if(!inFile) {
      debugf("Failed to open glb for collision triangles\n");
      return triangles;
    }
  
    size_t triangleCount;
    inFile.read(reinterpret_cast<char*>(&triangleCount), sizeof(size_t));
    debugf("Loading triangle count: %llu\n", triangleCount);
    triangles.resize(triangleCount);
  
    inFile.read(reinterpret_cast<char*>(triangles.data()), triangleCount * sizeof(Triangle));
    inFile.close();
  
    debugf("Loaded %d triangles from %s\n", triangleCount, binaryFilename.c_str());

    scaleTriangles(&triangles, 64.0f);
  
    /*
    debugf("BEFORE RETURNING ***************\n");
    for(auto tri : triangles) {
        debugf("v0 x: %.2f, y: %.2f, z: %.2f\n", tri.v0.x, tri.v0.y, tri.v0.z);
        debugf("v1 x: %.2f, y: %.2f, z: %.2f\n", tri.v1.x, tri.v1.y, tri.v1.z);
        debugf("v2 x: %.2f, y: %.2f, z: %.2f\n\n", tri.v2.x, tri.v2.y, tri.v2.z);
    }
        */

    return triangles;
}

//Moeller–Trumbore algorithm
bool collision::rayIntersectsTriangle(const T3DVec3& rayOrigin, const T3DVec3& rayDir, const Triangle& tri, float& outT, T3DVec3& outIntersection) {
    const float EPSILON = 0.0000001f;
    
    T3DVec3 edge1, edge2, h;
    t3d_vec3_diff(&edge1, &tri.v1, &tri.v0);
    t3d_vec3_diff(&edge2, &tri.v2, &tri.v0);
    t3d_vec3_cross(&h, &rayDir, &edge2);
    float a = t3d_vec3_dot(&edge1, &h);

    if (a > -EPSILON && a < EPSILON) return false; // Ray is parallel to the triangle


    float f = 1.0f / a;
    T3DVec3 s;

    t3d_vec3_diff(&s, &rayOrigin, &tri.v0);
    float u = f * t3d_vec3_dot(&s, &h);

    if (u < 0.0f || u > 1.0f) return false;

    T3DVec3 q;

    t3d_vec3_cross(&q, &s, &edge1);
    float v = f * t3d_vec3_dot(&rayDir, &q);

    if (v < 0.0f || u + v > 1.0f) return false;

    float t = f * t3d_vec3_dot(&edge2, &q);

    if (t > EPSILON) {
        outT = t;
        T3DVec3 rayDirScaled;
        t3d_vec3_scale(&rayDirScaled, &rayDir, t);
        t3d_vec3_add(&outIntersection, &rayOrigin, &rayDirScaled);
        return true;
    }

    return false;
}

T3DVec3 collision::findGroundIntersection(const std::vector<Triangle>& triangles, const T3DVec3& rayOrigin) {
    float outT;
    T3DVec3 intersectionCoord = {0, -500, 0};

    for (const auto& tri : triangles) {
        if(rayIntersectsTriangle(rayOrigin, {0,-1,0}, tri, outT, intersectionCoord)) {
            //return intersectionCoord;
            break;
        }
    }

    return intersectionCoord;
}

T3DVec3 collision::findGroundIntersection(const std::vector<Triangle>& triangles, const T3DVec3& rayOrigin, Triangle& triangleOut) {
    float outT;
    T3DVec3 intersectionCoord = {0, -500, 0};

    for (const auto& tri : triangles) {
        if(rayIntersectsTriangle(rayOrigin, {0,-1,0}, tri, outT, intersectionCoord)) {
            //return intersectionCoord;
            //triangleNormalOut = triangleNormal(tri);
            triangleOut = tri;
            break;
        }
    }

    /*
    if(intersectionCoord.y==-500) {
        triangleNormalOut = intersectionCoord;
    }
    */

    return intersectionCoord;
}

T3DVec3 collision::triangleNormal(const Triangle& tri) {
    T3DVec3 triangleNormal, diff1, diff2;
    t3d_vec3_diff(&diff1, &tri.v1, &tri.v0);
    t3d_vec3_diff(&diff2, &tri.v2, &tri.v0);
    t3d_vec3_cross(&triangleNormal, &diff1, &diff2);
    t3d_vec3_norm(&triangleNormal);
    return triangleNormal;
}

bool collision::raycastGround(const T3DVec3& playerPos, const std::vector<Triangle>& triangles, float maxSlopeAngle, T3DVec3& outGroundPos, T3DVec3& outSlideDirection) {
    T3DVec3 rayOrigin = playerPos;
    T3DVec3 rayDir = (T3DVec3){{0, -1, 0}};
    T3DVec3 bestIntersection, bestNormal;
    float closestT = 9999.0f;
    bool foundGround = false;
    bool isWalkable = false;

    for (const auto& tri : triangles) {
        float t;
        T3DVec3 intersection;

        if(rayIntersectsTriangle(rayOrigin, rayDir, tri, t, intersection) && t < closestT) {
            T3DVec3 normal = triangleNormal(tri);
            float angle = acosf(t3d_vec3_dot(normal, (T3DVec3){{0, 1, 0}}));

            closestT = t;
            bestIntersection = intersection;
            bestNormal = normal;
            foundGround = true;
            isWalkable = angle <= maxSlopeAngle;
        }
    }

    if(!foundGround) return false;

    outGroundPos = bestIntersection;

    if(isWalkable) {
        outSlideDirection = (T3DVec3){{0,0,0}};
    }
    else {
        //compute slide for steep slopes
        T3DVec3 gravity = {{0, -1, 0}};
        outSlideDirection = gravity - bestNormal * t3d_vec3_dot(&gravity, &bestNormal);
        t3d_vec3_norm(&outSlideDirection);
    }

    return true;
}

T3DVec3 collision::closestPointOnSegment(const T3DVec3& p, const T3DVec3& lineA, const T3DVec3& lineB, T3DVec3 best = (T3DVec3){{9999.0f, 9999.0f, 9999.0f}}){
    T3DVec3 ab = lineB - lineA;
    float t = t3d_vec3_dot(p - lineA, ab) / t3d_vec3_dot(ab, ab);
    
    if(t < 0.0f) {
        t = 0.0f;
    }
    else if (t > 1.0f) {
        t = 1.0f;
    }

    T3DVec3 scaledAB;
    t3d_vec3_scale(&scaledAB, &ab, t);

    T3DVec3 candidate = lineA + scaledAB;

    return (t3d_vec3_len2(candidate - p) < t3d_vec3_len2(best - p)) ? candidate : best;
}

//barycentric method
bool collision::pointInTriangle(const T3DVec3& p, const Triangle& tri) {
    T3DVec3 v0 = tri.v1 - tri.v0;
    T3DVec3 v1 = tri.v2 - tri.v0;
    T3DVec3 v2 = p - tri.v0;

    float d00 = t3d_vec3_dot(v0, v0);
    float d01 = t3d_vec3_dot(v0, v1);
    float d11 = t3d_vec3_dot(v1, v1);
    float d20 = t3d_vec3_dot(v2, v0);
    float d21 = t3d_vec3_dot(v2, v1);

    float denom = d00 * d11 - d01 * d01;
    if(denom==0) {
        return false;
    }
    float v = (d11 * d20 - d01 * d21) / denom;
    float w = (d00 * d21 - d01 * d20) / denom;
    float u = 1.0f - v - w;

    return (u >= 0) && (v >= 0) && (w >= 0);
}

T3DVec3 collision::closestPointOnTriangle(const T3DVec3& p, const Triangle& tri) {
    T3DVec3 ab = tri.v1 - tri.v0;
    T3DVec3 ac = tri.v2 - tri.v0;
    T3DVec3 bc = tri.v2 - tri.v1;

    T3DVec3 norm;

    t3d_vec3_cross(&norm, &ab, &ac);
    t3d_vec3_norm(&norm);

    //project point onto triangle plane
    float d = t3d_vec3_dot(&norm, &tri.v0);
    T3DVec3 projection = p - norm * (t3d_vec3_dot(&norm, &p) - d);

    //if the projection is inside the triangle, return it
    if (pointInTriangle(projection, tri)) {
        return projection;
    }

    //otherwise find closest point on triangle's edges
    T3DVec3 closest = closestPointOnSegment(p, tri.v0, tri.v1);
    closest = closestPointOnSegment(p, tri.v1, tri.v2, closest);
    closest = closestPointOnSegment(p, tri.v2, tri.v0, closest);
    return closest;
}

bool collision::sphereIntersectsTriangle(const T3DVec3& sphereCenter, float sphereRadius, const Triangle& tri) {
    T3DVec3 closestPoint = closestPointOnTriangle(sphereCenter, tri);
    float distance2 = t3d_vec3_distance2(&closestPoint, &sphereCenter);
    return distance2 <= (sphereRadius * sphereRadius);
}

T3DVec3 collision::resolveWallCollision(const T3DVec3& playerPos, float playerRadius, const std::vector<Triangle>& triangles, float maxSlopeAngle, const T3DVec3& movementDir, bool* grounded) {
    T3DVec3 correction = (T3DVec3){{0,0,0}};
    float EPSILON = 0.05f;
    *grounded = false;

    //vector to store normals of non-parallel triangles player is colliding with
    std::vector<T3DVec3> normals;

    //search through all triangles
    //TODO: implement BVH culling
    for(const auto& tri : triangles) {
        if(sphereIntersectsTriangle(playerPos+movementDir+correction, playerRadius, tri)) {
            T3DVec3 triNorm = triangleNormal(tri);
            float angle = acosf(t3d_vec3_dot(&triNorm, &(T3DVec3){{0,1,0}}));
            if(angle >= T3D_PI/2.0f) angle -= T3D_PI;
            else if(angle <= -T3D_PI/2.0f) angle += T3D_PI;
            
            if(abs(angle) < maxSlopeAngle) {
                *grounded = true;
            }
            
            //collect normals of all non-parallel planes player is colliding with (normals are parallel when their dot product is 1)
            bool addNewNormal = true;
            for (auto norm : normals) {
                float dot = t3d_vec3_dot(&triNorm, &norm);
                if(dot < 1+EPSILON && dot > 1-EPSILON) {
                    addNewNormal = false;
                    break;
                }
            }
            if(addNewNormal) {
                normals.push_back(triNorm);
            }
        }
    }

    for (auto norm : normals) {
        float dot = t3d_vec3_dot(&movementDir, &norm);//
        if(dot < 0) {
            //correction -= norm * t3d_vec3_dot(&movementDir, &norm);
            correction -= norm * dot;//
        }//
    }

    return movementDir + correction;
}

T3DVec3 collision::projectVectorOntoPlaneKeepMagnitude(const T3DVec3& V, const T3DVec3& N) {
    T3DVec3 projected = V - N * t3d_vec3_dot(&V, &N); // Remove normal component
    float projectedMagnitude = t3d_vec3_len(&projected);
    
    // Prevent division by zero in case the vector is already aligned with the normal
    if (projectedMagnitude < 1e-6f) return V; // Return original if no valid projection

    return projected * (t3d_vec3_len(&V) / projectedMagnitude); // Scale to maintain original magnitude
}

T3DVec3 collision::resolveWallCollisionLoop(const T3DVec3& playerPos, float playerRadius, const std::vector<Triangle>& triangles, float maxSlopeAngle, const T3DVec3& movementDir, bool* grounded) {
    T3DVec3 correction = (T3DVec3){{0,0,0}};
    float EPSILON = 0.05f;
    *grounded = false;
    T3DVec3 movDir = movementDir;

    //vector to store normals of non-parallel triangles player is colliding with
    std::vector<T3DVec3> normals;
    std::vector<Triangle> collidedTris;

    //search through all triangles
    //TODO: implement BVH culling
    for(const auto& tri : triangles) {
        //if(sphereIntersectsTriangle(playerPos+movementDir+correction, playerRadius, tri)) {
        if(sphereIntersectsTriangle(playerPos+movDir+correction, playerRadius, tri)) {
            T3DVec3 triNorm = triangleNormal(tri);
            float angle = acosf(t3d_vec3_dot(&triNorm, &(T3DVec3){{0,1,0}}));
            if(angle >= T3D_PI/2.0f) angle -= T3D_PI;
            else if(angle <= -T3D_PI/2.0f) angle += T3D_PI;
            
            if(abs(angle) < maxSlopeAngle) {
                *grounded = true;
                //movDir = projectVectorOntoPlaneKeepMagnitude(movDir, triNorm);
            }
            
            //collect normals of all non-parallel planes player is colliding with (normals are parallel when their dot product is 1)
            bool addNewNormal = true;
            for (auto norm : normals) {
                float dot = t3d_vec3_dot(&triNorm, &norm);
                if(dot < 1+EPSILON && dot > 1-EPSILON) {
                    addNewNormal = false;
                    break;
                }
            }
            if(addNewNormal) {
                collidedTris.push_back(tri);
                normals.push_back(triNorm);
            }
        }
    }

    //for (auto norm : normals) {
    //    correction -= norm * t3d_vec3_dot(&movementDir, &norm);
    //}

    EPSILON = 0.001f;

    do {
        T3DVec3 tempCorr = correction;
        for (auto tri : collidedTris) {
            T3DVec3 triNorm = triangleNormal(tri);
            //float dot = t3d_vec3_dot(&(movementDir + correction), &triNorm);//
            float dot = t3d_vec3_dot(&(movDir + correction), &triNorm);//
            if(dot < 0) {//
                tempCorr -= triNorm * dot;//
                //tempCorr -= triNorm * t3d_vec3_dot(&(movementDir + correction), &triNorm);
            }//
        }

        if(abs(tempCorr.x-correction.x)<EPSILON && abs(tempCorr.y-correction.y)<EPSILON && abs(tempCorr.z-correction.z)<EPSILON) {
            correction = tempCorr;
            break;
        }

        correction = tempCorr;

        
        //debugf("Correction:\n");
        //debugPrintVec(correction);
        //debugf("Size of collidedTris: %d\n\n", collidedTris.size());

        
        for (int i = 0; i < collidedTris.size(); i++) {
            //if(!sphereIntersectsTriangle(playerPos+movementDir+correction, playerRadius, collidedTris.at(i))) {
            if(!sphereIntersectsTriangle(playerPos+movDir+correction, playerRadius, collidedTris.at(i))) {
                collidedTris.erase(collidedTris.begin()+i);
                i--;
                //debugf("Erased!\n");
            }
        }
        //debugf("Size of collidedTris after loop: %d\n", collidedTris.size());
        
    } while(collidedTris.size() > 0);

    //return movementDir + correction;
    return movDir + correction;
}