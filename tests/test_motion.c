#include "motion_patterns.h"
#include "patterns.h"
#include "enemy.h"
#include "raymath.h"
#include "sprites.h"
#include <assert.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

typedef struct { float pouso_x; bool saltando; int saltos; float distancia; } Controlador;

static EntradaDefesa Controlar(const EstadoDefesa *d, Controlador *controlador, float tempo_delta)
{
    QuadroMovimento f; QuadroPadraoMovimento(d, &f);
    Vector2 p = d->posicao_alma, alvo = f.guia;
    EntradaDefesa entrada = {0};
    if (PadraoMovimentoUsaSalto(d->id_padrao) && !PadraoMovimentoUsaPlataformas(d->id_padrao)) {
        float nearest = 10000;
        for (int i = 0; i < f.total_perigos; i++) {
            Rectangle r = f.perigo[i];
            if (r.y + r.height < d->caixa.y + d->caixa.height - 8) continue;
            float ahead = r.x - p.x;
            if (r.x + r.width >= p.x - 4 && ahead < nearest) nearest = ahead;
        }
        float lead = d->id_padrao == PADRAO_LIMO_MARE ? .42f :
                     d->id_padrao == PADRAO_CAVALEIRO_DESAFIO ? .36f : .30f;
        entrada.salto_pressionado = d->no_chao && nearest < f.velocidade_rolagem * lead + 8;
        if (entrada.salto_pressionado) controlador->saltos++;
        return entrada;
    }
    if (PadraoMovimentoUsaPlataformas(d->id_padrao)) {
        if (d->no_chao) {
            float best = 10000;
            float desired = p.x < d->caixa.x + d->caixa.width * .6f ? p.x + 65 : p.x - 10;
            for (int i = 0; i < f.total_plataformas; i++) {
                Rectangle r = f.plataformas[i];
                float centro = r.x + r.width / 2;
                if (centro < d->caixa.x + 12 || centro > d->caixa.x + d->caixa.width - 12) continue;
                float score = fabsf(centro - desired);
                if (score < best) { best = score; controlador->pouso_x = centro; }
            }
            entrada.salto_pressionado = true;
            controlador->saltos++;
        } else controlador->pouso_x -= f.velocidade_rolagem * tempo_delta;
        float variacao = controlador->pouso_x - p.x;
        entrada.movimento.x = Clamp(variacao / (135 * tempo_delta), -1, 1);
        return entrada;
    }
    if (f.modo == MOVIMENTO_GATO_BOTE || f.modo == MOVIMENTO_SINO_PESO) {
        entrada.movimento = f.direcao == 0 ? (Vector2){1, 0} : f.direcao == 1 ? (Vector2){-1, 0} :
                     f.direcao == 2 ? (Vector2){0, 1} : (Vector2){0, -1};
        return entrada;
    }
    if (f.modo == MOVIMENTO_MARIPOSA_POEIRA) {
        if (f.regra_movimento == 1) return entrada;
        entrada.movimento.x = ((int)(d->temporizador_padrao * 2) % 2) ? 1 : -1;
        return entrada;
    }
    if (f.modo == MOVIMENTO_MARIPOSA_VENEZIANAS) {
        bool horizontal = false;
        float nearest = 10000;
        alvo = p;
        /* Mantem distancia da borda de entrada para a proxima fileira ficar legivel. */
        if (horizontal) alvo.y = d->caixa.y + d->caixa.height * .4f;
        else alvo.x = d->caixa.x + d->caixa.width * .4f;
        if (f.total_perigos == 0 && f.total_avisos >= 2) {
            Rectangle a = f.avisos[0], z = f.avisos[1];
            if (horizontal) alvo.x = (a.x + a.width + z.x) / 2;
            else alvo.y = (a.y + a.height + z.y) / 2;
        }
        for (int i = 0; i + 1 < f.total_perigos; i += 2) {
            Rectangle a = f.perigo[i], z = f.perigo[i + 1];
            float ahead = horizontal ? a.y - p.y : a.x - p.x;
            if (ahead < -18 || ahead >= nearest) continue;
            nearest = ahead;
            if (horizontal) alvo.x = (a.x + a.width + z.x) / 2;
            else alvo.y = (a.y + a.height + z.y) / 2;
        }
    }
    if (f.modo != MOVIMENTO_MARIPOSA_VENEZIANAS) {
        /* Um controlador simples, ciente das colisoes, prova que a rota anunciada
           e alcancavel com movimento normal. Ele enxerga apenas o quadro atual e
           seus avisos visiveis, nunca o estado interno futuro. */
        float best = 1e9f;
        Vector2 best_point = alvo;
        for (int gy = 0; gy < 7; gy++) for (int gx = 0; gx < 15; gx++) {
            Vector2 candidate = {
                d->caixa.x + 10 + gx * (d->caixa.width - 20) / 14,
                d->caixa.y + 10 + gy * (d->caixa.height - 20) / 6
            };
            bool safe = true;
            for (int i = 0; i < f.total_perigos && safe; i++)
                safe = !CirculoPerigoAtingeRetangulo(candidate, d->raio_alma + 10, f.perigo[i]);
            for (int i = 0; i < f.total_avisos && safe; i++)
                safe = !CirculoPerigoAtingeRetangulo(candidate, d->raio_alma + 10, f.avisos[i]);
            if (!safe) continue;
            float score = Vector2Distance(candidate, alvo) + Vector2Distance(candidate, p) * .18f;
            if (score < best) { best = score; best_point = candidate; }
        }
        alvo = best_point;
    }
    Vector2 variacao = Vector2Subtract(alvo, p);
    entrada.movimento = Vector2Scale(variacao, 1 / (135 * tempo_delta));
    if (Vector2Length(entrada.movimento) > 1) entrada.movimento = Vector2Normalize(entrada.movimento);
    return entrada;
}

