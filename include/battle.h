#ifndef BATALHA_H
#define BATALHA_H

#include "enemy.h"
#include "ui.h"
#include "audio.h"
#include "sprites.h"
#include "fx.h"

typedef enum {
    BATALHA_INTRODUCAO,
    BATALHA_JOGADOR_MENU,
    BATALHA_JOGADOR_ACAO,
    BATALHA_JOGADOR_QTE,
    BATALHA_INIMIGO_TURNO,
    BATALHA_VITORIA,
    BATALHA_DERROTA
} FaseBatalha;

typedef enum {
    MENU_ATAQUE,
    MENU_ACT,
    MENU_ITEM,
    MENU_SPARE
} OpcaoMenuPrincipal;

typedef enum {
    ACAO_NENHUM,
    ACAO_ATAQUES,
    ACAO_ACOES,
    ACAO_ITENS,
    ACAO_MENSAGEM
} MenuAcao;

typedef enum {
    BATALHA_OUTCOME_NENHUM,
    BATALHA_OUTCOME_DANO,
    BATALHA_OUTCOME_MISERICORDIA,
    BATALHA_OUTCOME_DERROTA
} DesfechoBatalha;

typedef struct {
    DesfechoBatalha desfecho;
    int moedas;
    int xp;
    int vida_perdida;
    int turnos;
    float duracao_segundos;
    int resultados_qte[4];
} ResultadoBatalha;

#define MAXIMO_BATALHA_PARTICULAS 32

typedef struct {
    bool ativo;
    Vector2 posicao;
    Vector2 velocidade;
    Color cor;
    float tempo_vida;
} ParticulaBatalha;

typedef struct {
    FaseBatalha fase;
    FaseBatalha proxima_fase;
    MenuAcao menu_acao;
    int selecao_principal;
    int andar;
    int dificuldade_qte;
    int selecao_acao;
    float tempo_decorrido;
    int dano_flutuante;
    float temporizador_dano_flutuante;
    float ultima_efetividade;
    int indice_ataque_qte;
    float temporizador_tremor_tela;
    float temporizador_pausa_impacto;
    int vida_inicial_jogador;
    int total_turnos;
    int dano_recebido_turno;
    int resultados_qte[4];
    float vida_inimigo_exibida;
    float rastro_vida_inimigo;
    float vida_jogador_exibida;
    float rastro_vida_jogador;
    ParticulaBatalha particulas[MAXIMO_BATALHA_PARTICULAS];
    EfeitoAtaque efeito_ataque;
    float misericordia_exibida;
    float agressividade_exibida;
    float temporizador_mudanca_status;
    int popup_mudanca_misericordia;
    int popup_mudanca_agressividade;
    bool vitoria_por_misericordia;
    ResultadoBatalha resultado;
    EstadoDefesa defesa;
} Batalha;

void BatalhaInicializar(Batalha *batalha, TextoDigitado *dialogo, int andar);
ResultadoBatalha BatalhaAtualizar(Batalha *batalha, AtributosJogador *jogador, Inimigo *inimigo,
                          TextoDigitado *dialogo, Vector2 posicao_mouse,
                          bool mouse_pressionado, Qte *qte, EstadoAudio *audio);
void BatalhaDesenhar(const Batalha *batalha, const AtributosJogador *jogador, const Inimigo *inimigo,
                const TextoDigitado *dialogo, const Qte *qte, bool tremor_linha,
                bool efeitos_reduzidos, const SistemaSprites *sprites);

#endif
