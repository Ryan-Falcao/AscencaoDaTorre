#include "patterns.h"
#include "enemy.h"
#include "raymath.h"
#include "sprites.h"
#include <assert.h>
#include <stdio.h>
#include <float.h>
#include <string.h>

static void Previsualizar(void)
{
    SetConfigFlags(FLAG_WINDOW_HIDDEN);
    InitWindow(640, 480, "Previa de padroes");
    RenderTexture2D folha = LoadRenderTexture(1600, 1040);
    RenderTexture2D cena = LoadRenderTexture(640, 480);
    SistemaSprites sprites = {0};
    BeginTextureMode(folha); ClearBackground((Color){20, 22, 30, 255}); EndTextureMode();
    for (int etapa = 0; etapa < 2; etapa++) for (int id = 0; id < INIMIGO_TOTAL; id++) {
        Inimigo inimigo = InimigoCriar((IdInimigo)id);
        EstadoDefesa d;
        AtributosJogador jogador;
        JogadorInicializar(&jogador); DefesaInicializar(&d);
        DefesaIniciar(&d, inimigo.ids_padroes[0], (Rectangle){42, 228, 556, 94}, 50, 0, 1, 1);
        if (etapa && inimigo.chefe) {
            PadraoDefinirSegundaFase(&d);
            DefesaRedimensionarCaixa(&d, 290, 190);
        }
        DefesaAtualizar(&d, &jogador, .25f);
        d.depurar_invencivel = true;
        for (int quadro = 0; quadro < (etapa ? 76 : 36); quadro++) DefesaAtualizar(&d, &jogador, 1.0f / 60);
        BeginTextureMode(cena);
        ClearBackground((Color){20, 22, 30, 255});
        DefesaDesenhar(&d, true, &sprites);
        EndTextureMode();
        BeginTextureMode(folha);
        float x = (id % 5) * 320.0f, y = (etapa * 2 + id / 5) * 260.0f;
        DrawText(inimigo.nome, (int)x + 6, (int)y + 6, 16, WHITE);
        DrawText(etapa ? "IMPACTO" : "AVISO", (int)x + 6, (int)y + 28, 10, etapa ? ORANGE : YELLOW);
        DrawTexturePro(cena.texture, (Rectangle){140, 115, 360, -260},
                       (Rectangle){x, y + 40, 320, 220}, (Vector2){0, 0}, 0, WHITE);
        EndTextureMode();
    }
    Image imagem = LoadImageFromTexture(folha.texture);
    ImageFlipVertical(&imagem);
    ExportImage(imagem, "build/patterns-preview.png");
    UnloadImage(imagem); UnloadRenderTexture(cena); UnloadRenderTexture(folha); CloseWindow();
}

static Vector2 Passo(Vector2 origem, Vector2 destino, float tempo_delta)
{
    float distancia = Vector2Distance(origem, destino);
    return distancia <= 135 * tempo_delta ? destino : Vector2Add(origem,
        Vector2Scale(Vector2Subtract(destino, origem), 135 * tempo_delta / distancia));
}

/* Planeja apenas pelos avisos ja visiveis, com latencia de reacao de 180 ms.
   Sem teleporte, informacao futura, invencibilidade ou troca de vida por dano. */
static bool CaminhoSeguro(const EstadoDefesa *d, Vector2 alvo, float tempo_delta)
{
    ConjuntoPerigos conjunto = d->perigos;
    AtributosJogador jogador;
    JogadorInicializar(&jogador);
    Vector2 p = d->posicao_alma;
    for (float tempo = 0; tempo < d->ciclo_ataque - .02f; tempo += tempo_delta) {
        if (tempo >= .18f) p = Passo(p, alvo, tempo_delta);
        PerigosAtualizar(&conjunto, &jogador, p, d->raio_alma, tempo_delta, 1, false);
        if (jogador.hp != jogador.vida_maxima) return false;
    }
    return true;
}

static bool EncontrarCaminho(const EstadoDefesa *d, Vector2 *alvo, float tempo_delta)
{
    float best = FLT_MAX;
    if (CaminhoSeguro(d, d->posicao_alma, tempo_delta)) { *alvo = d->posicao_alma; return true; }
    for (int y = 0; y <= 8; y++) for (int x = 0; x <= 12; x++) {
        Vector2 point = {d->caixa.x + 10 + (d->caixa.width - 20) * x / 12,
                         d->caixa.y + 10 + (d->caixa.height - 20) * y / 8};
        float distancia = Vector2Distance(point, d->posicao_alma);
        if (distancia < best && CaminhoSeguro(d, point, tempo_delta)) { best = distancia; *alvo = point; }
    }
    return best != FLT_MAX;
}

