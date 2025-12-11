#include "Inventory.h"

Inventory::Inventory() {
    itemMap = new std::map<int, inventoryItem>();

    itemMap->emplace(KEY_ITEM_ID, (inventoryItem){
        .itemName = "Key",
        .itemDesc = "Opens a locked Door",
        .useString = "",
        .canUse = false,
        .canDiscard = false,
        .itemSprite = sprite_load("rom:/sprites/iconKey.sprite"),
        .onUse = [](){},
    });

    itemMap->emplace(POTION_ITEM_ID, (inventoryItem){
        .itemName = "Salve",
        .itemDesc = "Bestows 2 Health",
        .useString = "Put on",
        .canUse = true,
        .canDiscard = true,
        .itemSprite = sprite_load("rom:/sprites/iconPotion.sprite"),
        .onUse = [](){},
    });
}

Inventory::~Inventory() {
    for(auto& [key, value] : *itemMap) {
        sprite_free(value.itemSprite);
        value.itemSprite = nullptr;
    }
    delete itemMap;
}

inventoryItem Inventory::getItem(int id) {
    return itemMap->at(id);
}