#include "GO_Cursor.h"
#include "../globals.h"
#include "Barricades/GO_BarricadeStandard.h"

#define STICK_DEADZONE 3

T3DModel* GO_Cursor::cursorModel = nullptr;
T3DModel* GO_Cursor::groundMarkerModel = nullptr;
uint8_t GO_Cursor::instanceCount = 0;

GO_Cursor::GO_Cursor(T3DVec3 position, std::vector<Triangle> *newCollisionTris) {
    position_ = position;
    objectWidth_ = 2;
    rotation_ = 0;

    

    instanceCount++;
    if(!cursorModel) {
        cursorModel = t3d_model_load("rom:/cursor.t3dm");
    }
    if(!groundMarkerModel) {
        groundMarkerModel = t3d_model_load("rom:/groundMarker.t3dm");
    }

    t3d_mat4_identity(&cursorMat);
    t3d_mat4_identity(&cursorEdgeMat1);
    t3d_mat4_identity(&cursorEdgeMat2);
    cursorMatFP = (T3DMat4FP*)malloc_uncached(sizeof(T3DMat4FP));
    cursorEdgeMatFP1 = (T3DMat4FP*)malloc_uncached(sizeof(T3DMat4FP));
    cursorEdgeMatFP2 = (T3DMat4FP*)malloc_uncached(sizeof(T3DMat4FP));

    t3d_mat4_identity(&groundMarkerMat);
    t3d_mat4_identity(&groundMarkerEdgeMat1);
    t3d_mat4_identity(&groundMarkerEdgeMat2);
    groundMarkerMatFP = (T3DMat4FP*)malloc_uncached(sizeof(T3DMat4FP));
    groundMarkerEdgeMatFP1 = (T3DMat4FP*)malloc_uncached(sizeof(T3DMat4FP));
    groundMarkerEdgeMatFP2 = (T3DMat4FP*)malloc_uncached(sizeof(T3DMat4FP));

    collisionTris = newCollisionTris;

    cursorState = global::CURSOR_STATE_BASE;

    
}

GO_Cursor::~GO_Cursor() {
    free_uncached(cursorMatFP);
    free_uncached(groundMarkerMatFP);
    instanceCount--;
    if(instanceCount == 0) {
        t3d_model_free(cursorModel);
        t3d_model_free(groundMarkerModel);
        cursorModel=nullptr;
        groundMarkerModel=nullptr;
    }
}

void GO_Cursor::handleInput() {
    joypad_inputs_t joypad = joypad_get_inputs(JOYPAD_PORT_1);
    joypad_buttons_t btn = joypad_get_buttons_pressed(JOYPAD_PORT_1);

    switch (cursorState) {

        //moving around
        case global::CURSOR_STATE_BASE:
            /*
            if(joypad.stick_x < -STICK_DEADZONE || joypad.stick_x > STICK_DEADZONE) {
                position_.x -= (float)(joypad.stick_x)/200.0f;
            }

            if(joypad.stick_y < -STICK_DEADZONE || joypad.stick_y > STICK_DEADZONE) {
                position_.z += (float)(joypad.stick_y)/200.0f;
            }
            */

            if(joypad.stick_x < -STICK_DEADZONE || joypad.stick_x > STICK_DEADZONE ||
               joypad.stick_y < -STICK_DEADZONE || joypad.stick_y > STICK_DEADZONE) {
                handleMovement((T3DVec3){
                        (float)(joypad.stick_x)/200.0f,
                        0,
                        (float)(joypad.stick_y)/200.0f
                    }
                );
            }

            if(btn.a) {
                cursorState = global::CURSOR_STATE_BARRICADE;
            }
        break;

        //making a barricade
        case global::CURSOR_STATE_BARRICADE:
            if(joypad.stick_x < -STICK_DEADZONE || joypad.stick_x > STICK_DEADZONE) {
                barricadeEdgeRelativeToCursor.x = -(float)(joypad.stick_x)/60.0f*barrierSize;
            }
            else {
                barricadeEdgeRelativeToCursor.x = 0;
            }

            if(joypad.stick_y < -STICK_DEADZONE || joypad.stick_y > STICK_DEADZONE) {
                barricadeEdgeRelativeToCursor.z = (float)(joypad.stick_y)/60.0f*barrierSize;
            }
            else {
                barricadeEdgeRelativeToCursor.z = 0;
            }

            if(!joypad.btn.a) {
                if(abs(barricadeEdgeRelativeToCursor.x) + abs(barricadeEdgeRelativeToCursor.z) > 2) {
                    global::gameState->barricadeList->push(
                        new GO_BarricadeStandard(
                            groundMarkerPos,//collision::findGroundIntersection(*collisionTris, position_, normalOfCurrTriangle), 
                            barricadeEdgeRelativeToCursor, 
                            (color_t){0xFF, 0, 0, 0x7F}
                        )
                    );
                }
                cursorState = global::CURSOR_STATE_BASE;
            }
        break;

        //over a repairable
        case global::CURSOR_STATE_REPAIR:
            
            if(joypad.stick_x < -STICK_DEADZONE || joypad.stick_x > STICK_DEADZONE) {
                position_.x -= (float)(joypad.stick_x)/200.0f;
            }

            if(joypad.stick_y < -STICK_DEADZONE || joypad.stick_y > STICK_DEADZONE) {
                position_.z += (float)(joypad.stick_y)/200.0f;
            }
            

            

            if(joypad.btn.a) {
                GO_Repairable* temp = global::gameState->repairableList->getCurrRepairable();
                if(temp) {
                    if(temp->HPCurrent_ < temp->HPTotal_) temp->HPCurrent_++;
                }
            }

        break;
    }

    if(cursorState!=global::CURSOR_STATE_BARRICADE) {
        if(btn.r) {
            GO_Repairable* temp = global::gameState->repairableList->getNextRepairable();
            if(temp) {
                position_.x = temp->position_.x;
                position_.z = temp->position_.z;
            }
        }
        else if(btn.l) {
            GO_Repairable* temp = global::gameState->repairableList->getPrevRepairable();
            if(temp) {
                position_.x = temp->position_.x;
                position_.z = temp->position_.z;
            }
        }
    }
    
}

