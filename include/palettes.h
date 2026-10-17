#ifndef PALETAS_H
#define PALETAS_H

#include "render.h"

static const PaletaAndar ANDAR_PALETAS[] = {
    {{248, 238, 226, 255}, {221, 181, 196, 255}, {226, 210, 194, 255}, false},
    {{231, 242, 248, 255}, {173, 213, 233, 255}, {202, 221, 228, 255}, false},
    {{235, 247, 235, 255}, {171, 215, 190, 255}, {204, 226, 208, 255}, false},
    {{250, 244, 218, 255}, {239, 204, 136, 255}, {230, 215, 181, 255}, false}
};

static const PaletaAndar CHEFE_ANDAR_PALETTE = {{37, 38, 43, 255}, {222, 99, 116, 255},
                                                {72, 73, 79, 255}, true};

#define ANDAR_PALETTE_TOTAL ((int)(sizeof(ANDAR_PALETAS) / sizeof(ANDAR_PALETAS[0])))

#endif
