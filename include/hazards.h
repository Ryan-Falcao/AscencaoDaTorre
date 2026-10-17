#ifndef PERIGOS_H
#define PERIGOS_H

#include "raylib.h"
#include "player.h"

typedef struct SistemaSprites SistemaSprites;

#define MAXIMO_PERIGOS 32

typedef enum { PERIGO_RETANGULO, PERIGO_CIRCULO, PERIGO_ANEL } FormaPerigo;
typedef enum { PERIGO_AVISO, PERIGO_ATIVA, PERIGO_DESVANECER } FasePerigo;

typedef struct {
    bool ativo;
    FormaPerigo forma;
    FasePerigo fase;
    Rectangle retangulo;
    Vector2 centro;
    float raio;
    float tempo_aviso;
    float tempo_ativo;
    float tempo_desvanecer;
    float temporizador;
    Tipo tipo;
    int dano;
    bool acerta_uma_vez;
    bool ja_acertou;
    float velocidade_expansao;
    bool apenas_em_movimento;
} Perigo;

typedef struct { Perigo perigos[MAXIMO_PERIGOS]; Vector2 alma_anterior; bool alma_rastreada; } ConjuntoPerigos;

void PerigosInicializar(ConjuntoPerigos *conjunto);
void PerigosLimpar(ConjuntoPerigos *conjunto);
bool PerigosAdicionarRetangulo(ConjuntoPerigos *conjunto, Rectangle retangulo, float aviso, float ativo, float desvanecer,
                    Tipo tipo, int dano, bool acerta_uma_vez);
bool PerigosAdicionarCirculo(ConjuntoPerigos *conjunto, Vector2 centro, float raio, float aviso, float ativo,
                      float desvanecer, Tipo tipo, int dano, bool acerta_uma_vez);
bool PerigosAdicionarAnel(ConjuntoPerigos *conjunto, Vector2 centro, float velocidade, float aviso, float ativo,
                    Tipo tipo, int dano);
void PerigosAtualizar(ConjuntoPerigos *conjunto, AtributosJogador *jogador, Vector2 alma, float raio_alma,
                   float tempo_delta, float multiplicador_dano, bool invencivel);
void PerigosDesenhar(const ConjuntoPerigos *conjunto, bool efeitos_reduzidos, const SistemaSprites *sprites,
                 bool depurar_colisoes);
bool CirculoPerigoAtingeRetangulo(Vector2 centro, float raio, Rectangle retangulo);
bool CirculoPerigoAtingeCirculo(Vector2 primeiro, float raio_primeiro, Vector2 segundo, float raio_segundo);

#endif