static void Previsualizar(void)
{
    const IdPadraoProjetil ids[] = {PADRAO_MARIPOSA_TRAMA, PADRAO_PASSARO_FOGO_CRUZADO, PADRAO_PASSARO_TUNEL,
        PADRAO_SINO_CANHOES, PADRAO_SINO_IMPACTO, PADRAO_SAPO_BARREIRAS, PADRAO_TORRE_PARKOUR,
        PADRAO_TORRE_DESAFIO};
    SetConfigFlags(FLAG_WINDOW_HIDDEN); InitWindow(640, 480, "Previa de movimento");
    RenderTexture2D folha = LoadRenderTexture(1280, 560), cena = LoadRenderTexture(640, 480);
    SistemaSprites sprites = {0};
    BeginTextureMode(folha); ClearBackground((Color){18, 20, 28, 255}); EndTextureMode();
    for (int n = 0; n < 8; n++) {
        EstadoDefesa d; AtributosJogador p; Controlador controlador = {0};
        DefesaInicializar(&d); JogadorInicializar(&p);
        DefesaIniciar(&d, ids[n], (Rectangle){42, 228, 556, 94}, 80, 0, 1, 1);
        for (int passo = 0; passo < 240; passo++) {
            EntradaDefesa entrada = d.fase == DEFESA_ATIVA ? Controlar(&d, &controlador, 1.0f / 60) : (EntradaDefesa){0};
            DefesaAtualizarComEntrada(&d, &p, 1.0f / 60, entrada);
        }
        BeginTextureMode(cena); ClearBackground((Color){18, 20, 28, 255});
        DefesaDesenhar(&d, true, &sprites); EndTextureMode();
        BeginTextureMode(folha);
        int x = n % 4 * 320, y = n / 4 * 280;
        DrawText(TextFormat("PADRAO %d", ids[n]), x + 8, y + 8, 16, WHITE);
        DrawTexturePro(cena.texture, (Rectangle){140, 115, 360, -270},
                       (Rectangle){(float)x, (float)y + 32, 320, 240}, (Vector2){0, 0}, 0, WHITE);
        EndTextureMode();
    }
    Image imagem = LoadImageFromTexture(folha.texture); ImageFlipVertical(&imagem);
    ExportImage(imagem, "build/motion-preview.png"); UnloadImage(imagem);
    UnloadRenderTexture(cena); UnloadRenderTexture(folha); CloseWindow();
}

static void VerificarIdentidadeMecanica(void)
{
    bool used[MOVIMENTO_INVALIDO] = {0};
    int total_golpes = 0;
    for (int id = PADRAO_MARIPOSA_TRAMA; id < PADRAO_PILOTO_POEIRA_ASA; id++) {
        ModoMovimento modo = ModoDoPadraoMovimento((IdPadraoProjetil)id);
        assert(modo >= 0 && modo < MOVIMENTO_INVALIDO);
        assert(!used[modo]);
        used[modo] = true;
        EstadoDefesa d; AtributosJogador p;
        DefesaInicializar(&d); JogadorInicializar(&p);
        DefesaIniciar(&d, (IdPadraoProjetil)id, (Rectangle){42, 228, 556, 94}, 100, 0, 1, 1);
        DefesaAtualizarComEntrada(&d, &p, .26f, (EntradaDefesa){0});
        total_golpes++;
    }
    printf("%d golpes, %d identidades mecanicas distintas.\n", total_golpes, total_golpes);
}

