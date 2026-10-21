#ifndef JOGO_H
#define JOGO_H

#include "raylib.h"

#include "battle.h"
#include "tower.h"
#include "save.h"
#include "render.h"
#include "audio.h"
#include "playtest.h"
#include "sprites.h"

#define LOGICO_LARGURA 640
#define LOGICO_ALTURA 480

typedef enum {
    TITULO,
    ESCOLHER_BROCHE,
    BATALHA,
    RECOMPENSA,
    DESCANSO,
    LOJA,
    INVENTARIO,
    PAUSA,
    FIM_JOGO,
    VITORIA,
    RELIQUIAS,
    OPCOES,
    CONFIRMAR_EXCLUSAO
    ,COMO_JOGAR, DEPURAR_EFEITOS
} TelaJogo;

typedef struct {
    TelaJogo tela;
    TelaJogo tela_antes_pausa;
    TelaJogo tela_antes_inventario;
    int selecao_titulo;
    int selecao_broche;
    float tempo_decorrido;
    bool saida_solicitada;
    AtributosJogador jogador;
    Inimigo inimigo;
    Batalha batalha;
    TextoDigitado dialogo;
    Qte qte;
    ResultadoBatalha ultimo_resultado_batalha;
    Vector2 posicao_mouse;
    Vector2 ultima_posicao_mouse;
    bool mouse_pressionado;
    Torre torre;
    int selecao_recompensa;
    int selecao_descanso;
    int selecao_loja;
    int selecao_final;
    float temporizador_transicao;
    unsigned int contador_partidas;
    char mensagem_tela[96];
    DadosSalvos salvamento;
    EstadoRenderizacao renderizacao;
    int selecao_reliquia;
    int selecao_opcao;
    bool excluir_sim;
    int selecao_pausa;
    int selecao_inventario;
    int selecao_equipamento;
    bool inventario_foco_equipamento;
    bool pausa_confirmar_saida;
    int dica_tutorial;
    float temporizador_tutorial;
    int fase_espera_tutorial;
    int ultimo_caractere_digitado;
    bool telemetria_ativada;
    float partida_iniciada_em;
    EstadoAudio audio;
    SistemaSprites sprites;
    int inimigo_depuracao_padrao;
    int id_depuracao_padrao;
    bool piloto_ativo, piloto_pausado;
    int piloto_inimigo, piloto_ataque, piloto_dificuldade, piloto_vida;
    Tipo piloto_tipo;
} EstadoJogo;

void JogoAbrirPiloto(EstadoJogo *jogo, int inimigo);
void JogoInicializar(EstadoJogo *jogo);
void JogoAtualizar(EstadoJogo *jogo);
void JogoDesenhar(const EstadoJogo *jogo);

#endif
