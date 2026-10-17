#ifndef PROJETEIS_H
#define PROJETEIS_H

#include "raylib.h"

#include "player.h"
#include "hazards.h"
typedef struct SistemaSprites SistemaSprites;

#define MAXIMO_PROJETEIS 512
#define VELOCIDADE_MOVIMENTO_DEFESA 135.0f
#define DISTANCIA_DASH_DEFESA 48.0f
#define RECARGA_DASH_DEFESA 0.65f

typedef enum {
    PADRAO_CHUVA,
    PADRAO_LATERAL_ONDA,
    PADRAO_ESPIRAL,
    PADRAO_RAPIDA_CHUVA,
    PADRAO_LARGA_ONDA,
    PADRAO_FECHADA_ESPIRAL,
    PADRAO_PETALA_ORBITA,
    PADRAO_REBOTE_ORBES,
    PADRAO_MOVEL_PAREDE,
    PADRAO_DIVISAO_DISPAROS,
    PADRAO_CRUZADOS_LASERS,
    PADRAO_PAUSA_DARDOS,
    PADRAO_ANEL_EXPLOSAO,
    PADRAO_DIAGONAL_CHUVA,
    PADRAO_PERSEGUICAO_ONDA,
    PADRAO_CATA_VENTO,
    PADRAO_GRAVIDADE_INVERSAO,
    PADRAO_SOMBRA_FLORACAO,
    PADRAO_MARIPOSA_ASAS,
    PADRAO_SAPO_SALTOS,
    PADRAO_PASSARO_MERGULHO,
    PADRAO_LIMO_CRESCIMENTO,
    PADRAO_FAGULHA_LUZ,
    PADRAO_LEBRE_PULOS,
    PADRAO_GATO_EMBOSCADA,
    PADRAO_CAVALEIRO_DUELO,
    PADRAO_SINO_ECO,
    PADRAO_TORRE_ESMAGAMENTO,
    PADRAO_MARIPOSA_TRAMA, PADRAO_MARIPOSA_CORRENTE,
    PADRAO_SAPO_BARREIRAS, PADRAO_SAPO_VITORIAS_REGIAS,
    PADRAO_PASSARO_FOGO_CRUZADO, PADRAO_PASSARO_TUNEL,PADRAO_CHUVA_FOGO,
    PADRAO_LIMO_MARE, PADRAO_LIMO_BALSAS,
    PADRAO_FAGULHA_CANHOES, PADRAO_FAGULHA_ORBITA,
    PADRAO_LEBRE_BARREIRAS, PADRAO_LEBRE_PARKOUR,
    PADRAO_GATO_IMPACTO, PADRAO_GATO_PERSEGUICAO,
    PADRAO_CAVALEIRO_LANCAS, PADRAO_CAVALEIRO_DESAFIO,
    PADRAO_SINO_CANHOES, PADRAO_SINO_IMPACTO, PADRAO_SINO_DESAFIO, PADRAO_SINO_GAS_SOMBRA,
    PADRAO_TORRE_PARKOUR, PADRAO_TORRE_CANHOES, PADRAO_TORRE_DESAFIO,
    PADRAO_TOTAL
} IdPadraoProjetil;

typedef enum {
    CRIACAO_CHUVA,
    CRIACAO_LATERAL_ONDA,
    CRIACAO_ESPIRAL
} ModoCriacaoProjetil;

typedef enum {
    DEFESA_ENTRANDO,
    DEFESA_ATIVA,
    DEFESA_SAINDO,
    DEFESA_FINALIZADA
} FaseDefesa;

typedef struct {
    bool ativo;
    Vector2 posicao;
    Vector2 velocidade;
    float raio;
    Tipo tipo;
    int dano;
    float tempo_vida;
    bool divisao_feita;
} Projetil;

typedef struct {
    float duracao;
    float largura_caixa;
    float altura_caixa;
    float intervalo_criacao;
    int projeteis_por_criacao;
    float velocidade;
    float raio;
    float abertura_angular;
    Tipo tipo;
    int dano;
    int dificuldade;
    ModoCriacaoProjetil modo_criacao;
    bool rotacionar;
    float escala_visual;
    float raio_colisao;
    bool rastro;
} PadraoProjetil;

typedef struct {
    Projetil projeteis[MAXIMO_PROJETEIS];
    PadraoProjetil padrao;
    IdPadraoProjetil id_padrao;
    Rectangle caixa;
    Rectangle caixa_inicial;
    Rectangle caixa_alvo;
    Vector2 posicao_alma;
    Vector2 rastro_alma[6];
    float raio_alma;
    float temporizador_invencibilidade;
    int ultimo_dano;
    float multiplicador_ultimo_dano;
    float temporizador_popup_dano;
    float temporizador_popup_raspar;
    float temporizador_fase;
    float temporizador_padrao;
    float temporizador_criacao;
    float angulo_espiral;
    int total_criacoes;
    bool depurar_invencivel;
    ConjuntoPerigos perigos;
    int parede_gravidade;
    int repeticao_gravidade;
    int repeticoes_gravidade;
    bool espinhos_gravidade_criados;
    float temporizador_especial;
    Vector2 origem_ataque;
    Vector2 alvo_ataque;
    float aviso_ataque;
    float ciclo_ataque;
    bool ataque_segunda_fase;
    Vector2 ancora_movimento;
    float velocidade_vertical;
    float buffer_salto;
    float temporizador_coyote;
    float temporizador_recarga_dash;
    bool no_chao;
    int batida_movimento;
    FaseDefesa fase;
} EstadoDefesa;

typedef enum {
    DEFESA_EM_ANDAMENTO,
    DEFESA_CONCLUIDA,
    DEFESA_JOGADOR_DERROTADO
} ResultadoDefesa;

typedef struct {
    Vector2 movimento;
    bool salto_pressionado;
    bool dash_pressionado;
} EntradaDefesa;

void DefesaInicializar(EstadoDefesa *defesa);
void DefesaIniciar(EstadoDefesa *defesa, IdPadraoProjetil id_padrao, Rectangle caixa_menu,
                  int agressividade, int bonus_dano, float multiplicador_dano, float multiplicador_velocidade);
void DefesaRedimensionarCaixa(EstadoDefesa *defesa, float width, float height);
ResultadoDefesa DefesaAtualizar(EstadoDefesa *defesa, AtributosJogador *jogador, float tempo_delta);
ResultadoDefesa DefesaAtualizarComEntrada(EstadoDefesa *defesa, AtributosJogador *jogador,
                                    float tempo_delta, EntradaDefesa entrada);
void DefesaDesenhar(const EstadoDefesa *defesa, bool desenhar_efeitos, const SistemaSprites *sprites);
int DificuldadePadraoProjetil(IdPadraoProjetil id_padrao);

#endif
