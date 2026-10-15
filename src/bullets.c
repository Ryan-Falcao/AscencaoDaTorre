#include "bullets.h"

#include "input.h"
#include "ui.h"
#include "sprites.h"
#include "balance.h"
#include "raymath.h"

#include <math.h>
#include <string.h>

#define AGGRESSION_LOW_MAX 33
#define AGGRESSION_HIGH_MIN 67
#define HIGH_AGGRESSION_SPEED_MULTIPLIER 1.15f
#define HIGH_AGGRESSION_DAMAGE_MULTIPLIER 1.25f
#define DEFENSE_START_GRACE_SECONDS 0.35f
#define SPIRAL_SAFE_SPAWN_RADIUS 34.0f

static const BulletPattern PATTERNS[PATTERN_COUNT] = {
    [PATTERN_RAIN] = {
        6.0f, 300.0f, 154.0f, 0.34f, 2, 106.0f, 6.0f, 0.0f, 0.0f,
        TYPE_WATER, 3, 1, SPAWN_RAIN
    },
    [PATTERN_SIDE_WAVE] = {
        7.0f, 320.0f, 168.0f, 0.88f, 7, 126.0f, 7.0f, 0.0f, 42.0f,
        TYPE_SHADOW, 4, 2, SPAWN_SIDE_WAVE
    },
    [PATTERN_SPIRAL] = {
        7.0f, 286.0f, 172.0f, 0.42f, 4, 92.0f, 6.0f, 1.57f, 0.0f,
        TYPE_FLAME, 3, 3, SPAWN_SPIRAL
    },
    [PATTERN_FAST_RAIN] = {
        6.0f, 300.0f, 154.0f, 0.24f, 3, 135.0f, 5.0f, 0.0f, 0.0f,
        TYPE_LIGHT, 4, 3, SPAWN_RAIN
    },
    [PATTERN_WIDE_WAVE] = {
        7.0f, 330.0f, 180.0f, 0.62f, 9, 145.0f, 6.0f, 0.0f, 52.0f,
        TYPE_PLANT, 4, 2, SPAWN_SIDE_WAVE
    },
    [PATTERN_TIGHT_SPIRAL] = {
        7.0f, 270.0f, 160.0f, 0.30f, 5, 120.0f, 5.0f, 1.25f, 0.0f,
        TYPE_SHADOW, 5, 3, SPAWN_SPIRAL
    },
    [PATTERN_PETAL_ORBIT] = {
        6.5f, 300.0f, 175.0f, 0.36f, 6, 82.0f, 5.0f, 1.05f, 0.0f,
        TYPE_PLANT, 4, 2, SPAWN_SPIRAL
    },
    [PATTERN_BOUNCE_ORBS] = {
        6.5f, 310.0f, 170.0f, 0.55f, 4, 116.0f, 7.0f, 0.75f, 0.0f,
        TYPE_WATER, 4, 2, SPAWN_SPIRAL
    },
    [PATTERN_MOVING_WALL] = {
        7.0f, 335.0f, 180.0f, 0.72f, 10, 130.0f, 6.0f, 0.0f, 58.0f,
        TYPE_NEUTRAL, 5, 3, SPAWN_SIDE_WAVE
    },
    [PATTERN_SPLIT_SHOTS] = {
        6.5f, 300.0f, 170.0f, 0.30f, 3, 138.0f, 5.0f, 0.48f, 0.0f,
        TYPE_FLAME, 5, 3, SPAWN_RAIN
    },
    [PATTERN_CROSS_LASERS] = {
        6.0f, 320.0f, 170.0f, 0.90f, 8, 158.0f, 5.0f, 0.0f, 48.0f,
        TYPE_LIGHT, 6, 3, SPAWN_SIDE_WAVE
    },
    [PATTERN_PAUSE_DARTS] = {
        6.5f, 290.0f, 165.0f, 0.46f, 5, 72.0f, 6.0f, 1.26f, 0.0f,
        TYPE_SHADOW, 5, 2, SPAWN_SPIRAL
    },
    [PATTERN_RING_BURST] = {
        6.0f, 300.0f, 170.0f, 0.58f, 8, 110.0f, 5.0f, 0.78f, 0.0f,
        TYPE_LIGHT, 5, 2, SPAWN_SPIRAL
    },
    [PATTERN_DIAGONAL_RAIN] = {
        6.5f, 305.0f, 175.0f, 0.28f, 4, 150.0f, 5.0f, 0.0f, 0.0f,
        TYPE_FLAME, 4, 2, SPAWN_RAIN
    },
    [PATTERN_CHASE_WAVE] = {
        7.0f, 320.0f, 176.0f, 0.66f, 8, 152.0f, 6.0f, 0.0f, 46.0f,
        TYPE_WATER, 5, 3, SPAWN_SIDE_WAVE
    },
    [PATTERN_PINWHEEL] = {
        7.0f, 285.0f, 165.0f, 0.34f, 6, 126.0f, 5.0f, 1.05f, 0.0f,
        TYPE_PLANT, 5, 3, SPAWN_SPIRAL
    },
    [PATTERN_GRAVITY_FLIP] = {
        8.0f, 300.0f, 175.0f, 99.0f, 1, 0.0f, 1.0f, 0.0f, 0.0f,
        TYPE_PLANT, BALANCE_GRAVITY_DAMAGE, 3, SPAWN_SPIRAL
    },
    [PATTERN_SHADOW_BLOOM] = {
        7.0f, 300.0f, 175.0f, 99.0f, 1, 0.0f, 1.0f, 0.0f, 0.0f,
        TYPE_SHADOW, BALANCE_SHADOW_BLOOM_DAMAGE, 3, SPAWN_SPIRAL
    }
};

