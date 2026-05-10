#include "GO_Enemy.h"
#include "../../globals.h"

void GO_Enemy::updatePathfinding() {
    //debugf("\nEntered updatePathfinding\n");
    // Only active in SEEKING state with a valid target
    if ((enemyState_ != global::ENEMY_STATE_SEEKING && enemyState_ != global::ENEMY_STATE_CHASING_CURSOR) || !target_) {
        return;
    }

    //debugf("Target exists\n");

    GameState* gs = global::gameState;
    if (!gs || !gs->pathfindingGraph) {
        return;  // level has no graph, skip
    }

    //debugf("Pathfinding graph exists\n");

    // Keep targetPos_ in sync with the current target's position
    //targetPos_ = target_->position_;

    bool needRecalc = !pathReady_;

    //debugf("needRecalc: %d\n", needRecalc);

    // Check if we reached the current waypoint
    if (pathReady_ && currentWaypointIdx_ < (int)currentPath_.size()) {
        //debugf("Not at final waypoint\n");
        float dx = targetNodePos_.x - position_.x;
        float dz = targetNodePos_.z - position_.z;
        float dist2 = dx * dx + dz * dz;
        //debugf("dist2=%.2f\n", dist2);
        if (dist2 < 1) {  // reached this waypoint (threshold = 1)
            //debugf("node %d of %d reached at dist2=%.2f\n", currentWaypointIdx_, (int)currentPath_.size(), dist2);
            currentWaypointIdx_++;
            //needRecalc = true;
            if(currentWaypointIdx_ < (int)currentPath_.size()){
                targetNodePos_ = gs->pathfindingGraph->nodes[currentPath_[currentWaypointIdx_]].position;
                //debugf("now seeking node %d\n\n", currentWaypointIdx_);
            }
        }
    }

    // If we've reached the last graph node, the next "waypoint" is the target itself
    if (pathReady_ && currentWaypointIdx_ >= (int)currentPath_.size()) {
        //debugf("Final waypoint reached, going after target\n");
        targetNodePos_ = targetPos_;
        return;
    }

    if (needRecalc) {
        //debugf("Calculating path\n");
        int startNode = gs->pathfindingGraph->findNearestNode(position_);
        int endNode   = gs->pathfindingGraph->findNearestNode(targetPos_);
        currentPath_  = gs->pathfindingGraph->findPath(startNode, endNode);
        currentWaypointIdx_ = 0;
        pathReady_ = true;

        if (currentPath_.empty()) {
            //debugf("no path found\n");
            // No path found -- fall back to direct movement (do nothing)
            pathReady_ = false;
            return;
        }
        //debugf("path found, setting first waypoint\n");

        // Set first waypoint
        targetNodePos_ = gs->pathfindingGraph->nodes[currentPath_[currentWaypointIdx_]].position;

        //debugf("first waypoint set\n");
    }

    // Set intended rotation toward the current waypoint
    // This overrides the default "rotate toward target" behavior each frame
    // so the path direction always wins over the 5-second random-rotation timer.
    intendedRotation_ = fm_atan2f(targetNodePos_.z - position_.z,
                                  targetNodePos_.x - position_.x);
    //debugf("intendedRotation set: %.2f\n", intendedRotation_);
}
