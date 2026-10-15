#ifndef ENEMY_H
#define ENEMY_H

#include <stdbool.h>

#include "player.h"
#include "bullets.h"

#define MAX_ENEMY_PATTERNS 10
#define MAX_ENEMY_ACTIONS 4

typedef enum {
    ENEMY_DUST_MOTH,
    ENEMY_GLOW_FROG,
    ENEMY_CINDER_BIRD,
    ENEMY_MINT_SLIME,
    ENEMY_LANTERN_WISP,
    ENEMY_THORN_HARE,
    ENEMY_SHADE_CAT,
    ENEMY_PETAL_KNIGHT,
    ENEMY_BOSS_BELLKEEPER,
    ENEMY_BOSS_LAST_TOWER,
    ENEMY_COUNT
} EnemyId;

typedef enum {
    ENEMY_ACTION_ANALYZE,
    ENEMY_ACTION_SOCIAL
} EnemyActionEffect;

typedef struct {
    const char *name;
    const char *response;
    EnemyActionEffect effect;
    int mercy_delta;
    int aggression_delta;
    int max_uses;
    int cooldown_turns;
    float repeat_penalty;
    int max_aggression_required;
    bool has_type_bonus;
    Type bonus_type;
    float type_multiplier;
    int aggression_threshold;
    float calm_multiplier;
    float angry_multiplier;
} EnemyAction;

typedef struct {
    char name[24];
    int hp;
    int hp_max;
    EnemyId id;
    bool boss;
    bool phase_two;
    Type type;
    Type type2;
    bool has_type2;
    int aggression;
    int mercy;
    bool spareable;
    bool exhausted;
    bool analyzed;
    float hit_flash_timer;
    float hit_flash_strength;
    BulletPatternId pattern_ids[MAX_ENEMY_PATTERNS];
    int pattern_count;
    int next_pattern_index;
    char description[128];
    EnemyAction actions[MAX_ENEMY_ACTIONS];
    int action_uses[MAX_ENEMY_ACTIONS];
    int action_cooldowns[MAX_ENEMY_ACTIONS];
    int action_last_turn[MAX_ENEMY_ACTIONS];
    int last_action_index;
    int repeated_action_uses;
    int player_turn;
    int last_mercy_delta;
    int last_aggression_delta;
    int action_count;
    int projectile_damage_bonus;
    float projectile_damage_multiplier;
    int reward_bonus;
    StatusEffect status;
    int status_turns;
} Enemy;

Enemy EnemyCreate(EnemyId id);
void EnemyTakeDamage(Enemy *enemy, int damage, float effectiveness);
bool EnemyApplyAction(Enemy *enemy, int action_index, char *message, int message_size);
void EnemyRefreshSpareable(Enemy *enemy);
void EnemyAdvanceTurn(Enemy *enemy);
bool EnemyCanSpare(const Enemy *enemy);
bool EnemyActionAvailable(const Enemy *enemy, int action_index, char *reason, int reason_size);
void EnemyApplyStatus(Enemy *enemy, StatusEffect status, int turns);
bool EnemyAdvanceStatus(Enemy *enemy, int *damage);

#endif