static Rectangle LerpRectangle(Rectangle from, Rectangle to, float amount)
{
    return (Rectangle){
        from.x + (to.x - from.x) * amount,
        from.y + (to.y - from.y) * amount,
        from.width + (to.width - from.width) * amount,
        from.height + (to.height - from.height) * amount
    };
}

static void ClearBullets(DefenseState *defense)
{
    for (int bullet_index = 0; bullet_index < MAX_BULLETS; bullet_index++) {
        defense->bullets[bullet_index].active = false;
    }
}

static Rectangle GravityThornRect(const DefenseState *defense)
{
    const Rectangle box = defense->box;
    const float depth = BALANCE_GRAVITY_THORN_DEPTH;
    switch (defense->gravity_wall) {
        case 0: return (Rectangle){box.x, box.y, box.width, depth};
        case 1: return (Rectangle){box.x, box.y + box.height - depth, box.width, depth};
        case 2: return (Rectangle){box.x, box.y, depth, box.height};
        default: return (Rectangle){box.x + box.width - depth, box.y, depth, box.height};
    }
}

static void StartGravityCycle(DefenseState *defense)
{
    defense->gravity_wall = GetRandomValue(0, 3);
    defense->gravity_thorns_created = false;
    defense->special_timer = 0.0f;
}

static void StartShadowBloom(DefenseState *defense)
{
    const int count = defense->gravity_repeats > 2 ? 7 : 5;
    for (int index = 0; index < count; index++) {
        Vector2 position;
        do {
            position = (Vector2){(float)GetRandomValue((int)defense->box.x + 34,
                                                         (int)(defense->box.x + defense->box.width - 34)),
                                 (float)GetRandomValue((int)defense->box.y + 34,
                                                         (int)(defense->box.y + defense->box.height - 34))};
        } while (Vector2Distance(position, defense->soul_position) < 72.0f);
        HazardsAddCircle(&defense->hazards, position,
                         BALANCE_SHADOW_BLOOM_RADIUS + (float)(index % 2) * 8.0f,
                         BALANCE_SHADOW_BLOOM_GROW_TIME + (float)index * BALANCE_SHADOW_BLOOM_STAGGER,
                         0.10f, 0.15f, TYPE_SHADOW, BALANCE_SHADOW_BLOOM_DAMAGE, true);
    }
}

