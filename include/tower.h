#ifndef TOWER_H
#define TOWER_H

#include "enemy.h"
#include "items.h"

#define TOWER_FLOORS 10
#define TOWER_REWARD_OPTIONS 3

typedef enum {
    REWARD_ATTACK,
    REWARD_ITEM,
    REWARD_HP,
    REWARD_DAMAGE
} TowerRewardKind;

typedef struct {
    TowerRewardKind kind;
    int id;
    int value;
} TowerReward;

typedef struct {
    int floor;
    unsigned int seed;
    EnemyId enemies[TOWER_FLOORS];
    int coins;
    int mercy_wins;
    int damage_wins;
    TowerReward rewards[TOWER_REWARD_OPTIONS];
    ItemId shop_items[3];
    int shop_prices[3];
} Tower;

void TowerInit(Tower *tower, unsigned int seed);
bool TowerIsBossFloor(int floor);
EnemyId TowerEnemyForFloor(const Tower *tower, int floor);
void TowerScaleEnemy(Enemy *enemy, int floor);
void TowerRollRewards(Tower *tower, const PlayerStats *player);
void TowerRollShop(Tower *tower);
bool TowerShouldRest(int completed_floor);
bool TowerShouldShop(int completed_floor);

#endif
