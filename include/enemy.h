#ifndef INIMIGO_H
#define INIMIGO_H

#include <stdbool.h>

#include "player.h"
#include "bullets.h"

#define MAXIMO_PADROES_INIMIGO 10
#define MAXIMO_ACOES_INIMIGO 4

typedef enum {
    INIMIGO_POEIRA_MARIPOSA,
    INIMIGO_BRILHO_SAPO,
    INIMIGO_BRASA_PASSARO,
    INIMIGO_MENTA_LIMO,
    INIMIGO_LANTERNA_FAGULHA,
    INIMIGO_ESPINHO_LEBRE,
    INIMIGO_SOMBRA_GATO,
    INIMIGO_PETALA_CAVALEIRO,
    INIMIGO_CHEFE_SINEIRO,
    INIMIGO_CHEFE_ULTIMA_TORRE,
    INIMIGO_SOLDADO_SARCAS, INIMIGO_HERBALISTA_PESTE,
    INIMIGO_LEGIONARIO_AFOGADO, INIMIGO_BRUXA_LODO, INIMIGO_REVENANTE_GELO,
    INIMIGO_ESQUELETO_CARBONIZADO, INIMIGO_ACENDEDOR_FORJA,
    INIMIGO_PENITENTE_SEM_ROSTO, INIMIGO_LADRAO_MORTALHA,
    INIMIGO_ACOLITO_VITRAL, INIMIGO_GOBLIN_LANCEIRO, INIMIGO_AUTOMATO_PEDRA,
    INIMIGO_MERCENARIO_FERRUGEM, INIMIGO_CARCEREIRO_CORRENTES,
    INIMIGO_CHEFE_CULTISTA_RAIZES, INIMIGO_CHEFE_AFOGADO_CORAL,
    INIMIGO_CHEFE_MAGO_CINZAS, INIMIGO_CHEFE_GOBLIN_CRIPTA,
    INIMIGO_CHEFE_INQUISIDOR_SOL,
    INIMIGO_TOTAL
} IdInimigo;

typedef enum {
    INIMIGO_ACAO_ANALISAR,
    INIMIGO_ACAO_SOCIAL
} EfeitoAcaoInimigo;

typedef struct {
    const char *nome;
    const char *resposta;
    EfeitoAcaoInimigo efeito;
    int variacao_misericordia;
    int variacao_agressividade;
    int maximo_usos;
    int turnos_recarga;
    float penalidade_repeticao;
    int agressividade_maxima_exigida;
    bool possui_bonus_tipo;
    Tipo tipo_bonus;
    float multiplicador_tipo;
    int limite_agressividade;
    float multiplicador_calmo;
    float multiplicador_furioso;
} AcaoInimigo;

typedef struct {
    char nome[24];
    int hp;
    int vida_maxima;
    IdInimigo id;
    bool chefe;
    bool segunda_fase;
    Tipo tipo;
    Tipo tipo2;
    bool possui_tipo2;
    bool todos_tipos;
    int agressividade;
    int misericordia;
    bool poupavel;
    bool esgotado;
    bool analisado;
    float temporizador_flash_acerto;
    float forca_flash_acerto;
    IdPadraoProjetil ids_padroes[MAXIMO_PADROES_INIMIGO];
    int total_padroes;
    int indice_proximo_padrao;
    char descricao[128];
    AcaoInimigo acoes[MAXIMO_ACOES_INIMIGO];
    int usos_acoes[MAXIMO_ACOES_INIMIGO];
    int recargas_acoes[MAXIMO_ACOES_INIMIGO];
    int ultimo_turno_acoes[MAXIMO_ACOES_INIMIGO];
    int indice_ultima_acao;
    int usos_acao_repetida;
    int turno_jogador;
    int ultima_variacao_misericordia;
    int ultima_variacao_agressividade;
    int total_acoes;
    int bonus_dano_projetil;
    float multiplicador_dano_projetil;
    int bonus_recompensa;
    EfeitoStatus status;
    int turnos_status;
    int sangramento;
} Inimigo;

Inimigo InimigoCriar(IdInimigo id);
void InimigoReceberDano(Inimigo *inimigo, int dano, float efetividade);
bool InimigoAplicarAcao(Inimigo *inimigo, int indice_acao, char *mensagem, int tamanho_mensagem);
void InimigoAtualizarPoupavel(Inimigo *inimigo);
void InimigoAvancarTurno(Inimigo *inimigo);
bool InimigoPodePoupar(const Inimigo *inimigo);
bool InimigoAcaoDisponivel(const Inimigo *inimigo, int indice_acao, char *motivo, int tamanho_motivo);
void InimigoAplicarStatus(Inimigo *inimigo, EfeitoStatus status, int turnos);
bool InimigoAvancarStatus(Inimigo *inimigo, int *dano);
int InimigoAplicarSangramento(Inimigo *inimigo, int quantidade);
int InimigoProcessarSangramento(Inimigo *inimigo);
void InimigoCurarSangramento(Inimigo *inimigo);

#endif