int main(int argc, char **argv)
{
    if (argc > 1 && strcmp(argv[1], "--preview") == 0) { Previsualizar(); return 0; }
    int scenarios = 0, idle_scenarios = 0;
    for (int id = 0; id < INIMIGO_TOTAL; id++) {
        Inimigo inimigo = InimigoCriar((IdInimigo)id);
        assert(inimigo.total_padroes >= 3);
        for (int anterior = 0; anterior < id; anterior++)
            assert(InimigoCriar((IdInimigo)anterior).ids_padroes[0] != inimigo.ids_padroes[0]);
        for (int padrao = 0; padrao < inimigo.total_padroes; padrao++) {
            assert(inimigo.ids_padroes[padrao] >= 0 && inimigo.ids_padroes[padrao] < PADRAO_TOTAL);
            for (int anterior = 0; anterior < padrao; anterior++)
                assert(inimigo.ids_padroes[anterior] != inimigo.ids_padroes[padrao]);
        }
        /* Mantem a cobertura de regressao do ataque original com aviso. */
        int indice_original = id == INIMIGO_BRASA_PASSARO ? 2 : inimigo.total_padroes - 1;
        inimigo.ids_padroes[0] = inimigo.ids_padroes[indice_original];
        for (int enraged = 0; enraged <= (inimigo.chefe ? 1 : 0); enraged++)
        for (int agressividade = 0; agressividade <= 100; agressividade += 50) {
            for (int small = 0; small <= 1; small++) for (int inicio = 0; inicio < 9; inicio++) {
                EstadoDefesa d;
                AtributosJogador jogador;
                JogadorInicializar(&jogador);
                DefesaInicializar(&d);
                DefesaIniciar(&d, inimigo.ids_padroes[0], (Rectangle){42, 228, 556, 94},
                             agressividade, 0, 1, 1);
                if (enraged) PadraoDefinirSegundaFase(&d);
                if (small) DefesaRedimensionarCaixa(&d, 290, 190);
                DefesaAtualizar(&d, &jogador, .25f);
                d.posicao_alma = (Vector2){d.caixa.x + 8 + (d.caixa.width - 16) * (inicio % 3) / 2,
                                           d.caixa.y + 8 + (d.caixa.height - 16) * (inicio / 3) / 2};
                Vector2 alvo = d.posicao_alma;
                int ciclo = 0;
                float reaction = 0;
                float tempo_delta = small ? 1.0f / 30 : 1.0f / 60;
                while (d.fase != DEFESA_FINALIZADA) {
                    if (reaction > 0) reaction -= tempo_delta;
                    else d.posicao_alma = Passo(d.posicao_alma, alvo, tempo_delta);
                    DefesaAtualizar(&d, &jogador, tempo_delta);
                    if (d.total_criacoes != ciclo) {
                        ciclo = d.total_criacoes;
                        if (!EncontrarCaminho(&d, &alvo, tempo_delta)) {
                            fprintf(stderr, "No escape: enemy=%d aggression=%d small=%d start=%d cycle=%d phase2=%d\n",
                                    id, agressividade, small, inicio, ciclo, enraged);
                            return 1;
                        }
                        reaction = .18f;
                    }
                    if (jogador.hp != jogador.vida_maxima) {
                        fprintf(stderr, "Route hit: enemy=%d start=%d cycle=%d\n", id, inicio, ciclo);
                        return 1;
                    }
                }
                assert(ciclo >= (enraged ? 6 : 5));
                scenarios++;
            }
        }
        /* Ficar parado deve falhar no centro E perto de todos os cantos e bordas. */
        for (int inicio = 0; inicio < 9; inicio++) {
            EstadoDefesa d;
            AtributosJogador jogador;
            JogadorInicializar(&jogador); DefesaInicializar(&d);
            DefesaIniciar(&d, inimigo.ids_padroes[0], (Rectangle){42, 228, 556, 94}, 50, 0, 1, 1);
            DefesaAtualizar(&d, &jogador, .25f);
            d.posicao_alma = (Vector2){d.caixa.x + 8 + (d.caixa.width - 16) * (inicio % 3) / 2,
                                       d.caixa.y + 8 + (d.caixa.height - 16) * (inicio / 3) / 2};
            for (int quadro = 0; quadro < 700; quadro++) DefesaAtualizar(&d, &jogador, 1.0f / 60);
            if (jogador.hp == jogador.vida_maxima) {
                fprintf(stderr, "Idle exploit: enemy=%d start=%d\n", id, inicio); return 1;
            }
            idle_scenarios++;
        }
        printf("%s: rotas sem dano e jogador parado verificados.\n", inimigo.nome);
    }
    printf("%d cenarios de esquiva; %d posicoes paradas.\n", scenarios, idle_scenarios);
    return 0;
}
