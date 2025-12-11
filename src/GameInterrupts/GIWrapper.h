#pragma once

template <typename T> class GIWrapper {
    T* create() {
        return new T();
    }
    T* create(float f) {
        return new T(f);
    }
    T* create(GameState* gs) {
        return new T(gs);
    }
    T* create(GameState* gs, float f) {
        return new T(gs, f);
    }
};