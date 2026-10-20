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
    assert(SpriteIdItem(ITEM_AMULETO_AMPULHETA) == SPRITE_ITEM_AMULETO_AMPULHETA);
    for (int item = ITEM_CHA; item < ITEM_TOTAL; item++)
        assert(SpriteIdItem((IdItem)item) == (IdSprite)(SPRITE_ITEM_CHA + item));
    assert(SpriteIdAtaque(ATAQUE_BRASA) == SPRITE_ATAQUE_BRASA);
    assert(SpriteIdEfeitoAtaque(ATAQUE_SINO_VAZIO) == SPRITE_EFEITO_ATAQUE_SINO_VAZIO);
    assert(SpriteIdEfeitoTipo(TIPO_LUZ) == SPRITE_EFEITO_TIPO_LUZ);
    assert(SpriteIdPadraoProjetil(PADRAO_CATA_VENTO) == SPRITE_PROJETIL_PADRAO_CATA_VENTO);
    assert(SpriteIdPadraoProjetil(PADRAO_MARIPOSA_ASAS) == SPRITE_DEFESA_MARIPOSA_ASAS);
    assert(SpriteIdPadraoProjetil(PADRAO_INQUISIDOR_VEREDITO) == SPRITE_DEFESA_INQUISIDOR_VEREDITO);
    for (int padrao = PADRAO_CHUVA; padrao < PADRAO_TOTAL; padrao++)
        assert(SpriteIdPadraoProjetil((IdPadraoProjetil)padrao) != SPRITE_TOTAL);
    assert(SpriteIdInimigo(INIMIGO_SOLDADO_SARCAS, false) == SPRITE_INIMIGO_SOLDADO_SARCAS);
    assert(SpriteIdInimigo(INIMIGO_CHEFE_MAGO_CINZAS, false) == SPRITE_CHEFE_MAGO_CINZAS);
    assert(SpriteIdInimigo(INIMIGO_CHEFE_MAGO_CINZAS, true) == SPRITE_CHEFE_ONI_BRASAS);

    puts("sprite tests passed");
    return 0;
}
