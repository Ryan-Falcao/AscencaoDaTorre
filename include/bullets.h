#ifndef BULLETS_H
#define BULLETS_H

#include "raylib.h"

#include "player.h"
#include "hazards.h"
typedef struct SpriteSystem SpriteSystem;

#define MAX_BULLETS 512

typedef enum {
    PATTERN_RAIN,
    PATTERN_SIDE_WAVE,
    PATTERN_SPIRAL,
    PATTERN_FAST_RAIN,
    PATTERN_WIDE_WAVE,
    PATTERN_TIGHT_SPIRAL,
    PATTERN_PETAL_ORBIT,
    PATTERN_BOUNCE_ORBS,
    PATTERN_MOVING_WALL,
    PATTERN_SPLIT_SHOTS,
    PATTERN_CROSS_LASERS,
    PATTERN_PAUSE_DARTS,
    PATTERN_RING_BURST,
    PATTERN_DIAGONAL_RAIN,
    PATTERN_CHASE_WAVE,
    PATTERN_PINWHEEL,
    PATTERN_GRAVITY_FLIP,
    PATTERN_SHADOW_BLOOM,
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
    bool split_done;
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
    bool rotate;
    float visual_scale;
    float hitbox_radius;
    bool trail;
    const char *bullet_skin;
} BulletPattern;

typedef struct {
    Bullet bullets[MAX_BULLETS];
    BulletPattern pattern;
    BulletPatternId pattern_id;
    Rectangle box;
    Rectangle start_box;
    Rectangle target_box;
    Vector2 soul_position;
    Vector2 soul_trail[6];
    float soul_radius;
    float invincibility_timer;
    int last_damage;
    float last_damage_multiplier;
    float damage_popup_timer;
    float graze_popup_timer;
    int graze_count;
    float phase_timer;
    float pattern_timer;
    float spawn_timer;
    float spiral_angle;
    int spawn_count;
    bool debug_invincible;
    HazardPool hazards;
    int gravity_wall;
    int gravity_repeat;
    int gravity_repeats;
    bool gravity_thorns_created;
    float special_timer;
    DefensePhase phase;
} DefenseState;

typedef enum {
    DEFENSE_RUNNING,
    DEFENSE_COMPLETE,
    DEFENSE_PLAYER_DEFEATED
} DefenseResult;

void DefenseInit(DefenseState *defense);
void DefenseStart(DefenseState *defense, BulletPatternId pattern_id, Rectangle menu_box,
                  int aggression, int damage_bonus, float damage_multiplier, float speed_multiplier);
void DefenseResizeBox(DefenseState *defense, float width, float height);
DefenseResult DefenseUpdate(DefenseState *defense, PlayerStats *player, float delta_time);
void DefenseDraw(const DefenseState *defense, bool draw_effects, const SpriteSystem *sprites);
int BulletPatternDifficulty(BulletPatternId pattern_id);

#endif
