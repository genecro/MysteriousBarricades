#pragma once

#include "GameObject.h"

enum {
    CAM_CLOSE = 0,
    CAM_MID = 1,
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
    T3DMat4 playerMat;
    T3DMat4FP* playerMatFP;
    T3DModel *modelPlayer;
    rspq_block_t *dplPlayer;

    T3DVec3 moveDir;
    bool grounded;
};