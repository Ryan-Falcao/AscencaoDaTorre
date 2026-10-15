#ifndef HAZARDS_H
#define HAZARDS_H

#include "raylib.h"
#include "player.h"

typedef struct SpriteSystem SpriteSystem;

#define MAX_HAZARDS 32

typedef enum { HAZARD_RECT, HAZARD_CIRCLE } HazardShape;
typedef enum { HAZARD_WARN, HAZARD_ACTIVE, HAZARD_FADE } HazardPhase;

typedef struct {
    bool active;
    HazardShape shape;
    HazardPhase phase;
    Rectangle rect;
    Vector2 center;
    float radius;
    float warning_time;
    float active_time;
    float fade_time;
    float timer;
    Type type;
    int damage;
    bool hit_once;
    bool has_hit;
} Hazard;

typedef struct { Hazard hazards[MAX_HAZARDS]; } HazardPool;

void HazardsInit(HazardPool *pool);
void HazardsClear(HazardPool *pool);
bool HazardsAddRect(HazardPool *pool, Rectangle rect, float warn, float active, float fade,
                    Type type, int damage, bool hit_once);
bool HazardsAddCircle(HazardPool *pool, Vector2 center, float radius, float warn, float active,
                      float fade, Type type, int damage, bool hit_once);
void HazardsUpdate(HazardPool *pool, PlayerStats *player, Vector2 soul, float soul_radius,
                   float delta_time, float damage_multiplier, bool invincible);
void HazardsDraw(const HazardPool *pool, bool reduced_effects, const SpriteSystem *sprites,
                 bool debug_hitboxes);
bool HazardCircleHitsRect(Vector2 center, float radius, Rectangle rect);
bool HazardCircleHitsCircle(Vector2 first, float first_radius, Vector2 second, float second_radius);

#endif
