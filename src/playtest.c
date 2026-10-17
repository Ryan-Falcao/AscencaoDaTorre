#include "playtest.h"

#include <stdio.h>

#define CAMINHO_TESTE_JOGO "playtest_log.csv"

static void GravarCabecalhoSeNecessario(FILE *arquivo)
{
    if (ftell(arquivo) == 0) {
        fputs("kind,floor,enemy,outcome,hp_lost,turns,duration_seconds,qte_perfect,qte_good,qte_bad,qte_error,coins\n", arquivo);
    }
}

void TesteJogoRegistrarBatalha(int andar, const Inimigo *inimigo, const ResultadoBatalha *resultado)
{
    FILE *arquivo = fopen(CAMINHO_TESTE_JOGO, "a+");
    if (arquivo == NULL) return;
    GravarCabecalhoSeNecessario(arquivo);
    fprintf(arquivo, "battle,%d,%s,%d,%d,%d,%.2f,%d,%d,%d,%d,%d\n", andar, inimigo->nome,
            resultado->desfecho, resultado->vida_perdida, resultado->turnos, resultado->duracao_segundos,
            resultado->resultados_qte[0], resultado->resultados_qte[1], resultado->resultados_qte[2],
            resultado->resultados_qte[3], resultado->moedas);
    fclose(arquivo);
}

void TesteJogoRegistrarPartida(int andar, int moedas, bool vitoria, float duracao_segundos)
{
    FILE *arquivo = fopen(CAMINHO_TESTE_JOGO, "a+");
    if (arquivo == NULL) return;
    GravarCabecalhoSeNecessario(arquivo);
    fprintf(arquivo, "run,%d,,%s,0,0,%.2f,0,0,0,0,%d\n", andar,
            vitoria ? "victory" : "defeat", duracao_segundos, moedas);
    fclose(arquivo);
}
