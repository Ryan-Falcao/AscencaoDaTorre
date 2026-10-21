#ifndef BALANCEAMENTO_H
#define BALANCEAMENTO_H

#include "save.h"
#include "bullets.h"

/* Todos os numeros que alteram a dificuldade de uma run ficam aqui. */
#define BALANCEAMENTO_COMUM_VIDA_PER_ANDAR 3
#define BALANCEAMENTO_CHEFE_VIDA_PER_ANDAR 7
#define BALANCEAMENTO_CHEFE_PRESSAO 0.16f
#define BALANCEAMENTO_PROJETIL_BONUS_A_CADA 3
#define BALANCEAMENTO_LOJA_BASE_PRECO 5
#define BALANCEAMENTO_LOJA_PRECO_VARIACAO 7

/* Ataques defensivos especiais do Sineiro. Os valores sao conservadores de proposito: a faixa de
   fuga tem 48px, menos de um terco ate da menor arena, com 270px. */
#define BALANCEAMENTO_GRAVIDADE_FUGA_TEMPO 1.15f
#define BALANCEAMENTO_GRAVIDADE_FUGA_TEMPO_FASE_DOIS 0.85f
#define BALANCEAMENTO_GRAVIDADE_FORCA 430.0f
#define BALANCEAMENTO_GRAVIDADE_ESPINHO_PROFUNDIDADE 48.0f
#define BALANCEAMENTO_GRAVIDADE_ESPINHO_ATIVA_TEMPO 0.65f
#define BALANCEAMENTO_GRAVIDADE_ESPINHO_DESVANECER_TEMPO 0.12f
#define BALANCEAMENTO_GRAVIDADE_DANO 6
#define BALANCEAMENTO_SOMBRA_FLORACAO_TEMPO_CRESCIMENTO 1.0f
#define BALANCEAMENTO_SOMBRA_FLORACAO_ATRASO 0.13f
#define BALANCEAMENTO_SOMBRA_FLORACAO_RAIO 56.0f
#define BALANCEAMENTO_SOMBRA_FLORACAO_DANO 6

static inline float BalanceamentoMultiplicadorDanoRecebido(DificuldadeJogo dificuldade)
{
    static const float valores[] = {0.75f, 1.00f, 1.25f};
    return valores[dificuldade < DIFICULDADE_CALMA || dificuldade > DIFICULDADE_DIFICIL
                      ? DIFICULDADE_NORMAL : dificuldade];
}

static inline int BalanceamentoDificuldadeQte(int andar, DificuldadeJogo dificuldade)
{
    const int deslocamento = dificuldade == DIFICULDADE_CALMA ? -1 :
                       (dificuldade == DIFICULDADE_DIFICIL ? 1 : 0);
    const int valor = andar + deslocamento;
    return valor < 1 ? 1 : valor;
}

/* Sem inflacao oculta nos primeiros andares; os numeros ficam nas tabelas por ataque. */
static inline float BalanceamentoMultiplicadorDanoAndar(int andar)
{
    (void)andar;
    return 1.0f;
}


#define BALANCEAMENTO_IFRAMES 0.85f
#define BALANCEAMENTO_TETO_COMUM_PERCENTUAL 35
#define BALANCEAMENTO_TETO_FINAL_PERCENTUAL 45
#define BALANCEAMENTO_TETO_IMPACTO_LEGADO 8
#define BALANCEAMENTO_PASSO_FISICA (1.0f / 240.0f)
#define BALANCEAMENTO_AVISO_MINIMO 0.70f
#define BALANCEAMENTO_AVISO_CHEFE 0.90f
#define BALANCEAMENTO_AVISO_FINAL 1.20f

#define BALANCEAMENTO_CONVES_PREPARO 0.35f
#define BALANCEAMENTO_GELO_CONTATO 0.28f
#define BALANCEAMENTO_GELO_ACELERACAO 470.0f
#define BALANCEAMENTO_GELO_FREIO 250.0f
#define BALANCEAMENTO_LODO_VELOCIDADE 0.56f
#define BALANCEAMENTO_LODO_AMOSTRA 0.24f

typedef struct {
    const char *nome;
    int dano, teto_impacto, percentual_rodada, sangramento;
    float duracao, aviso, ciclo, velocidade, raio, intervalo;
    int quantidade;
} ConfiguracaoPiloto;

