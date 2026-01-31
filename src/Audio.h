//adapted from HailtoDodongo's 2024 game jam entry
#pragma once

#include <array>
#include <t3d/t3dmath.h>
#include <unordered_map>
#include <string>
//#include "miscMath.h"

struct SfxConf {
  float volume{1.0};
  uint8_t loop{0};
  uint8_t is2D{0};
  uint8_t variation{0};
  bool concurrent{true};
};

enum {
  BGM_MAIN_MENU = 0,
  BGM_INTRO = 1,
  BGM_LEVEL_SELECT = 2,
  BGM_TRAINING = 3,
  BGM_INTERIOR = 4,
  BGM_LEVEL = 5,
  BGM_BOSS = 6,
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
    float volBGM{0.7f};
    //math::Timer bgmVolume{};

    std::unordered_map<std::string, SFX> sfxMap;
    wav64_t bgm{};

    void playBGM(uint8_t bgmIdx, float vol);
    void stopBGM();
    void pauseMenuBGM();
    void unpauseMenuBGM();
    uint8_t currBGM = 255;

    uint32_t playSFX(std::string name, SfxConf conf = {});

    void clearSFX();

    std::string BGMList[7] = {
      "rom:/bgm_main_menu.wav64",
      "rom:/bgm_intro.wav64",
      "rom:/bgm_level_select.wav64",
      "rom:/bgm_training.wav64",
      "rom:/bgm_interior.wav64",
      "rom:/bgm_level.wav64",
      "rom:/bgm_boss.wav64",
    };
};