static void StartSpecialPattern(DefenseState *defense)
{
    defense->special_timer = 0.0f;
    if (defense->pattern_id == PATTERN_GRAVITY_FLIP) StartGravityCycle(defense);
    else if (defense->pattern_id == PATTERN_SHADOW_BLOOM) StartShadowBloom(defense);
}

static Bullet *SpawnBullet(DefenseState *defense, Vector2 position, Vector2 velocity)
{
    for (int bullet_index = 0; bullet_index < MAX_BULLETS; bullet_index++) {
        Bullet *bullet = &defense->bullets[bullet_index];

        if (!bullet->active) {
            *bullet = (Bullet){
                true,
                position,
                velocity,
                defense->pattern.hitbox_radius,
                defense->pattern.type,
                defense->pattern.damage,
                0.0f,
                false
            };
            return bullet;
        }
    }
    return NULL;
}

static void SpawnPatternShot(DefenseState *defense)
{
    const Rectangle box = defense->box;
    const BulletPattern *pattern = &defense->pattern;

    if (pattern->spawn_mode == SPAWN_RAIN) {
        for (int bullet_index = 0; bullet_index < pattern->bullets_per_spawn; bullet_index++) {
            const float x = (float)GetRandomValue((int)box.x + 8, (int)(box.x + box.width - 8));
            const float diagonal = defense->pattern_id == PATTERN_DIAGONAL_RAIN ?
                (bullet_index % 2 == 0 ? pattern->speed * 0.42f : -pattern->speed * 0.42f) : 0.0f;
            SpawnBullet(defense, (Vector2){x, box.y - pattern->radius},
                        (Vector2){diagonal, pattern->speed});
        }
    } else if (pattern->spawn_mode == SPAWN_SIDE_WAVE) {
        const int gap = defense->spawn_count % pattern->bullets_per_spawn;
        const float spacing = box.height / (float)(pattern->bullets_per_spawn + 1);

        for (int bullet_index = 0; bullet_index < pattern->bullets_per_spawn; bullet_index++) {
            if (bullet_index == gap || bullet_index == gap + 1) continue;
            SpawnBullet(defense,
                        (Vector2){box.x - pattern->radius,
                                  box.y + spacing * (float)(bullet_index + 1)},
                        (Vector2){pattern->speed, 0.0f});
        }
    } else if (pattern->spawn_mode == SPAWN_SPIRAL) {
        const Vector2 center = {box.x + box.width * 0.5f, box.y + box.height * 0.5f};

        for (int bullet_index = 0; bullet_index < pattern->bullets_per_spawn; bullet_index++) {
            const float angle = defense->spiral_angle + pattern->angle_spread * (float)bullet_index;
            const Vector2 velocity = {cosf(angle) * pattern->speed, sinf(angle) * pattern->speed};
            const Vector2 position = {
                center.x + cosf(angle) * SPIRAL_SAFE_SPAWN_RADIUS,
                center.y + sinf(angle) * SPIRAL_SAFE_SPAWN_RADIUS
            };

            /* The soul begins in the centre: never spawn a spiral bullet inside its hitbox. */
            SpawnBullet(defense, position, velocity);
        }
        defense->spiral_angle += 0.48f;
    }

    defense->spawn_count++;
}

void DefenseInit(DefenseState *defense)
{
    memset(defense, 0, sizeof(*defense));
    defense->phase = DEFENSE_FINISHED;
    defense->soul_radius = 4.0f;
}

