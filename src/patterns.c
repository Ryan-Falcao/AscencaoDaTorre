#include "patterns.h"
#include "motion_patterns.h"
#include "raymath.h"
#include <math.h>

bool PadraoEhCaracteristico(IdPadraoProjetil id)
{
    return id >= PADRAO_MARIPOSA_ASAS && id < PADRAO_TOTAL;
}

const char *InstrucaoPadrao(IdPadraoProjetil id)
{
    if (PadraoMovimentoEh(id)) return InstrucaoPadraoMovimento(id);
    switch (id) {
        case PADRAO_MARIPOSA_ASAS: return "ASAS: atravesse a abertura; saia da poeira marcada";
        case PADRAO_SAPO_SALTOS: return "SAPO: saia da sombra; desvie da lingua";
        case PADRAO_PASSARO_MERGULHO: return "MERGULHO: saia da rota das penas em chamas";
        case PADRAO_LIMO_CRESCIMENTO: return "BROTO: abandone a poca antes que ela cresca";
        case PADRAO_FAGULHA_LUZ: return "LANTERNA: entre no quadrado de luz";
        case PADRAO_LEBRE_PULOS: return "LEBRE: tres saltos; nao siga as pegadas";
        case PADRAO_GATO_EMBOSCADA: return "GATO: PARE no olhar; saia das garras";
        case PADRAO_CAVALEIRO_DUELO: return "DUELO: corte vertical, depois horizontal";
        case PADRAO_SINO_ECO: return "SINO: ecos alternados; nao pare na primeira fuga";
        case PADRAO_TORRE_ESMAGAMENTO: return "PRENSA: troque de eixo; cuidado com o desabamento";
        default: return "Desvie dos projeteis";
    }
}

static void RetanguloPerigo(EstadoDefesa *d, Rectangle r, float aviso, float ativo)
{
    if (r.width > 0 && r.height > 0)
        PerigosAdicionarRetangulo(&d->perigos, r, aviso, ativo, 0.12f,
                       d->padrao.tipo, d->padrao.dano, true);
}

static void CirculoPerigo(EstadoDefesa *d, Vector2 p, float raio, float aviso, float ativo)
{
    PerigosAdicionarCirculo(&d->perigos, p, raio, aviso, ativo, 0.12f,
                     d->padrao.tipo, d->padrao.dano, true);
}

/* Um destino deslocado da alma exige reacao, mas nunca pede mais de 70 pixels
   de percurso (0,52 s na velocidade normal). */
static float ProximaAbertura(float valor, float baixo, float alto, int ciclo)
{
    float destino = valor + (ciclo % 2 == 0 ? 70.0f : -70.0f);
    if (destino > alto || destino < baixo)
        destino = valor + (ciclo % 2 == 0 ? -70.0f : 70.0f);
    return Clamp(destino, baixo, alto);
}

