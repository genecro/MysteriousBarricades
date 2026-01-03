//adapted from HailtoDodongo's 2024 game jam entry
#include "Audio.h"

namespace {
    constexpr int CHANNEL_COUNT = 16;

    constexpr int CHANNEL_BGM = 1;
    constexpr int CHANNEL_INFO = 3;
    constexpr int CHANNEL_SFX = 4;
    constexpr int CHANNEL_SFX_COUNT = 8;

    constexpr float BGM_FADE_TIME = 2.0f;
    uint32_t lastIdx{};

    int findFreeChannel() {
        lastIdx += 1; // Note: for some reason not cycling through sometimes causes SFX to not play
        for(int i=0; i<CHANNEL_SFX_COUNT; ++i) {
            int idx = CHANNEL_SFX + ((i + lastIdx) % CHANNEL_SFX_COUNT);
            if(!mixer_ch_playing(idx))return idx;
        }
        return -1;
    }
}

Audio::Audio() {
    lastIdx = CHANNEL_SFX;
}

Audio::~Audio() {
    for(auto &sfx : sfxMap) {
        wav64_close(sfx.second.source);
    }
}

uint32_t Audio::playSFX(std::string name, SfxConf conf = {}) {

    auto it = sfxMap.find(name);
    if(it==sfxMap.end()) {
        wav64_loadparms_t parms{}; parms.streaming_mode = WAV64_STREAMING_NONE;
        wav64_t* sfx = wav64_load(name.c_str(), &parms);
        it = sfxMap.insert({name, {sfx}}).first;
        for(auto & instance : it->second.instances) {
            instance.wave = it->second.source;
        }
    }

    // check if any channel is free
    int ch = findFreeChannel();
    if(ch < 0) {
        return 0;
    }

    if(!conf.concurrent) {
        for(auto & instance : it->second.instances) {
            if(mixer_ch_playing(instance.channel)) {
                return 0;
            }
        }
    }
    
    for(auto & instance : it->second.instances) {
        if(instance.channel == 0 || !mixer_ch_playing(instance.channel)) {
            instance.channel = ch;
            float vol = conf.volume * volSFX;
            mixer_ch_set_vol(ch, vol, vol);
            mixer_ch_play(ch, &instance.wave->wave);
            return 0;
        }
    }
    

    debugf("No free instance\n");
    return 0;
}