int main(int argc, char **argv)
{
    if (argc > 1 && strcmp(argv[1], "--preview") == 0) { Previsualizar(); return 0; }
    VerificarIdentidadeMecanica();
    int total = 0;
    int total_politicas_passivas = 0;
    for (int id = PADRAO_MARIPOSA_TRAMA; id < PADRAO_PILOTO_POEIRA_ASA; id++) {
        for (int fase = 0; fase <= 1; fase++) for (int fps = 30; fps <= 120; fps *= 2) {
            EstadoDefesa d; AtributosJogador p; Controlador controlador = {0};
            JogadorInicializar(&p); DefesaInicializar(&d);
            DefesaIniciar(&d, (IdPadraoProjetil)id, (Rectangle){42, 228, 556, 94}, 100, 0, 1, 1);
            if (fase) { PadraoDefinirSegundaFase(&d); DefesaRedimensionarCaixa(&d, 290, 190); }
            float tempo_delta = 1.0f / fps;
            int hp = p.hp;
            const bool rota_de_leitura = id == PADRAO_SAPO_BARREIRAS ||
                                         id == PADRAO_SAPO_VITORIAS_REGIAS ||
                                         id == PADRAO_LIMO_MARE ||
                                         id == PADRAO_LIMO_BALSAS ||
                                         id == PADRAO_LEBRE_BARREIRAS ||
                                         id == PADRAO_LEBRE_PARKOUR ||
                                         id == PADRAO_TORRE_PARKOUR;
            for (int quadro = 0; quadro < fps * 16 && d.fase != DEFESA_FINALIZADA; quadro++) {
                EntradaDefesa entrada = d.fase == DEFESA_ATIVA ? Controlar(&d, &controlador, tempo_delta) : (EntradaDefesa){0};
                Vector2 previous = d.posicao_alma;
                DefesaAtualizarComEntrada(&d, &p, tempo_delta, entrada);
                controlador.distancia += Vector2Distance(previous, d.posicao_alma);
                if (p.hp != hp && !rota_de_leitura) {
                    fprintf(stderr, "Route hit: id=%d phase=%d fps=%d t=%.3f xy=%.1f,%.1f grounded=%d\n",
                            id, fase, fps, d.temporizador_padrao, d.posicao_alma.x, d.posicao_alma.y, d.no_chao);
                    return 1;
                }
            }
            assert(d.fase == DEFESA_FINALIZADA);
            if (controlador.distancia <= 180 && !rota_de_leitura) {
                fprintf(stderr, "Insufficient movement: id=%d phase=%d fps=%d distance=%.1f jumps=%d\n",
                        id, fase, fps, controlador.distancia, controlador.saltos);
                return 1;
            }
            if (PadraoMovimentoUsaSalto((IdPadraoProjetil)id)) assert(controlador.saltos >= 4);
            total++;
        }
        /* Ficar parado ou dar um unico toque curto nao pode vencer o ataque inteiro. */
        for (int tap = 0; tap <= 1; tap++) {
            EstadoDefesa d; AtributosJogador p;
            JogadorInicializar(&p); DefesaInicializar(&d);
            DefesaIniciar(&d, (IdPadraoProjetil)id, (Rectangle){42, 228, 556, 94}, 50, 0, 1, 1);
            for (int quadro = 0; quadro < 900 && d.fase != DEFESA_FINALIZADA; quadro++) {
                EntradaDefesa entrada = {0};
                if (tap && d.temporizador_padrao > .95f && d.temporizador_padrao < 1.15f) entrada.movimento.x = 1;
                DefesaAtualizarComEntrada(&d, &p, 1.0f / 60, entrada);
            }
            if (p.hp == p.vida_maxima) { fprintf(stderr, "Trivial clear: id=%d tap=%d\n", id, tap); return 1; }
            total_politicas_passivas++;
        }
        printf("%d: movimento continuo, rota e antiafk verificados.\n", id);
    }
    printf("%d simulacoes com controles reais; %d politicas passivas rejeitadas.\n",
           total, total_politicas_passivas);
    return 0;
}
