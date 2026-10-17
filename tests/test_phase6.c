#include <assert.h>
#include <stdio.h>

#include "enemy.h"
#include "player.h"

int main(void)
{
    Inimigo mariposa = InimigoCriar(INIMIGO_POEIRA_MARIPOSA);
    Inimigo sineiro = InimigoCriar(INIMIGO_CHEFE_SINEIRO);
    Inimigo tired_moth;
    AtributosJogador jogador;
    const DefinicaoItem *item_usado;
    int valor_aplicado;
    char mensagem[128];

    bool possui_gas_sombra = false;
    for (int indice = 0; indice < sineiro.total_padroes; indice++)
        if (sineiro.ids_padroes[indice] == PADRAO_SINO_GAS_SOMBRA) possui_gas_sombra = true;
    assert(sineiro.total_padroes == 5);
    assert(possui_gas_sombra);

    assert(!InimigoAcaoDisponivel(&mariposa, 1, mensagem, (int)sizeof(mensagem)));
    assert(InimigoAplicarAcao(&mariposa, 0, mensagem, (int)sizeof(mensagem)));
    assert(mariposa.analisado);
    assert(!InimigoAcaoDisponivel(&mariposa, 0, mensagem, (int)sizeof(mensagem)));
    assert(InimigoAplicarAcao(&mariposa, 1, mensagem, (int)sizeof(mensagem)));
    assert(mariposa.misericordia == 30 && mariposa.agressividade == 35);
    assert(!InimigoAcaoDisponivel(&mariposa, 1, mensagem, (int)sizeof(mensagem)));
    InimigoAvancarTurno(&mariposa);
    assert(!InimigoAcaoDisponivel(&mariposa, 1, mensagem, (int)sizeof(mensagem)));
    InimigoAvancarTurno(&mariposa);
    assert(InimigoAplicarAcao(&mariposa, 1, mensagem, (int)sizeof(mensagem)));
    assert(mariposa.misericordia == 45); /* O segundo Elogiar seguido cai de +30 para +15. */
    InimigoAvancarTurno(&mariposa);
    InimigoAvancarTurno(&mariposa);
    assert(!InimigoAcaoDisponivel(&mariposa, 1, mensagem, (int)sizeof(mensagem)));

    mariposa.misericordia = 95;
    mariposa.agressividade = 90;
    assert(InimigoAplicarAcao(&mariposa, 2, mensagem, (int)sizeof(mensagem)));
    assert(mariposa.misericordia == 85 && mariposa.agressividade == 100);
    mariposa.misericordia = 100;
    InimigoAtualizarPoupavel(&mariposa);
    assert(!InimigoPodePoupar(&mariposa));
    mariposa.agressividade = 66;
    assert(InimigoPodePoupar(&mariposa));

    tired_moth = InimigoCriar(INIMIGO_POEIRA_MARIPOSA);
    InimigoReceberDano(&tired_moth, 28, 1.0f);
    assert(tired_moth.esgotado && tired_moth.poupavel);

    JogadorInicializar(&jogador);
    assert(JogadorUsarItem(&jogador, 1, &item_usado, &valor_aplicado));
    assert(item_usado->efeito == ITEM_EFEITO_DANO_BUFF);
    assert(JogadorMultiplicadorBuffAtaque(&jogador) == 1.5f);
    JogadorConsumirBuffAtaque(&jogador);
    assert(JogadorMultiplicadorBuffAtaque(&jogador) == 1.0f);

    puts("Mercy, agressividade, repeticao, cooldown, exaustao e buffs verificados.");
    return 0;
}