void DefenseStart(DefenseState *defense, BulletPatternId pattern_id, Rectangle menu_box,
                  int aggression, int damage_bonus, float damage_multiplier, float speed_multiplier)
{
    if (pattern_id < PATTERN_RAIN || pattern_id >= PATTERN_COUNT) pattern_id = PATTERN_RAIN;

    ClearBullets(defense);
    HazardsClear(&defense->hazards);
    defense->pattern = PATTERNS[pattern_id];
    defense->pattern_id = pattern_id;
    if (defense->pattern.visual_scale <= 0.0f) defense->pattern.visual_scale = 1.0f;
    if (defense->pattern.hitbox_radius <= 0.0f) defense->pattern.hitbox_radius = defense->pattern.radius;
    /* Defaults keep old data compatible while still giving every new pattern a clear look. */
    defense->pattern.rotate = pattern_id == PATTERN_DIAGONAL_RAIN ||
                               pattern_id == PATTERN_PAUSE_DARTS ||
                               pattern_id == PATTERN_CHASE_WAVE;
    defense->pattern.trail = pattern_id == PATTERN_SPIRAL || pattern_id == PATTERN_TIGHT_SPIRAL ||
                             pattern_id == PATTERN_PETAL_ORBIT || pattern_id == PATTERN_PINWHEEL;
    if (pattern_id == PATTERN_CROSS_LASERS || pattern_id == PATTERN_MOVING_WALL) {
        defense->pattern.visual_scale = 1.35f;
    }
    defense->pattern.damage += damage_bonus;
    defense->pattern.damage = (int)ceilf((float)defense->pattern.damage * damage_multiplier);
    defense->pattern.speed *= speed_multiplier;
    if (aggression >= AGGRESSION_HIGH_MIN) {
        defense->pattern.speed *= HIGH_AGGRESSION_SPEED_MULTIPLIER;
        defense->pattern.damage = (int)ceilf((float)defense->pattern.damage *
                                             HIGH_AGGRESSION_DAMAGE_MULTIPLIER);
    } else if (aggression <= AGGRESSION_LOW_MAX) {
        defense->pattern.speed *= 0.90f;
    }
    defense->start_box = menu_box;
    defense->target_box = (Rectangle){
        (640.0f - defense->pattern.box_width) * 0.5f,
        148.0f,
        defense->pattern.box_width,
        defense->pattern.box_height
    };
    defense->box = menu_box;
    defense->soul_position = (Vector2){
        defense->target_box.x + defense->target_box.width * 0.5f,
        defense->target_box.y + defense->target_box.height * 0.5f
    };
    for (int index = 0; index < 6; index++) defense->soul_trail[index] = defense->soul_position;
    /* A short readable gap after the box opens prevents an unavoidable first hit. */
    defense->invincibility_timer = DEFENSE_START_GRACE_SECONDS + 0.25f;
    defense->phase_timer = 0.0f;
    defense->pattern_timer = 0.0f;
    defense->spawn_timer = -DEFENSE_START_GRACE_SECONDS;
    defense->spiral_angle = 0.0f;
    defense->spawn_count = 0;
    defense->gravity_repeats = 2;
    defense->phase = DEFENSE_ENTERING;
}

int BulletPatternDifficulty(BulletPatternId pattern_id)
{
    if (pattern_id < PATTERN_RAIN || pattern_id >= PATTERN_COUNT) return 1;
    return PATTERNS[pattern_id].difficulty;
}

void DefenseResizeBox(DefenseState *defense, float width, float height)
{
    defense->target_box.width = width;
    defense->target_box.height = height;
    defense->target_box.x = (640.0f - width) * 0.5f;
}

