#include "save.h"

#include <stdio.h>
#include <string.h>

#define SALVAMENTO_MAGIC 0x41535452u
#define CAMINHO_SALVAMENTO "save.dat"

typedef struct {
    uint32_t assinatura;
    uint32_t versao;
    uint32_t soma_verificacao;
} CabecalhoSalvamento;

/* Layout exato da versao 2: carrega o progresso preservado e inicia o tutorial. */
typedef struct {
    int melhor_andar, total_runs, vitorias, vitorias_misericordia, vitorias_dano, fragmentos;
    uint32_t itens_desbloqueados, ataques_desbloqueados;
    float volume_musica, volume_efeitos;
    bool tela_cheia, tremor_linha, efeitos_reduzidos;
    unsigned int tutorial_visto;
    DificuldadeJogo dificuldade;
    TipoEscolhaReliquia tipo_reliquia;
    int id_reliquia;
} DadosSalvosV2;

typedef struct {
    int floor_required;
    int victories_required;
    int mercy_required;
    TipoEscolhaReliquia categoria;
    int id;
} RegraDesbloqueio;

static const RegraDesbloqueio DESBLOQUEIOS[] = {
    {2, 0, 0, RELIQUIA_ITEM, ITEM_LUA_ERVA}, {3, 0, 0, RELIQUIA_ATAQUE, 3},
    {5, 0, 0, RELIQUIA_ITEM, ITEM_TORRE_SINO}, {5, 0, 0, RELIQUIA_ATAQUE, 6},
    {1, 1, 0, RELIQUIA_ATAQUE, 9}, {1, 0, 10, RELIQUIA_ITEM, ITEM_ESPINHO_BOMBA}
};

static uint32_t SomaVerificacaoBytes(const void *data, size_t tamanho)
{
    const unsigned char *bytes = data;
    uint32_t valor = 2166136261u;

    for (size_t indice = 0; indice < tamanho; indice++) valor = (valor ^ bytes[indice]) * 16777619u;
    return valor;
}

static uint32_t SomaVerificacao(const DadosSalvos *salvamento) { return SomaVerificacaoBytes(salvamento, sizeof(*salvamento)); }

void SalvamentoDefinirPadroes(DadosSalvos *salvamento)
{
    memset(salvamento, 0, sizeof(*salvamento));
    salvamento->volume_musica = 0.70f;
    salvamento->volume_efeitos = 0.80f;
    salvamento->tremor_linha = true;
    salvamento->dificuldade = DIFICULDADE_NORMAL;
    salvamento->itens_desbloqueados = (1u << ITEM_CHA) | (1u << ITEM_VERMELHA_FITA) |
                            (1u << ITEM_MOLHADO_MANTO) | (1u << ITEM_SOLAR_BROCHE);
    salvamento->ataques_desbloqueados = (1u << 0) | (1u << 1) | (1u << 2);
}

