#include "ui.h"
#include "sprites.h"

#include <stdio.h>
#include <string.h>

static const Color UI_SOMBRA = {25, 27, 34, 105};
static const Color UI_PAPEL = {255, 250, 239, 230};
static const Color UI_PAPEL_ELEVADO = {255, 253, 247, 242};
static const Color UI_TEXTO = {31, 33, 40, 255};
static const Color UI_DESABILITADO = {177, 178, 181, 255};
static const Color UI_ROSA_SUAVE = {233, 132, 153, 180};

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
        texto->caracteres_visiveis++;
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
    const int tamanho_fonte = 16;
    const int largura_texto = MeasureText(rotulo, tamanho_fonte);

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
    DrawText(rotulo, (int)(botao.x + (botao.width - largura_texto) / 2.0f),
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
    const int largura = MeasureText(texto, tamanho_fonte) + 14;
    DrawRectangle(x, y, largura, 20, (Color){42, 44, 52, 225});
    DrawRectangleLines(x, y, largura, 20, (Color){238, 231, 214, 255});
    DrawText(texto, x + 7, y + 4, tamanho_fonte, RAYWHITE);
}

void InterfaceDesenharTextoQuebrado(const char *texto, Rectangle area, int tamanho_fonte,
                                    int espacamento_linhas, Color cor)
{
    char linha[DIALOGO_CAPACIDADE] = "";
    int tamanho_linha = 0;
    int y = (int)area.y;
    const char *cursor = texto;

    while (*cursor != '\0' && y + tamanho_fonte <= area.y + area.height) {
        while (*cursor == ' ') cursor++;
        if (*cursor == '\n') {
            DrawText(linha, (int)area.x, y, tamanho_fonte, cor);
            linha[0] = '\0';
            tamanho_linha = 0;
            y += tamanho_fonte + espacamento_linhas;
            cursor++;
            continue;
        }
        if (*cursor == '\0') break;

        const char *inicio_palavra = cursor;
        while (*cursor != '\0' && *cursor != ' ' && *cursor != '\n') cursor++;
        int tamanho_palavra = (int)(cursor - inicio_palavra);
        char candidata[DIALOGO_CAPACIDADE];
        int espaco = tamanho_linha > 0 ? 1 : 0;
        int tamanho_candidata = tamanho_linha + espaco + tamanho_palavra;
        if (tamanho_candidata >= DIALOGO_CAPACIDADE) tamanho_candidata = DIALOGO_CAPACIDADE - 1;

        memcpy(candidata, linha, (size_t)tamanho_linha);
        if (espaco) candidata[tamanho_linha] = ' ';
        int copia = tamanho_candidata - tamanho_linha - espaco;
        memcpy(candidata + tamanho_linha + espaco, inicio_palavra, (size_t)copia);
        candidata[tamanho_candidata] = '\0';

        if (tamanho_linha > 0 && MeasureText(candidata, tamanho_fonte) > area.width) {
            DrawText(linha, (int)area.x, y, tamanho_fonte, cor);
            y += tamanho_fonte + espacamento_linhas;
            linha[0] = '\0';
            tamanho_linha = 0;
            if (y + tamanho_fonte > area.y + area.height) break;
            tamanho_candidata = tamanho_palavra < DIALOGO_CAPACIDADE ? tamanho_palavra : DIALOGO_CAPACIDADE - 1;
            memcpy(candidata, inicio_palavra, (size_t)tamanho_candidata);
            candidata[tamanho_candidata] = '\0';
        }
        memcpy(linha, candidata, (size_t)tamanho_candidata + 1);
        tamanho_linha = tamanho_candidata;
    }
    if (tamanho_linha > 0 && y + tamanho_fonte <= area.y + area.height)
        DrawText(linha, (int)area.x, y, tamanho_fonte, cor);
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
    char visible[DIALOGO_CAPACIDADE];
    int character_count = texto->caracteres_visiveis;

    if (character_count >= DIALOGO_CAPACIDADE) character_count = DIALOGO_CAPACIDADE - 1;
    memcpy(visible, texto->texto, (size_t)character_count);
    visible[character_count] = '\0';

    InterfaceDesenharPainel(caixa);
    InterfaceDesenharTextoQuebrado(visible,
                (Rectangle){caixa.x + 18, caixa.y + 14, caixa.width - 36, caixa.height - 36},
                18, 3, UI_TEXTO);
    if (texto->completo)
        InterfaceDesenharAtalho("ESPACO  continuar", (int)(caixa.x + caixa.width - 142),
                                (int)(caixa.y + caixa.height - 25));
}