static void IniciarCiclo(EstadoDefesa *d)
{
    Rectangle b = d->caixa;
    Vector2 p = d->posicao_alma;
    float w = d->aviso_ataque;
    int ciclo = d->total_criacoes++;
    d->origem_ataque = d->alvo_ataque;
    d->alvo_ataque = p; /* Trava a mira agora. Nunca persegue durante o impacto. */

    switch (d->id_padrao) {
        case PADRAO_MARIPOSA_ASAS:
            if (ciclo % 2 == 0) {
                float abertura = ProximaAbertura(p.y, b.y + 34, b.y + b.height - 34, ciclo / 2);
                d->alvo_ataque.y = abertura;
                RetanguloPerigo(d, (Rectangle){b.x, b.y, b.width, abertura - 20 - b.y}, w, .45f);
                RetanguloPerigo(d, (Rectangle){b.x, abertura + 20, b.width, b.y + b.height - abertura - 20}, w, .45f);
            } else {
                CirculoPerigo(d, p, 46, w, .7f);
                CirculoPerigo(d, (Vector2){Clamp(p.x + (ciclo % 4 == 1 ? 58 : -58), b.x + 25, b.x + b.width - 25), p.y},
                       24, w + .25f, .45f);
            }
            break;
        case PADRAO_SAPO_SALTOS:
            if (ciclo % 2 == 0) {
                CirculoPerigo(d, p, 46, w, .35f);
                /* O impacto e uma onda expansiva, nao um disco de projetil preenchido. */
                PerigosAdicionarAnel(&d->perigos, p, 118, w + .35f, .65f,
                               d->padrao.tipo, d->padrao.dano);
            } else {
                RetanguloPerigo(d, (Rectangle){b.x, p.y - 18, b.width, 36}, w, .32f);
                CirculoPerigo(d, p, 34, w + .3f, .4f);
            }
            break;
        case PADRAO_PASSARO_MERGULHO:
            /* Um mergulho escalonado, com trajeto visivel e rastro em chamas. */
            d->origem_ataque = (Vector2){ciclo % 2 ? b.x + b.width - 12 : b.x + 12, b.y + 12};
            for (int i = 0; i < 5; i++) {
                Vector2 point = Vector2Lerp(d->origem_ataque, p, (float)i / 4);
                CirculoPerigo(d, point, 36, w + (float)i * .10f, .4f);
            }
            break;
        case PADRAO_LIMO_CRESCIMENTO:
            /* Cada broto se divide; a fuga precisa evitar as duas pocas resultantes. */
            CirculoPerigo(d, p, 48, w, .75f);
            {
                for (int side = -1; side <= 1; side += 2)
                    CirculoPerigo(d, (Vector2){Clamp(p.x + side * 48, b.x + 20, b.x + b.width - 20), p.y},
                           30, w + .3f, .45f);
            }
            break;
        case PADRAO_FAGULHA_LUZ: {
            float x = ProximaAbertura(p.x, b.x + 42, b.x + b.width - 42, ciclo);
            float y = Clamp(p.y, b.y + 42, b.y + b.height - 42);
            d->alvo_ataque = (Vector2){x, y};
            RetanguloPerigo(d, (Rectangle){b.x, b.y, x - 24 - b.x, b.height}, w, .55f);
            RetanguloPerigo(d, (Rectangle){x + 24, b.y, b.x + b.width - x - 24, b.height}, w, .55f);
            RetanguloPerigo(d, (Rectangle){x - 24, b.y, 48, y - 24 - b.y}, w, .55f);
            RetanguloPerigo(d, (Rectangle){x - 24, y + 24, 48, b.y + b.height - y - 24}, w, .55f);
            break;
        }
        case PADRAO_LEBRE_PULOS:
            for (int hop = 0; hop < 3; hop++) {
                float x = Clamp(p.x + (hop - 1) * 52, b.x + 24, b.x + b.width - 24);
                CirculoPerigo(d, (Vector2){x, p.y + (hop == 1 ? 16 : -8)}, 36, w + hop * .23f, .32f);
            }
            break;
        case PADRAO_GATO_EMBOSCADA:
            if (ciclo % 2 == 0) {
                RetanguloPerigo(d, b, w, .5f);
                for (int i = 0; i < MAXIMO_PERIGOS; i++) {
                    Perigo *h = &d->perigos.perigos[i];
                    if (h->ativo) h->apenas_em_movimento = true;
                }
            } else {
                /* Duas marcas de garra perpendiculares; os cantos continuam abertos. */
                RetanguloPerigo(d, (Rectangle){p.x - 29, b.y, 58, b.height}, w, .35f);
                RetanguloPerigo(d, (Rectangle){b.x, p.y - 29, b.width, 58}, w, .35f);
            }
            break;
        case PADRAO_CAVALEIRO_DUELO:
            RetanguloPerigo(d, (Rectangle){p.x - 35, b.y, 70, b.height}, w, .28f);
            RetanguloPerigo(d, (Rectangle){b.x, p.y - 35, b.width, 70}, w + .45f, .28f);
            break;
        case PADRAO_SINO_ECO:
            PerigosAdicionarAnel(&d->perigos, p, 124, w, .85f, d->padrao.tipo, d->padrao.dano);
            if (d->ataque_segunda_fase) {
                Vector2 echo = {p.x < b.x + b.width / 2 ? b.x + b.width - 12 : b.x + 12,
                                Clamp(p.y, b.y + 12, b.y + b.height - 12)};
                PerigosAdicionarAnel(&d->perigos, echo, 124, w + .22f, .85f,
                               d->padrao.tipo, d->padrao.dano);
            }
            break;
        case PADRAO_TORRE_ESMAGAMENTO:
            if (d->ataque_segunda_fase && ciclo % 3 == 2) {
                CirculoPerigo(d, p, 46, w, .5f);
                break;
            }
            if ((d->ataque_segunda_fase ? ciclo % 3 : ciclo % 2) == 0) {
                float abertura = ProximaAbertura(p.x, b.x + 42, b.x + b.width - 42, ciclo / 2);
                float metade = d->ataque_segunda_fase ? 19 : 21;
                d->alvo_ataque.x = abertura;
                RetanguloPerigo(d, (Rectangle){b.x, b.y, abertura - metade - b.x, b.height}, w, .55f);
                RetanguloPerigo(d, (Rectangle){abertura + metade, b.y, b.x + b.width - abertura - metade, b.height}, w, .55f);
            } else {
                float abertura = ProximaAbertura(p.y, b.y + 36, b.y + b.height - 36, ciclo / 2);
                float metade = d->ataque_segunda_fase ? 19 : 21;
                d->alvo_ataque.y = abertura;
                RetanguloPerigo(d, (Rectangle){b.x, b.y, b.width, abertura - metade - b.y}, w, .55f);
                RetanguloPerigo(d, (Rectangle){b.x, abertura + metade, b.width, b.y + b.height - abertura - metade}, w, .55f);
            }
            break;
        default: break;
    }
}

