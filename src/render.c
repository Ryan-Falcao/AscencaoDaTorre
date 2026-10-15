#include "render.h"
#include "palettes.h"

#include <math.h>

#define ENABLE_OPTIONAL_SHADER false


static unsigned char BlendByte(unsigned char from, unsigned char to, float amount)
{
    return (unsigned char)((float)from + ((float)to - (float)from) * amount);
}

static Color BlendColor(Color from, Color to, float amount)
{
    return (Color){BlendByte(from.r, to.r, amount), BlendByte(from.g, to.g, amount),
                   BlendByte(from.b, to.b, amount), 255};
}

void RenderInit(RenderState *render, bool line_tremor)
{
    render->current = FLOOR_PALETTES[0];
    render->target = FLOOR_PALETTES[0];
    render->transition = 1.0f;
    render->shader_enabled = ENABLE_OPTIONAL_SHADER;
    render->line_tremor = line_tremor;
}

void RenderSetFloor(RenderState *render, int floor, bool boss)
{
    render->target = boss ? BOSS_FLOOR_PALETTE : FLOOR_PALETTES[(floor - 1) % FLOOR_PALETTE_COUNT];
    render->transition = 0.0f;
}

void RenderUpdate(RenderState *render, float delta_time)
{
    if (render->transition < 1.0f) {
        render->transition += delta_time * 1.4f;
        if (render->transition >= 1.0f) {
            render->transition = 1.0f;
            render->current = render->target;
        }
    }
}

void RenderDrawBackground(const RenderState *render, float time)
{
    const Color background = BlendColor(render->current.background, render->target.background,
                                        render->transition);
    const Color floating = BlendColor(render->current.floating, render->target.floating,
                                      render->transition);

    ClearBackground(background);
    for (int index = 0; index < 28; index++) {
        const int x = (index * 71) % 640;
        const int y = (index * 43) % 480;
        DrawCircle(x, y, 1, render->current.paper);
    }
    if (!render->current.boss) {
        DrawEllipse(100, 80 + (int)(sinf(time * 0.4f) * 4.0f), 70, 15, floating);
        DrawEllipse(520, 150 + (int)(cosf(time * 0.3f) * 4.0f), 82, 16, floating);
    }
}
