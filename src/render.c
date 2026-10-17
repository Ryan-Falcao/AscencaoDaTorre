#include "render.h"
#include "palettes.h"

#include <math.h>

static unsigned char MisturarByte(unsigned char origem, unsigned char destino, float quantidade)
{
    return (unsigned char)((float)origem + ((float)destino - (float)origem) * quantidade);
}

static Color MisturarCor(Color origem, Color destino, float quantidade)
{
    return (Color){MisturarByte(origem.r, destino.r, quantidade), MisturarByte(origem.g, destino.g, quantidade),
                   MisturarByte(origem.b, destino.b, quantidade), 255};
}

void RenderizacaoInicializar(EstadoRenderizacao *renderizacao, bool tremor_linha)
{
    renderizacao->atual = ANDAR_PALETAS[0];
    renderizacao->alvo = ANDAR_PALETAS[0];
    renderizacao->transicao = 1.0f;
    renderizacao->tremor_linha = tremor_linha;
}

void RenderizacaoDefinirAndar(EstadoRenderizacao *renderizacao, int andar, bool chefe)
{
    renderizacao->alvo = chefe ? CHEFE_ANDAR_PALETTE : ANDAR_PALETAS[(andar - 1) % ANDAR_PALETTE_TOTAL];
    renderizacao->transicao = 0.0f;
}

void RenderizacaoAtualizar(EstadoRenderizacao *renderizacao, float tempo_delta)
{
    if (renderizacao->transicao < 1.0f) {
        renderizacao->transicao += tempo_delta * 1.4f;
        if (renderizacao->transicao >= 1.0f) {
            renderizacao->transicao = 1.0f;
            renderizacao->atual = renderizacao->alvo;
        }
    }
}

void RenderizacaoDesenharFundo(const EstadoRenderizacao *renderizacao, float tempo)
{
    const Color fundo = MisturarCor(renderizacao->atual.fundo, renderizacao->alvo.fundo,
                                        renderizacao->transicao);
    const Color flutuante = MisturarCor(renderizacao->atual.flutuante, renderizacao->alvo.flutuante,
                                      renderizacao->transicao);

    ClearBackground(fundo);
    for (int indice = 0; indice < 28; indice++) {
        const int x = (indice * 71) % 640;
        const int y = (indice * 43) % 480;
        DrawCircle(x, y, 1, renderizacao->atual.papel);
    }
    if (!renderizacao->atual.chefe) {
        DrawEllipse(100, 80 + (int)(sinf(tempo * 0.4f) * 4.0f), 70, 15, flutuante);
        DrawEllipse(520, 150 + (int)(cosf(tempo * 0.3f) * 4.0f), 82, 16, flutuante);
    }
}
