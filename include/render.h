#ifndef RENDERIZACAO_H
#define RENDERIZACAO_H

#include <stdbool.h>

#include "raylib.h"

typedef struct {
    Color fundo;
    Color flutuante;
    Color papel;
    bool chefe;
} PaletaAndar;

typedef struct {
    PaletaAndar atual;
    PaletaAndar alvo;
    float transicao;
    bool tremor_linha;
} EstadoRenderizacao;

void RenderizacaoInicializar(EstadoRenderizacao *renderizacao, bool tremor_linha);
void RenderizacaoDefinirAndar(EstadoRenderizacao *renderizacao, int andar, bool chefe);
void RenderizacaoAtualizar(EstadoRenderizacao *renderizacao, float tempo_delta);
void RenderizacaoDesenharFundo(const EstadoRenderizacao *renderizacao, float tempo);

#endif
