#include "tower.h"
#include "balance.h"

#include <string.h>

static unsigned int ProximoAleatorio(unsigned int *semente)
{
    *semente = *semente * 1664525u + 1013904223u;
    return *semente;
}

bool TorreAndarDeChefe(int andar)
{
    return andar > 0 && andar % 5 == 0;
}

Tipo TorreTipoDoAndar(int andar)
{
    static const Tipo tipos[] = {TIPO_PLANTA, TIPO_AGUA, TIPO_FOGO,
                                 TIPO_SOMBRA, TIPO_LUZ, TIPO_NEUTRO};
    const int regiao = (andar - 1) / 5;
    return andar < 1 || andar > TOTAL_ANDARES_TORRE ? TIPO_NEUTRO : tipos[regiao];
}

void TorreInicializar(Torre *torre, unsigned int semente)
{
    static const IdInimigo regioes[6][4] = {
        {INIMIGO_SOLDADO_SARCAS,INIMIGO_HERBALISTA_PESTE,INIMIGO_MENTA_LIMO,INIMIGO_ESPINHO_LEBRE},
        {INIMIGO_LEGIONARIO_AFOGADO,INIMIGO_BRUXA_LODO,INIMIGO_REVENANTE_GELO,INIMIGO_BRILHO_SAPO},
        {INIMIGO_ESQUELETO_CARBONIZADO,INIMIGO_ACENDEDOR_FORJA,INIMIGO_BRASA_PASSARO,INIMIGO_BRASA_PASSARO},
        {INIMIGO_PENITENTE_SEM_ROSTO,INIMIGO_LADRAO_MORTALHA,INIMIGO_POEIRA_MARIPOSA,INIMIGO_SOMBRA_GATO},
        {INIMIGO_ACOLITO_VITRAL,INIMIGO_PETALA_CAVALEIRO,INIMIGO_LANTERNA_FAGULHA,INIMIGO_LANTERNA_FAGULHA},
        {INIMIGO_GOBLIN_LANCEIRO,INIMIGO_AUTOMATO_PEDRA,INIMIGO_MERCENARIO_FERRUGEM,INIMIGO_CARCEREIRO_CORRENTES}
    };
    static const IdInimigo chefes[6] = {INIMIGO_CHEFE_CULTISTA_RAIZES,INIMIGO_CHEFE_AFOGADO_CORAL,
        INIMIGO_CHEFE_MAGO_CINZAS,INIMIGO_CHEFE_SINEIRO,INIMIGO_CHEFE_INQUISIDOR_SOL,
        INIMIGO_CHEFE_ULTIMA_TORRE};
    static const unsigned int total_por_regiao[6] = {4,4,3,4,3,4};
    unsigned int aleatorio = semente == 0 ? 1u : semente;

    memset(torre, 0, sizeof(*torre));
    torre->semente = aleatorio;
    torre->andar = 1;
    for (int indice = 0; indice < TOTAL_ANDARES_TORRE; indice++) {
        IdInimigo inimigo;

        const int regiao = indice / 5;
        if ((indice + 1) % 5 == 0) inimigo = chefes[regiao];
        else {
            unsigned int escolha = ProximoAleatorio(&aleatorio) % total_por_regiao[regiao];
            inimigo = regioes[regiao][escolha];
            if (indice > 0 && inimigo == torre->inimigos[indice - 1]) {
                escolha = (escolha + 1u) % total_por_regiao[regiao];
                inimigo = regioes[regiao][escolha];
            }
        }
        torre->inimigos[indice] = inimigo;
    }
    torre->semente = aleatorio;
    TorreSortearLoja(torre);
}

IdInimigo TorreInimigoDoAndar(const Torre *torre, int andar)
{
    if (andar < 1 || andar > TOTAL_ANDARES_TORRE) return INIMIGO_POEIRA_MARIPOSA;
    return torre->inimigos[andar - 1];
}

void TorreEscalarInimigo(Inimigo *inimigo, int andar)
{
    const int multiplicador = andar - 1;

    inimigo->vida_maxima += multiplicador * (inimigo->chefe ? BALANCEAMENTO_CHEFE_VIDA_PER_ANDAR : BALANCEAMENTO_COMUM_VIDA_PER_ANDAR);
    inimigo->hp = inimigo->vida_maxima;
    inimigo->bonus_dano_projetil = multiplicador / BALANCEAMENTO_PROJETIL_BONUS_A_CADA;
    inimigo->multiplicador_dano_projetil = inimigo->chefe ? 1.0f : BalanceamentoMultiplicadorDanoAndar(andar);
    inimigo->bonus_recompensa = multiplicador;
}

void TorreSortearRecompensas(Torre *torre, const AtributosJogador *jogador)
{
    unsigned int aleatorio = torre->semente;
    static const uint32_t familias[] = {FAMILIA_DROP_PLANTA,FAMILIA_DROP_AGUA,FAMILIA_DROP_FOGO,
        FAMILIA_DROP_SOMBRA,FAMILIA_DROP_LUZ,FAMILIA_DROP_NEUTRO};
    const uint32_t familia = familias[(torre->andar - 1) / 5];
    const IdItem equipamento = ItemSortearDaFamilia(familia, &aleatorio, true);
    const IdItem consumivel = ItemSortearDaFamilia(familia, &aleatorio, false);
    (void)jogador;

    torre->recompensas[0] = (RecompensaTorre){RECOMPENSA_EQUIPAMENTO, equipamento, 1};
    torre->recompensas[1] = (RecompensaTorre){RECOMPENSA_ITEM, consumivel, 1};
    torre->recompensas[2] = (RecompensaTorre){ProximoAleatorio(&aleatorio) % 2u ? RECOMPENSA_VIDA : RECOMPENSA_DANO, 0, 4};
    torre->semente = aleatorio;
}

void TorreSortearLoja(Torre *torre)
{
    unsigned int aleatorio = torre->semente;

    for (int indice = 0; indice < 3; indice++) {
        torre->itens_loja[indice] = (IdItem)(ProximoAleatorio(&aleatorio) % ITEM_TOTAL);
        torre->precos_loja[indice] = BALANCEAMENTO_LOJA_BASE_PRECO +
                                    (int)(ProximoAleatorio(&aleatorio) % BALANCEAMENTO_LOJA_PRECO_VARIACAO);
    }
    torre->semente = aleatorio;
}

bool TorreDeveDescansar(int andar_concluido) { return andar_concluido > 0 && andar_concluido % 5 == 4; }
bool TorreDeveTerLoja(int andar_concluido) { return andar_concluido > 5 && andar_concluido % 5 == 2; }
