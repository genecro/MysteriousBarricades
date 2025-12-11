#pragma once

#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3ddebug.h>
#include <map>
#include <string>
#include <functional>

enum {
    KEY_ITEM_ID = 0,
    POTION_ITEM_ID = 1,
};

struct inventoryItem {
    std::string itemName;
    std::string itemDesc;
    std::string useString;
    bool canUse;
    bool canDiscard;
    sprite_t* itemSprite;
    std::function<void()> onUse;
};

class Inventory {
    public:

    Inventory();
    ~Inventory();

    inventoryItem getItem(int);

    private:
    std::map<int, inventoryItem>* itemMap;
};