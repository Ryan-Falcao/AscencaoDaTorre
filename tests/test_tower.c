#include <assert.h>
#include <stdio.h>

#include "tower.h"

int main(void)
{
    Torre torre;
    AtributosJogador jogador;

    TorreInicializar(&torre, 12345u);
    assert(TorreAndarDeChefe(5) && TorreAndarDeChefe(10) && TorreAndarDeChefe(30));
    assert(TorreInimigoDoAndar(&torre, 5) == INIMIGO_CHEFE_CULTISTA_RAIZES);
    assert(TorreInimigoDoAndar(&torre, 10) == INIMIGO_CHEFE_AFOGADO_CORAL);
    assert(TorreInimigoDoAndar(&torre, 15) == INIMIGO_CHEFE_MAGO_CINZAS);
    assert(TorreInimigoDoAndar(&torre, 20) == INIMIGO_CHEFE_GOBLIN_CRIPTA);
    assert(TorreInimigoDoAndar(&torre, 25) == INIMIGO_CHEFE_INQUISIDOR_SOL);
    assert(TorreInimigoDoAndar(&torre, 30) == INIMIGO_CHEFE_ULTIMA_TORRE);
    assert(InimigoCriar(INIMIGO_CHEFE_ULTIMA_TORRE).todos_tipos);
    static const Tipo tipo_regiao[] = {TIPO_PLANTA,TIPO_AGUA,TIPO_FOGO,TIPO_SOMBRA,TIPO_LUZ,TIPO_NEUTRO};
    for (int andar = 1; andar <= TOTAL_ANDARES_TORRE; andar++) {
        assert(TorreTipoDoAndar(andar) == tipo_regiao[(andar - 1) / 5]);
        Inimigo inimigo = InimigoCriar(TorreInimigoDoAndar(&torre, andar));
        if (TorreAndarDeChefe(andar)) assert(inimigo.chefe);
        else {
            assert(!inimigo.chefe);
            assert(inimigo.tipo == tipo_regiao[(andar - 1) / 5] ||
                   (inimigo.possui_tipo2 && inimigo.tipo2 == tipo_regiao[(andar - 1) / 5]));
        }
    }
    for (int andar = 2; andar <= TOTAL_ANDARES_TORRE; andar++) {
        if (!TorreAndarDeChefe(andar) && !TorreAndarDeChefe(andar - 1)) {
            assert(TorreInimigoDoAndar(&torre, andar) != TorreInimigoDoAndar(&torre, andar - 1));
        }
    }

    JogadorInicializar(&jogador);
    TorreSortearRecompensas(&torre, &jogador);
    assert(torre.recompensas[0].categoria == RECOMPENSA_EQUIPAMENTO);
    assert(ItemEhEquipamento((IdItem)torre.recompensas[0].id));
    assert(torre.recompensas[1].categoria == RECOMPENSA_ITEM);
    assert(!ItemEhEquipamento((IdItem)torre.recompensas[1].id));

    for (int item = 0; item < MAXIMO_INVENTARIO_ITENS; item++) {
        jogador.inventario[item] = (ItemInventario){(IdItem)item, 1};
    }
    assert(!JogadorAdicionarItem(&jogador, ITEM_TOTAL, 1));
    jogador.inventario[7].quantidade = 0;
    assert(JogadorAdicionarItem(&jogador, ITEM_CHA, 1));

    puts("Geracao da torre, recompensas e limite da loja verificados.");
    return 0;
}
