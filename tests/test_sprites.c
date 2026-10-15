#include "sprites.h"

#include <assert.h>
#include <stdio.h>

int main(void)
{
    SpriteMetadata metadata = {0};

    assert(SpriteParseMetadataText("frames=4 fps=6 frame_w=96 frame_h=96", &metadata));
    assert(metadata.frames == 4 && metadata.fps == 6);
    assert(metadata.frame_width == 96 && metadata.frame_height == 96);
    assert(!SpriteParseMetadataText("frames=0 fps=6 frame_w=96 frame_h=96", &metadata));
    assert(!SpriteParseMetadataText("frames=4 fps=6", &metadata));
    assert(SpriteEnemyId(ENEMY_DUST_MOTH, false) == SPRITE_ENEMY_DUST_MOTH);
    assert(SpriteEnemyId(ENEMY_BOSS_BELLKEEPER, true) == SPRITE_BOSS_BELLKEEPER_P2);
    assert(SpriteTypeId(TYPE_SHADOW) == SPRITE_TYPE_SHADOW);
    assert(SpriteBulletId(TYPE_WATER) == SPRITE_BULLET_WATER);
    assert(SpriteItemId(ITEM_TEA) == SPRITE_ITEM_TEA);
    assert(SpriteAttackId(ATTACK_BRASA) == SPRITE_ATTACK_BRASA);
    assert(SpriteFxAttackId(ATTACK_SINO_VAZIO) == SPRITE_FX_ATTACK_SINO_VAZIO);
    assert(SpriteFxTypeId(TYPE_LIGHT) == SPRITE_FX_TYPE_LIGHT);
    assert(SpriteBulletPatternId(PATTERN_PINWHEEL) == SPRITE_BULLET_PATTERN_PINWHEEL);

    puts("sprite tests passed");
    return 0;
}
