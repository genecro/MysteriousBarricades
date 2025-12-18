//adapted from HailtoDodongo's 2024 game jam entry
#pragma once

#include <array>
#include <t3d/t3dmath.h>
#include <unordered_map>
#include <string>

struct SfxConf {
  float volume{1.0};
  uint8_t loop{0};
  uint8_t is2D{0};
  uint8_t variation{0};
};

class Audio {
public:
    Audio();
    ~Audio();

    struct SFXInstance {
      wav64_t* wave{};
      uint8_t channel{};
    };

    struct SFX {
      wav64_t* source{};
      std::array<SFXInstance, 4> instances{};
    };

    float volSFX{0.9f};

    std::unordered_map<std::string, SFX> sfxMap;

    

    uint32_t playSFX(std::string name, SfxConf conf = {});
};