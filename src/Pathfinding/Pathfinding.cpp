#include "Pathfinding.h"
#include <cfloat>
#include <math.h>
#include <limits.h>

int PathfindingGraph::findNearestNode(T3DVec3 pos) {
    int nearest = 0;
    float nearestDist2 = FLT_MAX;

    for (int i = 0; i < (int)nodes.size(); i++) {
        float dx = nodes[i].position.x - pos.x;
        float dz = nodes[i].position.z - pos.z;
        float dist2 = dx * dx + dz * dz;
        if (dist2 < nearestDist2) {
            nearestDist2 = dist2;
            nearest = i;
        }
    }

    return nearest;
}

std::vector<int> PathfindingGraph::findPath(int startNodeIdx, int endNodeIdx) {
    int numNodes = (int)nodes.size();

    // Handle trivial cases
    if (startNodeIdx < 0 || startNodeIdx >= numNodes ||
        endNodeIdx < 0 || endNodeIdx >= numNodes) {
        return {};
    }

    if (startNodeIdx == endNodeIdx) {
        return {startNodeIdx};
    }

    // Dijkstra's algorithm
    // dist[i] = shortest distance from start to node i
    // prev[i] = previous node on shortest path (for backtracking)
    // visited[i] = whether node i has been finalized
    std::vector<float> dist(numNodes, FLT_MAX);
    std::vector<int> prev(numNodes, -1);
    std::vector<bool> visited(numNodes, false);

    dist[startNodeIdx] = 0.0f;

    for (int iter = 0; iter < numNodes; iter++) {
        // Find unvisited node with smallest distance
        int u = -1;
        float minDist = FLT_MAX;
        for (int i = 0; i < numNodes; i++) {
            if (!visited[i] && dist[i] < minDist) {
                minDist = dist[i];
                u = i;
            }
        }

        if (u == -1) break;  // remaining nodes unreachable
        if (u == endNodeIdx) break;  // reached target

        visited[u] = true;

        // Relax edges
        for (int v : nodes[u].neighbors) {
            // Compute XZ-plane distance for edge weight
            float dx = nodes[v].position.x - nodes[u].position.x;
            float dz = nodes[v].position.z - nodes[u].position.z;
            float weight = sqrtf(dx * dx + dz * dz);

            if (!visited[v] && dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
                prev[v] = u;
            }
        }
    }

    // Backtrack path from end to start
    if (prev[endNodeIdx] == -1 && startNodeIdx != endNodeIdx) {
        return {};  // no path found
    }

    std::vector<int> path;
    int current = endNodeIdx;
    while (current != -1) {
        path.push_back(current);
        current = prev[current];
    }

    // Reverse to get start -> end order
    std::vector<int> result;
    result.reserve(path.size());
    for (int i = (int)path.size() - 1; i >= 0; i--) {
        result.push_back(path[i]);
    }

    return result;
}
