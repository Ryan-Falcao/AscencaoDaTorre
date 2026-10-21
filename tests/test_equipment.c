#include <assert.h>
#include <math.h>
#include <stdio.h>

#include "enemy.h"
#include "player.h"

static int EncontrarItem(const AtributosJogador *jogador, IdItem id)
{
    for (int indice = 0; indice < MAXIMO_INVENTARIO_ITENS; indice++)
        if (jogador->inventario[indice].quantidade > 0 && jogador->inventario[indice].id == id) return indice;
    return -1;
}

int main(void)
{
    AtributosJogador jogador;
    Inimigo inimigo = InimigoCriar(INIMIGO_POEIRA_MARIPOSA);

    for (int id = 0; id < ITEM_TOTAL; id++) assert(ItemRaridadeValida(ItemObterDefinicao((IdItem)id)));
    assert(ItemTotalAtributos(ItemObterDefinicao(ITEM_AMULETO_ESPELHO)) == 1);
    assert(ItemObterDefinicao(ITEM_AMULETO_ESPELHO)->passiva != PASSIVA_NENHUMA);

    JogadorInicializar(&jogador);
    /* Uma run inicia leve: broche e espada de treino, sem armadura gratis. */
    assert(jogador.bonus_equipamento.ataque == 1);
    assert(jogador.bonus_equipamento.defesa == 0);
    assert(jogador.vida_maxima == 40);
    assert(jogador.total_ataques == 3 && jogador.ataques[0].tipo == TIPO_PLANTA);
    assert(fabsf(jogador.multiplicador_dano_recebido - 1.0f) < 0.001f);

    assert(JogadorAdicionarItem(&jogador, ITEM_ESCUDO_MARE, 1));
    int indice = EncontrarItem(&jogador, ITEM_ESCUDO_MARE);
    assert(indice >= 0 && JogadorEquiparDoInventario(&jogador, indice, ESPACO_ESCUDO));
    assert(jogador.bonus_equipamento.defesa == 4);
    assert(jogador.multiplicador_dano_recebido < 1.0f);

    assert(JogadorAdicionarItem(&jogador, ITEM_CAJADO_GELO, 1));
    indice = EncontrarItem(&jogador, ITEM_CAJADO_GELO);
    assert(indice >= 0 && JogadorEquiparDoInventario(&jogador, indice, ESPACO_ARMAMENTO));
    assert(jogador.ataques[0].tipo == TIPO_AGUA);
    assert(jogador.mana_maxima == 18 && jogador.mana_por_turno == 4);
    jogador.mana = 7;
    JogadorIniciarBatalha(&jogador);
    assert(jogador.mana == 7);
    JogadorIniciarTurno(&jogador);
    assert(jogador.mana == 11);

    assert(JogadorAdicionarItem(&jogador, ITEM_SOLAR_BROCHE, 1));
    indice = EncontrarItem(&jogador, ITEM_SOLAR_BROCHE);
    assert(indice >= 0 && JogadorEquiparDoInventario(&jogador, indice, ESPACO_AMULETO_TIPO));
    assert(jogador.total_ataques == 6);
    assert(jogador.ataques[0].tipo == TIPO_AGUA && jogador.ataques[3].tipo == TIPO_LUZ);

    assert(JogadorAdicionarItem(&jogador, ITEM_BOTAS_CINZA, 1));
    indice = EncontrarItem(&jogador, ITEM_BOTAS_CINZA);
    assert(indice >= 0 && JogadorEquiparDoInventario(&jogador, indice, ESPACO_BOTAS));
    assert(jogador.bonus_equipamento.velocidade == 8);
    assert(jogador.bonus_equipamento.chance_critica == 8);

    assert(JogadorAdicionarItem(&jogador, ITEM_AMULETO_OLHO, 1));
    indice = EncontrarItem(&jogador, ITEM_AMULETO_OLHO);
    assert(!ItemPodeEquiparNoEspaco(ITEM_AMULETO_OLHO, ESPACO_AMULETO_TIPO));
    assert(indice >= 0 && JogadorEquiparDoInventario(&jogador, indice, ESPACO_AMULETO_1));
    assert(jogador.mana_maxima == 23 && jogador.mana_por_turno == 6);

    assert(JogadorAdicionarItem(&jogador, ITEM_ESCUDO_INQUISIDOR, 1));
    indice = EncontrarItem(&jogador, ITEM_ESCUDO_INQUISIDOR);
    assert(indice >= 0 && JogadorEquiparDoInventario(&jogador, indice, ESPACO_ESCUDO));
    jogador.tipo_defesa = TIPO_FOGO;
    assert(fabsf(JogadorEfetividadeDanoRecebido(&jogador, TIPO_AGUA) - 1.75f) < 0.001f);

    assert(InimigoAplicarSangramento(&inimigo, 5) == 5);
    assert(InimigoProcessarSangramento(&inimigo) == 5 && inimigo.sangramento == 5);
    assert(InimigoAplicarSangramento(&inimigo, 5) == 5);
    assert(InimigoProcessarSangramento(&inimigo) == 10 && inimigo.sangramento == 10);
    InimigoCurarSangramento(&inimigo);
    assert(inimigo.sangramento == 0);

    assert(JogadorAplicarSangramento(&jogador, 5) == 5);
    assert(JogadorProcessarSangramento(&jogador) == 5 && jogador.sangramento == 5);
    JogadorCurarSangramento(&jogador);
    assert(jogador.sangramento == 0);

    puts("Equipamentos, raridade, mana, defesa e sangramento verificados.");
    return 0;
}
