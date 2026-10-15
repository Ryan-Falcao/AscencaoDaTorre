#include "enemy.h"

#include <assert.h>
#include <stdio.h>

int main(void)
{
    PlayerStats player;
    Enemy enemy = EnemyCreate(ENEMY_DUST_MOTH);
    char reason[24] = "";
    Attack attack;
    int burn_damage = 0;

    PlayerInit(&player);
    attack = PlayerRewardAttack(ATTACK_RAIZ_RAPIDA);
    assert(PlayerAttackAvailable(&player, &attack, reason, (int)sizeof(reason)));
    PlayerUseAttack(&player, &attack);
    assert(!PlayerAttackAvailable(&player, &attack, reason, (int)sizeof(reason)));
    PlayerAdvanceAttackCooldowns(&player);
    assert(PlayerAttackAvailable(&player, &attack, reason, (int)sizeof(reason)));

    EnemyApplyStatus(&enemy, STATUS_BURN, 2);
    assert(EnemyAdvanceStatus(&enemy, &burn_damage) && burn_damage == 3);
    assert(enemy.status_turns == 1);
    EnemyAdvanceStatus(&enemy, &burn_damage);
    assert(enemy.status == STATUS_NONE && enemy.status_turns == 0);

    puts("status, cooldown and sprite fallback ids verified.");
    return 0;
}
