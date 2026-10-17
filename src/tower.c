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
    return andar == 5 || andar == TOTAL_ANDARES_TORRE;
}

void TorreInicializar(Torre *torre, unsigned int semente)
{
    static const IdInimigo common_enemies[] = {
        INIMIGO_POEIRA_MARIPOSA, INIMIGO_BRILHO_SAPO, INIMIGO_BRASA_PASSARO, INIMIGO_MENTA_LIMO,
        INIMIGO_LANTERNA_FAGULHA, INIMIGO_ESPINHO_LEBRE, INIMIGO_SOMBRA_GATO, INIMIGO_PETALA_CAVALEIRO
    };
    unsigned int aleatorio = semente == 0 ? 1u : semente;

    memset(torre, 0, sizeof(*torre));
    torre->semente = aleatorio;
    torre->andar = 1;
    for (int indice = 0; indice < TOTAL_ANDARES_TORRE; indice++) {
        IdInimigo inimigo;

        if (indice + 1 == 5) inimigo = INIMIGO_CHEFE_SINEIRO;
        else if (indice + 1 == TOTAL_ANDARES_TORRE) inimigo = INIMIGO_CHEFE_ULTIMA_TORRE;
        else {
            const int disponivel = indice < 3 ? 4 : (indice < 7 ? 6 : 8);
            inimigo = common_enemies[ProximoAleatorio(&aleatorio) % (unsigned int)disponivel];
            if (indice > 0 && inimigo == torre->inimigos[indice - 1]) {
                inimigo = common_enemies[(ProximoAleatorio(&aleatorio) + 1u) % (unsigned int)disponivel];
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
    int id_ataque = (int)(ProximoAleatorio(&aleatorio) % (unsigned int)JogadorTotalAtaquesRecompensa());
    const int total_ataques = JogadorTotalAtaquesRecompensa();

    /* Busca circularmente para nunca oferecer um duplicado na ultima posicao da tabela. */
    for (int attempt = 0; attempt < total_ataques; attempt++) {
        const int candidate = (id_ataque + attempt) % total_ataques;
        if (!JogadorPossuiAtaque(jogador, JogadorAtaqueRecompensa(candidate))) {
            id_ataque = candidate;
            break;
        }
    }
    torre->recompensas[0] = (RecompensaTorre){RECOMPENSA_ATAQUE, id_ataque, 0};
    torre->recompensas[1] = (RecompensaTorre){RECOMPENSA_ITEM, (int)(ProximoAleatorio(&aleatorio) % ITEM_TOTAL), 1};
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

bool TorreDeveDescansar(int andar_concluido) { return andar_concluido == 3 || andar_concluido == 9; }
bool TorreDeveTerLoja(int andar_concluido) { return andar_concluido == 6; }
