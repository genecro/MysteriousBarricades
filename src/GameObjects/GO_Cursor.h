#pragma once

#include "GameObject.h"
#include "Enemies/GO_Enemy.h"

class GO_Cursor: public GameObject {
public:
    GO_Cursor(T3DVec3 position, std::vector<Triangle> *newCollisionTris);
    GO_Cursor(std::string name);
    virtual ~GO_Cursor();
    virtual void handleInput() override;
    virtual void update() override;
    virtual void renderT3d() override;
    virtual void renderRdpq() override;

    int cursorState;
    float barrierSize = 5.0f;

    float repairSpeed_;
    bool overARepairable_ = false;
    bool repellingEnemies_ = false;
    float repellingEnemiesRate_ = 1.0f*60.0f;
    float repellingEnemiesDistance_ = 3.0f;
    float repellingEnemiesTimer = 0;
    uint8_t repellingEnemiesCtr_ = 3;

    float RPCurrent_ = 25;
    float RPTotal_;
    float RPRectangleBaseLength = 100;
    float RPRectangleHeight = 10;

    int totalBarricadeCt;
    sprite_t* barricadeIndicatorFull;
    sprite_t* barricadeIndicatorEmpty;
    bool displayBarricadeIndicator = true;
    float barricadeIndicatorBlinkTimer = 0;
    float barricadeIndicatorBlinkTimerMax = 3*60;

    void setStateRepair();
    void setStateBaseState();
    void setStateMakingBarricade();
    
    void setPosition(T3DVec3 newPos);

    T3DVec3 position_;
    T3DVec3 groundMarkerPos;
    T3DVec3 normalOfCurrTriangle;

    T3DVec3 barricadeEdgeRelativeToCursor = (T3DVec3){0,0,0};

    color_t cursorColorBase = color_t{255, 40, 0, 150};
    color_t cursorColorRepair = color_t{0, 255, 0, 150};
    color_t cursorColor = cursorColorBase;
    color_t groundMarkerColor = color_t{0, 255, 255, 255};

    void handleMovement(T3DVec3 intendedMovement);

    std::vector<GO_Enemy*> enemiesBeingRepelled{};

private:
    T3DMat4 cursorMat;
    T3DMat4 cursorEdgeMat1;
    T3DMat4 cursorEdgeMat2;
    T3DMat4FP* cursorMatFP;
    T3DMat4FP* cursorEdgeMatFP1;
    T3DMat4FP* cursorEdgeMatFP2;
    static T3DModel* cursorModel;

    T3DMat4 groundMarkerMat;
    T3DMat4 groundMarkerEdgeMat1;
    T3DMat4 groundMarkerEdgeMat2;
    T3DMat4FP* groundMarkerMatFP;
    T3DMat4FP* groundMarkerEdgeMatFP1;
    T3DMat4FP* groundMarkerEdgeMatFP2;
    static T3DModel* groundMarkerModel;

    T3DMat4 repelRingMat;
    T3DMat4FP* repelRingMatFP;
    T3DModel* repelRingModel;
    float repelRingScale;
    
    static uint8_t instanceCount;

    rspq_block_t* dplRepelRing;
    rspq_block_t* dplCursor;
    rspq_block_t* dplGroundMarker;
};