static void UpdateSoul(DefenseState *defense, float delta_time)
{
    Vector2 movement = {0.0f, 0.0f};
    const float speed = 135.0f;

    if (input_left_down()) movement.x -= 1.0f;
    if (input_right_down()) movement.x += 1.0f;
    if (input_up_down()) movement.y -= 1.0f;
    if (input_down_down()) movement.y += 1.0f;
    if (movement.x != 0.0f && movement.y != 0.0f) movement = Vector2Scale(movement, 0.7071f);

    for (int index = 5; index > 0; index--) defense->soul_trail[index] = defense->soul_trail[index - 1];
    defense->soul_trail[0] = defense->soul_position;
    defense->soul_position = Vector2Add(defense->soul_position, Vector2Scale(movement, speed * delta_time));
    if (defense->pattern_id == PATTERN_GRAVITY_FLIP && !defense->gravity_thorns_created) {
        const float pull = BALANCE_GRAVITY_STRENGTH * delta_time;
        if (defense->gravity_wall == 0) defense->soul_position.y -= pull;
        else if (defense->gravity_wall == 1) defense->soul_position.y += pull;
        else if (defense->gravity_wall == 2) defense->soul_position.x -= pull;
        else defense->soul_position.x += pull;
    }
    defense->soul_position.x = Clamp(defense->soul_position.x, defense->box.x + defense->soul_radius,
                                     defense->box.x + defense->box.width - defense->soul_radius);
    defense->soul_position.y = Clamp(defense->soul_position.y, defense->box.y + defense->soul_radius,
                                     defense->box.y + defense->box.height - defense->soul_radius);
}

static void UpdateSpecialPattern(DefenseState *defense, PlayerStats *player, float delta_time)
{
    if (defense->pattern_id == PATTERN_GRAVITY_FLIP) {
        const float escape = defense->gravity_repeats > 2 ? BALANCE_GRAVITY_ESCAPE_TIME_PHASE_TWO :
                                                            BALANCE_GRAVITY_ESCAPE_TIME;
        defense->special_timer += delta_time;
        if (!defense->gravity_thorns_created && defense->special_timer >= escape) {
            HazardsAddRect(&defense->hazards, GravityThornRect(defense), 0.0f,
                           BALANCE_GRAVITY_THORN_ACTIVE_TIME, BALANCE_GRAVITY_THORN_FADE_TIME, defense->pattern.type,
                           defense->pattern.damage, true);
            defense->gravity_thorns_created = true;
        }
        if (defense->gravity_thorns_created && defense->special_timer >=
            escape + BALANCE_GRAVITY_THORN_ACTIVE_TIME + BALANCE_GRAVITY_THORN_FADE_TIME) {
            defense->gravity_repeat++;
            if (defense->gravity_repeat < defense->gravity_repeats) StartGravityCycle(defense);
            else defense->pattern_timer = defense->pattern.duration;
        }
    }
    const int hp_before = player->hp;
    HazardsUpdate(&defense->hazards, player, defense->soul_position, defense->soul_radius, delta_time,
                  1.0f, defense->debug_invincible || defense->invincibility_timer > 0.0f);
    if (player->hp < hp_before) {
        defense->last_damage = hp_before - player->hp;
        defense->last_damage_multiplier = type_effectiveness(defense->pattern.type, player->defense_type);
        defense->damage_popup_timer = 0.75f;
        defense->invincibility_timer = 1.0f;
    }
}