static const ConfiguracaoPiloto BALANCEAMENTO_PILOTO[PADRAO_TOTAL-PADRAO_PILOTO_POEIRA_ASA] = {
    {"Poeira de Asa", 3, 5, 35, 0, 5.0f, 0.70f, 2.4f, 92.0f, 4.0f, 0.36f, 9},
    {"Po em Contratempo", 4, 6, 35, 0, 6.0f, 0.70f, 2.4f, 92.0f, 4.0f, 0.36f, 9},
    {"Revoada Cega", 5, 6, 35, 0, 7.0f, 0.70f, 2.4f, 92.0f, 4.0f, 0.36f, 9},
    {"Adaga de Retorno", 4, 6, 35, 0, 5.0f, 0.70f, 1.60f, 125.0f, 4.0f, 0.55f, 3},
    {"Reflexo Roubado", 5, 7, 35, 0, 6.0f, 0.70f, 1.45f, 125.0f, 4.0f, 0.55f, 3},
    {"Roubo de Passagem", 6, 7, 35, 0, 7.0f, 0.70f, 1.65f, 125.0f, 4.0f, 0.55f, 3},
    {"Ancora de Coral", 5, 7, 35, 0, 7.0f, 0.90f, 2.15f, 175.0f, 8.0f, 0.38f, 7},
    {"Ressaca Calcificada", 6, 8, 35, 0, 8.0f, 0.90f, 2.8f, 122.0f, 4.0f, 0.52f, 12},
    {"Recife em Ruptura", 7, 8, 45, 0, 10.0f, 1.20f, 2.45f, 170.0f, 8.0f, 0.34f, 9},
    {"Estilhaco de Lanca", 4, 6, 35, 0, 5.50f, 0.70f, 1.70f, 155.0f, 4.0f, 0.30f, 5},
    {"Escudo Enraizado", 5, 7, 35, 0, 6.00f, 0.70f, 1.85f, 165.0f, 4.0f, 0.32f, 3},
    {"Tampas em Dobradica", 5, 7, 35, 0, 6.50f, 0.70f, 1.90f, 150.0f, 4.0f, 0.28f, 2},
    {"Frasco Agitado", 4, 6, 35, 0, 5.50f, 0.70f, 2.20f, 145.0f, 4.0f, 0.30f, 6},
    {"Receita Reversa", 4, 6, 35, 0, 6.00f, 0.70f, 2.20f, 125.0f, 4.0f, 0.42f, 3},
    {"Almofariz Transbordante", 5, 7, 35, 0, 6.50f, 0.70f, 2.60f, 175.0f, 4.0f, 0.30f, 8},
    {"Gotas Gemeas", 3, 5, 35, 0, 5.00f, 0.70f, 1.80f, 150.0f, 4.0f, 0.30f, 4},
    {"Arrasto de Menta", 4, 6, 35, 0, 5.50f, 0.70f, 2.00f, 150.0f, 4.0f, 0.30f, 2},
    {"Divisao Celular", 5, 6, 35, 0, 6.00f, 0.70f, 2.10f, 145.0f, 4.0f, 0.30f, 2},
    {"Sacudida Espinhosa", 4, 6, 35, 0, 5.50f, 0.70f, 2.20f, 280.0f, 4.0f, 1.35f, 7},
    {"Tocas em Cadeia", 4, 6, 35, 0, 6.00f, 0.70f, 1.80f, 180.0f, 4.0f, 0.30f, 4},
    {"Disparada da Roseira", 5, 7, 35, 0, 6.50f, 0.70f, 2.50f, 230.0f, 4.0f, 0.30f, 9},
    {"Ceifa Germinante", 6, 7, 35, 0, 7.00f, 0.90f, 2.50f, 185.0f, 4.0f, 0.18f, 7},
    {"Raizes de Procura", 6, 7, 35, 0, 8.00f, 0.90f, 2.60f, 150.0f, 4.0f, 0.24f, 3},
    {"Ritual da Colheita", 7, 8, 45, 0, 9.00f, 1.20f, 2.90f, 185.0f, 4.0f, 0.28f, 10},
    {"Tsunami da Falange", 5, 6, 35, 0, 6.00f, 0.80f, 2.20f, 190.0f, 4.0f, 0.32f, 1},
    {"Jato de Quadrante", 5, 6, 35, 0, 6.00f, 0.70f, 2.10f, 170.0f, 4.0f, 0.30f, 1},
    {"Conves a Deriva", 6, 7, 35, 0, 7.00f, 0.80f, 2.10f, 150.0f, 4.0f, 0.30f, 1},
    {"Pegadas do Brejo", 4, 6, 35, 0, 6.00f, 0.72f, 2.40f, 110.0f, 4.0f, 0.24f, 12},
    {"Bancos de Viscosidade", 5, 6, 35, 0, 6.50f, 0.80f, 2.50f, 165.0f, 4.0f, 0.40f, 3},
    {"Bolha Inversa", 5, 6, 35, 0, 7.00f, 0.80f, 2.70f, 130.0f, 4.0f, 0.30f, 1},
    {"Avalanche Obliqua", 5, 6, 35, 0, 6.50f, 0.80f, 3.10f, 210.0f, 4.0f, 0.38f, 2},
    {"Estacas de Geada", 6, 7, 35, 0, 7.00f, 0.80f, 2.80f, 250.0f, 4.0f, 0.14f, 5},
    {"Prisao Deslizante", 6, 7, 35, 0, 9.00f, 0.80f, 2.50f, 150.0f, 4.0f, 1.65f, 2},
    {"Jato Refratado", 4, 6, 35, 0, 6.00f, 0.70f, 2.10f, 160.0f, 4.0f, 0.30f, 1},
    {"Reflexo do Lago", 5, 6, 35, 0, 6.50f, 0.80f, 2.50f, 140.0f, 4.0f, 0.32f, 2},
    {"Membrana de Impulso", 6, 7, 35, 0, 7.00f, 0.80f, 2.20f, 220.0f, 4.0f, 1.50f, 1},
};

