#include "ui.h"
#include "sprites.h"

#include <math.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

static const Color UI_SOMBRA = {25, 27, 34, 105};
static const Color UI_PAPEL = {255, 250, 239, 230};
static const Color UI_PAPEL_ELEVADO = {255, 253, 247, 242};
static const Color UI_TEXTO = {31, 33, 40, 255};
static const Color UI_DESABILITADO = {177, 178, 181, 255};
static const Color UI_ROSA_SUAVE = {233, 132, 153, 180};


static Font fonte_texto;
static Font fonte_titulo;
static bool fontes_inicializadas;

static Font CarregarFonteInterface(const char *arquivo, int tamanho)
{
    char caminho[1024];
    const char *bases[] = {"", "../", "../../", GetApplicationDirectory()};
    int caracteres[191];
    for (int i = 0; i < 95; i++) caracteres[i] = 32 + i;
    for (int i = 95; i < 191; i++) caracteres[i] = 160 + i - 95;
    for (int i = 0; i < 4; i++) {
        snprintf(caminho, sizeof(caminho), "%sassets/fonts/%s", bases[i], arquivo);
        if (!FileExists(caminho)) continue;
        Font fonte = LoadFontEx(caminho, tamanho, caracteres, 191);
        if (fonte.texture.id != 0 && fonte.texture.id != GetFontDefault().texture.id) {
            SetTextureFilter(fonte.texture, TEXTURE_FILTER_BILINEAR);
            return fonte;
        }
    }
    return GetFontDefault();
}

void InterfaceCarregarFontes(void)
{
    if (fontes_inicializadas) return;
    fonte_texto = CarregarFonteInterface("AlegreyaSans-Medium.ttf", 32);
    fonte_titulo = CarregarFonteInterface("Alice-Regular.ttf", 48);
    fontes_inicializadas = true;
}

void InterfaceDescarregarFontes(void)
{
    if (!fontes_inicializadas) return;
    if (fonte_texto.texture.id != GetFontDefault().texture.id) UnloadFont(fonte_texto);
    if (fonte_titulo.texture.id != GetFontDefault().texture.id) UnloadFont(fonte_titulo);
    fontes_inicializadas = false;
}

static Font FonteInterface(int tamanho)
{
    if (!fontes_inicializadas) return GetFontDefault();
    return tamanho >= 20 ? fonte_titulo : fonte_texto;
}

static float EspacamentoFonte(int tamanho)
{
    return tamanho >= 20 ? 0.5f : 0.25f;
}

int InterfaceMedirTexto(const char *texto, int tamanho)
{
    return (int)ceilf(MeasureTextEx(FonteInterface(tamanho), texto,
                                  (float)tamanho, EspacamentoFonte(tamanho)).x);
}

void InterfaceDesenharTexto(const char *texto, int x, int y, int tamanho, Color cor)
{
    DrawTextEx(FonteInterface(tamanho), texto, (Vector2){(float)x, (float)y},
               (float)tamanho, EspacamentoFonte(tamanho), cor);
}

void InterfaceDesenharTextoAjustado(const char *texto, Rectangle area, int tamanho, Color cor)
{
    if (area.width <= 0 || area.height <= 0) return;
    Font fonte = FonteInterface(tamanho);
    float escala = 1.0f;
    Vector2 medida = MeasureTextEx(fonte, texto, (float)tamanho, EspacamentoFonte(tamanho));
    if (medida.x > area.width) escala = area.width / medida.x;
    if (tamanho * escala > area.height) escala = area.height / tamanho;
    DrawTextEx(fonte, texto, (Vector2){area.x, area.y + (area.height - tamanho * escala) * 0.5f},
               tamanho * escala, EspacamentoFonte(tamanho) * escala, cor);
}

typedef struct { int inicio; int fim; } LinhaTexto;
#define MAXIMO_LINHAS_TEXTO 128
#define CAPACIDADE_LINHA_TEXTO 1024

static int ProximoCaractere(const char *texto, int indice)
{
    int bytes = 1;
    GetCodepointNext(texto + indice, &bytes);
    return indice + (bytes > 0 ? bytes : 1);
}

