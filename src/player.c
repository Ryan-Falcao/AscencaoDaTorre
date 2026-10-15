#include "player.h"

#include <stdio.h>
#include <string.h>

#define ATTACK(ID, NAME, TYPE, QTE, DMG, FX, HITS, STATUS, DURATION, USES, COOLDOWN, CRIT) \
    {ID, NAME, TYPE, QTE, DMG, FX, HITS, STATUS, DURATION, USES, COOLDOWN, CRIT}

static const Attack REWARD_ATTACKS[PLAYER_ATTACK_COUNT] = {
    ATTACK(ATTACK_BRASA, "Brasa", TYPE_FLAME, QTE_TIMING, 8, FX_IMPACT, 1, STATUS_BURN, 3, 0, 0, true),
    ATTACK(ATTACK_GOTA_CLARA, "Gota Clara", TYPE_WATER, QTE_SEQUENCE, 7, FX_PROJECTILE, 1, STATUS_WET, 2, 0, 0, true),
    ATTACK(ATTACK_RAIZ_RAPIDA, "Raiz Rapida", TYPE_PLANT, QTE_HOLD, 9, FX_SLASH, 1, STATUS_ROOT, 1, 3, 1, false),
    ATTACK(ATTACK_RAIO_MANSO, "Raio Manso", TYPE_LIGHT, QTE_TIMING, 10, FX_BEAM, 1, STATUS_BLIND, 1, 3, 1, true),
    ATTACK(ATTACK_ECO_SOMBRIO, "Eco Sombrio", TYPE_SHADOW, QTE_SEQUENCE, 10, FX_IMPACT, 2, STATUS_NONE, 0, 0, 0, false),
    ATTACK(ATTACK_GOLPE_NEUTRO, "Golpe Neutro", TYPE_NEUTRAL, QTE_HOLD, 11, FX_SLASH, 1, STATUS_STUN, 1, 2, 2, true),
    ATTACK(ATTACK_CHAMA_ALTA, "Chama Alta", TYPE_FLAME, QTE_HOLD, 13, FX_RAIN, 2, STATUS_BURN, 3, 2, 2, true),
    ATTACK(ATTACK_MARE_CURTA, "Mare Curta", TYPE_WATER, QTE_TIMING, 12, FX_PROJECTILE, 1, STATUS_WET, 2, 3, 1, false),
    ATTACK(ATTACK_VINHA_FINA, "Vinha Fina", TYPE_PLANT, QTE_SEQUENCE, 12, FX_SLASH, 2, STATUS_ROOT, 1, 2, 1, true),
    ATTACK(ATTACK_LUZ_GUIA, "Luz Guia", TYPE_LIGHT, QTE_HOLD, 13, FX_BEAM, 1, STATUS_BLIND, 1, 2, 2, false),
    ATTACK(ATTACK_SOMBRA_FRIA, "Sombra Fria", TYPE_SHADOW, QTE_TIMING, 14, FX_IMPACT, 1, STATUS_STUN, 1, 2, 2, true),
    ATTACK(ATTACK_PULSO_CALMO, "Pulso Calmo", TYPE_NEUTRAL, QTE_SEQUENCE, 13, FX_RAIN, 3, STATUS_NONE, 0, 2, 1, false),
    ATTACK(ATTACK_FAGULHA, "Fagulha", TYPE_FLAME, QTE_TIMING, 6, FX_PROJECTILE, 1, STATUS_BURN, 2, 0, 0, false),
    ATTACK(ATTACK_ORVALHO, "Orvalho", TYPE_WATER, QTE_HOLD, 15, FX_BEAM, 1, STATUS_WET, 3, 2, 2, true),
    ATTACK(ATTACK_ESPINHO_LUNAR, "Espinho Lunar", TYPE_PLANT, QTE_SEQUENCE, 15, FX_RAIN, 3, STATUS_ROOT, 1, 2, 2, true),
    ATTACK(ATTACK_SINO_VAZIO, "Sino Vazio", TYPE_LIGHT, QTE_TIMING, 16, FX_IMPACT, 1, STATUS_STUN, 1, 1, 3, true)
};

void PlayerInit(PlayerStats *player)
{
    memset(player, 0, sizeof(*player));
    snprintf(player->name, sizeof(player->name), "Ari");
    player->hp_max = 40;
    player->hp = player->hp_max;
    player->level = 1;
    player->incoming_damage_multiplier = 1.0f;
    player->defense_type = TYPE_NEUTRAL;

    player->attacks[0] = REWARD_ATTACKS[ATTACK_BRASA];
    player->attacks[1] = REWARD_ATTACKS[ATTACK_GOTA_CLARA];
    player->attacks[2] = REWARD_ATTACKS[ATTACK_RAIZ_RAPIDA];
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

bool PlayerAttackAvailable(const PlayerStats *player, const Attack *attack, char *reason, int reason_size)
{
    const int id = (int)attack->id;

    if (id < 0 || id >= PLAYER_ATTACK_COUNT) return false;
    if (attack->max_uses > 0 && player->attack_uses[id] >= attack->max_uses) {
        snprintf(reason, (size_t)reason_size, "Esgotado");
        return false;
    }
    if (player->attack_cooldowns[id] > 0) {
        snprintf(reason, (size_t)reason_size, "Espere %d", player->attack_cooldowns[id]);
        return false;
    }
    return true;
}

void PlayerUseAttack(PlayerStats *player, const Attack *attack)
{
    const int id = (int)attack->id;
    if (id < 0 || id >= PLAYER_ATTACK_COUNT) return;
    player->attack_uses[id]++;
    player->attack_cooldowns[id] = attack->cooldown_turns;
}

void PlayerAdvanceAttackCooldowns(PlayerStats *player)
{
    for (int index = 0; index < PLAYER_ATTACK_COUNT; index++) {
        if (player->attack_cooldowns[index] > 0) player->attack_cooldowns[index]--;
    }
}

const char *PlayerAttackIdName(AttackId id)
{
    return id >= 0 && id < PLAYER_ATTACK_COUNT ? REWARD_ATTACKS[id].name : "desconhecido";
}

const char *PlayerStatusName(StatusEffect status)
{
    static const char *const names[] = {"", "QUEIMAR", "MOLHAR", "ENRAIZAR", "CEGAR", "ATORDOAR"};
    return status >= STATUS_NONE && status <= STATUS_STUN ? names[status] : "";
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
