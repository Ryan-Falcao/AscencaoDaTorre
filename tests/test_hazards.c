#include "hazards.h"
#include "balance.h"

#include <assert.h>

int main(void)
{
    AtributosJogador jogador;
    ConjuntoPerigos conjunto;
    const Vector2 alma = {100.0f, 100.0f};

    JogadorInicializar(&jogador);
    const int vida_inicial = jogador.hp;
    PerigosInicializar(&conjunto);
    assert(PerigosAdicionarCirculo(&conjunto, alma, 20.0f, 0.10f, 0.20f, 0.10f, TIPO_SOMBRA, 5, true));
    PerigosAtualizar(&conjunto, &jogador, alma, 4.0f, 0.10f, 1.0f, false);
    assert(jogador.hp == vida_inicial); /* o aviso nunca causa dano */
    PerigosAtualizar(&conjunto, &jogador, alma, 4.0f, 0.01f, 1.0f, false);
    assert(jogador.hp < vida_inicial);
    const int apos_acerto = jogador.hp;
    PerigosAtualizar(&conjunto, &jogador, alma, 4.0f, 0.05f, 1.0f, false);
    assert(jogador.hp == apos_acerto); /* acerta_uma_vez */

    PerigosLimpar(&conjunto);
    assert(PerigosAdicionarRetangulo(&conjunto, (Rectangle){90.0f, 90.0f, 20.0f, 20.0f}, 0.0f, 0.2f, 0.1f,
                          TIPO_PLANTA, 4, true));
    PerigosAtualizar(&conjunto, &jogador, alma, 4.0f, 0.01f, 1.0f, false);
    PerigosAtualizar(&conjunto, &jogador, alma, 4.0f, 0.01f, 1.0f, false);
    assert(jogador.hp < apos_acerto);
    assert(!CirculoPerigoAtingeRetangulo((Vector2){10.0f, 10.0f}, 2.0f, (Rectangle){30, 30, 10, 10}));
    assert(CirculoPerigoAtingeCirculo(alma, 4.0f, (Vector2){105.0f, 100.0f}, 2.0f));

    PerigosLimpar(&conjunto);
    for (int indice = 0; indice < MAXIMO_PERIGOS; indice++) {
        assert(PerigosAdicionarCirculo(&conjunto, alma, 2.0f, 1.0f, 1.0f, 1.0f, TIPO_NEUTRO, 1, true));
    }
    assert(!PerigosAdicionarCirculo(&conjunto, alma, 2.0f, 1.0f, 1.0f, 1.0f, TIPO_NEUTRO, 1, true));
    assert(BALANCEAMENTO_GRAVIDADE_ESPINHO_PROFUNDIDADE <= 270.0f / 3.0f);
    assert(BALANCEAMENTO_GRAVIDADE_FUGA_TEMPO * 135.0f > BALANCEAMENTO_GRAVIDADE_ESPINHO_PROFUNDIDADE * 2.0f);

    /* O olhar do gato pune movimento, nunca uma alma parada. */
    JogadorInicializar(&jogador); PerigosLimpar(&conjunto);
    PerigosAdicionarRetangulo(&conjunto, (Rectangle){0, 0, 200, 200}, .1f, 1, .1f, TIPO_NEUTRO, 4, true);
    conjunto.perigos[0].apenas_em_movimento = true;
    PerigosAtualizar(&conjunto, &jogador, alma, 4, .1f, 1, false);
    PerigosAtualizar(&conjunto, &jogador, alma, 4, .1f, 1, false);
    assert(jogador.hp == jogador.vida_maxima);
    PerigosAtualizar(&conjunto, &jogador, (Vector2){102, 100}, 4, .1f, 1, false);
    assert(jogador.hp == jogador.vida_maxima - 4);

    /* Um eco e um anel vazio: seu centro fica seguro DEPOIS da passagem da onda. */
    JogadorInicializar(&jogador); PerigosLimpar(&conjunto);
    PerigosAdicionarAnel(&conjunto, alma, 80, .1f, 1, TIPO_NEUTRO, 4);
    PerigosAtualizar(&conjunto, &jogador, alma, 4, .1f, 1, false);
    PerigosAtualizar(&conjunto, &jogador, alma, 4, .5f, 1, false);
    assert(jogador.hp == jogador.vida_maxima);
    PerigosAtualizar(&conjunto, &jogador, (Vector2){148, 100}, 4, .1f, 1, false);
    assert(jogador.hp == jogador.vida_maxima - 4);

    /* Cortes de garra sobrepostos nao multiplicam o dano em um unico quadro. */
    JogadorInicializar(&jogador); PerigosLimpar(&conjunto);
    for (int i = 0; i < 3; i++)
        PerigosAdicionarCirculo(&conjunto, alma, 20, .1f, 1, .1f, TIPO_NEUTRO, 4, true);
    PerigosAtualizar(&conjunto, &jogador, alma, 4, .1f, 1, false);
    PerigosAtualizar(&conjunto, &jogador, alma, 4, .1f, 1, false);
    assert(jogador.hp == jogador.vida_maxima - 4);
    return 0;
}
