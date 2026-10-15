#include "fx.h"

#include <math.h>

void FxStart(AttackFx *fx, const Attack *attack, QteResult result)
{
    *fx = (AttackFx){true, attack->id, attack->fx_style, attack->type, result,
                     0.0f, result == QTE_ERROR ? 0.25f : 0.48f, {320.0f, 137.0f}};
}

void FxUpdate(AttackFx *fx, float delta_time)
{
    if (!fx->active) return;
    fx->timer += delta_time;
    if (fx->timer >= fx->duration) fx->active = false;
}

static void DrawFallback(const AttackFx *fx, float scale, Color color)
{
    const float progress = fx->timer / fx->duration;
    const Vector2 center = fx->style == FX_PROJECTILE ?
        (Vector2){112.0f + 208.0f * progress, 300.0f - 163.0f * progress} : fx->position;

    if (fx->result == QTE_ERROR) {
        for (int index = 0; index < 5; index++) {
            const float a = (float)index * 1.256f;
            DrawCircleV((Vector2){center.x + cosf(a) * 14.0f * progress,
                                  center.y + sinf(a) * 14.0f * progress}, 3.0f, GRAY);
        }
    } else if (fx->style == FX_BEAM) {
        DrawRectangle((int)(center.x - 80.0f), (int)(center.y - 4.0f * scale), 160,
                      (int)(8.0f * scale), color);
    } else if (fx->style == FX_SLASH) {
        DrawLineEx((Vector2){center.x - 36.0f * scale, center.y + 28.0f * scale},
                   (Vector2){center.x + 36.0f * scale, center.y - 28.0f * scale}, 5.0f, color);
    } else if (fx->style == FX_RAIN) {
        for (int index = 0; index < 4; index++) {
            DrawCircle((int)(center.x - 30.0f + index * 20.0f),
                       (int)(center.y - 35.0f + progress * 65.0f), 5, color);
        }
    } else {
        DrawCircleLines((int)center.x, (int)center.y, 28.0f * scale, color);
        DrawCircleLines((int)center.x, (int)center.y, 18.0f * scale, color);
    }
}

void FxDraw(const AttackFx *fx, const SpriteSystem *sprites, bool reduced_effects)
{
    if (!fx->active) return;
    const float result_scale = fx->result == QTE_PERFECT ? 1.35f :
                               (fx->result == QTE_BAD ? 0.75f : 1.0f);
    const Color color = fx->result == QTE_ERROR ? GRAY : type_color(fx->type);
    SpriteId id = SpriteFxAttackId(fx->attack_id);

    if (sprite_get(sprites, id) == NULL) id = SpriteFxTypeId(fx->type);
    if (!reduced_effects && fx->result != QTE_ERROR && sprite_get(sprites, id) != NULL) {
        SpriteDrawCenteredScaled(sprites, id, fx->position, color, fx->timer, result_scale,
                                 sprites->debug_bounds);
    } else {
        DrawFallback(fx, reduced_effects ? 0.7f : result_scale, color);
    }
}
