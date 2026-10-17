#ifndef SALVAMENTO_H
#define SALVAMENTO_H

#include <stdbool.h>
#include <stdint.h>

#include "items.h"

#define SALVAMENTO_VERSAO 3u
#define SALVAMENTO_ATAQUE_BITS 16

typedef enum {
    RELIQUIA_NENHUM,
    RELIQUIA_ITEM,
    RELIQUIA_ATAQUE
} TipoEscolhaReliquia;

typedef enum {
    DIFICULDADE_CALMA,
    DIFICULDADE_NORMAL,
    DIFICULDADE_DIFICIL
} DificuldadeJogo;

typedef struct {
    int melhor_andar;
    int total_runs;
    int vitorias;
    int vitorias_misericordia;
    int vitorias_dano;
    int fragmentos;
    uint32_t itens_desbloqueados;
    uint32_t ataques_desbloqueados;
    float volume_musica;
    float volume_efeitos;
    bool tela_cheia;
    bool tremor_linha;
    bool efeitos_reduzidos;
    unsigned int tutorial_visto;
    DificuldadeJogo dificuldade;
    TipoEscolhaReliquia tipo_reliquia;
    int id_reliquia;
    bool tutorial_concluido;
} DadosSalvos;

void SalvamentoDefinirPadroes(DadosSalvos *salvamento);
bool SalvamentoCarregar(DadosSalvos *salvamento);
bool SalvamentoCarregarCaminho(DadosSalvos *salvamento, const char *caminho);
bool SalvamentoGravar(const DadosSalvos *salvamento);
bool SalvamentoGravarCaminho(const DadosSalvos *salvamento, const char *caminho);
void SalvamentoAplicarDesbloqueios(DadosSalvos *salvamento);
bool SalvamentoItemDesbloqueado(const DadosSalvos *salvamento, IdItem id);
bool SalvamentoAtaqueDesbloqueado(const DadosSalvos *salvamento, int id_ataque);
bool SalvamentoComprarReliquia(DadosSalvos *salvamento, TipoEscolhaReliquia categoria, int id, int preco);

#endif
