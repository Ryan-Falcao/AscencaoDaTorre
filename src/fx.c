#include "fx.h"

#include <math.h>

void EfeitoIniciar(EfeitoAtaque *efeito, const Ataque *ataque, ResultadoQte resultado)
{
    *efeito = (EfeitoAtaque){true, ataque->id, ataque->estilo_efeito, ataque->tipo, resultado,
                     0.0f, resultado == QTE_ERRO ? 0.25f : 0.48f, {320.0f, 137.0f}};
}

void EfeitoAtualizar(EfeitoAtaque *efeito, float tempo_delta)
{
    if (!efeito->ativo) return;
    efeito->temporizador += tempo_delta;
    if (efeito->temporizador >= efeito->duracao) efeito->ativo = false;
}

static void DesenharAlternativa(const EfeitoAtaque *efeito, float escala, Color cor)
{
    const float progresso = efeito->temporizador / efeito->duracao;
    const Vector2 centro = efeito->estilo == EFEITO_PROJETIL ?
        (Vector2){112.0f + 208.0f * progresso, 300.0f - 163.0f * progresso} : efeito->posicao;

    if (efeito->resultado == QTE_ERRO) {
        for (int indice = 0; indice < 5; indice++) {
            const float a = (float)indice * 1.256f;
            DrawCircleV((Vector2){centro.x + cosf(a) * 14.0f * progresso,
                                  centro.y + sinf(a) * 14.0f * progresso}, 3.0f, GRAY);
        }
    } else if (efeito->estilo == EFEITO_FEIXE) {
        DrawRectangle((int)(centro.x - 80.0f), (int)(centro.y - 4.0f * escala), 160,
                      (int)(8.0f * escala), cor);
    } else if (efeito->estilo == EFEITO_CORTE) {
        DrawLineEx((Vector2){centro.x - 36.0f * escala, centro.y + 28.0f * escala},
                   (Vector2){centro.x + 36.0f * escala, centro.y - 28.0f * escala}, 5.0f, cor);
    } else if (efeito->estilo == EFEITO_CHUVA) {
        for (int indice = 0; indice < 4; indice++) {
            DrawCircle((int)(centro.x - 30.0f + indice * 20.0f),
                       (int)(centro.y - 35.0f + progresso * 65.0f), 5, cor);
        }
    } else {
        DrawCircleLines((int)centro.x, (int)centro.y, 28.0f * escala, cor);
        DrawCircleLines((int)centro.x, (int)centro.y, 18.0f * escala, cor);
    }
}

void EfeitoDesenhar(const EfeitoAtaque *efeito, const SistemaSprites *sprites, bool efeitos_reduzidos)
{
    if (!efeito->ativo) return;
    const float result_scale = efeito->resultado == QTE_PERFEITO ? 1.35f :
                               (efeito->resultado == QTE_RUIM ? 0.75f : 1.0f);
    const Color cor = efeito->resultado == QTE_ERRO ? GRAY : cor_tipo(efeito->tipo);
    IdSprite id = SpriteIdEfeitoAtaque(efeito->id_ataque);

    if (sprite_obter(sprites, id) == NULL) id = SpriteIdEfeitoTipo(efeito->tipo);
    if (!efeitos_reduzidos && efeito->resultado != QTE_ERRO && sprite_obter(sprites, id) != NULL) {
        SpriteDesenharCentralizadoEscalado(sprites, id, efeito->posicao, cor, efeito->temporizador, result_scale,
                                 sprites->depurar_limites);
    } else {
        DesenharAlternativa(efeito, efeitos_reduzidos ? 0.7f : result_scale, cor);
    }
}
