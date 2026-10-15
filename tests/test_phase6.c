#include <assert.h>
#include <stdio.h>

#include "enemy.h"
#include "player.h"

int main(void)
{
    Enemy moth = EnemyCreate(ENEMY_DUST_MOTH);
    Enemy tired_moth;
    PlayerStats player;
    const ItemDefinition *used_item;
    int applied_value;
    char message[128];

    assert(EnemyApplyAction(&moth, 1, message, (int)sizeof(message)));
    assert(moth.mercy == 30 && moth.aggression == 35);
    assert(!EnemyActionAvailable(&moth, 1, message, (int)sizeof(message)));
    EnemyAdvanceTurn(&moth);
    assert(!EnemyActionAvailable(&moth, 1, message, (int)sizeof(message)));
    EnemyAdvanceTurn(&moth);
    assert(EnemyApplyAction(&moth, 1, message, (int)sizeof(message)));
    assert(moth.mercy == 45); /* Second consecutive Elogiar is reduced from +30 to +15. */
    EnemyAdvanceTurn(&moth);
    EnemyAdvanceTurn(&moth);
    assert(!EnemyActionAvailable(&moth, 1, message, (int)sizeof(message)));

    moth.mercy = 95;
    moth.aggression = 90;
    assert(EnemyApplyAction(&moth, 2, message, (int)sizeof(message)));
    assert(moth.mercy == 85 && moth.aggression == 100);
    moth.mercy = 100;
    EnemyRefreshSpareable(&moth);
    assert(!EnemyCanSpare(&moth));
    moth.aggression = 66;
    assert(EnemyCanSpare(&moth));

    tired_moth = EnemyCreate(ENEMY_DUST_MOTH);
    EnemyTakeDamage(&tired_moth, 28, 1.0f);
    assert(tired_moth.exhausted && tired_moth.spareable);

    PlayerInit(&player);
    assert(PlayerUseItem(&player, 1, &used_item, &applied_value));
    assert(used_item->effect == ITEM_EFFECT_DAMAGE_BUFF);
    assert(PlayerAttackBuffMultiplier(&player) == 1.5f);
    PlayerConsumeAttackBuff(&player);
    assert(PlayerAttackBuffMultiplier(&player) == 1.0f);

    puts("Mercy, agressividade, repeticao, cooldown, exaustao e buffs verificados.");
    return 0;
}
