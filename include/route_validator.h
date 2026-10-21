#ifndef ROUTE_VALIDATOR_H
#define ROUTE_VALIDATOR_H
#include "pilot_patterns.h"
#define ROTA_MAXIMO_PASSOS 2048
typedef struct {
    EntradaDefesa entradas[ROTA_MAXIMO_PASSOS];
    int total, fps;
    Vector2 inicio;
    Rectangle caixa;
    bool encontrada;
} RotaValidada;
bool PilotoEncontrarRota(IdPadraoProjetil id, Rectangle caixa, Vector2 inicio, int fps, RotaValidada *rota);
bool PilotoReproduzirRota(IdPadraoProjetil id, const RotaValidada *rota, int vida_maxima, bool segunda_fase, int *dano);
#endif
