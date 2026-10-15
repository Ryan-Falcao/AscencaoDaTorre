#include <assert.h>
#include <stdio.h>

#include "tower.h"

int main(void)
{
    Tower tower;
    PlayerStats player;

    TowerInit(&tower, 12345u);
    assert(TowerIsBossFloor(5) && TowerIsBossFloor(10));
    assert(TowerEnemyForFloor(&tower, 5) == ENEMY_BOSS_BELLKEEPER);
    assert(TowerEnemyForFloor(&tower, 10) == ENEMY_BOSS_LAST_TOWER);
    for (int floor = 2; floor <= TOWER_FLOORS; floor++) {
        if (!TowerIsBossFloor(floor) && !TowerIsBossFloor(floor - 1)) {
            assert(TowerEnemyForFloor(&tower, floor) != TowerEnemyForFloor(&tower, floor - 1));
        }
    }

    PlayerInit(&player);
    TowerRollRewards(&tower, &player);
    assert(tower.rewards[0].kind == REWARD_ATTACK);
    assert(!PlayerHasAttack(&player, PlayerRewardAttack(tower.rewards[0].id)));

    for (int item = 0; item < MAX_INVENTORY_ITEMS; item++) {
        player.inventory[item] = (InventoryItem){(ItemId)item, 1};
    }
    assert(!PlayerAddItem(&player, ITEM_COUNT, 1));
    player.inventory[7].quantity = 0;
    assert(PlayerAddItem(&player, ITEM_TEA, 1));

    puts("Geracao da torre, recompensas e limite da loja verificados.");
    return 0;
}
