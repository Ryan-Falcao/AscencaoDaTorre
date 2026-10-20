#ifndef PADROES_H
#define PADROES_H
#include "bullets.h"

bool PadraoEhCaracteristico(IdPadraoProjetil id);
const char *InstrucaoPadrao(IdPadraoProjetil id);
void PadraoAtualizar(EstadoDefesa *defesa, float tempo_delta);
void PadraoDefinirSegundaFase(EstadoDefesa *defesa);
void PadraoDesenhar(const EstadoDefesa *defesa, const SistemaSprites *sprites);
#endif
