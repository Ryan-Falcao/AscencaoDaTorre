#ifndef BALANCEAMENTO_H
#define BALANCEAMENTO_H

#include "save.h"

/* Todos os numeros que alteram a dificuldade de uma run ficam aqui. */
#define BALANCEAMENTO_COMUM_VIDA_PER_ANDAR 3
#define BALANCEAMENTO_CHEFE_VIDA_PER_ANDAR 7
#define BALANCEAMENTO_CHEFE_PRESSAO 0.16f
#define BALANCEAMENTO_PROJETIL_BONUS_A_CADA 3
#define BALANCEAMENTO_LOJA_BASE_PRECO 5
#define BALANCEAMENTO_LOJA_PRECO_VARIACAO 7

/* Ataques defensivos especiais do Sineiro. Os valores sao conservadores de proposito: a faixa de
   fuga tem 48px, menos de um terco ate da menor arena, com 270px. */
#define BALANCEAMENTO_GRAVIDADE_FUGA_TEMPO 1.15f
#define BALANCEAMENTO_GRAVIDADE_FUGA_TEMPO_FASE_DOIS 0.85f
#define BALANCEAMENTO_GRAVIDADE_FORCA 430.0f
#define BALANCEAMENTO_GRAVIDADE_ESPINHO_PROFUNDIDADE 48.0f
#define BALANCEAMENTO_GRAVIDADE_ESPINHO_ATIVA_TEMPO 0.65f
#define BALANCEAMENTO_GRAVIDADE_ESPINHO_DESVANECER_TEMPO 0.12f
#define BALANCEAMENTO_GRAVIDADE_DANO 6
#define BALANCEAMENTO_SOMBRA_FLORACAO_TEMPO_CRESCIMENTO 1.0f
#define BALANCEAMENTO_SOMBRA_FLORACAO_ATRASO 0.13f
#define BALANCEAMENTO_SOMBRA_FLORACAO_RAIO 56.0f
#define BALANCEAMENTO_SOMBRA_FLORACAO_DANO 6

static inline float BalanceamentoMultiplicadorDanoRecebido(DificuldadeJogo dificuldade)
{
    static const float valores[] = {0.75f, 1.00f, 1.25f};
    return valores[dificuldade < DIFICULDADE_CALMA || dificuldade > DIFICULDADE_DIFICIL
                      ? DIFICULDADE_NORMAL : dificuldade];
}

static inline int BalanceamentoDificuldadeQte(int andar, DificuldadeJogo dificuldade)
{
    const int deslocamento = dificuldade == DIFICULDADE_CALMA ? -1 :
                       (dificuldade == DIFICULDADE_DIFICIL ? 1 : 0);
    const int valor = andar + deslocamento;
    return valor < 1 ? 1 : valor;
}

/* Mantem os andares 1-4 ameacadores e preserva 60-80% da vida em lutas curtas bem jogadas. */
static inline float BalanceamentoMultiplicadorDanoAndar(int andar)
{
    static const float valores_iniciais[] = {3.0f, 2.3f, 1.7f, 1.7f};
    return andar >= 1 && andar <= 4 ? valores_iniciais[andar - 1] : 1.0f;
}

#endif
