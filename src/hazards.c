#include "hazards.h"

#include "sprites.h"
#include "types.h"

#include <math.h>
#include <string.h>

void PerigosInicializar(ConjuntoPerigos *conjunto) { memset(conjunto, 0, sizeof(*conjunto)); }
void PerigosLimpar(ConjuntoPerigos *conjunto) { memset(conjunto, 0, sizeof(*conjunto)); }

static Perigo *ProximoPerigo(ConjuntoPerigos *conjunto)
{
    for (int indice = 0; indice < MAXIMO_PERIGOS; indice++) if (!conjunto->perigos[indice].ativo) return &conjunto->perigos[indice];
    return NULL;
}

static bool AdicionarPerigo(ConjuntoPerigos *conjunto, Perigo perigo)
{
    Perigo *espaco = ProximoPerigo(conjunto);
    if (espaco == NULL) return false;
    *espaco = perigo;
    return true;
}

bool PerigosAdicionarRetangulo(ConjuntoPerigos *conjunto, Rectangle retangulo, float aviso, float ativo, float desvanecer,
                    Tipo tipo, int dano, bool acerta_uma_vez)
{
    return AdicionarPerigo(conjunto, (Perigo){true, PERIGO_RETANGULO, PERIGO_AVISO, retangulo, {0.0f, 0.0f}, 0.0f,
                                    aviso, ativo, desvanecer, 0.0f, tipo, dano, acerta_uma_vez, false, 0.0f, false});
}

bool PerigosAdicionarCirculo(ConjuntoPerigos *conjunto, Vector2 centro, float raio, float aviso, float ativo,
                      float desvanecer, Tipo tipo, int dano, bool acerta_uma_vez)
{
    return AdicionarPerigo(conjunto, (Perigo){true, PERIGO_CIRCULO, PERIGO_AVISO, {0.0f, 0.0f, 0.0f, 0.0f}, centro, raio,
                                    aviso, ativo, desvanecer, 0.0f, tipo, dano, acerta_uma_vez, false, 0.0f, false});
}

bool CirculoPerigoAtingeRetangulo(Vector2 centro, float raio, Rectangle retangulo)
{
    const float x = fmaxf(retangulo.x, fminf(centro.x, retangulo.x + retangulo.width));
    const float y = fmaxf(retangulo.y, fminf(centro.y, retangulo.y + retangulo.height));
    const float dx = centro.x - x, dy = centro.y - y;
    return dx * dx + dy * dy <= raio * raio;
}

bool PerigosAdicionarAnel(ConjuntoPerigos *conjunto, Vector2 centro, float velocidade, float aviso, float ativo,
                    Tipo tipo, int dano)
{
    Perigo ring = {0};
    ring.ativo = true; ring.forma = PERIGO_ANEL; ring.fase = PERIGO_AVISO;
    ring.centro = centro; ring.tempo_aviso = aviso; ring.tempo_ativo = ativo;
    ring.tempo_desvanecer = 0.15f; ring.tipo = tipo; ring.dano = dano;
    ring.acerta_uma_vez = true; ring.velocidade_expansao = velocidade;
    return AdicionarPerigo(conjunto, ring);
}

bool CirculoPerigoAtingeCirculo(Vector2 primeiro, float raio_primeiro, Vector2 segundo, float raio_segundo)
{
    const float dx = primeiro.x - segundo.x, dy = primeiro.y - segundo.y;
    const float range = raio_primeiro + raio_segundo;
    return dx * dx + dy * dy <= range * range;
}

static void AplicarDanoPerigo(Perigo *perigo, AtributosJogador *jogador, float multiplicador_dano)
{
    int dano = (int)ceilf((float)perigo->dano * efetividade_tipo(perigo->tipo, jogador->tipo_defesa) *
                            jogador->multiplicador_dano_recebido * multiplicador_dano);
    if (dano < 1) dano = 1;
    jogador->hp -= dano;
    if (jogador->hp < 0) jogador->hp = 0;
    perigo->ja_acertou = true;
}

