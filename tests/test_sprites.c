#include "sprites.h"

#include <assert.h>
#include <stdio.h>

int main(void)
{
    MetadadosSprite metadados = {0};

    assert(SpriteInterpretarMetadadosTexto("frames=4 fps=6 frame_w=96 frame_h=96", &metadados));
    assert(metadados.quadros == 4 && metadados.fps == 6);
    assert(metadados.largura_quadro == 96 && metadados.altura_quadro == 96);
    assert(!SpriteInterpretarMetadadosTexto("frames=0 fps=6 frame_w=96 frame_h=96", &metadados));
    assert(!SpriteInterpretarMetadadosTexto("frames=4 fps=6", &metadados));
    assert(SpriteIdInimigo(INIMIGO_POEIRA_MARIPOSA, false) == SPRITE_INIMIGO_POEIRA_MARIPOSA);
    assert(SpriteIdInimigo(INIMIGO_CHEFE_SINEIRO, true) == SPRITE_CHEFE_SINEIRO_P2);
    assert(SpriteIdTipo(TIPO_SOMBRA) == SPRITE_TIPO_SOMBRA);
    assert(SpriteIdProjetil(TIPO_AGUA) == SPRITE_PROJETIL_AGUA);
    assert(SpriteIdItem(ITEM_CHA) == SPRITE_ITEM_CHA);
    assert(SpriteIdAtaque(ATAQUE_BRASA) == SPRITE_ATAQUE_BRASA);
    assert(SpriteIdEfeitoAtaque(ATAQUE_SINO_VAZIO) == SPRITE_EFEITO_ATAQUE_SINO_VAZIO);
    assert(SpriteIdEfeitoTipo(TIPO_LUZ) == SPRITE_EFEITO_TIPO_LUZ);
    assert(SpriteIdPadraoProjetil(PADRAO_CATA_VENTO) == SPRITE_PROJETIL_PADRAO_CATA_VENTO);

    puts("sprite tests passed");
    return 0;
}
