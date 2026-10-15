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

/* Bellkeeper special-defense attacks. Values are deliberately conservative: the escape strip is
   48px, less than one third of even the smallest 270px arena. */
#define BALANCE_GRAVITY_ESCAPE_TIME 1.15f
#define BALANCE_GRAVITY_ESCAPE_TIME_PHASE_TWO 0.85f
#define BALANCE_GRAVITY_STRENGTH 430.0f
#define BALANCE_GRAVITY_THORN_DEPTH 48.0f
#define BALANCE_GRAVITY_THORN_ACTIVE_TIME 0.65f
#define BALANCE_GRAVITY_THORN_FADE_TIME 0.12f
#define BALANCE_GRAVITY_DAMAGE 6
#define BALANCE_SHADOW_BLOOM_GROW_TIME 1.0f
#define BALANCE_SHADOW_BLOOM_STAGGER 0.13f
#define BALANCE_SHADOW_BLOOM_RADIUS 56.0f
#define BALANCE_SHADOW_BLOOM_DAMAGE 6

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
