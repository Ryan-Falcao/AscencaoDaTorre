#ifndef EFEITO_H
#define EFEITO_H

#include "player.h"
#include "sprites.h"

typedef struct {
    bool ativo;
    IdAtaque id_ataque;
    EstiloEfeitoAtaque estilo;
    Tipo tipo;
    ResultadoQte resultado;
    float temporizador;
    float duracao;
    Vector2 posicao;
} EfeitoAtaque;

void EfeitoIniciar(EfeitoAtaque *efeito, const Ataque *ataque, ResultadoQte resultado);
void EfeitoAtualizar(EfeitoAtaque *efeito, float tempo_delta);
void EfeitoDesenhar(const EfeitoAtaque *efeito, const SistemaSprites *sprites, bool efeitos_reduzidos);

#endif
