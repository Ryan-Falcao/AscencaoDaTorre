#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>

#include "types.h"
#include "qte.h"
#include "items.h"

#define MAX_EQUIPPED_ATTACKS 4
#define MAX_INVENTORY_ITEMS 8
#define PLAYER_ATTACK_COUNT 16

typedef enum {
    FX_IMPACT, FX_PROJECTILE, FX_SLASH, FX_RAIN, FX_BEAM
} AttackFxStyle;

typedef enum {
    STATUS_NONE, STATUS_BURN, STATUS_WET, STATUS_ROOT, STATUS_BLIND, STATUS_STUN
} StatusEffect;

typedef enum {
    ATTACK_BRASA, ATTACK_GOTA_CLARA, ATTACK_RAIZ_RAPIDA, ATTACK_RAIO_MANSO,
    ATTACK_ECO_SOMBRIO, ATTACK_GOLPE_NEUTRO, ATTACK_CHAMA_ALTA, ATTACK_MARE_CURTA,
    ATTACK_VINHA_FINA, ATTACK_LUZ_GUIA, ATTACK_SOMBRA_FRIA, ATTACK_PULSO_CALMO,
    ATTACK_FAGULHA, ATTACK_ORVALHO, ATTACK_ESPINHO_LUNAR, ATTACK_SINO_VAZIO
} AttackId;

typedef struct {
    AttackId id;
    char name[24];
    Type type;
    QteKind qte_kind;
    int base_damage;
    AttackFxStyle fx_style;
    int hits;
    StatusEffect status;
    int status_duration;
    int max_uses;
    int cooldown_turns;
    bool crit_on_perfect;
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
    float incoming_damage_multiplier;
    int attack_uses[PLAYER_ATTACK_COUNT];
    int attack_cooldowns[PLAYER_ATTACK_COUNT];
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
bool PlayerAttackAvailable(const PlayerStats *player, const Attack *attack, char *reason, int reason_size);
void PlayerUseAttack(PlayerStats *player, const Attack *attack);
void PlayerAdvanceAttackCooldowns(PlayerStats *player);
const char *PlayerAttackIdName(AttackId id);
const char *PlayerStatusName(StatusEffect status);

#endif
