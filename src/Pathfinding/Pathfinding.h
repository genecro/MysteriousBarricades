#pragma once

#include <t3d/t3d.h>
#include <vector>

struct PathNode {
    T3DVec3 position;
    std::vector<int> neighbors;  // indices into the graph's node list
};

class PathfindingGraph {
public:
    std::vector<PathNode> nodes;

    // Find the index of the node closest to a given position (XZ plane distance)
    int findNearestNode(T3DVec3 pos);

    // Dijkstra from startNodeIdx to endNodeIdx.
    // Returns a vector of node indices representing the path,
    // or an empty vector if no path exists.
    std::vector<int> findPath(int startNodeIdx, int endNodeIdx);
};
