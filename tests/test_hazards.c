#include "hazards.h"
#include "balance.h"

#include <assert.h>

int main(void)
{
    PlayerStats player;
    HazardPool pool;
    const Vector2 soul = {100.0f, 100.0f};

    PlayerInit(&player);
    const int initial_hp = player.hp;
    HazardsInit(&pool);
    assert(HazardsAddCircle(&pool, soul, 20.0f, 0.10f, 0.20f, 0.10f, TYPE_SHADOW, 5, true));
    HazardsUpdate(&pool, &player, soul, 4.0f, 0.10f, 1.0f, false);
    assert(player.hp == initial_hp); /* warning never hurts */
    HazardsUpdate(&pool, &player, soul, 4.0f, 0.01f, 1.0f, false);
    assert(player.hp < initial_hp);
    const int after_hit = player.hp;
    HazardsUpdate(&pool, &player, soul, 4.0f, 0.05f, 1.0f, false);
    assert(player.hp == after_hit); /* hit_once */

    HazardsClear(&pool);
    assert(HazardsAddRect(&pool, (Rectangle){90.0f, 90.0f, 20.0f, 20.0f}, 0.0f, 0.2f, 0.1f,
                          TYPE_PLANT, 4, true));
    HazardsUpdate(&pool, &player, soul, 4.0f, 0.01f, 1.0f, false);
    HazardsUpdate(&pool, &player, soul, 4.0f, 0.01f, 1.0f, false);
    assert(player.hp < after_hit);
    assert(!HazardCircleHitsRect((Vector2){10.0f, 10.0f}, 2.0f, (Rectangle){30, 30, 10, 10}));
    assert(HazardCircleHitsCircle(soul, 4.0f, (Vector2){105.0f, 100.0f}, 2.0f));

    HazardsClear(&pool);
    for (int index = 0; index < MAX_HAZARDS; index++) {
        assert(HazardsAddCircle(&pool, soul, 2.0f, 1.0f, 1.0f, 1.0f, TYPE_NEUTRAL, 1, true));
    }
    assert(!HazardsAddCircle(&pool, soul, 2.0f, 1.0f, 1.0f, 1.0f, TYPE_NEUTRAL, 1, true));
    assert(BALANCE_GRAVITY_THORN_DEPTH <= 270.0f / 3.0f);
    assert(BALANCE_GRAVITY_ESCAPE_TIME * 135.0f > BALANCE_GRAVITY_THORN_DEPTH * 2.0f);
    return 0;
}
