#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>

#include "types.h"
#include "qte.h"
#include "items.h"

#define MAX_EQUIPPED_ATTACKS 4
#define MAX_INVENTORY_ITEMS 8

typedef struct {
    char name[24];
    Type type;
    QteKind qte_kind;
    int base_damage;
} Attack;

typedef struct {
    ItemId id;
    int quantity;
} InventoryItem;

typedef struct {
    char name[24];
    int hp;
    int hp_max;
    int level;
    Type defense_type;
    Attack attacks[MAX_EQUIPPED_ATTACKS];
    int attack_count;
    InventoryItem inventory[MAX_INVENTORY_ITEMS];
    int damage_buff_percent;
    int damage_buff_turns;
    int permanent_damage_bonus;
    int xp;
} PlayerStats;

void PlayerInit(PlayerStats *player);
int PlayerHeal(PlayerStats *player, int amount);
bool PlayerUseItem(PlayerStats *player, int inventory_index, const ItemDefinition **used_item,
                   int *applied_value);
float PlayerAttackBuffMultiplier(const PlayerStats *player);
void PlayerConsumeAttackBuff(PlayerStats *player);
bool PlayerAddItem(PlayerStats *player, ItemId id, int quantity);
bool PlayerHasAttack(const PlayerStats *player, Attack attack);
int PlayerRewardAttackCount(void);
Attack PlayerRewardAttack(int index);
bool PlayerAddAttack(PlayerStats *player, Attack attack, int replace_index);
bool PlayerGainXp(PlayerStats *player, int xp);

#endif