void GO_Cursor::update() {
    rotation_ += 0.007;
    if(rotation_ >= 2*T3D_PI) {
        rotation_ -= 2*T3D_PI;
    }

    t3d_mat4_from_srt_euler(&cursorMat,
        (float[3]){0.02f, 0.02f, 0.02f},
        (float[3]){0.0f, rotation_, 0.0f},
        position_.v
    );
    t3d_mat4_to_fixed(cursorMatFP, &cursorMat);

    groundMarkerPos = collision::findGroundIntersection(*collisionTris, position_);//, normalOfCurrTriangle);

    t3d_mat4_from_srt_euler(&groundMarkerMat,
        (float[3]){0.003f, 0.003f, 0.003f},
        (float[3]){0.0f, rotation_, 0.0f},
        groundMarkerPos.v
    );
    t3d_mat4_to_fixed(groundMarkerMatFP, &groundMarkerMat);

    bool nearRepairable = false;
    for(GO_Repairable* i: *(global::gameState->repairableList->repairables)) {
        if(t3d_vec3_distance2((T3DVec3){i->position_.x,0,i->position_.z}, (T3DVec3){position_.x, 0, position_.z}) <=20) {
            nearRepairable = true;
            global::gameState->repairableList->setCurrentRepairable(i);
            break;
        }
    }

    if(nearRepairable) {
        setStateRepair();
    }
    else if(cursorState!=global::CURSOR_STATE_BARRICADE) {
        setStateBaseState();
    }
    

    switch (cursorState) {
        case global::CURSOR_STATE_BASE:
            cursorColor = cursorColorBase;
        break;

        case global::CURSOR_STATE_BARRICADE:
            t3d_mat4_from_srt_euler(&cursorEdgeMat1,
                (float[3]){0.015f, 0.015f, 0.015f},
                (float[3]){0.0f, rotation_, 0.0f},
                (position_ + barricadeEdgeRelativeToCursor).v
            );
            t3d_mat4_to_fixed(cursorEdgeMatFP1, &cursorEdgeMat1);

            t3d_mat4_from_srt_euler(&cursorEdgeMat2,
                (float[3]){0.015f, 0.015f, 0.015f},
                (float[3]){0.0f, rotation_, 0.0f},
                (position_ - barricadeEdgeRelativeToCursor).v
            );
            t3d_mat4_to_fixed(cursorEdgeMatFP2, &cursorEdgeMat2);

            t3d_mat4_from_srt_euler(&groundMarkerEdgeMat1,
                (float[3]){0.003f, 0.003f, 0.003f},
                (float[3]){0.0f, rotation_, 0.0f},
                collision::findGroundIntersection(*collisionTris, position_ + barricadeEdgeRelativeToCursor).v
            );
            t3d_mat4_to_fixed(groundMarkerEdgeMatFP1, &groundMarkerEdgeMat1);

            t3d_mat4_from_srt_euler(&groundMarkerEdgeMat2,
                (float[3]){0.003f, 0.003f, 0.003f},
                (float[3]){0.0f, rotation_, 0.0f},
                collision::findGroundIntersection(*collisionTris, position_ - barricadeEdgeRelativeToCursor).v
            );
            t3d_mat4_to_fixed(groundMarkerEdgeMatFP2, &groundMarkerEdgeMat2);
        break;

        case global::CURSOR_STATE_REPAIR:
            cursorColor = cursorColorRepair;
        break;
    }
}

