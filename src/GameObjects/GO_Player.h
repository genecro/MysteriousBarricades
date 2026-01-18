#pragma once

#include "GameObject.h"

enum {
    CAM_CLOSE = 0,
    CAM_MID = 1,
};

enum {
    PLAYER_STATE_BASE = 0,
    PLAYER_STATE_CURSOR = 1,
    PLAYER_STATE_BARRICADE = 2,
};

class GO_Player : public GameObject
{
public:
    GO_Player(std::string name);
    virtual ~GO_Player();
    virtual void handleInput() override;
    virtual void update() override;
    virtual void renderT3d() override;
    virtual void renderRdpq() override;

    bool canInteract(T3DVec3 target, float targetWidth);
    bool isTouching(T3DVec3 target, float targetWidth);
    

    struct camState_t {
        uint8_t closeness;
        bool followPlayerRot;
    } camState_;

    struct inventory_t {
        uint8_t keys;
        uint16_t coins;
        std::map<int, int>* items;
    } inventory_;

    int removeItem(int id, int qty);
    int addItem(int id, int qty);

protected:
    T3DMat4FP* playerMatFP;
    T3DModel *modelPlayer;
    rspq_block_t *dplPlayer;

    T3DSkeleton skel;
    T3DSkeleton skelBlend;
    T3DAnim animIdle;
    T3DAnim animWalk;
    float animBlend = 0.0f;
    int frameIdx = 0;

    int playerState_ = PLAYER_STATE_BASE;
    float barrierSize = 5.0f;

    T3DVec3 moveDir;
    bool grounded;

    T3DVec3 cursorPos_;
    float cursorRotation_ = 0;
    float cursorHeight_ = 12.0f;

    T3DMat4 cursorMat;
    T3DMat4 cursorEdgeMat1;
    T3DMat4 cursorEdgeMat2;
    T3DMat4FP* cursorMatFP;
    T3DMat4FP* cursorEdgeMatFP1;
    T3DMat4FP* cursorEdgeMatFP2;
    T3DModel* cursorModel;

    T3DMat4 groundMarkerMat;
    T3DMat4 groundMarkerEdgeMat1;
    T3DMat4 groundMarkerEdgeMat2;
    T3DMat4FP* groundMarkerMatFP;
    T3DMat4FP* groundMarkerEdgeMatFP1;
    T3DMat4FP* groundMarkerEdgeMatFP2;
    T3DModel* groundMarkerModel;

    int totalBarricadeCt;
    sprite_t* barricadeIndicatorFull;
    sprite_t* barricadeIndicatorEmpty;
    bool displayBarricadeIndicator = true;
    float barricadeIndicatorBlinkTimer = 0;
    float barricadeIndicatorBlinkTimerMax = 3*60;

    T3DVec3 groundMarkerPos = (T3DVec3){0,0,0};
    T3DVec3 barricadeEdgeRelativeToCursor = (T3DVec3){0,0,0};

    color_t cursorColorBase = color_t{255, 40, 0, 150};
    color_t cursorColorRepair = color_t{0, 255, 0, 150};
    color_t cursorColor = cursorColorBase;
    color_t groundMarkerColor = color_t{0, 255, 255, 255};
};