#ifndef FX_H
#define FX_H

#include "player.h"
#include "sprites.h"

typedef struct {
    bool active;
    AttackId attack_id;
    AttackFxStyle style;
    Type type;
    QteResult result;
    float timer;
    float duration;
    Vector2 position;
} AttackFx;

void FxStart(AttackFx *fx, const Attack *attack, QteResult result);
void FxUpdate(AttackFx *fx, float delta_time);
void FxDraw(const AttackFx *fx, const SpriteSystem *sprites, bool reduced_effects);

#endif
