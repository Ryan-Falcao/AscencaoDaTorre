#include "hazards.h"

#include "sprites.h"
#include "types.h"

#include <math.h>
#include <string.h>

void HazardsInit(HazardPool *pool) { memset(pool, 0, sizeof(*pool)); }
void HazardsClear(HazardPool *pool) { memset(pool, 0, sizeof(*pool)); }

static Hazard *HazardsNext(HazardPool *pool)
{
    for (int index = 0; index < MAX_HAZARDS; index++) if (!pool->hazards[index].active) return &pool->hazards[index];
    return NULL;
}

static bool HazardAdd(HazardPool *pool, Hazard hazard)
{
    Hazard *slot = HazardsNext(pool);
    if (slot == NULL) return false;
    *slot = hazard;
    return true;
}

bool HazardsAddRect(HazardPool *pool, Rectangle rect, float warn, float active, float fade,
                    Type type, int damage, bool hit_once)
{
    return HazardAdd(pool, (Hazard){true, HAZARD_RECT, HAZARD_WARN, rect, {0.0f, 0.0f}, 0.0f,
                                    warn, active, fade, 0.0f, type, damage, hit_once, false});
}

bool HazardsAddCircle(HazardPool *pool, Vector2 center, float radius, float warn, float active,
                      float fade, Type type, int damage, bool hit_once)
{
    return HazardAdd(pool, (Hazard){true, HAZARD_CIRCLE, HAZARD_WARN, {0.0f, 0.0f, 0.0f, 0.0f}, center, radius,
                                    warn, active, fade, 0.0f, type, damage, hit_once, false});
}

bool HazardCircleHitsRect(Vector2 center, float radius, Rectangle rect)
{
    const float x = fmaxf(rect.x, fminf(center.x, rect.x + rect.width));
    const float y = fmaxf(rect.y, fminf(center.y, rect.y + rect.height));
    const float dx = center.x - x, dy = center.y - y;
    return dx * dx + dy * dy <= radius * radius;
}

bool HazardCircleHitsCircle(Vector2 first, float first_radius, Vector2 second, float second_radius)
{
    const float dx = first.x - second.x, dy = first.y - second.y;
    const float range = first_radius + second_radius;
    return dx * dx + dy * dy <= range * range;
}

static void HazardDamage(Hazard *hazard, PlayerStats *player, float damage_multiplier)
{
    int damage = (int)ceilf((float)hazard->damage * type_effectiveness(hazard->type, player->defense_type) *
                            player->incoming_damage_multiplier * damage_multiplier);
    if (damage < 1) damage = 1;
    player->hp -= damage;
    if (player->hp < 0) player->hp = 0;
    hazard->has_hit = true;
}

void HazardsUpdate(HazardPool *pool, PlayerStats *player, Vector2 soul, float soul_radius,
                   float delta_time, float damage_multiplier, bool invincible)
{
    for (int index = 0; index < MAX_HAZARDS; index++) {
        Hazard *hazard = &pool->hazards[index];
        if (!hazard->active) continue;
        hazard->timer += delta_time;
        if (hazard->phase == HAZARD_WARN && hazard->timer >= hazard->warning_time) {
            hazard->phase = HAZARD_ACTIVE; hazard->timer = 0.0f;
            continue; /* The warning frame itself never deals damage. */
        } else if (hazard->phase == HAZARD_ACTIVE && hazard->timer >= hazard->active_time) {
            hazard->phase = HAZARD_FADE; hazard->timer = 0.0f;
        } else if (hazard->phase == HAZARD_FADE && hazard->timer >= hazard->fade_time) {
            hazard->active = false; continue;
        }
        if (hazard->phase != HAZARD_ACTIVE || invincible || (hazard->hit_once && hazard->has_hit)) continue;
        const bool hit = hazard->shape == HAZARD_RECT ?
            HazardCircleHitsRect(soul, soul_radius, hazard->rect) :
            HazardCircleHitsCircle(soul, soul_radius, hazard->center, hazard->radius);
        if (hit) HazardDamage(hazard, player, damage_multiplier);
    }
    (void)damage_multiplier;
}

void HazardsDraw(const HazardPool *pool, bool reduced_effects, const SpriteSystem *sprites,
                 bool debug_hitboxes)
{
    for (int index = 0; index < MAX_HAZARDS; index++) {
        const Hazard *hazard = &pool->hazards[index];
        if (!hazard->active) continue;
        const Color color = type_color(hazard->type);
        const float alpha = hazard->phase == HAZARD_WARN ? 0.45f : (hazard->phase == HAZARD_FADE ? 0.35f : 0.85f);
        if (hazard->shape == HAZARD_RECT) {
            const SpriteId thorn = hazard->phase == HAZARD_WARN ? SPRITE_THORN_WALL_WARN : SPRITE_THORN_WALL;
            if (sprite_get(sprites, thorn) != NULL) SpriteDrawFit(sprites, thorn, hazard->rect, WHITE);
            else if (hazard->phase == HAZARD_WARN) DrawRectangleLinesEx(hazard->rect, 2.0f, Fade(color, alpha));
            else DrawRectangleRec(hazard->rect, Fade(color, alpha));
        } else {
            const float progress = fmaxf(0.18f, fminf(hazard->timer / hazard->warning_time, 1.0f));
            const float radius = hazard->phase == HAZARD_WARN ? hazard->radius * progress : hazard->radius;
            if (hazard->phase == HAZARD_WARN) {
                if (sprite_get(sprites, SPRITE_SHADOW_ORB) != NULL) {
                    SpriteDrawCenteredScaled(sprites, SPRITE_SHADOW_ORB, hazard->center, WHITE,
                                            hazard->timer, radius / 16.0f, debug_hitboxes);
                } else DrawCircleV(hazard->center, radius * 0.16f, Fade(color, 0.9f));
                if (sprite_get(sprites, SPRITE_SHADOW_BLOOM_WARN) != NULL) {
                    SpriteDrawCenteredScaled(sprites, SPRITE_SHADOW_BLOOM_WARN, hazard->center, WHITE,
                                            hazard->timer, radius / 32.0f, debug_hitboxes);
                } else DrawCircleLines((int)hazard->center.x, (int)hazard->center.y, radius, Fade(color, alpha));
            } else {
                if (sprite_get(sprites, SPRITE_SHADOW_BURST) != NULL && !reduced_effects) {
                    SpriteDrawCenteredScaled(sprites, SPRITE_SHADOW_BURST, hazard->center, WHITE,
                                            hazard->timer, radius / 48.0f, debug_hitboxes);
                } else DrawCircleV(hazard->center, radius, Fade(color, alpha));
            }
        }
        if (debug_hitboxes) {
            if (hazard->shape == HAZARD_RECT) DrawRectangleLinesEx(hazard->rect, 1.0f, MAGENTA);
            else DrawCircleLines((int)hazard->center.x, (int)hazard->center.y, hazard->radius, MAGENTA);
        }
    }
}