bool SalvamentoCarregarCaminho(DadosSalvos *salvamento, const char *caminho)
{
    CabecalhoSalvamento cabecalho;
    DadosSalvos carregado;
    FILE *arquivo = fopen(caminho, "rb");

    if (arquivo == NULL) {
        SalvamentoDefinirPadroes(salvamento);
        return false;
    }
    const bool header_ok = fread(&cabecalho, sizeof(cabecalho), 1, arquivo) == 1 && cabecalho.assinatura == SALVAMENTO_MAGIC;
    bool valido = false;
    if (header_ok && cabecalho.versao == SALVAMENTO_VERSAO) {
        valido = fread(&carregado, sizeof(carregado), 1, arquivo) == 1 && cabecalho.soma_verificacao == SomaVerificacao(&carregado);
    } else if (header_ok && cabecalho.versao == 2u) {
        DadosSalvosV2 antigo = {0};
        if (fread(&antigo, sizeof(antigo), 1, arquivo) == 1 &&
            cabecalho.soma_verificacao == SomaVerificacaoBytes(&antigo, sizeof(antigo))) {
            SalvamentoDefinirPadroes(&carregado);
            carregado.melhor_andar = antigo.melhor_andar; carregado.total_runs = antigo.total_runs;
            carregado.vitorias = antigo.vitorias; carregado.vitorias_misericordia = antigo.vitorias_misericordia;
            carregado.vitorias_dano = antigo.vitorias_dano; carregado.fragmentos = antigo.fragmentos;
            carregado.itens_desbloqueados = antigo.itens_desbloqueados; carregado.ataques_desbloqueados = antigo.ataques_desbloqueados;
            carregado.volume_musica = antigo.volume_musica; carregado.volume_efeitos = antigo.volume_efeitos;
            carregado.tela_cheia = antigo.tela_cheia; carregado.tremor_linha = antigo.tremor_linha;
            carregado.efeitos_reduzidos = antigo.efeitos_reduzidos; carregado.tutorial_visto = antigo.tutorial_visto;
            carregado.dificuldade = antigo.dificuldade; carregado.tipo_reliquia = antigo.tipo_reliquia; carregado.id_reliquia = antigo.id_reliquia;
            carregado.tutorial_concluido = false;
            valido = true;
        }
    }
    fclose(arquivo);
    if (!valido) {
        SalvamentoDefinirPadroes(salvamento);
        return false;
    }
    *salvamento = carregado;
    return true;
}

bool SalvamentoCarregar(DadosSalvos *salvamento) { return SalvamentoCarregarCaminho(salvamento, CAMINHO_SALVAMENTO); }

bool SalvamentoGravarCaminho(const DadosSalvos *salvamento, const char *caminho)
{
    const CabecalhoSalvamento cabecalho = {SALVAMENTO_MAGIC, SALVAMENTO_VERSAO, SomaVerificacao(salvamento)};
    FILE *arquivo = fopen(caminho, "wb");

    if (arquivo == NULL) return false;
    const bool gravado = fwrite(&cabecalho, sizeof(cabecalho), 1, arquivo) == 1 &&
                         fwrite(salvamento, sizeof(*salvamento), 1, arquivo) == 1;
    fclose(arquivo);
    return gravado;
}

bool SalvamentoGravar(const DadosSalvos *salvamento) { return SalvamentoGravarCaminho(salvamento, CAMINHO_SALVAMENTO); }

void SalvamentoAplicarDesbloqueios(DadosSalvos *salvamento)
{
    for (size_t indice = 0; indice < sizeof(DESBLOQUEIOS) / sizeof(DESBLOQUEIOS[0]); indice++) {
        const RegraDesbloqueio *regra = &DESBLOQUEIOS[indice];
        if (salvamento->melhor_andar >= regra->floor_required && salvamento->vitorias >= regra->victories_required &&
            salvamento->vitorias_misericordia >= regra->mercy_required) {
            if (regra->categoria == RELIQUIA_ITEM) salvamento->itens_desbloqueados |= 1u << regra->id;
            else salvamento->ataques_desbloqueados |= 1u << regra->id;
        }
    }
}

bool SalvamentoItemDesbloqueado(const DadosSalvos *salvamento, IdItem id)
{
    return id >= 0 && id < ITEM_TOTAL && (salvamento->itens_desbloqueados & (1u << id)) != 0;
}

bool SalvamentoAtaqueDesbloqueado(const DadosSalvos *salvamento, int id_ataque)
{
    return id_ataque >= 0 && id_ataque < SALVAMENTO_ATAQUE_BITS &&
           (salvamento->ataques_desbloqueados & (1u << id_ataque)) != 0;
}

bool SalvamentoComprarReliquia(DadosSalvos *salvamento, TipoEscolhaReliquia categoria, int id, int preco)
{
    const bool unlocked = categoria == RELIQUIA_ITEM ? SalvamentoItemDesbloqueado(salvamento, (IdItem)id) :
                          SalvamentoAtaqueDesbloqueado(salvamento, id);

    if (!unlocked || salvamento->fragmentos < preco) return false;
    salvamento->fragmentos -= preco;
    salvamento->tipo_reliquia = categoria;
    salvamento->id_reliquia = id;
    return true;
}