static void UpdateBullets(DefenseState *defense, PlayerStats *player, float delta_time)
{
    for (int bullet_index = 0; bullet_index < MAX_BULLETS; bullet_index++) {
        Bullet *bullet = &defense->bullets[bullet_index];

        if (!bullet->active) continue;

        bullet->lifetime += delta_time;
        if (defense->pattern_id == PATTERN_BOUNCE_ORBS) {
            const float next_x = bullet->position.x + bullet->velocity.x * delta_time;
            const float next_y = bullet->position.y + bullet->velocity.y * delta_time;
            if (next_x < defense->box.x + bullet->radius ||
                next_x > defense->box.x + defense->box.width - bullet->radius) bullet->velocity.x *= -1.0f;
            if (next_y < defense->box.y + bullet->radius ||
                next_y > defense->box.y + defense->box.height - bullet->radius) bullet->velocity.y *= -1.0f;
        } else if (defense->pattern_id == PATTERN_PAUSE_DARTS) {
            if (bullet->lifetime > 0.42f && bullet->lifetime < 0.82f) {
                bullet->velocity = Vector2Scale(bullet->velocity, 0.82f);
            } else if (bullet->lifetime >= 0.82f) {
                Vector2 direction = Vector2Subtract(defense->soul_position, bullet->position);
                if (Vector2Length(direction) > 0.01f) {
                    direction = Vector2Normalize(direction);
                    bullet->velocity = Vector2Scale(direction, defense->pattern.speed * 1.65f);
                }
            }
        } else if (defense->pattern_id == PATTERN_CHASE_WAVE) {
            Vector2 direction = Vector2Subtract(defense->soul_position, bullet->position);
            if (Vector2Length(direction) > 0.01f) {
                direction = Vector2Normalize(direction);
                bullet->velocity = Vector2Lerp(bullet->velocity,
                    Vector2Scale(direction, defense->pattern.speed), 0.025f);
            }
        } else if (defense->pattern_id == PATTERN_PETAL_ORBIT) {
            const Vector2 center = {defense->box.x + defense->box.width * 0.5f,
                                    defense->box.y + defense->box.height * 0.5f};
            Vector2 radial = Vector2Subtract(bullet->position, center);
            if (Vector2Length(radial) > 0.01f) {
                radial = Vector2Normalize(radial);
                bullet->velocity = Vector2Scale((Vector2){-radial.y, radial.x}, defense->pattern.speed);
            }
        }
        bullet->position = Vector2Add(bullet->position, Vector2Scale(bullet->velocity, delta_time));

        if (defense->pattern_id == PATTERN_SPLIT_SHOTS && !bullet->split_done &&
            bullet->lifetime >= 0.65f) {
            const float base_angle = atan2f(bullet->velocity.y, bullet->velocity.x);
            bullet->split_done = true;
            for (int shard = -1; shard <= 1; shard++) {
                const float angle = base_angle + (float)shard * 0.42f;
                Bullet *shard_bullet = SpawnBullet(defense, bullet->position,
                    (Vector2){cosf(angle) * defense->pattern.speed,
                              sinf(angle) * defense->pattern.speed});
                if (shard_bullet != NULL) shard_bullet->split_done = true;
            }
            bullet->active = false;
            continue;
        }
        if (bullet->position.x < defense->box.x - 24.0f ||
            bullet->position.x > defense->box.x + defense->box.width + 24.0f ||
            bullet->position.y < defense->box.y - 24.0f ||
            bullet->position.y > defense->box.y + defense->box.height + 24.0f) {
            bullet->active = false;
            continue;
        }

        const float soul_distance = Vector2Distance(defense->soul_position, bullet->position);
        if (soul_distance <= bullet->radius + defense->soul_radius + 5.0f &&
            soul_distance > bullet->radius + defense->soul_radius) {
            defense->graze_popup_timer = 0.22f;
            defense->graze_count++;
        }
        if (!defense->debug_invincible && defense->invincibility_timer <= 0.0f &&
            CheckCollisionCircles(defense->soul_position, defense->soul_radius,
                                  bullet->position, bullet->radius)) {
            const float multiplier = type_effectiveness(bullet->type, player->defense_type);
            int damage = (int)ceilf((float)bullet->damage * multiplier *
                                    player->incoming_damage_multiplier);

            if (damage < 1) damage = 1;
            player->hp -= damage;
            if (player->hp < 0) player->hp = 0;
            defense->invincibility_timer = 1.0f;
            defense->last_damage = damage;
            defense->last_damage_multiplier = multiplier;
            defense->damage_popup_timer = 0.75f;
        }
    }
}