static inline bool BalanceamentoFinal(IdPadraoProjetil id)
{
    return id == PADRAO_PILOTO_RECIFE_RUPTURA || id == PADRAO_PLANTA_RITUAL ||
           id == PADRAO_SINO_DESAFIO || id == PADRAO_TORRE_DESAFIO;
}

static inline int BalanceamentoTetoImpacto(IdPadraoProjetil id)
{
    return id >= PADRAO_PILOTO_POEIRA_ASA && id < PADRAO_TOTAL ?
        BALANCEAMENTO_PILOTO[id - PADRAO_PILOTO_POEIRA_ASA].teto_impacto : BALANCEAMENTO_TETO_IMPACTO_LEGADO;
}

static inline int BalanceamentoSangramento(IdPadraoProjetil id)
{
    switch (id) {
        case PADRAO_LEBRE_BARREIRAS: case PADRAO_LEBRE_PARKOUR: case PADRAO_LEBRE_PULOS:
        case PADRAO_GATO_IMPACTO: case PADRAO_GATO_PERSEGUICAO: case PADRAO_GATO_EMBOSCADA:
        case PADRAO_CAVALEIRO_LANCAS: case PADRAO_CAVALEIRO_DESAFIO: case PADRAO_CAVALEIRO_DUELO: return 2;
        default: return 0;
    }
}

