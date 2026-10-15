#ifndef BULLETS_H
#define BULLETS_H

#include "raylib.h"

#include "player.h"

#define MAX_BULLETS 512

typedef enum {
    PATTERN_RAIN,
    PATTERN_SIDE_WAVE,
    PATTERN_SPIRAL,
    PATTERN_FAST_RAIN,
    PATTERN_WIDE_WAVE,
    PATTERN_TIGHT_SPIRAL,
    PATTERN_COUNT
} BulletPatternId;

typedef enum {
    SPAWN_RAIN,
    SPAWN_SIDE_WAVE,
    SPAWN_SPIRAL
} BulletSpawnMode;

typedef enum {
    DEFENSE_ENTERING,
    DEFENSE_ACTIVE,
    DEFENSE_EXITING,
    DEFENSE_FINISHED
} DefensePhase;

typedef struct {
    bool active;
    Vector2 position;
    Vector2 velocity;
    float radius;
    Type type;
    int damage;
    float lifetime;
} Bullet;

typedef struct {
    float duration;
    float box_width;
    float box_height;
    float spawn_interval;
    int bullets_per_spawn;
    float speed;
    float radius;
    float angle_spread;
    float gap_size;
    Type type;
    int damage;
    int difficulty;
    BulletSpawnMode spawn_mode;
} BulletPattern;

typedef struct {
    Bullet bullets[MAX_BULLETS];
    BulletPattern pattern;
    Rectangle box;
    Rectangle start_box;
    Rectangle target_box;
    Vector2 soul_position;
    float soul_radius;
    float invincibility_timer;
    int last_damage;
    float last_damage_multiplier;
    float damage_popup_timer;
    float phase_timer;
    float pattern_timer;
    float spawn_timer;
    float spiral_angle;
    int spawn_count;
    DefensePhase phase;
} DefenseState;

typedef enum {
    DEFENSE_RUNNING,
    DEFENSE_COMPLETE,
    DEFENSE_PLAYER_DEFEATED
} DefenseResult;

void DefenseInit(DefenseState *defense);
void DefenseStart(DefenseState *defense, BulletPatternId pattern_id, Rectangle menu_box,
                  int aggression, int damage_bonus);
void DefenseResizeBox(DefenseState *defense, float width, float height);
DefenseResult DefenseUpdate(DefenseState *defense, PlayerStats *player, float delta_time);
void DefenseDraw(const DefenseState *defense);
int BulletPatternDifficulty(BulletPatternId pattern_id);

#endif
