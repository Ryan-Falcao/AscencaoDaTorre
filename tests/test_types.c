#include "types.h"

#include <stdio.h>

int main(void)
{
    const float expected[TIPO_TOTAL][TIPO_TOTAL] = {
        {1.0f, 0.5f, 2.0f, 1.0f, 1.0f, 1.0f},
        {2.0f, 1.0f, 0.5f, 1.0f, 1.0f, 1.0f},
        {0.5f, 2.0f, 1.0f, 1.0f, 1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f, 1.0f, 2.0f, 1.0f},
        {1.0f, 1.0f, 1.0f, 2.0f, 1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f}
    };
    int failures = 0;

    for (Tipo atacante = TIPO_FOGO; atacante < TIPO_TOTAL; atacante++) {
        for (Tipo defensor = TIPO_FOGO; defensor < TIPO_TOTAL; defensor++) {
            const float actual = efetividade_tipo(atacante, defensor);
            if (actual != expected[atacante][defensor]) {
                printf("Falha: %s contra %s = %.1f, esperado %.1f\n", nome_tipo(atacante),
                       nome_tipo(defensor), actual, expected[atacante][defensor]);
                failures++;
            }
        }
    }

    if (failures == 0) printf("36 combinacoes de tipos verificadas.\n");
    return failures == 0 ? 0 : 1;
}
