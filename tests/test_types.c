#include "types.h"

#include <stdio.h>

int main(void)
{
    const float expected[TYPE_COUNT][TYPE_COUNT] = {
        {1.0f, 0.5f, 2.0f, 1.0f, 1.0f, 1.0f},
        {2.0f, 1.0f, 0.5f, 1.0f, 1.0f, 1.0f},
        {0.5f, 2.0f, 1.0f, 1.0f, 1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f, 1.0f, 2.0f, 1.0f},
        {1.0f, 1.0f, 1.0f, 2.0f, 1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f}
    };
    int failures = 0;

    for (Type attacker = TYPE_FLAME; attacker < TYPE_COUNT; attacker++) {
        for (Type defender = TYPE_FLAME; defender < TYPE_COUNT; defender++) {
            const float actual = type_effectiveness(attacker, defender);
            if (actual != expected[attacker][defender]) {
                printf("Falha: %s contra %s = %.1f, esperado %.1f\n", type_name(attacker),
                       type_name(defender), actual, expected[attacker][defender]);
                failures++;
            }
        }
    }

    if (failures == 0) printf("36 combinacoes de tipos verificadas.\n");
    return failures == 0 ? 0 : 1;
}
