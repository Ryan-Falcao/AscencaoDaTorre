#ifndef TIPOS_H
#define TIPOS_H

#include "raylib.h"

typedef enum {
    TIPO_FOGO,
    TIPO_AGUA,
    TIPO_PLANTA,
    TIPO_LUZ,
    TIPO_SOMBRA,
    TIPO_NEUTRO,
    TIPO_TOTAL
} Tipo;

float efetividade_tipo(Tipo atacante, Tipo defensor);
const char *nome_tipo(Tipo tipo);
Color cor_tipo(Tipo tipo);

#endif