void PadraoDefinirSegundaFase(EstadoDefesa *d)
{
    d->ataque_segunda_fase = true;
    if (PadraoMovimentoEh(d->id_padrao)) {
        d->padrao.duracao = 12.6f;
        return;
    }
    /* Mudancas reais de fase: ecos pareados ou prensas alternadas com alvenaria caindo. */
    d->aviso_ataque = fmaxf(.62f, d->aviso_ataque - .10f);
    d->ciclo_ataque = d->aviso_ataque + (d->id_padrao == PADRAO_SINO_ECO ? 1.22f : .70f);
    d->padrao.duracao = 12.5f;
}

void PadraoAtualizar(EstadoDefesa *d, float tempo_delta)
{
    /* Ciclos sem sobreposicao impedem que novos ataques fechem uma rota anterior.
       Reducoes de velocidade por status alongam o aviso e nunca o encurtam. */
    if (d->total_criacoes == 0 || d->temporizador_especial >= d->ciclo_ataque) {
        if (d->padrao.duracao - d->temporizador_padrao < d->ciclo_ataque) {
            /* Nao deixa uma pausa vazia depois da ultima sequencia completa. */
            d->temporizador_padrao = d->padrao.duracao;
            return;
        }
        PerigosLimpar(&d->perigos);
        d->temporizador_especial = 0;
        IniciarCiclo(d);
    }
    d->temporizador_especial += tempo_delta;
}