static const PadraoProjetil BALANCEAMENTO_PADROES[PADRAO_TOTAL] = {
    [PADRAO_AGUA_TSUNAMI]={.duracao=6.00f,.largura_caixa=320,.altura_caixa=178,.intervalo_criacao=99,.projeteis_por_criacao=1,.raio=4,.tipo=TIPO_AGUA,.dano=5,.dificuldade=1},
    [PADRAO_AGUA_JATO_QUADRANTE]={.duracao=6.00f,.largura_caixa=320,.altura_caixa=178,.intervalo_criacao=99,.projeteis_por_criacao=1,.raio=4,.tipo=TIPO_AGUA,.dano=5,.dificuldade=2},
    [PADRAO_AGUA_CONVES]={.duracao=7.00f,.largura_caixa=320,.altura_caixa=178,.intervalo_criacao=99,.projeteis_por_criacao=1,.raio=4,.tipo=TIPO_AGUA,.dano=6,.dificuldade=3},
    [PADRAO_AGUA_PEGADAS]={.duracao=6.00f,.largura_caixa=320,.altura_caixa=178,.intervalo_criacao=99,.projeteis_por_criacao=1,.raio=4,.tipo=TIPO_AGUA,.dano=4,.dificuldade=1},
    [PADRAO_AGUA_VISCOSIDADE]={.duracao=6.50f,.largura_caixa=320,.altura_caixa=178,.intervalo_criacao=99,.projeteis_por_criacao=1,.raio=4,.tipo=TIPO_AGUA,.dano=5,.dificuldade=2},
    [PADRAO_AGUA_BOLHA]={.duracao=7.00f,.largura_caixa=320,.altura_caixa=178,.intervalo_criacao=99,.projeteis_por_criacao=1,.raio=4,.tipo=TIPO_AGUA,.dano=5,.dificuldade=3},
    [PADRAO_AGUA_AVALANCHE]={.duracao=6.50f,.largura_caixa=320,.altura_caixa=178,.intervalo_criacao=99,.projeteis_por_criacao=1,.raio=4,.tipo=TIPO_AGUA,.dano=5,.dificuldade=1},
    [PADRAO_AGUA_ESTACAS]={.duracao=7.00f,.largura_caixa=320,.altura_caixa=178,.intervalo_criacao=99,.projeteis_por_criacao=1,.raio=4,.tipo=TIPO_AGUA,.dano=6,.dificuldade=2},
    [PADRAO_AGUA_PRISAO]={.duracao=9.00f,.largura_caixa=320,.altura_caixa=178,.intervalo_criacao=99,.projeteis_por_criacao=1,.raio=4,.tipo=TIPO_AGUA,.dano=6,.dificuldade=3},
    [PADRAO_AGUA_REFRACAO]={.duracao=6.00f,.largura_caixa=320,.altura_caixa=178,.intervalo_criacao=99,.projeteis_por_criacao=1,.raio=4,.tipo=TIPO_AGUA,.dano=4,.dificuldade=1},
    [PADRAO_AGUA_REFLEXO]={.duracao=6.50f,.largura_caixa=320,.altura_caixa=178,.intervalo_criacao=99,.projeteis_por_criacao=1,.raio=4,.tipo=TIPO_AGUA,.dano=5,.dificuldade=2},
    [PADRAO_AGUA_MEMBRANA]={.duracao=7.00f,.largura_caixa=320,.altura_caixa=178,.intervalo_criacao=99,.projeteis_por_criacao=1,.raio=4,.tipo=TIPO_AGUA,.dano=6,.dificuldade=3},
    [PADRAO_PLANTA_ESTILHACO_LANCA] = {.duracao=5.50f,.largura_caixa=320,.altura_caixa=178,.intervalo_criacao=99,.projeteis_por_criacao=1,.raio=4,.tipo=TIPO_PLANTA,.dano=4,.dificuldade=1},
    [PADRAO_PLANTA_ESCUDO_ENRAIZADO] = {.duracao=6.00f,.largura_caixa=320,.altura_caixa=178,.intervalo_criacao=99,.projeteis_por_criacao=1,.raio=4,.tipo=TIPO_PLANTA,.dano=5,.dificuldade=2},
    [PADRAO_PLANTA_TAMPAS_DOBRADICA] = {.duracao=6.50f,.largura_caixa=320,.altura_caixa=178,.intervalo_criacao=99,.projeteis_por_criacao=1,.raio=4,.tipo=TIPO_PLANTA,.dano=5,.dificuldade=3},
    [PADRAO_PLANTA_FRASCO_AGITADO] = {.duracao=5.50f,.largura_caixa=320,.altura_caixa=178,.intervalo_criacao=99,.projeteis_por_criacao=1,.raio=4,.tipo=TIPO_PLANTA,.dano=4,.dificuldade=1},
    [PADRAO_PLANTA_RECEITA_REVERSA] = {.duracao=6.00f,.largura_caixa=320,.altura_caixa=178,.intervalo_criacao=99,.projeteis_por_criacao=1,.raio=4,.tipo=TIPO_PLANTA,.dano=4,.dificuldade=2},
    [PADRAO_PLANTA_ALMOFARIZ] = {.duracao=6.50f,.largura_caixa=320,.altura_caixa=178,.intervalo_criacao=99,.projeteis_por_criacao=1,.raio=4,.tipo=TIPO_PLANTA,.dano=5,.dificuldade=3},
    [PADRAO_PLANTA_GOTAS_GEMEAS] = {.duracao=5.00f,.largura_caixa=320,.altura_caixa=178,.intervalo_criacao=99,.projeteis_por_criacao=1,.raio=4,.tipo=TIPO_PLANTA,.dano=3,.dificuldade=1},
    [PADRAO_PLANTA_ARRASTO_MENTA] = {.duracao=5.50f,.largura_caixa=320,.altura_caixa=178,.intervalo_criacao=99,.projeteis_por_criacao=1,.raio=4,.tipo=TIPO_PLANTA,.dano=4,.dificuldade=2},
    [PADRAO_PLANTA_DIVISAO_CELULAR] = {.duracao=6.00f,.largura_caixa=320,.altura_caixa=178,.intervalo_criacao=99,.projeteis_por_criacao=1,.raio=4,.tipo=TIPO_PLANTA,.dano=5,.dificuldade=3},
    [PADRAO_PLANTA_SACUDIDA] = {.duracao=5.50f,.largura_caixa=320,.altura_caixa=178,.intervalo_criacao=99,.projeteis_por_criacao=1,.raio=4,.tipo=TIPO_PLANTA,.dano=4,.dificuldade=1},
    [PADRAO_PLANTA_TOCAS_CADEIA] = {.duracao=6.00f,.largura_caixa=320,.altura_caixa=178,.intervalo_criacao=99,.projeteis_por_criacao=1,.raio=4,.tipo=TIPO_PLANTA,.dano=4,.dificuldade=2},
    [PADRAO_PLANTA_DISPARADA] = {.duracao=6.50f,.largura_caixa=320,.altura_caixa=178,.intervalo_criacao=99,.projeteis_por_criacao=1,.raio=4,.tipo=TIPO_PLANTA,.dano=5,.dificuldade=3},
    [PADRAO_PLANTA_CEIFA] = {.duracao=7.00f,.largura_caixa=320,.altura_caixa=178,.intervalo_criacao=99,.projeteis_por_criacao=1,.raio=4,.tipo=TIPO_PLANTA,.dano=6,.dificuldade=3},
    [PADRAO_PLANTA_RAIZES_PROCURA] = {.duracao=8.00f,.largura_caixa=320,.altura_caixa=178,.intervalo_criacao=99,.projeteis_por_criacao=1,.raio=4,.tipo=TIPO_PLANTA,.dano=6,.dificuldade=3},
    [PADRAO_PLANTA_RITUAL] = {.duracao=9.00f,.largura_caixa=320,.altura_caixa=178,.intervalo_criacao=99,.projeteis_por_criacao=1,.raio=4,.tipo=TIPO_PLANTA,.dano=7,.dificuldade=3},
#define CARACTERISTICO(id, element, power) [id] = { .duracao = 10.8f, .largura_caixa = 320, \
    .altura_caixa = 200, .intervalo_criacao = 99, .projeteis_por_criacao = 1, .raio = 5, \
    .tipo = element, .dano = power, .dificuldade = 2 }
    CARACTERISTICO(PADRAO_MARIPOSA_ASAS, TIPO_SOMBRA, 3),
    CARACTERISTICO(PADRAO_SAPO_SALTOS, TIPO_AGUA, 4),
    CARACTERISTICO(PADRAO_PASSARO_MERGULHO, TIPO_FOGO, 4),
    CARACTERISTICO(PADRAO_LIMO_CRESCIMENTO, TIPO_PLANTA, 3),
    CARACTERISTICO(PADRAO_FAGULHA_LUZ, TIPO_LUZ, 4),
    CARACTERISTICO(PADRAO_LEBRE_PULOS, TIPO_PLANTA, 4),
    CARACTERISTICO(PADRAO_GATO_EMBOSCADA, TIPO_SOMBRA, 4),
    CARACTERISTICO(PADRAO_CAVALEIRO_DUELO, TIPO_LUZ, 4),
    CARACTERISTICO(PADRAO_SINO_ECO, TIPO_LUZ, 5),
    CARACTERISTICO(PADRAO_TORRE_ESMAGAMENTO, TIPO_NEUTRO, 5),
    CARACTERISTICO(PADRAO_MARIPOSA_TRAMA, TIPO_SOMBRA, 3),
    CARACTERISTICO(PADRAO_MARIPOSA_CORRENTE, TIPO_SOMBRA, 3),
    CARACTERISTICO(PADRAO_SAPO_BARREIRAS, TIPO_AGUA, 4),
    CARACTERISTICO(PADRAO_SAPO_VITORIAS_REGIAS, TIPO_AGUA, 4),
    CARACTERISTICO(PADRAO_PASSARO_FOGO_CRUZADO, TIPO_FOGO, 4),
    CARACTERISTICO(PADRAO_PASSARO_TUNEL, TIPO_FOGO, 4),
    CARACTERISTICO(PADRAO_LIMO_MARE, TIPO_PLANTA, 3),
    CARACTERISTICO(PADRAO_LIMO_BALSAS, TIPO_PLANTA, 3),
    CARACTERISTICO(PADRAO_FAGULHA_CANHOES, TIPO_LUZ, 4),
    CARACTERISTICO(PADRAO_FAGULHA_ORBITA, TIPO_LUZ, 4),
    CARACTERISTICO(PADRAO_LEBRE_BARREIRAS, TIPO_PLANTA, 4),
    CARACTERISTICO(PADRAO_LEBRE_PARKOUR, TIPO_PLANTA, 4),
    CARACTERISTICO(PADRAO_GATO_IMPACTO, TIPO_SOMBRA, 4),
    CARACTERISTICO(PADRAO_GATO_PERSEGUICAO, TIPO_SOMBRA, 4),
    CARACTERISTICO(PADRAO_CAVALEIRO_LANCAS, TIPO_LUZ, 4),
    CARACTERISTICO(PADRAO_CAVALEIRO_DESAFIO, TIPO_LUZ, 4),
    CARACTERISTICO(PADRAO_SINO_CANHOES, TIPO_LUZ, 5),
    CARACTERISTICO(PADRAO_SINO_IMPACTO, TIPO_SOMBRA, 5),
    CARACTERISTICO(PADRAO_SINO_DESAFIO, TIPO_LUZ, 5),
    CARACTERISTICO(PADRAO_TORRE_PARKOUR, TIPO_NEUTRO, 5),
    CARACTERISTICO(PADRAO_TORRE_CANHOES, TIPO_NEUTRO, 5),
    CARACTERISTICO(PADRAO_TORRE_DESAFIO, TIPO_NEUTRO, 5),
    CARACTERISTICO(PADRAO_SINO_GAS_SOMBRA, TIPO_SOMBRA, 5),
    CARACTERISTICO(PADRAO_CHUVA_FOGO, TIPO_FOGO, 4),
    CARACTERISTICO(PADRAO_SARCAS_PORTOES, TIPO_PLANTA, 4),
    CARACTERISTICO(PADRAO_HERBALISTA_FRASCOS, TIPO_PLANTA, 4),
    CARACTERISTICO(PADRAO_LEGIONARIO_ESCUDOS, TIPO_AGUA, 4),
    CARACTERISTICO(PADRAO_BRUXA_CALDEIRAO, TIPO_AGUA, 4),
    CARACTERISTICO(PADRAO_REVENANTE_PRISAO, TIPO_AGUA, 5),
    CARACTERISTICO(PADRAO_ESQUELETO_ESTOUPIDO, TIPO_FOGO, 5),
    CARACTERISTICO(PADRAO_ACENDEDOR_MARTELO, TIPO_FOGO, 5),
    CARACTERISTICO(PADRAO_PENITENTE_INCENSARIO, TIPO_SOMBRA, 5),
    CARACTERISTICO(PADRAO_LADRAO_REFLEXO, TIPO_SOMBRA, 5),
    CARACTERISTICO(PADRAO_ACOLITO_VITRAL, TIPO_LUZ, 5),
    CARACTERISTICO(PADRAO_GOBLIN_ESTOCADAS, TIPO_NEUTRO, 5),
    CARACTERISTICO(PADRAO_AUTOMATO_ENGRENAGENS, TIPO_NEUTRO, 5),
    CARACTERISTICO(PADRAO_MERCENARIO_RICOCHETE, TIPO_NEUTRO, 5),
    CARACTERISTICO(PADRAO_CARCEREIRO_GRADE, TIPO_NEUTRO, 5),
    CARACTERISTICO(PADRAO_CULTISTA_LABIRINTO, TIPO_PLANTA, 6),
    CARACTERISTICO(PADRAO_AFOGADO_MARE_CORAL, TIPO_AGUA, 6),
    CARACTERISTICO(PADRAO_MAGO_ESPIRAL_CINZAS, TIPO_FOGO, 6),
    CARACTERISTICO(PADRAO_GOBLIN_EXERCITO, TIPO_SOMBRA, 6),
    CARACTERISTICO(PADRAO_INQUISIDOR_VEREDITO, TIPO_LUZ, 6),