void GO_Cursor::renderT3d() {
    rdpq_sync_pipe();

    rdpq_set_prim_color(cursorColor);
    t3d_matrix_set(cursorMatFP, true);
    t3d_model_draw(cursorModel);

    rdpq_set_prim_color(groundMarkerColor);
    t3d_matrix_set(groundMarkerMatFP, true);
    t3d_model_draw(groundMarkerModel);

    switch (cursorState) {
        case global::CURSOR_STATE_BASE:
        break;

        case global::CURSOR_STATE_BARRICADE:
            
            t3d_matrix_set(cursorEdgeMatFP1, true);
            t3d_model_draw(cursorModel);

            t3d_matrix_set(cursorEdgeMatFP2, true);
            t3d_model_draw(cursorModel);
            
            t3d_matrix_set(groundMarkerEdgeMatFP1, true);
            t3d_model_draw(groundMarkerModel);

            t3d_matrix_set(groundMarkerEdgeMatFP2, true);
            t3d_model_draw(groundMarkerModel);
        break;

        case global::CURSOR_STATE_REPAIR:
        break;
    }
}

void GO_Cursor::renderRdpq() {

}

void GO_Cursor::setPosition(T3DVec3 newPos) {
    position_.x = newPos.x;
    position_.z = newPos.z;
}

void GO_Cursor::setStateBaseState() {
    cursorState = global::CURSOR_STATE_BASE;
}

void GO_Cursor::setStateMakingBarricade() {
    cursorState = global::CURSOR_STATE_BARRICADE;
}

void GO_Cursor::setStateRepair() {
    cursorState = global::CURSOR_STATE_REPAIR;
}

void GO_Cursor::handleMovement(T3DVec3 intendedMovement) {
    //debugf("Distance to ground: %.2f\n", position_.y-groundMarkerPos.y);
    //if(position_.y-groundMarkerPos.y < 5) {// || position_.y-groundMarkerPos.y > 50) {
    //    intendedMovement.x-=normalOfCurrTriangle.x;
    //    intendedMovement.z+=normalOfCurrTriangle.z;
    //}

    Triangle tempTri;

    collision::findGroundIntersection(*collisionTris, (T3DVec3){position_.x-intendedMovement.x, position_.y, position_.z}, tempTri);

    if(fmaxf(tempTri.v0.y, fmaxf(tempTri.v1.y, tempTri.v2.y)) > position_.y-5.0f) {
        intendedMovement.x = 0;
    }

    collision::findGroundIntersection(*collisionTris, (T3DVec3){position_.x, position_.y, position_.z+intendedMovement.z}, tempTri);

    if(fmaxf(tempTri.v0.y, fmaxf(tempTri.v1.y, tempTri.v2.y)) > position_.y-5.0f) {
        intendedMovement.z = 0;
    }

    /*
    if(position_.y - collision::findGroundIntersection(*collisionTris, (T3DVec3){position_.x-intendedMovement.x, position_.y, position_.z}).y < 5) {
        intendedMovement.x = 0;
    }

    if(position_.y - collision::findGroundIntersection(*collisionTris, (T3DVec3){position_.x, position_.y, position_.z+intendedMovement.z}).y < 5) {
        intendedMovement.z *= 0.1f;
    }
    */

    position_.x -= intendedMovement.x;
    position_.z += intendedMovement.z;
}