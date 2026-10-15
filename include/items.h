#ifndef ITEMS_H
#define ITEMS_H

#include "types.h"

typedef enum {
    ITEM_TEA,
    ITEM_MOON_HERB,
    ITEM_RED_RIBBON,
    ITEM_TOWER_BELL,
    ITEM_WET_CLOAK,
    ITEM_SOLAR_BROOCH,
    ITEM_FIRECRACKER,
    ITEM_THORN_BOMB,
    ITEM_COUNT
} ItemId;

typedef enum {
    ITEM_EFFECT_HEAL,
    ITEM_EFFECT_DAMAGE_BUFF,
    ITEM_EFFECT_DEFENSE_TYPE,
    ITEM_EFFECT_SURPRISE_DAMAGE
} ItemEffect;

typedef struct {
    ItemId id;
    const char *name;
    const char *description;
    ItemEffect effect;
    int value;
    int duration;
    Type defense_type;
} ItemDefinition;

const ItemDefinition *ItemGetDefinition(ItemId id);
const char *ItemEffectLabel(ItemEffect effect);
int ItemShopPrice(ItemId id);

#endif