void PerigosAtualizar(ConjuntoPerigos *conjunto, AtributosJogador *jogador, Vector2 alma, float raio_alma,
                   float tempo_delta, float multiplicador_dano, bool invencivel)
{
    const bool movendo = conjunto->alma_rastreada &&
        (fabsf(alma.x - conjunto->alma_anterior.x) + fabsf(alma.y - conjunto->alma_anterior.y) > 0.01f);
    conjunto->alma_anterior = alma;
    conjunto->alma_rastreada = true;
    for (int indice = 0; indice < MAXIMO_PERIGOS; indice++) {
        Perigo *perigo = &conjunto->perigos[indice];
        if (!perigo->ativo) continue;
        perigo->temporizador += tempo_delta;
        if (perigo->fase == PERIGO_AVISO && perigo->temporizador >= perigo->tempo_aviso) {
            perigo->fase = PERIGO_ATIVA; perigo->temporizador = 0.0f;
            continue; /* O proprio quadro de aviso nunca causa dano. */
        } else if (perigo->fase == PERIGO_ATIVA && perigo->temporizador >= perigo->tempo_ativo) {
            perigo->fase = PERIGO_DESVANECER; perigo->temporizador = 0.0f;
        } else if (perigo->fase == PERIGO_DESVANECER && perigo->temporizador >= perigo->tempo_desvanecer) {
            perigo->ativo = false; continue;
        }
        if (perigo->fase == PERIGO_ATIVA && perigo->forma == PERIGO_ANEL)
            perigo->raio = perigo->temporizador * perigo->velocidade_expansao;
        if (perigo->fase != PERIGO_ATIVA || invencivel || (perigo->acerta_uma_vez && perigo->ja_acertou) ||
            (perigo->apenas_em_movimento && !movendo)) continue;
        const float dx = alma.x - perigo->centro.x, dy = alma.y - perigo->centro.y;
        const bool hit = perigo->forma == PERIGO_ANEL ?
            fabsf(sqrtf(dx * dx + dy * dy) - perigo->raio) <= raio_alma + 5.0f :
            perigo->forma == PERIGO_RETANGULO ?
            CirculoPerigoAtingeRetangulo(alma, raio_alma, perigo->retangulo) :
            CirculoPerigoAtingeCirculo(alma, raio_alma, perigo->centro, perigo->raio);
        if (hit) { AplicarDanoPerigo(perigo, jogador, multiplicador_dano); invencivel = true; }
    }
    (void)multiplicador_dano;
}

void PerigosDesenhar(const ConjuntoPerigos *conjunto, bool efeitos_reduzidos, const SistemaSprites *sprites,
                 bool depurar_colisoes)
{
    for (int indice = 0; indice < MAXIMO_PERIGOS; indice++) {
        const Perigo *perigo = &conjunto->perigos[indice];
        if (!perigo->ativo) continue;
        const Color cor = cor_tipo(perigo->tipo);
        const float alpha = perigo->fase == PERIGO_AVISO ? 0.45f : (perigo->fase == PERIGO_DESVANECER ? 0.35f : 0.85f);
        if (perigo->forma == PERIGO_ANEL) {
            const float raio = perigo->fase == PERIGO_AVISO ? 12.0f : fmaxf(1.0f, perigo->raio);
            DrawRing(perigo->centro, fmaxf(0.0f, raio - 4.0f), raio + 4.0f,
                     0, 360, 64, Fade(cor, alpha));
        } else if (perigo->forma == PERIGO_RETANGULO) {
            const IdSprite thorn = perigo->fase == PERIGO_AVISO ? SPRITE_ESPINHO_PAREDE_AVISO : SPRITE_ESPINHO_PAREDE;
            if (sprite_obter(sprites, thorn) != NULL) SpriteDesenharAjustado(sprites, thorn, perigo->retangulo, WHITE);
            else if (perigo->fase == PERIGO_AVISO) DrawRectangleLinesEx(perigo->retangulo, 2.0f, Fade(cor, alpha));
            else DrawRectangleRec(perigo->retangulo, Fade(cor, alpha));
        } else {
            if (perigo->fase == PERIGO_AVISO)
                DrawCircleLines((int)perigo->centro.x, (int)perigo->centro.y, perigo->raio, YELLOW);
            const float progresso = fmaxf(0.18f, fminf(perigo->temporizador / perigo->tempo_aviso, 1.0f));
            const float raio = perigo->fase == PERIGO_AVISO ? perigo->raio * progresso : perigo->raio;
            if (perigo->fase == PERIGO_AVISO) {
                if (sprite_obter(sprites, SPRITE_SOMBRA_ORBE) != NULL) {
                    SpriteDesenharCentralizadoEscalado(sprites, SPRITE_SOMBRA_ORBE, perigo->centro, WHITE,
                                            perigo->temporizador, raio / 16.0f, depurar_colisoes);
                } else DrawCircleV(perigo->centro, raio * 0.16f, Fade(cor, 0.9f));
                if (sprite_obter(sprites, SPRITE_SOMBRA_FLORACAO_AVISO) != NULL) {
                    SpriteDesenharCentralizadoEscalado(sprites, SPRITE_SOMBRA_FLORACAO_AVISO, perigo->centro, WHITE,
                                            perigo->temporizador, raio / 32.0f, depurar_colisoes);
                } else DrawCircleLines((int)perigo->centro.x, (int)perigo->centro.y, raio, Fade(cor, alpha));
            } else {
                if (sprite_obter(sprites, SPRITE_SOMBRA_EXPLOSAO) != NULL && !efeitos_reduzidos) {
                    SpriteDesenharCentralizadoEscalado(sprites, SPRITE_SOMBRA_EXPLOSAO, perigo->centro, WHITE,
                                            perigo->temporizador, raio / 48.0f, depurar_colisoes);
                } else DrawCircleV(perigo->centro, raio, Fade(cor, alpha));
            }
        }
        if (depurar_colisoes) {
            if (perigo->forma == PERIGO_RETANGULO) DrawRectangleLinesEx(perigo->retangulo, 1.0f, MAGENTA);
            else DrawCircleLines((int)perigo->centro.x, (int)perigo->centro.y, perigo->raio, MAGENTA);
        }
    }
}
