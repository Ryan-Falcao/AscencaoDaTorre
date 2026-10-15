#include "items.h"

static const ItemDefinition ITEM_DEFINITIONS[ITEM_COUNT] = {
    [ITEM_TEA] = {ITEM_TEA, "Cha Morno", "Recupera 12 HP.", ITEM_EFFECT_HEAL, 12, 0, TYPE_NEUTRAL},
    [ITEM_MOON_HERB] = {ITEM_MOON_HERB, "Erva Lunar", "Recupera 20 HP.", ITEM_EFFECT_HEAL, 20, 0, TYPE_NEUTRAL},
    [ITEM_RED_RIBBON] = {ITEM_RED_RIBBON, "Fita Rubra", "+50% de dano no proximo ataque.", ITEM_EFFECT_DAMAGE_BUFF, 50, 1, TYPE_NEUTRAL},
    [ITEM_TOWER_BELL] = {ITEM_TOWER_BELL, "Sino da Torre", "+35% de dano por dois ataques.", ITEM_EFFECT_DAMAGE_BUFF, 35, 2, TYPE_NEUTRAL},
    [ITEM_WET_CLOAK] = {ITEM_WET_CLOAK, "Manto Umido", "Muda a defesa para Agua.", ITEM_EFFECT_DEFENSE_TYPE, 0, 0, TYPE_WATER},
    [ITEM_SOLAR_BROOCH] = {ITEM_SOLAR_BROOCH, "Broche Solar", "Muda a defesa para Luz.", ITEM_EFFECT_DEFENSE_TYPE, 0, 0, TYPE_LIGHT},
    [ITEM_FIRECRACKER] = {ITEM_FIRECRACKER, "Estalinho", "Causa 10 de dano sem QTE.", ITEM_EFFECT_SURPRISE_DAMAGE, 10, 0, TYPE_NEUTRAL},
    [ITEM_THORN_BOMB] = {ITEM_THORN_BOMB, "Bomba de Espinhos", "Causa 16 de dano sem QTE.", ITEM_EFFECT_SURPRISE_DAMAGE, 16, 0, TYPE_NEUTRAL}
};

const ItemDefinition *ItemGetDefinition(ItemId id)
{
    if (id < ITEM_TEA || id >= ITEM_COUNT) return &ITEM_DEFINITIONS[ITEM_TEA];
    return &ITEM_DEFINITIONS[id];
}

const char *ItemEffectLabel(ItemEffect effect)
{
    switch (effect) {
        case ITEM_EFFECT_HEAL: return "CURA";
        case ITEM_EFFECT_DAMAGE_BUFF: return "BUFF";
        case ITEM_EFFECT_DEFENSE_TYPE: return "DEF";
        case ITEM_EFFECT_SURPRISE_DAMAGE: return "DANO";
        default: return "ITEM";
    }
}

int ItemShopPrice(ItemId id)
{
    const ItemDefinition *item = ItemGetDefinition(id);
    return item->effect == ITEM_EFFECT_SURPRISE_DAMAGE ? 11 :
           (item->effect == ITEM_EFFECT_DAMAGE_BUFF ? 9 : 6);
}
