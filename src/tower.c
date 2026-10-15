#include "tower.h"
#include "balance.h"

#include <string.h>

static unsigned int NextRandom(unsigned int *seed)
{
    *seed = *seed * 1664525u + 1013904223u;
    return *seed;
}

bool TowerIsBossFloor(int floor)
{
    return floor == 5 || floor == TOWER_FLOORS;
}

void TowerInit(Tower *tower, unsigned int seed)
{
    static const EnemyId common_enemies[] = {
        ENEMY_DUST_MOTH, ENEMY_GLOW_FROG, ENEMY_CINDER_BIRD, ENEMY_MINT_SLIME,
        ENEMY_LANTERN_WISP, ENEMY_THORN_HARE, ENEMY_SHADE_CAT, ENEMY_PETAL_KNIGHT
    };
    unsigned int random = seed == 0 ? 1u : seed;

    memset(tower, 0, sizeof(*tower));
    tower->seed = random;
    tower->floor = 1;
    for (int index = 0; index < TOWER_FLOORS; index++) {
        EnemyId enemy;

        if (index + 1 == 5) enemy = ENEMY_BOSS_BELLKEEPER;
        else if (index + 1 == TOWER_FLOORS) enemy = ENEMY_BOSS_LAST_TOWER;
        else {
            const int available = index < 3 ? 4 : (index < 7 ? 6 : 8);
            enemy = common_enemies[NextRandom(&random) % (unsigned int)available];
            if (index > 0 && enemy == tower->enemies[index - 1]) {
                enemy = common_enemies[(NextRandom(&random) + 1u) % (unsigned int)available];
            }
        }
        tower->enemies[index] = enemy;
    }
    tower->seed = random;
    TowerRollShop(tower);
}

EnemyId TowerEnemyForFloor(const Tower *tower, int floor)
{
    if (floor < 1 || floor > TOWER_FLOORS) return ENEMY_DUST_MOTH;
    return tower->enemies[floor - 1];
}

void TowerScaleEnemy(Enemy *enemy, int floor)
{
    const int multiplier = floor - 1;

    enemy->hp_max += multiplier * (enemy->boss ? BALANCE_BOSS_HP_PER_FLOOR : BALANCE_COMMON_HP_PER_FLOOR);
    enemy->hp = enemy->hp_max;
    enemy->projectile_damage_bonus = multiplier / BALANCE_PROJECTILE_BONUS_EVERY;
    enemy->projectile_damage_multiplier = enemy->boss ? 1.0f : BalanceFloorDamageMultiplier(floor);
    enemy->reward_bonus = multiplier;
}

void TowerRollRewards(Tower *tower, const PlayerStats *player)
{
    unsigned int random = tower->seed;
    int attack_id = (int)(NextRandom(&random) % (unsigned int)PlayerRewardAttackCount());
    const int attack_count = PlayerRewardAttackCount();

    /* Search circularly, so a duplicate at the last table position cannot be offered. */
    for (int attempt = 0; attempt < attack_count; attempt++) {
        const int candidate = (attack_id + attempt) % attack_count;
        if (!PlayerHasAttack(player, PlayerRewardAttack(candidate))) {
            attack_id = candidate;
            break;
        }
    }
    tower->rewards[0] = (TowerReward){REWARD_ATTACK, attack_id, 0};
    tower->rewards[1] = (TowerReward){REWARD_ITEM, (int)(NextRandom(&random) % ITEM_COUNT), 1};
    tower->rewards[2] = (TowerReward){NextRandom(&random) % 2u ? REWARD_HP : REWARD_DAMAGE, 0, 4};
    tower->seed = random;
}

void TowerRollShop(Tower *tower)
{
    unsigned int random = tower->seed;

    for (int index = 0; index < 3; index++) {
        tower->shop_items[index] = (ItemId)(NextRandom(&random) % ITEM_COUNT);
        tower->shop_prices[index] = BALANCE_SHOP_BASE_PRICE +
                                    (int)(NextRandom(&random) % BALANCE_SHOP_PRICE_VARIATION);
    }
    tower->seed = random;
}

bool TowerShouldRest(int completed_floor) { return completed_floor == 3 || completed_floor == 9; }
bool TowerShouldShop(int completed_floor) { return completed_floor == 6; }
