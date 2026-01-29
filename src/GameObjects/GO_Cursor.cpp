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

    repelRingModel = t3d_model_load("rom:/repelRing.t3dm");
    t3d_mat4_identity(&repelRingMat);
    repelRingMatFP = (T3DMat4FP*)malloc_uncached(sizeof(T3DMat4FP));
    repelRingScale = 1.0f;

    /*
    rspq_block_begin();
    t3d_model_draw(repelRingModel);
    dplRepelRing = rspq_block_end();

    rspq_block_begin();
    t3d_model_draw(cursorModel);
    dplCursor = rspq_block_end();

    rspq_block_begin();
    t3d_model_draw(groundMarkerModel);
    dplGroundMarker = rspq_block_end();
    */

    barricadeIndicatorFull = sprite_load("rom:/sprites/barricadeIndicatorFull.rgba16.sprite");
    barricadeIndicatorEmpty = sprite_load("rom:/sprites/barricadeIndicatorEmpty.rgba16.sprite");

    collisionTris = newCollisionTris;

    cursorState = global::CURSOR_STATE_BASE;

    RPTotal_ = global::gameProgress.rpCapacity;
    //global::gameProgress.numBarricades = global::gameProgress.numBarricades;
    repairSpeed_ = global::gameProgress.repairSpeedMultiplier;
}

GO_Cursor::~GO_Cursor() {
    free_uncached(cursorMatFP);
    free_uncached(cursorEdgeMatFP1);
    free_uncached(cursorEdgeMatFP2);
    free_uncached(groundMarkerMatFP);
    free_uncached(groundMarkerEdgeMatFP1);
    free_uncached(groundMarkerEdgeMatFP2);
    instanceCount--;
    if(instanceCount == 0) {
        t3d_model_free(cursorModel);
        t3d_model_free(groundMarkerModel);
        cursorModel=nullptr;
        groundMarkerModel=nullptr;
    }
    free_uncached(repelRingMatFP);
    t3d_model_free(repelRingModel);
    sprite_free(barricadeIndicatorFull);
    sprite_free(barricadeIndicatorEmpty);

    //rspq_block_free(dplRepelRing);
    //rspq_block_free(dplCursor);
    //rspq_block_free(dplGroundMarker);
}

