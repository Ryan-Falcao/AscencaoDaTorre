#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "save.h"

typedef struct {
    uint32_t assinatura, versao, soma_verificacao;
} CabecalhoTeste;

typedef struct {
    int melhor_andar, total_runs, vitorias, vitorias_misericordia, vitorias_dano, fragmentos;
    uint32_t itens_desbloqueados, ataques_desbloqueados;
    float volume_musica, volume_efeitos;
    bool tela_cheia, tremor_linha, efeitos_reduzidos;
    unsigned int tutorial_visto;
    DificuldadeJogo dificuldade;
    TipoEscolhaReliquia tipo_reliquia;
    int id_reliquia;
} TesteDadosSalvosV2;

static uint32_t SomaVerificacaoV2(const TesteDadosSalvosV2 *salvamento)
{
    const unsigned char *bytes = (const unsigned char *)salvamento;
    uint32_t valor = 2166136261u;
    for (size_t indice = 0; indice < sizeof(*salvamento); indice++) valor = (valor ^ bytes[indice]) * 16777619u;
    return valor;
}

int main(void)
{
    const char *caminho = "test_save_temp.dat";
    DadosSalvos saved;
    DadosSalvos carregado;
    FILE *arquivo;

    SalvamentoDefinirPadroes(&saved);
    saved.melhor_andar = 5;
    saved.fragmentos = 22;
    saved.vitorias_misericordia = 10;
    saved.efeitos_reduzidos = true;
    saved.dificuldade = DIFICULDADE_DIFICIL;
    SalvamentoAplicarDesbloqueios(&saved);
    assert(SalvamentoAtaqueDesbloqueado(&saved, 3));
    assert(SalvamentoItemDesbloqueado(&saved, ITEM_ESPINHO_BOMBA));
    assert(SalvamentoGravarCaminho(&saved, caminho));
    assert(SalvamentoCarregarCaminho(&carregado, caminho));
    assert(carregado.melhor_andar == 5 && carregado.fragmentos == 22);
    assert(carregado.efeitos_reduzidos && carregado.dificuldade == DIFICULDADE_DIFICIL);

    {
        TesteDadosSalvosV2 antigo = {0};
        antigo.melhor_andar = 4;
        antigo.fragmentos = 9;
        antigo.volume_musica = 0.5f;
        antigo.volume_efeitos = 0.6f;
        antigo.dificuldade = DIFICULDADE_CALMA;
        arquivo = fopen(caminho, "wb");
        assert(arquivo != NULL);
        const CabecalhoTeste cabecalho_antigo = {0x41535452u, 2u, SomaVerificacaoV2(&antigo)};
        assert(fwrite(&cabecalho_antigo, sizeof(cabecalho_antigo), 1, arquivo) == 1);
        assert(fwrite(&antigo, sizeof(antigo), 1, arquivo) == 1);
        fclose(arquivo);
        assert(SalvamentoCarregarCaminho(&carregado, caminho));
        assert(carregado.melhor_andar == 4 && carregado.fragmentos == 9);
        assert(!carregado.tutorial_concluido && carregado.dificuldade == DIFICULDADE_CALMA);
    }

    arquivo = fopen(caminho, "wb");
    assert(arquivo != NULL);
    fputs("corrompido", arquivo);
    fclose(arquivo);
    assert(!SalvamentoCarregarCaminho(&carregado, caminho));
    assert(carregado.melhor_andar == 0);
    assert(carregado.dificuldade == DIFICULDADE_NORMAL);

    arquivo = fopen(caminho, "wb");
    assert(arquivo != NULL);
    fwrite(&(uint32_t){0x41535452u}, sizeof(uint32_t), 1, arquivo);
    fwrite(&(uint32_t){0u}, sizeof(uint32_t), 1, arquivo);
    fclose(arquivo);
    assert(!SalvamentoCarregarCaminho(&carregado, caminho));
    remove(caminho);
    puts("Save, corrupcao, versao e desbloqueios verificados.");
    return 0;
}
