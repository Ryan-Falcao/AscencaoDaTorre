#ifndef TYPES_H
#define TYPES_H

#include "raylib.h"

typedef enum {
    TYPE_FLAME,
    TYPE_WATER,
    TYPE_PLANT,
    TYPE_LIGHT,
    TYPE_SHADOW,
    TYPE_NEUTRAL,
    TYPE_COUNT
} Type;

float type_effectiveness(Type attacker, Type defender);
const char *type_name(Type type);
Color type_color(Type type);

#endif
