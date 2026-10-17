#ifndef PADROES_MOVIMENTO_H
#define PADROES_MOVIMENTO_H
#include "bullets.h"

/* Uma mecanica publica por ataque. A fisica pode ser compartilhada internamente,
   mas dois ataques nunca podem virar silenciosamente a mesma regra para o jogador. */
typedef enum {
    MOVIMENTO_MARIPOSA_VENEZIANAS, MOVIMENTO_MARIPOSA_POEIRA,
    MOVIMENTO_SAPO_RIPPLES, MOVIMENTO_SAPO_VITORIAS_REGIAS,
    MOVIMENTO_PASSARO_FLOCK, MOVIMENTO_PASSARO_FIRE_TRAIL, MOVIMENTO_PASSARO_CHUVA_FOGO,
    MOVIMENTO_LIMO_MARE, MOVIMENTO_LIMO_BALSAS,
    MOVIMENTO_FAGULHA_FAROL, MOVIMENTO_FAGULHA_ORBITA,
    MOVIMENTO_LEBRE_BARREIRAS, MOVIMENTO_LEBRE_BURROWS,
    MOVIMENTO_GATO_BOTE, MOVIMENTO_GATO_GARRAS,
    MOVIMENTO_CAVALEIRO_LANCAS, MOVIMENTO_CAVALEIRO_JUSTA,
    MOVIMENTO_SINO_CARRILHAO, MOVIMENTO_SINO_PESO, MOVIMENTO_SINO_REQUIEM, MOVIMENTO_SINO_GAS_SOMBRA,
    MOVIMENTO_TORRE_ESCADARIA, MOVIMENTO_TORRE_BASTIAO, MOVIMENTO_TORRE_COLAPSO,
    MOVIMENTO_INVALIDO
} ModoMovimento;
#define MAXIMO_RETANGULOS_MOVIMENTO 64
typedef struct {
    Rectangle perigo[MAXIMO_RETANGULOS_MOVIMENTO];
    int total_perigos;
    Rectangle plataformas[12];
    int total_plataformas;
    Rectangle avisos[8];
    int total_avisos;
    Vector2 guia;
    ModoMovimento modo;
    int direcao;
    float velocidade_rolagem;
    int regra_movimento; /* 0 = normal, 1 = parar, 2 = continuar em movimento */
} QuadroMovimento;
bool PadraoMovimentoEh(IdPadraoProjetil id);
ModoMovimento ModoDoPadraoMovimento(IdPadraoProjetil id);
bool PadraoMovimentoUsaSalto(IdPadraoProjetil id);
bool PadraoMovimentoUsaPlataformas(IdPadraoProjetil id);
const char *InstrucaoPadraoMovimento(IdPadraoProjetil id);
void PadraoMovimentoIniciar(EstadoDefesa *d);
void QuadroPadraoMovimento(const EstadoDefesa *d, QuadroMovimento *quadro);
void PadraoMovimentoAtualizar(EstadoDefesa *d, AtributosJogador *jogador, EntradaDefesa entrada, float tempo_delta);
void PadraoMovimentoDesenhar(const EstadoDefesa *d);
#endif