static int MedirTrecho(const char *texto, int inicio, int fim, int tamanho)
{
    char trecho[CAPACIDADE_LINHA_TEXTO];
    int total = fim - inicio;
    if (total >= CAPACIDADE_LINHA_TEXTO) total = CAPACIDADE_LINHA_TEXTO - 1;
    memcpy(trecho, texto + inicio, (size_t)total);
    trecho[total] = '\0';
    return InterfaceMedirTexto(trecho, tamanho);
}

/* Quebra por palavras, preservando UTF-8 e dividindo palavras maiores que a caixa. */
static int OrganizarLinhas(const char *texto, float largura, int tamanho, LinhaTexto *linhas)
{
    int inicio = 0, cursor = 0, ultimo_espaco = -1, total = 0;
    while (texto[cursor] && total < MAXIMO_LINHAS_TEXTO) {
        if (texto[cursor] == '\n') {
            linhas[total++] = (LinhaTexto){inicio, cursor};
            inicio = ++cursor;
            ultimo_espaco = -1;
            continue;
        }
        if (texto[cursor] == ' ' || texto[cursor] == '\t') {
            if (cursor == inicio) { inicio = ++cursor; continue; }
            ultimo_espaco = cursor;
        }
        const int proximo = ProximoCaractere(texto, cursor);
        if (cursor > inicio && (proximo - inicio >= CAPACIDADE_LINHA_TEXTO ||
                               MedirTrecho(texto, inicio, proximo, tamanho) > largura)) {
            const bool por_palavra = ultimo_espaco >= inicio;
            const int fim = por_palavra ? ultimo_espaco : cursor;
            linhas[total++] = (LinhaTexto){inicio, fim};
            inicio = por_palavra ? ultimo_espaco + 1 : cursor;
            cursor = inicio;
            ultimo_espaco = -1;
        } else cursor = proximo;
    }
    if (total < MAXIMO_LINHAS_TEXTO && cursor > inicio)
        linhas[total++] = (LinhaTexto){inicio, cursor};
    return total;
}

static void DesenharTextoOrganizado(const char *texto, int visiveis, Rectangle area,
                                    int tamanho, int entrelinhas, Color cor)
{
    if (!texto || area.width <= 0 || area.height <= 0) return;
    LinhaTexto linhas[MAXIMO_LINHAS_TEXTO];
    int total = OrganizarLinhas(texto, area.width, tamanho, linhas);
    /* Reduz apenas quando a altura exige, antes de revelar o primeiro caractere. */
    const int minimo = tamanho > 10 ? 10 : tamanho;
    while (tamanho > minimo && total * tamanho + (total - 1) * entrelinhas > area.height) {
        tamanho--;
        total = OrganizarLinhas(texto, area.width, tamanho, linhas);
    }
    for (int i = 0; i < total; i++) {
        int y = (int)area.y + i * (tamanho + entrelinhas);
        if (y + tamanho > area.y + area.height) break;
        int fim = linhas[i].fim < visiveis ? linhas[i].fim : visiveis;
        if (fim <= linhas[i].inicio) continue;
        char trecho[CAPACIDADE_LINHA_TEXTO];
        int bytes = fim - linhas[i].inicio;
        memcpy(trecho, texto + linhas[i].inicio, (size_t)bytes);
        trecho[bytes] = '\0';
        InterfaceDesenharTexto(trecho, (int)area.x, y, tamanho, cor);
    }
}

static void DesenharCantosPainel(Rectangle area, Color cor)
{
    const int esquerda = (int)area.x + 5;
    const int direita = (int)(area.x + area.width) - 6;
    const int cima = (int)area.y + 5;
    const int baixo = (int)(area.y + area.height) - 6;

    DrawRectangle(esquerda, cima, 8, 2, cor);
    DrawRectangle(esquerda, cima, 2, 8, cor);
    DrawRectangle(direita - 7, cima, 8, 2, cor);
    DrawRectangle(direita - 1, cima, 2, 8, cor);
    DrawRectangle(esquerda, baixo - 1, 8, 2, cor);
    DrawRectangle(esquerda, baixo - 7, 2, 8, cor);
    DrawRectangle(direita - 7, baixo - 1, 8, 2, cor);
    DrawRectangle(direita - 1, baixo - 7, 2, 8, cor);
}

