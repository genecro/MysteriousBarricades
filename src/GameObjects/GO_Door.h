#pragma once

#include "GO_Portal.h"

template <typename T> class GO_Door : public GO_Portal<T>
{
public:
    GO_Door(T3DVec3 position, float rotation, float width, T3DVec3 playerStartPos, bool locked=false);
    ~GO_Door();
    virtual void handleInput() override;
    virtual void update() override;
    virtual void renderT3d() override;
    virtual void renderRdpq() override;

protected:
    bool locked_;
    static uint8_t instanceCount;

    T3DMat4 doorMat;
    T3DMat4FP* doorMatFP;
    static T3DModel* doorModel;
};

template <typename T> uint8_t GO_Door<T>::instanceCount = 0;
template <typename T> T3DModel* GO_Door<T>::doorModel = nullptr;

template <typename T> GO_Door<T>::GO_Door(T3DVec3 position, float rotation, float width, T3DVec3 playerStartPos, bool locked=false) :
                    GO_Portal<T>::GO_Portal(position, rotation, width, playerStartPos) {
    locked_ = locked;

    t3d_mat4_identity(&doorMat);
    doorMatFP = (T3DMat4FP*)malloc_uncached(sizeof(T3DMat4FP));
    instanceCount++;
    if(!doorModel) {
        doorModel = t3d_model_load("rom:/door.t3dm");
    }
}

template <typename T> GO_Door<T>::~GO_Door() {
    free_uncached(doorMatFP);
    instanceCount--;
    if(instanceCount == 0) {
        t3d_model_free(doorModel);
        doorModel = nullptr;
    }
}

template <typename T> void GO_Door<T>::handleInput() {
    joypad_buttons_t keys = joypad_get_buttons_pressed(JOYPAD_PORT_1);

    if(keys.a) {
        float rotDiff = (this->rotation_ - T3D_PI) - global::thePlayer->rotation_;
        if(rotDiff > T3D_PI) { 
            rotDiff -= 2.0f*T3D_PI;
        }
        else if (rotDiff < -T3D_PI) {
            rotDiff += 2.0f*T3D_PI;
        }
        debugf("rotDiff = %.2f\n", rotDiff);
        if(playerContact() &&
            abs(rotDiff) <= INTERACTION_ANGLE) {
            if(locked_) {
                global::GameInterruptStack->push_back(new GI_Alert("You need a key."));
            }
            else {
                global::GameInterruptStack->push_back(new GI_FadeToNextGS<T>(this->playerStartingPos_));
            }
        }
    }
}

template <typename T> void GO_Door<T>::update() {
    t3d_mat4_from_srt_euler(&doorMat,
        (float[3]){0.07f, 0.07f, 0.07f},
        (float[3]){0.0f, this->rotation_, 0.0f},
        this->position_.v
    );
    t3d_mat4_to_fixed(doorMatFP, &doorMat);
}

template <typename T> void GO_Door<T>::renderT3d() {
    t3d_matrix_set(doorMatFP, true);
    t3d_model_draw(doorModel);
}

template <typename T> void GO_Door<T>::renderRdpq() {

}