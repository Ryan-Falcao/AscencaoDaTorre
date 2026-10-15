#include "bullets.h"

#include "input.h"
#include "ui.h"
#include "raymath.h"

#include <math.h>
#include <string.h>

#define AGGRESSION_LOW_MAX 33
#define AGGRESSION_HIGH_MIN 67
#define HIGH_AGGRESSION_SPEED_MULTIPLIER 1.15f
#define HIGH_AGGRESSION_DAMAGE_MULTIPLIER 1.25f

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

static void SpawnBullet(DefenseState *defense, Vector2 position, Vector2 velocity)
{
    for (int bullet_index = 0; bullet_index < MAX_BULLETS; bullet_index++) {
        Bullet *bullet = &defense->bullets[bullet_index];

        if (!bullet->active) {
            *bullet = (Bullet){
                true,
                position,
                velocity,
                defense->pattern.radius,
                defense->pattern.type,
                defense->pattern.damage,
                0.0f
            };
            return;
        }
    }
}

static void SpawnPatternShot(DefenseState *defense)
{
    const Rectangle box = defense->box;
    const BulletPattern *pattern = &defense->pattern;

    if (pattern->spawn_mode == SPAWN_RAIN) {
        for (int bullet_index = 0; bullet_index < pattern->bullets_per_spawn; bullet_index++) {
            const float x = (float)GetRandomValue((int)box.x + 8, (int)(box.x + box.width - 8));
            SpawnBullet(defense, (Vector2){x, box.y - pattern->radius},
                        (Vector2){0.0f, pattern->speed});
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
            SpawnBullet(defense, center, velocity);
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
                  int aggression, int damage_bonus)
{
    if (pattern_id < PATTERN_RAIN || pattern_id >= PATTERN_COUNT) pattern_id = PATTERN_RAIN;

    ClearBullets(defense);
    defense->pattern = PATTERNS[pattern_id];
    defense->pattern.damage += damage_bonus;
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
    defense->invincibility_timer = 0.0f;
    defense->phase_timer = 0.0f;
    defense->pattern_timer = 0.0f;
    defense->spawn_timer = 0.0f;
    defense->spiral_angle = 0.0f;
    defense->spawn_count = 0;
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

    defense->soul_position = Vector2Add(defense->soul_position, Vector2Scale(movement, speed * delta_time));
    defense->soul_position.x = Clamp(defense->soul_position.x, defense->box.x + defense->soul_radius,
                                     defense->box.x + defense->box.width - defense->soul_radius);
    defense->soul_position.y = Clamp(defense->soul_position.y, defense->box.y + defense->soul_radius,
                                     defense->box.y + defense->box.height - defense->soul_radius);
}

static void UpdateBullets(DefenseState *defense, PlayerStats *player, float delta_time)
{
    for (int bullet_index = 0; bullet_index < MAX_BULLETS; bullet_index++) {
        Bullet *bullet = &defense->bullets[bullet_index];

        if (!bullet->active) continue;

        bullet->position = Vector2Add(bullet->position, Vector2Scale(bullet->velocity, delta_time));
        bullet->lifetime += delta_time;
        if (bullet->position.x < defense->box.x - 24.0f ||
            bullet->position.x > defense->box.x + defense->box.width + 24.0f ||
            bullet->position.y < defense->box.y - 24.0f ||
            bullet->position.y > defense->box.y + defense->box.height + 24.0f) {
            bullet->active = false;
            continue;
        }

        if (defense->invincibility_timer <= 0.0f &&
            CheckCollisionCircles(defense->soul_position, defense->soul_radius,
                                  bullet->position, bullet->radius)) {
            const float multiplier = type_effectiveness(bullet->type, player->defense_type);
            int damage = (int)ceilf((float)bullet->damage * multiplier);

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

    if (defense->phase == DEFENSE_ENTERING) {
        const float progress = Clamp(defense->phase_timer / 0.25f, 0.0f, 1.0f);
        defense->box = LerpRectangle(defense->start_box, defense->target_box, progress);
        if (progress >= 1.0f) {
            defense->phase = DEFENSE_ACTIVE;
            defense->phase_timer = 0.0f;
        }
    } else if (defense->phase == DEFENSE_ACTIVE) {
        defense->pattern_timer += delta_time;
        defense->spawn_timer += delta_time;
        UpdateSoul(defense, delta_time);
        while (defense->spawn_timer >= defense->pattern.spawn_interval) {
            defense->spawn_timer -= defense->pattern.spawn_interval;
            SpawnPatternShot(defense);
        }
        UpdateBullets(defense, player, delta_time);

        if (player->hp == 0) {
            ClearBullets(defense);
            defense->phase = DEFENSE_FINISHED;
            return DEFENSE_PLAYER_DEFEATED;
        }
        if (defense->pattern_timer >= defense->pattern.duration) {
            ClearBullets(defense);
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

void DefenseDraw(const DefenseState *defense)
{
    DrawRectangleRec(defense->box, BLACK);
    DrawRectangleLinesEx(defense->box, 4.0f, RAYWHITE);

    if (defense->phase == DEFENSE_ACTIVE) {
        for (int bullet_index = 0; bullet_index < MAX_BULLETS; bullet_index++) {
            const Bullet *bullet = &defense->bullets[bullet_index];
            if (bullet->active) DrawCircleV(bullet->position, bullet->radius, type_color(bullet->type));
        }

        if (defense->damage_popup_timer > 0.0f) {
            const int font_size = defense->last_damage_multiplier >= 2.0f ? 25 : 19;
            const Color color = defense->last_damage_multiplier >= 2.0f ? RED : MAROON;
            DrawText(TextFormat("-%d", defense->last_damage), (int)defense->soul_position.x + 9,
                     (int)defense->soul_position.y - 18, font_size, color);
        }

        if (defense->invincibility_timer <= 0.0f || ((int)(defense->invincibility_timer * 14.0f) % 2 == 0)) {
            const Vector2 soul = defense->soul_position;
            const Color heart = {231, 87, 112, 255};
            DrawCircle((int)soul.x - 3, (int)soul.y - 2, 4, heart);
            DrawCircle((int)soul.x + 3, (int)soul.y - 2, 4, heart);
            DrawTriangle((Vector2){soul.x - 7, soul.y}, (Vector2){soul.x + 7, soul.y},
                         (Vector2){soul.x, soul.y + 8}, heart);
        }
    }
}