void InterfaceDefinirTexto(TextoDigitado *texto, const char *mensagem)
{
    snprintf(texto->texto, sizeof(texto->texto), "%s", mensagem);
    texto->caracteres_visiveis = 0;
    texto->temporizador_caractere = 0.0f;
    texto->completo = false;
}

void InterfaceAtualizarTexto(TextoDigitado *texto, float tempo_delta)
{
    const int length = (int)strlen(texto->texto);

    if (texto->completo) return;

    texto->temporizador_caractere += tempo_delta;
    while (texto->temporizador_caractere >= 0.025f && texto->caracteres_visiveis < length) {
        texto->temporizador_caractere -= 0.025f;
        texto->caracteres_visiveis = ProximoCaractere(texto->texto, texto->caracteres_visiveis);
    }
    if (texto->caracteres_visiveis >= length) texto->completo = true;
}

bool InterfaceConfirmarTexto(TextoDigitado *texto)
{
    if (!texto->completo) {
        texto->caracteres_visiveis = (int)strlen(texto->texto);
        texto->completo = true;
        return false;
    }

    return true;
}

void InterfaceDesenharPainel(Rectangle painel)
{
    DrawRectangleRec((Rectangle){painel.x + 4, painel.y + 5, painel.width, painel.height}, UI_SOMBRA);
    DrawRectangleRec(painel, UI_PAPEL);
    DrawRectangleLinesEx(painel, 3.0f, UI_TEXTO);
    DrawRectangleLinesEx((Rectangle){painel.x + 5.0f, painel.y + 5.0f,
                                      painel.width - 10.0f, painel.height - 10.0f},
                         1.0f, Fade(UI_ROSA_SUAVE, 0.55f));
    DesenharCantosPainel(painel, UI_ROSA_SUAVE);
}

void InterfaceDesenharBotao(const SistemaSprites *sprites, Rectangle botao, const char *rotulo, bool selecionado,
                            bool habilitado, Color destaque)
{
    const Color fundo = !habilitado ? (Color){218, 216, 210, 225} :
                        (selecionado ? ColorLerp(UI_PAPEL_ELEVADO, destaque, 0.30f) : UI_PAPEL_ELEVADO);
    const Color contorno = selecionado ? destaque : UI_TEXTO;
    const Color texto = habilitado ? UI_TEXTO : UI_DESABILITADO;
    int tamanho_fonte = 16;
    while (tamanho_fonte > 10 && InterfaceMedirTexto(rotulo, tamanho_fonte) > botao.width - 32) tamanho_fonte--;
    const int largura_texto = InterfaceMedirTexto(rotulo, tamanho_fonte);

    const IdSprite id_sprite = selecionado ? SPRITE_BOTAO_TITULO_SELECIONADO : SPRITE_BOTAO_TITULO;
    const bool tem_moldura = sprites != NULL && sprite_obter(sprites, id_sprite) != NULL;

    if (tem_moldura) {
        const Color tinta = habilitado ? WHITE : (Color){185, 185, 185, 220};
        SpriteDesenharAjustado(sprites, id_sprite, botao, tinta);
    } else {
        DrawRectangleRec((Rectangle){botao.x + 3, botao.y + 4, botao.width, botao.height}, UI_SOMBRA);
        DrawRectangleRec(botao, fundo);
        DrawRectangleLinesEx(botao, selecionado ? 3.0f : 2.0f, contorno);
        DrawRectangleLinesEx((Rectangle){botao.x + 4.0f, botao.y + 4.0f,
                                          botao.width - 8.0f, botao.height - 8.0f},
                             1.0f, selecionado ? Fade(WHITE, 0.80f) : Fade(UI_ROSA_SUAVE, 0.42f));
        if (selecionado) {
            DrawCircle((int)botao.x + 12, (int)(botao.y + botao.height * 0.5f), 4.0f, destaque);
            DrawCircleLines((int)botao.x + 12, (int)(botao.y + botao.height * 0.5f), 5.0f, UI_TEXTO);
        }
    }
    InterfaceDesenharTexto(rotulo, (int)(botao.x + (botao.width - largura_texto) / 2.0f),
             (int)(botao.y + (botao.height - tamanho_fonte) / 2.0f), tamanho_fonte, texto);
}

