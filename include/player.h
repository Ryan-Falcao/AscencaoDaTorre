#ifndef JOGADOR_H
#define JOGADOR_H

#include <stdbool.h>

#include "types.h"
#include "qte.h"
#include "items.h"

#define MAXIMO_EQUIPADOS_ATAQUES 4
#define MAXIMO_INVENTARIO_ITENS 8
#define JOGADOR_ATAQUE_TOTAL 16

typedef enum {
    EFEITO_IMPACTO, EFEITO_PROJETIL, EFEITO_CORTE, EFEITO_CHUVA, EFEITO_FEIXE
} EstiloEfeitoAtaque;

typedef enum {
    STATUS_NENHUM, STATUS_QUEIMAR, STATUS_MOLHADO, STATUS_ENRAIZAR, STATUS_CEGAR, STATUS_ATORDOAR
} EfeitoStatus;

typedef enum {
    ATAQUE_BRASA, ATAQUE_GOTA_CLARA, ATAQUE_RAIZ_RAPIDA, ATAQUE_RAIO_MANSO,
    ATAQUE_ECO_SOMBRIO, ATAQUE_GOLPE_NEUTRO, ATAQUE_CHAMA_ALTA, ATAQUE_MARE_CURTA,
    ATAQUE_VINHA_FINA, ATAQUE_LUZ_GUIA, ATAQUE_SOMBRA_FRIA, ATAQUE_PULSO_CALMO,
    ATAQUE_FAGULHA, ATAQUE_ORVALHO, ATAQUE_ESPINHO_LUNAR, ATAQUE_SINO_VAZIO
} IdAtaque;

typedef struct {
    IdAtaque id;
    char nome[24];
    Tipo tipo;
    TipoQte tipo_qte;
    int dano_base;
    EstiloEfeitoAtaque estilo_efeito;
    int golpes;
    EfeitoStatus status;
    int duracao_status;
    int turnos_recarga;
    bool critico_no_perfeito;
} Ataque;

typedef struct {
    IdItem id;
    int quantidade;
} ItemInventario;

typedef struct {
    char nome[24];
    int hp;
    int vida_maxima;
    int nivel;
    Tipo tipo_defesa;
    Ataque ataques[MAXIMO_EQUIPADOS_ATAQUES];
    int total_ataques;
    ItemInventario inventario[MAXIMO_INVENTARIO_ITENS];
    int percentual_buff_dano;
    int turnos_buff_dano;
    int bonus_dano_permanente;
    int xp;
    float multiplicador_dano_recebido;
    int recargas_ataques[JOGADOR_ATAQUE_TOTAL];
} AtributosJogador;

void JogadorInicializar(AtributosJogador *jogador);
int JogadorCurar(AtributosJogador *jogador, int quantidade);
bool JogadorUsarItem(AtributosJogador *jogador, int indice_inventario, const DefinicaoItem **item_usado,
                   int *valor_aplicado);
float JogadorMultiplicadorBuffAtaque(const AtributosJogador *jogador);
void JogadorConsumirBuffAtaque(AtributosJogador *jogador);
bool JogadorAdicionarItem(AtributosJogador *jogador, IdItem id, int quantidade);
bool JogadorPossuiAtaque(const AtributosJogador *jogador, Ataque ataque);
int JogadorTotalAtaquesRecompensa(void);
Ataque JogadorAtaqueRecompensa(int indice);
bool JogadorAdicionarAtaque(AtributosJogador *jogador, Ataque ataque, int indice_substituicao);
bool JogadorGanharXp(AtributosJogador *jogador, int xp);
bool JogadorAtaqueDisponivel(const AtributosJogador *jogador, const Ataque *ataque, char *motivo, int tamanho_motivo);
void JogadorUsarAtaque(AtributosJogador *jogador, const Ataque *ataque);
void JogadorAvancarRecargasAtaques(AtributosJogador *jogador);
const char *JogadorNomeStatus(EfeitoStatus status);

#endif
