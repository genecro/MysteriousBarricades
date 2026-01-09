#pragma once

#include "GameState.h"

#define NUM_STORY_PAGES 6

class GS_StoryIntro : public GameState {
public:
    GS_StoryIntro(T3DVec3 startingPos);
    ~GS_StoryIntro();

    virtual void handleInput() override;
    virtual void update() override;
    virtual void renderT3d() override;
    virtual void renderRdpq() override;

    virtual void testFunc() override;

private:

    uint8_t currPage = 0;

    std::string storyPages[NUM_STORY_PAGES] = {
        "Ah, we have been waiting for you, [name]. Our oracles foretold of your arrival.",
        "You are among the last of your kind, the Impedimancers, those practitioners of the art, science, and magic of barricades.",
        "Our realm has been besieged by frightful hordes of unknown origin... unfeeling, mechanical automata animated by some hideous power.",
        "We beg for your assistance with our defenses and with the repair of our damaged fortifications.",
        "If you prove to be skilled in your art, we shall see to it that you are rewarded amply...",
        "...and perhaps we may uncover the origins of these infernal machines..."
    };
    
};