DefenseResult DefenseUpdate(DefenseState *defense, PlayerStats *player, float delta_time)
{
    defense->phase_timer += delta_time;
    if (defense->invincibility_timer > 0.0f) defense->invincibility_timer -= delta_time;
    if (defense->damage_popup_timer > 0.0f) defense->damage_popup_timer -= delta_time;
    if (defense->graze_popup_timer > 0.0f) defense->graze_popup_timer -= delta_time;

    if (defense->phase == DEFENSE_ENTERING) {
        const float progress = Clamp(defense->phase_timer / 0.25f, 0.0f, 1.0f);
        defense->box = LerpRectangle(defense->start_box, defense->target_box, progress);
        if (progress >= 1.0f) {
            defense->phase = DEFENSE_ACTIVE;
            defense->phase_timer = 0.0f;
            StartSpecialPattern(defense);
        }
    } else if (defense->phase == DEFENSE_ACTIVE) {
        defense->pattern_timer += delta_time;
        defense->spawn_timer += delta_time;
        UpdateSoul(defense, delta_time);
        if (defense->pattern_id == PATTERN_GRAVITY_FLIP || defense->pattern_id == PATTERN_SHADOW_BLOOM) {
            UpdateSpecialPattern(defense, player, delta_time);
        }
        while (defense->pattern_id != PATTERN_GRAVITY_FLIP && defense->pattern_id != PATTERN_SHADOW_BLOOM &&
               defense->spawn_timer >= defense->pattern.spawn_interval) {
            defense->spawn_timer -= defense->pattern.spawn_interval;
            SpawnPatternShot(defense);
        }
        UpdateBullets(defense, player, delta_time);

        if (player->hp == 0) {
            ClearBullets(defense);
            HazardsClear(&defense->hazards);
            defense->phase = DEFENSE_FINISHED;
            return DEFENSE_PLAYER_DEFEATED;
        }
        if (defense->pattern_timer >= defense->pattern.duration) {
            ClearBullets(defense);
            HazardsClear(&defense->hazards);
            defense->phase = DEFENSE_EXITING;
            defense->phase_timer = 0.0f;
        }
    } else if (defense->phase == DEFENSE_EXITING) {
        const float progress = Clamp(defense->phase_timer / 0.25f, 0.0f, 1.0f);
        defense->box = LerpRectangle(defense->target_box, defense->start_box, progress);
        if (progress >= 1.0f) {
            defense->phase = DEFENSE_FINISHED;
            return DEFENSE_COMPLETE;
        }
    }

    return DEFENSE_RUNNING;
}

