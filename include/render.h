#ifndef RENDER_H
#define RENDER_H

#include <stdbool.h>

#include "raylib.h"

typedef struct {
    Color background;
    Color floating;
    Color paper;
    bool boss;
} FloorPalette;

typedef struct {
    FloorPalette current;
    FloorPalette target;
    float transition;
    bool shader_enabled;
    bool line_tremor;
} RenderState;

void RenderInit(RenderState *render, bool line_tremor);
void RenderSetFloor(RenderState *render, int floor, bool boss);
void RenderUpdate(RenderState *render, float delta_time);
void RenderDrawBackground(const RenderState *render, float time);

#endif