void GO_Cursor::handleInput() {
    
    joypad_buttons_t btn = joypad_get_buttons_pressed(JOYPAD_PORT_1);
    joypad_buttons_t btnRel = joypad_get_buttons_released(JOYPAD_PORT_1);
    joypad_buttons_t btnHeld = joypad_get_buttons_held(JOYPAD_PORT_1);
    joypad_inputs_t joypad = joypad_get_inputs(JOYPAD_PORT_1);

    switch (cursorState) {

        //moving around
        case global::CURSOR_STATE_BASE:
            
            if(overARepairable_) {
                GO_Repairable* temp = global::gameState->repairableList->getCurrRepairable();

                if(btn.b) {
                    enemiesBeingRepelled.clear();
                    for (GO_Enemy* e : *global::gameState->enemyList->gameObjects_) {
                        if(e->getState() == global::ENEMY_STATE_ATTACKING && e->target_ == temp) {
                            enemiesBeingRepelled.push_back(e);
                        }
                    }

                    if(enemiesBeingRepelled.size() > 0) {
                        repellingEnemies_ = true;
                        repellingEnemiesTimer = repellingEnemiesRate_ * repellingEnemiesCtr_;
                        for(GO_Enemy* e : enemiesBeingRepelled) {
                            e->stun(4);
                            e->pushAwayFromRepairable(temp, repellingEnemiesDistance_);
                            //e->setStateSeeking(temp);
                        }
                    }
                    
                    else if(temp) {
                        if(temp->HPCurrent_ < temp->HPTotal_ && RPCurrent_ > 0) {
                            temp->HPCurrent_ += repairSpeed_*global::frameTimeMultiplier;
                            RPCurrent_ -= repairSpeed_*global::frameTimeMultiplier;
                        }
                    }
                }

                else if(btnHeld.b) {
                //if(btn.b) {
                    if(repellingEnemies_) {
                        enemiesBeingRepelled.clear();
                        for (GO_Enemy* e : *global::gameState->enemyList->gameObjects_) {
                            if(e->getState() == global::ENEMY_STATE_ATTACKING && e->target_ == temp) {
                                enemiesBeingRepelled.push_back(e);
                                if(!e->isStunned_) {
                                    e->stun(4);
                                    e->pushAwayFromRepairable(temp, repellingEnemiesDistance_);
                                }
                            }
                        }
                        float prevRepelTimer = repellingEnemiesTimer;
                        repellingEnemiesTimer -= global::frameTimeMultiplier;
                        if((int)(prevRepelTimer / repellingEnemiesRate_) != (int)(repellingEnemiesTimer / repellingEnemiesRate_)
                            || repellingEnemiesTimer <= 0) {
                            for(GO_Enemy* e : enemiesBeingRepelled) {
                                e->stun(4);
                                e->pushAwayFromRepairable(temp, repellingEnemiesDistance_ * repellingEnemiesTimer <= 0? 2 : 1);
                            }
                        }
                        if(repellingEnemiesTimer <= 0) {
                            repellingEnemies_ = false;
                            for(GO_Enemy* e : enemiesBeingRepelled) {
                                e->setStateSeeking(e->target_);
                            }
                            enemiesBeingRepelled.clear();
                        }
                    }
                    else if(temp) {
                        float enemiesAttacking = false;
                        for (GO_Enemy* e : *global::gameState->enemyList->gameObjects_) {
                            if(e->getState() == global::ENEMY_STATE_ATTACKING && e->target_ == temp) {
                                enemiesAttacking = true;
                                break;
                            }
                        }
                        if(!enemiesAttacking && temp->HPCurrent_ < temp->HPTotal_ && RPCurrent_ > 0) {
                            temp->HPCurrent_ += repairSpeed_*global::frameTimeMultiplier;
                            RPCurrent_ -= repairSpeed_*global::frameTimeMultiplier;
                        }
                    }
                }

                else if(btn.a) {
                    cursorState = global::CURSOR_STATE_BARRICADE;
                }

                if(btnRel.b) {
                //if(!btn.b) {
                    if(repellingEnemies_) {
                        repellingEnemies_ = false;
                        for(GO_Enemy* e : enemiesBeingRepelled) {
                            e->setStateSeeking(e->target_);
                        }
                        enemiesBeingRepelled.clear();
                    }
                }
            }
            else if(btn.a) {
                cursorState = global::CURSOR_STATE_BARRICADE;
                //tell gamestate cursor is making a barricade
                global::gameState->enemyList->cursorMakingBarricade(position_);
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
                //tell the enemies that the cursor is not making a barricade anymore
                global::gameState->enemyList->cursorNotMakingBarricade();

                if(global::gameState->barricadeList->gameObjects_->size() < global::gameProgress.numBarricades) {
                    if(abs(barricadeEdgeRelativeToCursor.x) + abs(barricadeEdgeRelativeToCursor.z) > 1) {
                        global::gameState->barricadeList->push(
                            new GO_BarricadeStandard(
                                groundMarkerPos,
                                barricadeEdgeRelativeToCursor, 
                                (color_t){0xFF, 0xFF, 0, 0x7F}
                            )
                        );
                    }
                }
                else {
                    barricadeIndicatorBlinkTimer = barricadeIndicatorBlinkTimerMax;
                    global::audioManager->playSFX("rom:/metallicDodgeChance5.wav64", {.volume = 0.4f});
                    global::gameState->triedToCastWithoutSlots();
                }
                cursorState = global::CURSOR_STATE_BASE;
            }
        break;
    }

    if(cursorState!=global::CURSOR_STATE_BARRICADE && !repellingEnemies_) {
        if(btn.r) {
            GO_Repairable* temp = global::gameState->repairableList->getNextRepairable();
            if(temp) {
                global::audioManager->playSFX("rom:/vibratoWhimsy4.wav64", {.volume = 0.4f});
                position_.x = temp->position_.x;
                position_.z = temp->position_.z;
            }
        }
        else if(btn.l) {
            GO_Repairable* temp = global::gameState->repairableList->getPrevRepairable();
            if(temp) {
                global::audioManager->playSFX("rom:/vibratoWhimsy4.wav64", {.volume = 0.4f});
                position_.x = temp->position_.x;
                position_.z = temp->position_.z;
            }
        }

        //debugf("Checking for joystick\n");
        if(joypad.stick_x < -STICK_DEADZONE || joypad.stick_x > STICK_DEADZONE ||
           joypad.stick_y < -STICK_DEADZONE || joypad.stick_y > STICK_DEADZONE) {
            //debugf("joystick moving\n");
            handleMovement((T3DVec3){
                    (float)(joypad.stick_x)/200.0f*global::frameTimeMultiplier,
                    0,
                    (float)(joypad.stick_y)/200.0f*global::frameTimeMultiplier
                }
            );
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

    groundMarkerPos = collision::findGroundIntersection(*collisionTris, position_);

    t3d_mat4_from_srt_euler(&groundMarkerMat,
        (float[3]){0.003f, 0.003f, 0.003f},
        (float[3]){0.0f, rotation_, 0.0f},
        groundMarkerPos.v
    );
    t3d_mat4_to_fixed(groundMarkerMatFP, &groundMarkerMat);

    overARepairable_ = false;
    for(GO_Repairable* i: *(global::gameState->repairableList->repairables)) {
        if(t3d_vec3_distance2((T3DVec3){i->position_.x,0,i->position_.z}, (T3DVec3){position_.x, 0, position_.z}) <=20) {
            //nearRepairable = true;
            overARepairable_ = true;
            global::gameState->repairableList->setCurrentRepairable(i);
            break;
        }
    }

    switch (cursorState) {
        case global::CURSOR_STATE_BASE:
            cursorColor = overARepairable_ ? cursorColorRepair : cursorColorBase;
        break;

        case global::CURSOR_STATE_BARRICADE:
            cursorColor = cursorColorBase;
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
    }

    if(repellingEnemies_) {
        t3d_mat4_from_srt_euler(&repelRingMat,
            (float[3]){0.08f*repelRingScale, 0.08f, 0.08f*repelRingScale},
            (float[3]){0.0f, rotation_, 0.0f},
            global::gameState->repairableList->getCurrRepairable()->position_.v
        );
        t3d_mat4_to_fixed(repelRingMatFP, &repelRingMat);
    }

    for(auto& r : *global::gameState->repairableList->repairables) {
        if(r->fullyRepaired) {
            r->updateRepelRing();
        }
    }

    if(barricadeIndicatorBlinkTimer > 0) {
        float prevBlinkTimer = barricadeIndicatorBlinkTimer;
        barricadeIndicatorBlinkTimer -= global::frameTimeMultiplier;
        if((int)(prevBlinkTimer / (5.0f)) != (int)(barricadeIndicatorBlinkTimer / (5.0f))) {
            displayBarricadeIndicator = !displayBarricadeIndicator;
        }
        if(barricadeIndicatorBlinkTimer <= 0) {
            displayBarricadeIndicator = true;
        }
    }
}

void GO_Cursor::renderT3d() {
    rdpq_sync_pipe();

    rdpq_set_prim_color(cursorColor);
    t3d_matrix_set(cursorMatFP, true);
    t3d_model_draw(cursorModel);
    //rspq_block_run(dplCursor);

    if(repellingEnemies_) {
        t3d_matrix_set(repelRingMatFP, true);
        t3d_model_draw(repelRingModel);
        //rspq_block_run(dplRepelRing);
    }

    for(auto& r : *global::gameState->repairableList->repairables) {
        if(r->fullyRepaired) {
            r->renderRepelRing();
        }
    }

    rdpq_set_prim_color(groundMarkerColor);
    t3d_matrix_set(groundMarkerMatFP, true);
    t3d_model_draw(groundMarkerModel);
    //rspq_block_run(dplGroundMarker);

    switch (cursorState) {
        case global::CURSOR_STATE_BASE:
        break;

        case global::CURSOR_STATE_BARRICADE:
            
            t3d_matrix_set(cursorEdgeMatFP1, true);
            t3d_model_draw(cursorModel);
            //rspq_block_run(dplCursor);

            t3d_matrix_set(cursorEdgeMatFP2, true);
            t3d_model_draw(cursorModel);
            //rspq_block_run(dplCursor);

            t3d_matrix_set(groundMarkerEdgeMatFP1, true);
            t3d_model_draw(groundMarkerModel);
            //rspq_block_run(dplGroundMarker);

            t3d_matrix_set(groundMarkerEdgeMatFP2, true);
            t3d_model_draw(groundMarkerModel);
            //rspq_block_run(dplGroundMarker);
        break;

        case global::CURSOR_STATE_REPAIR:
        break;
    }
}

void GO_Cursor::renderRdpq() {

    rdpq_sync_pipe();

    //draw barricade tracker
    
    if(displayBarricadeIndicator) {
        rdpq_set_mode_standard();
        rdpq_mode_alphacompare(1);
        int numFreeBarricades = global::gameProgress.numBarricades-global::gameState->barricadeList->gameObjects_->size();

        for(int i = 0; i < global::gameProgress.numBarricades; i++) {
            rdpq_sprite_blit(i+1 > numFreeBarricades ? barricadeIndicatorEmpty : barricadeIndicatorFull,
                27 + i*16,
                32, 
                &(rdpq_blitparms_t){
                    .theta = T3D_PI/4.0f
                }
            );
        }
    }

    //draw RP bar
    rdpq_set_mode_fill((color_t){0,0,0,0xFF});
    rdpq_fill_rectangle(25, 15, 25+RPRectangleBaseLength*100.0f/RPTotal_, 15+RPRectangleHeight);

    rdpq_set_fill_color((color_t){0,0xFF,0,0xFF});
    rdpq_fill_rectangle(25+1, 15+1, 25+RPRectangleBaseLength*RPCurrent_/RPTotal_, 15+RPRectangleHeight-1);
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
    Triangle tempTri;

    intendedMovement.x *= -1;

    for (int i = 0; i < 5; i++) {
        bool collided = false;
        T3DVec3 groundLoc = collision::findGroundIntersection(*collisionTris, (T3DVec3){position_.x+intendedMovement.x, position_.y, position_.z+intendedMovement.z}, tempTri);
        if(groundLoc.y > -100 && fmaxf(tempTri.v0.y, fmaxf(tempTri.v1.y, tempTri.v2.y)) > position_.y-5.0f) {
            collided = true;
            T3DVec3 triNorm = collision::triangleNormal(tempTri);
            triNorm.y=0;

            t3d_vec3_norm(&triNorm);

            float intoWall = t3d_vec3_dot(triNorm, intendedMovement);

            if(intoWall < 0.0f) {
                intendedMovement -= triNorm * intoWall;
            }
        }
        if(!collided) break;
    }

    position_.x += intendedMovement.x;
    position_.z += intendedMovement.z;
}