#include "types.h"

/* Linhas: tipo atacante. Colunas: tipo defensor.
   Ordem dos dois eixos: Fogo, Agua, Planta, Luz, Sombra, Neutro. */
static const float EFETIVIDADE[TIPO_TOTAL][TIPO_TOTAL] = {
    /* Fogo    */ {1.0f, 0.5f, 2.0f, 1.0f, 1.0f, 1.0f},
    /* Agua    */ {2.0f, 1.0f, 0.5f, 1.0f, 1.0f, 1.0f},
    /* Planta  */ {0.5f, 2.0f, 1.0f, 1.0f, 1.0f, 1.0f},
    /* Luz     */ {1.0f, 1.0f, 1.0f, 1.0f, 2.0f, 1.0f},
    /* Sombra  */ {1.0f, 1.0f, 1.0f, 2.0f, 1.0f, 1.0f},
    /* Neutro  */ {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f}
};

float efetividade_tipo(Tipo atacante, Tipo defensor)
{
    if (atacante < TIPO_FOGO || atacante >= TIPO_TOTAL ||
        defensor < TIPO_FOGO || defensor >= TIPO_TOTAL) return 1.0f;
    return EFETIVIDADE[atacante][defensor];
}

const char *nome_tipo(Tipo tipo)
{
    static const char *names[TIPO_TOTAL] = {
        "Chama", "Agua", "Planta", "Luz", "Sombra", "Neutro"
    };

    if (tipo < TIPO_FOGO || tipo >= TIPO_TOTAL) return "Desconhecido";
    return names[tipo];
}

Color cor_tipo(Tipo tipo)
{
    switch (tipo) {
        case TIPO_FOGO: return (Color){227, 92, 68, 255};
        case TIPO_AGUA: return (Color){66, 151, 220, 255};
        case TIPO_PLANTA: return (Color){76, 167, 96, 255};
        case TIPO_LUZ: return (Color){225, 181, 49, 255};
        case TIPO_SOMBRA: return (Color){111, 75, 151, 255};
        case TIPO_NEUTRO:
        default: return (Color){112, 119, 126, 255};
    }
}
