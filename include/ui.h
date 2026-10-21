#ifndef UI_H
#define UI_H

#include "raylib.h"

typedef struct SistemaSprites SistemaSprites;

#define DIALOGO_CAPACIDADE 192

typedef struct {
    char texto[DIALOGO_CAPACIDADE];
    int caracteres_visiveis;
    float temporizador_caractere;
    bool completo;
} TextoDigitado;

void InterfaceDefinirTexto(TextoDigitado *texto, const char *mensagem);
void InterfaceAtualizarTexto(TextoDigitado *texto, float tempo_delta);
bool InterfaceConfirmarTexto(TextoDigitado *texto);
void InterfaceDesenharCaixaTexto(const TextoDigitado *texto, Rectangle caixa);
void InterfaceDesenharPainel(Rectangle painel);
void InterfaceDesenharBotao(const SistemaSprites *sprites, Rectangle botao, const char *rotulo, bool selecionado,
                            bool habilitado, Color destaque);
void InterfaceDesenharLinhaOpcao(Rectangle area, bool selecionada, bool habilitada,
                                 Color destaque);
void InterfaceDesenharAtalho(const char *texto, int x, int y);
void InterfaceDesenharTextoQuebrado(const char *texto, Rectangle area, int tamanho_fonte,
                                    int espacamento_linhas, Color cor);
void InterfaceDesenharMedidor(Rectangle medidor, float quantidade, float maximo, Color preenchimento);
float InterfaceAproximar(float atual, float alvo, float velocidade, float tempo_delta);
#endif