static void DesenharPerigosCaracteristicos(const EstadoDefesa *d)
{
    for (int i = 0; i < MAXIMO_PERIGOS; i++) {
        const Perigo *h = &d->perigos.perigos[i];
        if (!h->ativo) continue;
        bool aviso = h->fase == PERIGO_AVISO;
        Color cor = h->apenas_em_movimento ? SKYBLUE : cor_tipo(h->tipo);
        if (h->fase == PERIGO_DESVANECER) cor = Fade(cor, .25f);
        if (h->forma == PERIGO_ANEL) {
            if (aviso) {
                DrawCircleLines((int)h->centro.x, (int)h->centro.y, h->velocidade_expansao * h->tempo_ativo,
                                Fade(GOLD, .4f));
                DrawCircleLines((int)h->centro.x, (int)h->centro.y, 10, GOLD);
            } else {
                float raio = fmaxf(1, h->raio);
                DrawRing(h->centro, fmaxf(0, raio - 4), raio + 4, 0, 360, 64, cor);
            }
        } else if (h->forma == PERIGO_RETANGULO) {
            Rectangle r = h->retangulo;
            DrawRectangleRec(r, Fade(cor, aviso ? .12f : .65f));
            DrawRectangleLinesEx(r, aviso ? 2 : 3, aviso ? YELLOW : cor);
            if (h->apenas_em_movimento) {
                DrawRectangleLinesEx(r, 3, SKYBLUE);
            } else if (d->id_padrao == PADRAO_CAVALEIRO_DUELO || d->id_padrao == PADRAO_GATO_EMBOSCADA) {
                /* Golpes de espada e garra continuam visualmente diferentes de paredes. */
                bool vertical = r.height > r.width;
                for (int slash = 0; slash < (d->id_padrao == PADRAO_GATO_EMBOSCADA ? 3 : 1); slash++) {
                    float deslocamento = (slash - 1) * 7.0f;
                    Vector2 a = vertical ? (Vector2){r.x + r.width / 2 + deslocamento, r.y} :
                                           (Vector2){r.x, r.y + r.height / 2 + deslocamento};
                    Vector2 b = vertical ? (Vector2){a.x, r.y + r.height} : (Vector2){r.x + r.width, a.y};
                    DrawLineEx(a, b, aviso ? 1 : 4, WHITE);
                }
            } else if (d->id_padrao == PADRAO_TORRE_ESMAGAMENTO) {
                for (float y = r.y + 16; y < r.y + r.height; y += 20)
                    DrawLine((int)r.x, (int)y, (int)(r.x + r.width), (int)y, Fade(GRAY, .7f));
                for (float x = r.x + 20; x < r.x + r.width; x += 30)
                    DrawLine((int)x, (int)r.y, (int)x, (int)(r.y + r.height), Fade(GRAY, .7f));
            }
        } else {
            Vector2 p = h->centro;
            float growth = aviso ? Clamp(h->temporizador / h->tempo_aviso, 0, 1) : 1;
            /* Sempre mostra o limite final da colisao, inclusive durante o crescimento. */
            DrawCircleLines((int)p.x, (int)p.y, h->raio, aviso ? YELLOW : cor);
            if (d->id_padrao == PADRAO_LIMO_CRESCIMENTO) {
                DrawCircleV(p, h->raio * growth, Fade(GREEN, aviso ? .3f : .8f));
                DrawCircleV((Vector2){p.x - 6, p.y - 4}, 3, WHITE);
                DrawCircleV((Vector2){p.x + 6, p.y - 4}, 3, WHITE);
            } else if (d->id_padrao == PADRAO_LEBRE_PULOS) {
                DrawEllipse((int)p.x - 7, (int)p.y, 5, 10, Fade(cor, .7f));
                DrawEllipse((int)p.x + 7, (int)p.y, 5, 10, Fade(cor, .7f));
                if (!aviso) {
                    DrawCircleV(p, h->raio, Fade(cor, .6f));
                    for (int thorn = 0; thorn < 8; thorn++) {
                        float a = thorn * PI / 4;
                        DrawLineEx(p, (Vector2){p.x + cosf(a) * h->raio, p.y + sinf(a) * h->raio}, 3, WHITE);
                    }
                }
            } else if (d->id_padrao == PADRAO_PASSARO_MERGULHO) {
                if (!aviso) DrawCircleV(p, h->raio, Fade(ORANGE, .7f));
                DrawLineEx((Vector2){p.x - 7, p.y - 9}, (Vector2){p.x + 7, p.y + 9}, 3, ORANGE);
                DrawLineEx((Vector2){p.x - 7, p.y + 1}, (Vector2){p.x + 4, p.y + 5}, 2, GOLD);
            } else if (d->id_padrao == PADRAO_TORRE_ESMAGAMENTO) {
                if (!aviso) DrawCircleV(p, h->raio, Fade(GRAY, .55f));
                for (int stone = 0; stone < 5; stone++) {
                    float angulo = stone * 2.4f;
                    float raio = stone * 7.0f;
                    DrawRectangle((int)(p.x + cosf(angulo) * raio) - 5,
                                  (int)(p.y + sinf(angulo) * raio) - 5, 10, 10,
                                  aviso ? Fade(GRAY, .5f) : LIGHTGRAY);
                }
            } else if (d->id_padrao == PADRAO_MARIPOSA_ASAS) {
                if (!aviso) DrawCircleV(p, h->raio, Fade(cor, .35f));
                for (int dust = 0; dust < 12; dust++) {
                    float a = dust * 2.399f;
                    float raio = sqrtf((float)dust / 12) * h->raio;
                    DrawCircleV((Vector2){p.x + cosf(a) * raio, p.y + sinf(a) * raio}, 2, WHITE);
                }
            } else {
                DrawEllipse((int)p.x, (int)p.y, h->raio, aviso ? 7 : h->raio,
                            Fade(cor, aviso ? .25f : .65f));
            }
        }
    }
}

