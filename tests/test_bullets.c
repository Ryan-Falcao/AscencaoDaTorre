#include "bullets.h"

#include <assert.h>

/* This is deliberately windowless: it validates the data copied into a defense turn. */
int main(void)
{
    const Rectangle menu_box = {160.0f, 285.0f, 320.0f, 90.0f};

    for (int id = PATTERN_RAIN; id < PATTERN_COUNT; id++) {
        DefenseState defense;
        DefenseInit(&defense);
        DefenseStart(&defense, (BulletPatternId)id, menu_box, 50, 0, 1.0f, 1.0f);
        assert(defense.pattern_id == (BulletPatternId)id);
        assert(defense.pattern.box_width > 0.0f && defense.pattern.box_width <= 640.0f);
        assert(defense.pattern.box_height > 0.0f && defense.pattern.box_height <= 480.0f);
        assert(defense.pattern.hitbox_radius > 0.0f);
        assert(defense.pattern.bullets_per_spawn > 0);
        assert(BulletPatternDifficulty((BulletPatternId)id) >= 1);
    }
    return 0;
}
