#pragma once
#include <libdragon.h>
#include <string>
#include <unordered_map>
#include <map>
#include "../Fonts.h"
#include "GameInterrupt.h"
//#include "../globals.h"
//#include "../GameStates/GameState.h"

class GI_MultiChoice : public GameInterrupt
{
public:
    template<typename... Args> GI_MultiChoice(Args&&... args) {
        addPairs(std::forward<Args>(args)...);
        numChoices_ = menuChoices.size();
    }
    virtual ~GI_MultiChoice();
    virtual void handleInput() override;
    virtual void update() override;
    virtual void renderT3d() override;
    virtual void renderRdpq() override;

    //GI_Alert* setNextAlert(GI_Alert* nextAlert);

private:
    std::string name_;
    //GI_Alert* nextAlert_ = nullptr;
    bool titleText_ = false;

    std::map<std::string, GameInterrupt*> menuChoices;

    int currentChoice_ = 0;
    int numChoices_ = 0;

    std::string currentChoiceString_ = "";

    void addPairs() {}

    template<typename S, typename P, typename... Rest> void addPairs(S&& label, P&& interrupt, Rest&&... rest) {
        static_assert(std::is_convertible_v<S, std::string>,
                      "First of each pair must be a string");
        static_assert(std::is_convertible_v<P, GameInterrupt*>,
                      "Second of each pair must be a GameInterrupt*");

        menuChoices.emplace(std::forward<S>(label),
                      static_cast<GameInterrupt*>(interrupt));

        addPairs(std::forward<Rest>(rest)...);
    }
};