void DefenseDraw(const DefenseState *defense, bool draw_effects, const SpriteSystem *sprites)
{
    if (sprite_get(sprites, SPRITE_ARENA_BG) != NULL) {
        SpriteDrawFit(sprites, SPRITE_ARENA_BG, defense->box, WHITE);
    } else {
        DrawRectangleRec(defense->box, BLACK);
    }
    if (sprite_get(sprites, SPRITE_ARENA_BORDER) != NULL) {
        SpriteDrawFit(sprites, SPRITE_ARENA_BORDER, defense->box, WHITE);
    } else {
        DrawRectangleLinesEx(defense->box, 4.0f, RAYWHITE);
    }

    if (defense->phase == DEFENSE_ACTIVE) {
        HazardsDraw(&defense->hazards, !draw_effects, sprites, sprites->debug_bounds);
        if (defense->pattern_timer < 0.40f && defense->pattern.radius >= 6.0f) {
            const Vector2 warning = {defense->box.x + defense->box.width * 0.5f,
                                     defense->box.y + defense->box.height * 0.5f};
            DrawCircleLines((int)warning.x, (int)warning.y, 18, (Color){245, 90, 82, 255});
            DrawText("!", (int)warning.x - 3, (int)warning.y - 10, 18, (Color){245, 90, 82, 255});
            if (sprite_get(sprites, SPRITE_WARN_DANGER) != NULL) {
                SpriteDrawCentered(sprites, SPRITE_WARN_DANGER, warning, WHITE,
                                   defense->pattern_timer, sprites->debug_bounds);
            }
        }
        for (int bullet_index = 0; bullet_index < MAX_BULLETS; bullet_index++) {
            const Bullet *bullet = &defense->bullets[bullet_index];
            if (!bullet->active) continue;
            const SpriteId pattern_sprite = SpriteBulletPatternId(defense->pattern_id);
            const SpriteId sprite_id = sprite_get(sprites, pattern_sprite) != NULL ? pattern_sprite :
                                       SpriteBulletId(bullet->type);
            if (sprite_get(sprites, sprite_id) != NULL) {
                const float angle = defense->pattern.rotate ?
                    atan2f(bullet->velocity.y, bullet->velocity.x) * 57.2958f : 0.0f;
                SpriteDrawCenteredRotated(sprites, sprite_id, bullet->position, WHITE,
                                          bullet->lifetime, angle, defense->pattern.visual_scale,
                                          sprites->debug_bounds);
                DrawCircleLines((int)bullet->position.x, (int)bullet->position.y,
                                bullet->radius + 1.0f, Fade(type_color(bullet->type), 0.65f));
            } else {
                DrawCircleV(bullet->position, bullet->radius, type_color(bullet->type));
            }
            if (defense->pattern.trail && draw_effects) {
                DrawCircleV((Vector2){bullet->position.x - bullet->velocity.x * 0.05f,
                                      bullet->position.y - bullet->velocity.y * 0.05f},
                            bullet->radius * 0.55f, Fade(type_color(bullet->type), 0.35f));
            }
        }

        if (defense->damage_popup_timer > 0.0f) {
            const int font_size = defense->last_damage_multiplier >= 2.0f ? 25 : 19;
            const Color color = defense->last_damage_multiplier >= 2.0f ? RED : MAROON;
            DrawText(TextFormat("-%d", defense->last_damage), (int)defense->soul_position.x + 9,
                     (int)defense->soul_position.y - 18, font_size, color);
        }
        if (draw_effects && defense->graze_popup_timer > 0.0f) {
            DrawText("RASPOU", (int)defense->soul_position.x + 10,
                     (int)defense->soul_position.y + 9, 10, (Color){125, 220, 205, 255});
        }

        if (draw_effects) {
            for (int index = 5; index > 0; index--) {
                const unsigned char alpha = (unsigned char)(index * 18);
                DrawCircleV(defense->soul_trail[index], 3.0f, (Color){231, 87, 112, alpha});
            }
        }
        if (defense->invincibility_timer <= 0.0f || ((int)(defense->invincibility_timer * 14.0f) % 2 == 0)) {
            const Vector2 soul = defense->soul_position;
            const Color heart = {231, 87, 112, 255};
            if (sprite_get(sprites, SPRITE_SOUL) != NULL) {
                SpriteDrawCentered(sprites, SPRITE_SOUL, soul, heart, defense->pattern_timer,
                                   sprites->debug_bounds);
            } else {
                DrawCircle((int)soul.x - 3, (int)soul.y - 2, 4, heart);
                DrawCircle((int)soul.x + 3, (int)soul.y - 2, 4, heart);
                DrawTriangle((Vector2){soul.x - 7, soul.y}, (Vector2){soul.x + 7, soul.y},
                             (Vector2){soul.x, soul.y + 8}, heart);
            }
        }
        if (draw_effects && defense->damage_popup_timer > 0.0f) {
            const float radius = 9.0f + (0.75f - defense->damage_popup_timer) * 18.0f;
            DrawCircleLines((int)defense->soul_position.x, (int)defense->soul_position.y,
                            radius, Fade(RED, 0.75f));
            if (sprite_get(sprites, SPRITE_FX_HIT_SOUL) != NULL) {
                SpriteDrawCentered(sprites, SPRITE_FX_HIT_SOUL, defense->soul_position, WHITE,
                                   defense->pattern_timer, sprites->debug_bounds);
            }
        }
    }
}