#undef CARACTERISTICO
    [PADRAO_PILOTO_POEIRA_ASA] = {.duracao = 5.0f, .largura_caixa = 320, .altura_caixa = 178, .intervalo_criacao = 99, .projeteis_por_criacao = 1, .raio = 4, .tipo = TIPO_SOMBRA, .dano = 3, .dificuldade = 1},
    [PADRAO_PILOTO_PO_CONTRATEMPO] = {.duracao = 6.0f, .largura_caixa = 320, .altura_caixa = 178, .intervalo_criacao = 99, .projeteis_por_criacao = 1, .raio = 4, .tipo = TIPO_SOMBRA, .dano = 4, .dificuldade = 2},
    [PADRAO_PILOTO_REVOADA_CEGA] = {.duracao = 7.0f, .largura_caixa = 320, .altura_caixa = 178, .intervalo_criacao = 99, .projeteis_por_criacao = 1, .raio = 4, .tipo = TIPO_SOMBRA, .dano = 5, .dificuldade = 3},
    [PADRAO_PILOTO_ADAGA_RETORNO] = {.duracao = 5.0f, .largura_caixa = 320, .altura_caixa = 178, .intervalo_criacao = 99, .projeteis_por_criacao = 1, .raio = 4, .tipo = TIPO_SOMBRA, .dano = 4, .dificuldade = 1},
    [PADRAO_PILOTO_REFLEXO_ROUBADO] = {.duracao = 6.0f, .largura_caixa = 320, .altura_caixa = 178, .intervalo_criacao = 99, .projeteis_por_criacao = 1, .raio = 4, .tipo = TIPO_SOMBRA, .dano = 5, .dificuldade = 2},
    [PADRAO_PILOTO_ROUBO_PASSAGEM] = {.duracao = 7.0f, .largura_caixa = 320, .altura_caixa = 178, .intervalo_criacao = 99, .projeteis_por_criacao = 1, .raio = 4, .tipo = TIPO_SOMBRA, .dano = 6, .dificuldade = 3},
    [PADRAO_PILOTO_ANCORA_CORAL] = {.duracao = 6.0f, .largura_caixa = 320, .altura_caixa = 178, .intervalo_criacao = 99, .projeteis_por_criacao = 1, .raio = 4, .tipo = TIPO_AGUA, .dano = 5, .dificuldade = 3},
    [PADRAO_PILOTO_RESSACA_CALCIFICADA] = {.duracao = 8.0f, .largura_caixa = 320, .altura_caixa = 178, .intervalo_criacao = 99, .projeteis_por_criacao = 1, .raio = 4, .tipo = TIPO_AGUA, .dano = 6, .dificuldade = 3},
    [PADRAO_PILOTO_RECIFE_RUPTURA] = {.duracao = 10.0f, .largura_caixa = 320, .altura_caixa = 178, .intervalo_criacao = 99, .projeteis_por_criacao = 1, .raio = 4, .tipo = TIPO_AGUA, .dano = 7, .dificuldade = 3},
    [PADRAO_CHUVA] = {
        6.0f, 300.0f, 154.0f, 0.34f, 2, 106.0f, 6.0f, 0.0f,
        TIPO_AGUA, 3, 1, CRIACAO_CHUVA
    },
    [PADRAO_LATERAL_ONDA] = {
        7.0f, 320.0f, 168.0f, 0.88f, 7, 126.0f, 7.0f, 0.0f,
        TIPO_SOMBRA, 4, 2, CRIACAO_LATERAL_ONDA
    },
    [PADRAO_ESPIRAL] = {
        7.0f, 310.0f, 190.0f, 0.62f, 4, 82.0f, 5.0f, 1.57f,
        TIPO_FOGO, 3, 3, CRIACAO_ESPIRAL
    },
    [PADRAO_RAPIDA_CHUVA] = {
        6.0f, 300.0f, 154.0f, 0.24f, 3, 135.0f, 5.0f, 0.0f,
        TIPO_LUZ, 4, 3, CRIACAO_CHUVA
    },
    [PADRAO_LARGA_ONDA] = {
        7.0f, 330.0f, 180.0f, 0.62f, 9, 145.0f, 6.0f, 0.0f,
        TIPO_PLANTA, 4, 2, CRIACAO_LATERAL_ONDA
    },
    [PADRAO_FECHADA_ESPIRAL] = {
        7.0f, 270.0f, 160.0f, 0.30f, 5, 120.0f, 5.0f, 1.25f,
        TIPO_SOMBRA, 5, 3, CRIACAO_ESPIRAL
    },
    [PADRAO_PETALA_ORBITA] = {
        6.5f, 300.0f, 175.0f, 0.36f, 6, 82.0f, 5.0f, 1.05f,
        TIPO_PLANTA, 4, 2, CRIACAO_ESPIRAL
    },
    [PADRAO_REBOTE_ORBES] = {
        6.5f, 310.0f, 170.0f, 0.55f, 4, 116.0f, 7.0f, 0.75f,
        TIPO_AGUA, 4, 2, CRIACAO_ESPIRAL
    },
    [PADRAO_MOVEL_PAREDE] = {
        7.0f, 335.0f, 180.0f, 0.72f, 10, 130.0f, 6.0f, 0.0f,
        TIPO_NEUTRO, 5, 3, CRIACAO_LATERAL_ONDA
    },
    [PADRAO_DIVISAO_DISPAROS] = {
        6.5f, 320.0f, 190.0f, 0.65f, 2, 112.0f, 5.0f, 0.48f,
        TIPO_FOGO, 5, 3, CRIACAO_CHUVA
    },
    [PADRAO_CRUZADOS_LASERS] = {
        6.0f, 320.0f, 170.0f, 0.90f, 8, 158.0f, 5.0f, 0.0f,
        TIPO_LUZ, 6, 3, CRIACAO_LATERAL_ONDA
    },
    [PADRAO_PAUSA_DARDOS] = {
        6.5f, 290.0f, 165.0f, 0.46f, 5, 72.0f, 6.0f, 1.26f,
        TIPO_SOMBRA, 5, 2, CRIACAO_ESPIRAL
    },
    [PADRAO_ANEL_EXPLOSAO] = {
        6.0f, 300.0f, 170.0f, 0.58f, 8, 110.0f, 5.0f, 0.78f,
        TIPO_LUZ, 5, 2, CRIACAO_ESPIRAL
    },
    [PADRAO_DIAGONAL_CHUVA] = {
        6.5f, 320.0f, 190.0f, 0.48f, 3, 120.0f, 5.0f, 0.0f,
        TIPO_FOGO, 4, 2, CRIACAO_CHUVA
    },
    [PADRAO_PERSEGUICAO_ONDA] = {
        7.0f, 320.0f, 176.0f, 0.66f, 8, 152.0f, 6.0f, 0.0f,
        TIPO_AGUA, 5, 3, CRIACAO_LATERAL_ONDA
    },
    [PADRAO_CATA_VENTO] = {
        7.0f, 285.0f, 165.0f, 0.34f, 6, 126.0f, 5.0f, 1.05f,
        TIPO_PLANTA, 5, 3, CRIACAO_ESPIRAL
    },
    [PADRAO_GRAVIDADE_INVERSAO] = {
        8.0f, 300.0f, 175.0f, 99.0f, 1, 0.0f, 1.0f, 0.0f,
        TIPO_PLANTA, BALANCEAMENTO_GRAVIDADE_DANO, 3, CRIACAO_ESPIRAL
    },
    [PADRAO_SOMBRA_FLORACAO] = {
        7.0f, 300.0f, 175.0f, 99.0f, 1, 0.0f, 1.0f, 0.0f,
        TIPO_SOMBRA, BALANCEAMENTO_SOMBRA_FLORACAO_DANO, 3, CRIACAO_ESPIRAL
    }
};

#endif
