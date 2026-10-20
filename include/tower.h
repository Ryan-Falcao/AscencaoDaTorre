#ifndef TORRE_H
#define TORRE_H

#include "enemy.h"
#include "items.h"

#define TOTAL_ANDARES_TORRE 30
#define TORRE_RECOMPENSA_OPCOES 3

typedef enum {
    RECOMPENSA_EQUIPAMENTO,
    RECOMPENSA_ITEM,
    RECOMPENSA_VIDA,
    RECOMPENSA_DANO
} TipoRecompensaTorre;

typedef struct {
    TipoRecompensaTorre categoria;
    int id;
    int valor;
} RecompensaTorre;

typedef struct {
    int andar;
    unsigned int semente;
    IdInimigo inimigos[TOTAL_ANDARES_TORRE];
    int moedas;
    int vitorias_misericordia;
    int vitorias_dano;
    RecompensaTorre recompensas[TORRE_RECOMPENSA_OPCOES];
    IdItem itens_loja[3];
    int precos_loja[3];
} Torre;

void TorreInicializar(Torre *torre, unsigned int semente);
bool TorreAndarDeChefe(int andar);
IdInimigo TorreInimigoDoAndar(const Torre *torre, int andar);
void TorreEscalarInimigo(Inimigo *inimigo, int andar);
void TorreSortearRecompensas(Torre *torre, const AtributosJogador *jogador);
void TorreSortearLoja(Torre *torre);
bool TorreDeveDescansar(int andar_concluido);
bool TorreDeveTerLoja(int andar_concluido);

#endif
