#pragma once

#include <functional>
#include <vector>
#include "GameObject.h"

class GO_Target : public GameObject {
public:
    GO_Target(T3DVec3 position, float rotation, std::vector<GameObject*>* objectsToActivate, std::function<void()> onActivate=nullptr);
    virtual ~GO_Target();
    virtual void handleInput() override;
    virtual void update() override;
    virtual void renderT3d() override;
    virtual void renderRdpq() override;

private:
    T3DMat4 targetMat;
    T3DMat4FP* targetMatFP;
    static T3DModel* targetModel;

    static uint8_t instanceCount;

    float targetAmount_ = 10.0f;

    std::vector<GameObject*>* objectsToActivate_ = nullptr;

    bool checkProjectionCollision(GameObject* p);
    void checkProjectiles();

    color_t targetColor_ = (color_t){0xFF, 0, 0, 0xFF};

    std::function<void()> onActivate_;
};