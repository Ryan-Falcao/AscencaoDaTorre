#include "types.h"

/* Rows: attacking type. Columns: defending type.
   Order for both axes: Flame, Water, Plant, Light, Shadow, Neutral. */
static const float EFFECTIVENESS[TYPE_COUNT][TYPE_COUNT] = {
    /* Flame   */ {1.0f, 0.5f, 2.0f, 1.0f, 1.0f, 1.0f},
    /* Water   */ {2.0f, 1.0f, 0.5f, 1.0f, 1.0f, 1.0f},
    /* Plant   */ {0.5f, 2.0f, 1.0f, 1.0f, 1.0f, 1.0f},
    /* Light   */ {1.0f, 1.0f, 1.0f, 1.0f, 2.0f, 1.0f},
    /* Shadow  */ {1.0f, 1.0f, 1.0f, 2.0f, 1.0f, 1.0f},
    /* Neutral */ {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f}
};

float type_effectiveness(Type attacker, Type defender)
{
    if (attacker < TYPE_FLAME || attacker >= TYPE_COUNT ||
        defender < TYPE_FLAME || defender >= TYPE_COUNT) return 1.0f;
    return EFFECTIVENESS[attacker][defender];
}

const char *type_name(Type type)
{
    static const char *names[TYPE_COUNT] = {
        "Chama", "Agua", "Planta", "Luz", "Sombra", "Neutro"
    };

    if (type < TYPE_FLAME || type >= TYPE_COUNT) return "Desconhecido";
    return names[type];
}

Color type_color(Type type)
{
    switch (type) {
        case TYPE_FLAME: return (Color){227, 92, 68, 255};
        case TYPE_WATER: return (Color){66, 151, 220, 255};
        case TYPE_PLANT: return (Color){76, 167, 96, 255};
        case TYPE_LIGHT: return (Color){225, 181, 49, 255};
        case TYPE_SHADOW: return (Color){111, 75, 151, 255};
        case TYPE_NEUTRAL:
        default: return (Color){112, 119, 126, 255};
    }
}
