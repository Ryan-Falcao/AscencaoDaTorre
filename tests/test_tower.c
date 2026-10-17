#include <assert.h>
#include <stdio.h>

#include "tower.h"

int main(void)
{
    Torre torre;
    AtributosJogador jogador;

    TorreInicializar(&torre, 12345u);
    assert(TorreAndarDeChefe(5) && TorreAndarDeChefe(10));
    assert(TorreInimigoDoAndar(&torre, 5) == INIMIGO_CHEFE_SINEIRO);
    assert(TorreInimigoDoAndar(&torre, 10) == INIMIGO_CHEFE_ULTIMA_TORRE);
    for (int andar = 2; andar <= TOTAL_ANDARES_TORRE; andar++) {
        if (!TorreAndarDeChefe(andar) && !TorreAndarDeChefe(andar - 1)) {
            assert(TorreInimigoDoAndar(&torre, andar) != TorreInimigoDoAndar(&torre, andar - 1));
        }
    }

    JogadorInicializar(&jogador);
    TorreSortearRecompensas(&torre, &jogador);
    assert(torre.recompensas[0].categoria == RECOMPENSA_ATAQUE);
    assert(!JogadorPossuiAtaque(&jogador, JogadorAtaqueRecompensa(torre.recompensas[0].id)));

    for (int item = 0; item < MAXIMO_INVENTARIO_ITENS; item++) {
        jogador.inventario[item] = (ItemInventario){(IdItem)item, 1};
    }
    assert(!JogadorAdicionarItem(&jogador, ITEM_TOTAL, 1));
    jogador.inventario[7].quantidade = 0;
    assert(JogadorAdicionarItem(&jogador, ITEM_CHA, 1));

    puts("Geracao da torre, recompensas e limite da loja verificados.");
    return 0;
}
