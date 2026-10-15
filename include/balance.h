#ifndef BALANCE_H
#define BALANCE_H

#include "save.h"

/* All numbers that alter the difficulty of a run live here. */
#define BALANCE_COMMON_HP_PER_FLOOR 3
#define BALANCE_BOSS_HP_PER_FLOOR 7
#define BALANCE_BOSS_PRESSURE 0.16f
#define BALANCE_PROJECTILE_BONUS_EVERY 3
#define BALANCE_SHOP_BASE_PRICE 5
#define BALANCE_SHOP_PRICE_VARIATION 7
#define BALANCE_COMMON_COIN_BONUS 0
#define BALANCE_BOSS_COIN_BONUS 8
#define BALANCE_BOSS_XP_BONUS 10

static inline float BalanceIncomingDamageMultiplier(GameDifficulty difficulty)
{
    static const float values[] = {0.75f, 1.00f, 1.25f};
    return values[difficulty < DIFFICULTY_CALM || difficulty > DIFFICULTY_HARD
                      ? DIFFICULTY_NORMAL : difficulty];
}

static inline int BalanceQteDifficulty(int floor, GameDifficulty difficulty)
{
    const int offset = difficulty == DIFFICULTY_CALM ? -1 :
                       (difficulty == DIFFICULTY_HARD ? 1 : 0);
    const int value = floor + offset;
    return value < 1 ? 1 : value;
}

/* Keeps floors 1-4 threatening while their shorter fights still preserve 60-80% HP for good play. */
static inline float BalanceFloorDamageMultiplier(int floor)
{
    static const float early_values[] = {3.0f, 2.3f, 1.7f, 1.7f};
    return floor >= 1 && floor <= 4 ? early_values[floor - 1] : 1.0f;
}

#endif