void InterfaceDesenharLinhaOpcao(Rectangle area, bool selecionada, bool habilitada,
                                 Color destaque)
{
    const Color fundo = !habilitada ? (Color){218, 216, 210, 205} :
                        (selecionada ? ColorLerp(UI_PAPEL, destaque, 0.24f) : Fade(UI_PAPEL_ELEVADO, 0.86f));
    DrawRectangleRec((Rectangle){area.x + 2.0f, area.y + 3.0f, area.width, area.height}, Fade(UI_SOMBRA, 0.55f));
    DrawRectangleRec(area, fundo);
    DrawRectangleLinesEx(area, selecionada ? 2.5f : 1.0f, selecionada ? destaque : Fade(UI_TEXTO, 0.65f));
    if (selecionada) {
        DrawCircle((int)area.x + 12, (int)(area.y + area.height * 0.5f), 3.0f, destaque);
    }
}

void InterfaceDesenharAtalho(const char *texto, int x, int y)
{
    const int tamanho_fonte = 12;
    const int largura_desejada = InterfaceMedirTexto(texto, tamanho_fonte) + 14;
    const int largura = largura_desejada < 628 - x ? largura_desejada : 628 - x;
    DrawRectangle(x, y, largura, 20, (Color){42, 44, 52, 225});
    DrawRectangleLines(x, y, largura, 20, (Color){238, 231, 214, 255});
    InterfaceDesenharTextoAjustado(texto, (Rectangle){x + 7, y + 3, largura - 14, 14}, tamanho_fonte, RAYWHITE);
}

void InterfaceDesenharTextoQuebrado(const char *texto, Rectangle area, int tamanho_fonte,
                                    int espacamento_linhas, Color cor)
{
    DesenharTextoOrganizado(texto, INT_MAX, area, tamanho_fonte, espacamento_linhas, cor);
}

void InterfaceDesenharMedidor(Rectangle medidor, float quantidade, float maximo, Color preenchimento)
{
    float ratio = maximo > 0.0f ? quantidade / maximo : 0.0f;

    if (ratio < 0.0f) ratio = 0.0f;
    if (ratio > 1.0f) ratio = 1.0f;
    DrawRectangleRec(medidor, BLACK);
    DrawRectangle((int)medidor.x + 2, (int)medidor.y + 2, (int)medidor.width - 4,
                  (int)medidor.height - 4, RAYWHITE);
    DrawRectangle((int)medidor.x + 2, (int)medidor.y + 2,
                  (int)((medidor.width - 4.0f) * ratio), (int)medidor.height - 4, preenchimento);
}

float InterfaceAproximar(float atual, float alvo, float velocidade, float tempo_delta)
{
    const float distancia = alvo - atual;
    const float passo = velocidade * tempo_delta;

    if (distancia > passo) return atual + passo;
    if (distancia < -passo) return atual - passo;
    return alvo;
}

void InterfaceDesenharCaixaTexto(const TextoDigitado *texto, Rectangle caixa)
{
    InterfaceDesenharPainel(caixa);
    DesenharTextoOrganizado(texto->texto, texto->caracteres_visiveis,
                (Rectangle){caixa.x + 18, caixa.y + 12, caixa.width - 36, caixa.height - 42},
                18, 3, UI_TEXTO);
    if (texto->completo)
        InterfaceDesenharAtalho("ESPACO  continuar", (int)(caixa.x + caixa.width - 142),
                                (int)(caixa.y + caixa.height - 25));
}
