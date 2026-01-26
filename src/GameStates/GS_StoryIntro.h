#pragma once

#include "GameState.h"

#define NUM_STORY_PAGES 9

class GS_StoryIntro : public GameState {
public:
    GS_StoryIntro(T3DVec3 startingPos);
    ~GS_StoryIntro();

    virtual void handleInput() override;
    virtual void update() override;
    virtual void renderT3d() override;
    virtual void renderRdpq() override;

private:

    uint8_t currPage = 0;

    std::string storyPages[NUM_STORY_PAGES] = {
        "Ah, we have been waiting for you,\n" + global::playerName + ".",
        "Our oracles foretold of your arrival.",
        "You are among the last of your kind,\nthe Impedimancers...",
        "...those practitioners of the art,\nscience, and magic of barricades.",
        "Our realm has been besieged by\nfrightful hordes of unknown origin...",
        "...unfeeling, mechanical automata\nanimated by some hideous power.",
        "We beg for your assistance with our\ndefenses and with the repair of\nour damaged fortifications.",
        "If you prove to be skilled in your\nart, we shall see to it that you are\nrewarded amply...",
        "...and perhaps we may uncover the\norigins of these infernal machines..."
    };
    
};