void PadraoDesenhar(const EstadoDefesa *d)
{
    if (PadraoMovimentoEh(d->id_padrao)) { PadraoMovimentoDesenhar(d); return; }
    if (d->total_criacoes == 0) return;
    DesenharPerigosCaracteristicos(d);
    const float t = d->temporizador_especial;
    const float w = d->aviso_ataque;
    const Vector2 p = d->alvo_ataque;
    Color cor = cor_tipo(d->padrao.tipo);
    DrawText(InstrucaoPadrao(d->id_padrao), (int)d->caixa.x, (int)d->caixa.y - 22, 10, WHITE);
    if (d->ataque_segunda_fase) {
        DrawRectangleLinesEx(d->caixa, 3, MAROON);
        DrawText("FASE 2", (int)(d->caixa.x + d->caixa.width) - 57, (int)d->caixa.y + 8, 12, RED);
    }
    if (t > w + 1.1f) return;
    if (d->id_padrao == PADRAO_SAPO_SALTOS || d->id_padrao == PADRAO_LEBRE_PULOS) {
        bool frog = d->id_padrao == PADRAO_SAPO_SALTOS;
        if (frog && d->total_criacoes % 2 == 0) {
            if (t < w) DrawLineEx((Vector2){d->caixa.x, p.y}, p, 2, YELLOW);
            else DrawLineEx((Vector2){d->caixa.x, p.y}, (Vector2){d->caixa.x + d->caixa.width, p.y}, 8, PINK);
        } else {
            float tempo_local = frog ? t : t - Clamp(floorf((t - w) / .23f), 0, 2) * .23f;
            float arc = sinf(Clamp(tempo_local / w, 0, 1) * PI) * 45;
            float x = p.x;
            float y = p.y;
            if (!frog) {
                int hop = (int)Clamp(floorf((t - w) / .23f), 0, 2);
                x = Clamp(p.x + (hop - 1) * 52, d->caixa.x + 24, d->caixa.x + d->caixa.width - 24);
                y += hop == 1 ? 16 : -8;
            }
            DrawEllipse((int)x, (int)y, frog ? 42 : 31, 6, Fade(cor, .35f));
            DrawCircleV((Vector2){x, y - arc}, 10, cor);
            if (frog) {
                DrawCircle((int)x - 6, (int)(p.y - arc) - 8, 4, WHITE);
                DrawCircle((int)x + 6, (int)(p.y - arc) - 8, 4, WHITE);
            } else {
                DrawRectangle((int)x - 7, (int)(y - arc) - 23, 4, 15, cor);
                DrawRectangle((int)x + 3, (int)(y - arc) - 23, 4, 15, cor);
            }
        }
    } else if (d->id_padrao == PADRAO_PASSARO_MERGULHO) {
        DrawLineEx(d->origem_ataque, p, 2, t < w ? YELLOW : ORANGE);
        Vector2 bird = Vector2Lerp(d->origem_ataque, p, Clamp((t - w) / .4f, 0, 1));
        DrawTriangle((Vector2){bird.x, bird.y + 12}, (Vector2){bird.x + 15, bird.y - 7},
                     (Vector2){bird.x - 15, bird.y - 7}, ORANGE);
    } else if (d->id_padrao == PADRAO_FAGULHA_LUZ) {
        DrawRectangleLinesEx((Rectangle){p.x - 22, p.y - 22, 44, 44}, 3, GOLD);
        DrawText("LUZ", (int)p.x - 10, (int)p.y - 5, 10, GOLD);
    } else if (d->id_padrao == PADRAO_GATO_EMBOSCADA) {
        DrawText(d->total_criacoes % 2 ? "PARE" : "GARRAS: MOVA", (int)d->caixa.x + 8,
                 (int)d->caixa.y + 8, 18, d->total_criacoes % 2 ? SKYBLUE : WHITE);
        if (d->total_criacoes % 2) {
            DrawEllipse((int)d->caixa.x + 32, (int)d->caixa.y + 42, 10, 5, SKYBLUE);
            DrawEllipse((int)d->caixa.x + 60, (int)d->caixa.y + 42, 10, 5, SKYBLUE);
        }
    } else if (d->id_padrao == PADRAO_SINO_ECO) {
        DrawText(t < w ? (d->ataque_segunda_fase ? "ECO DUPLO..." : "DING...") : "DONG!",
                 (int)p.x - 20, (int)p.y - 12, 14, GOLD);
    } else if (d->id_padrao == PADRAO_CAVALEIRO_DUELO) {
        DrawText(t < w + .4f ? "1: VERTICAL" : "2: HORIZONTAL", (int)d->caixa.x + 8,
                 (int)d->caixa.y + 8, 12, WHITE);
    } else if (d->id_padrao == PADRAO_TORRE_ESMAGAMENTO) {
        DrawText(d->ataque_segunda_fase && (d->total_criacoes - 1) % 3 == 2 ? "DESABAMENTO" : "ABERTURA",
                 (int)p.x - 24, (int)p.y - 6, 10, LIME);
    } else if (d->id_padrao == PADRAO_MARIPOSA_ASAS) {
        DrawEllipse((int)d->caixa.x + 15, (int)p.y, 13, 24, cor);
        DrawEllipse((int)(d->caixa.x + d->caixa.width) - 15, (int)p.y, 13, 24, cor);
    } else if (d->id_padrao == PADRAO_LIMO_CRESCIMENTO) {
        DrawText("CRESCENDO", (int)p.x - 28, (int)p.y - 8, 10, GREEN);
    }
}
