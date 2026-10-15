#include "player.h"

#include <stdio.h>
#include <string.h>

static const Attack REWARD_ATTACKS[] = {
    {"Brasa", TYPE_FLAME, QTE_TIMING, 8}, {"Gota Clara", TYPE_WATER, QTE_SEQUENCE, 7},
    {"Raiz Rapida", TYPE_PLANT, QTE_HOLD, 9}, {"Raio Manso", TYPE_LIGHT, QTE_TIMING, 10},
    {"Eco Sombrio", TYPE_SHADOW, QTE_SEQUENCE, 10}, {"Golpe Neutro", TYPE_NEUTRAL, QTE_HOLD, 11},
    {"Chama Alta", TYPE_FLAME, QTE_HOLD, 13}, {"Maré Curta", TYPE_WATER, QTE_TIMING, 12},
    {"Vinha Fina", TYPE_PLANT, QTE_SEQUENCE, 12}, {"Luz Guia", TYPE_LIGHT, QTE_HOLD, 13},
    {"Sombra Fria", TYPE_SHADOW, QTE_TIMING, 14}, {"Pulso Calmo", TYPE_NEUTRAL, QTE_SEQUENCE, 13}
};

void PlayerInit(PlayerStats *player)
{
    memset(player, 0, sizeof(*player));
    snprintf(player->name, sizeof(player->name), "Ari");
    player->hp_max = 40;
    player->hp = player->hp_max;
    player->level = 1;
    player->defense_type = TYPE_NEUTRAL;

    player->attacks[0] = (Attack){"Brasa", TYPE_FLAME, QTE_TIMING, 8};
    player->attacks[1] = (Attack){"Gota Clara", TYPE_WATER, QTE_SEQUENCE, 7};
    player->attacks[2] = (Attack){"Raiz Rapida", TYPE_PLANT, QTE_HOLD, 9};
    player->attack_count = 3;

    player->inventory[0] = (InventoryItem){ITEM_TEA, 2};
    player->inventory[1] = (InventoryItem){ITEM_RED_RIBBON, 1};
    player->inventory[2] = (InventoryItem){ITEM_WET_CLOAK, 1};
    player->inventory[3] = (InventoryItem){ITEM_SOLAR_BROOCH, 1};
}

bool PlayerAddItem(PlayerStats *player, ItemId id, int quantity)
{
    for (int index = 0; index < MAX_INVENTORY_ITEMS; index++) {
        if (player->inventory[index].quantity > 0 && player->inventory[index].id == id) {
            player->inventory[index].quantity += quantity;
            return true;
        }
    }
    for (int index = 0; index < MAX_INVENTORY_ITEMS; index++) {
        if (player->inventory[index].quantity == 0) {
            player->inventory[index] = (InventoryItem){id, quantity};
            return true;
        }
    }
    return false;
}

bool PlayerHasAttack(const PlayerStats *player, Attack attack)
{
    for (int index = 0; index < player->attack_count; index++) {
        if (strcmp(player->attacks[index].name, attack.name) == 0) return true;
    }
    return false;
}

int PlayerRewardAttackCount(void)
{
    return (int)(sizeof(REWARD_ATTACKS) / sizeof(REWARD_ATTACKS[0]));
}

Attack PlayerRewardAttack(int index)
{
    if (index < 0 || index >= PlayerRewardAttackCount()) return REWARD_ATTACKS[0];
    return REWARD_ATTACKS[index];
}

bool PlayerAddAttack(PlayerStats *player, Attack attack, int replace_index)
{
    if (PlayerHasAttack(player, attack)) return false;
    if (player->attack_count < MAX_EQUIPPED_ATTACKS) {
        player->attacks[player->attack_count++] = attack;
        return true;
    }
    if (replace_index < 0 || replace_index >= MAX_EQUIPPED_ATTACKS) return false;
    player->attacks[replace_index] = attack;
    return true;
}

bool PlayerGainXp(PlayerStats *player, int xp)
{
    const int old_level = player->level;

    player->xp += xp;
    while (player->xp >= player->level * 20) {
        player->xp -= player->level * 20;
        player->level++;
        player->hp_max += 5;
        player->hp += 5;
        player->permanent_damage_bonus++;
    }
    return player->level > old_level;
}

bool PlayerUseItem(PlayerStats *player, int inventory_index, const ItemDefinition **used_item,
                   int *applied_value)
{
    InventoryItem *slot;
    const ItemDefinition *item;

    if (inventory_index < 0 || inventory_index >= MAX_INVENTORY_ITEMS) return false;
    slot = &player->inventory[inventory_index];
    if (slot->quantity <= 0) return false;

    item = ItemGetDefinition(slot->id);
    slot->quantity--;
    *used_item = item;
    *applied_value = item->value;
    if (item->effect == ITEM_EFFECT_HEAL) {
        *applied_value = PlayerHeal(player, item->value);
    } else if (item->effect == ITEM_EFFECT_DAMAGE_BUFF) {
        player->damage_buff_percent = item->value;
        player->damage_buff_turns = item->duration;
    } else if (item->effect == ITEM_EFFECT_DEFENSE_TYPE) {
        player->defense_type = item->defense_type;
    }
    return true;
}

float PlayerAttackBuffMultiplier(const PlayerStats *player)
{
    return 1.0f + (float)player->damage_buff_percent / 100.0f;
}

void PlayerConsumeAttackBuff(PlayerStats *player)
{
    if (player->damage_buff_turns <= 0) return;
    player->damage_buff_turns--;
    if (player->damage_buff_turns == 0) player->damage_buff_percent = 0;
}

int PlayerHeal(PlayerStats *player, int amount)
{
    const int old_hp = player->hp;

    player->hp += amount;
    if (player->hp > player->hp_max) player->hp = player->hp_max;
    return player->hp - old_hp;
}
