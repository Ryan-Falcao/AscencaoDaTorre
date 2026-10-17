#ifndef TESTE_JOGO_H
#define TESTE_JOGO_H

#include "battle.h"

void TesteJogoRegistrarBatalha(int andar, const Inimigo *inimigo, const ResultadoBatalha *resultado);
void TesteJogoRegistrarPartida(int andar, int moedas, bool vitoria, float duracao_segundos);

#endif
