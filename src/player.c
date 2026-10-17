#include "player.h"

#include <stdio.h>
#include <string.h>

#define ATAQUE(ID, NAME, TIPO, QTE, DMG, EFEITO, HITS, STATUS, DURATION, COOLDOWN, CRIT) \
    {ID, NAME, TIPO, QTE, DMG, EFEITO, HITS, STATUS, DURATION, COOLDOWN, CRIT}

static const Ataque RECOMPENSA_ATAQUES[JOGADOR_ATAQUE_TOTAL] = {
    ATAQUE(ATAQUE_BRASA, "Brasa", TIPO_FOGO, QTE_IMPACTO, 8, EFEITO_IMPACTO, 1, STATUS_QUEIMAR, 3, 0, true),
    ATAQUE(ATAQUE_GOTA_CLARA, "Gota Clara", TIPO_AGUA, QTE_RITMO, 7, EFEITO_PROJETIL, 1, STATUS_MOLHADO, 2, 0, true),
    ATAQUE(ATAQUE_RAIZ_RAPIDA, "Raiz Rapida", TIPO_PLANTA, QTE_CARGA, 9, EFEITO_CORTE, 1, STATUS_ENRAIZAR, 1, 1, false),
    ATAQUE(ATAQUE_RAIO_MANSO, "Raio Manso", TIPO_LUZ, QTE_IMPACTO, 10, EFEITO_FEIXE, 1, STATUS_CEGAR, 1, 1, true),
    ATAQUE(ATAQUE_ECO_SOMBRIO, "Eco Sombrio", TIPO_SOMBRA, QTE_RITMO, 10, EFEITO_IMPACTO, 2, STATUS_NENHUM, 0, 0, false),
    ATAQUE(ATAQUE_GOLPE_NEUTRO, "Golpe Neutro", TIPO_NEUTRO, QTE_CARGA, 11, EFEITO_CORTE, 1, STATUS_ATORDOAR, 1, 2, true),
    ATAQUE(ATAQUE_CHAMA_ALTA, "Chama Alta", TIPO_FOGO, QTE_CARGA, 13, EFEITO_CHUVA, 2, STATUS_QUEIMAR, 3, 2, true),
    ATAQUE(ATAQUE_MARE_CURTA, "Mare Curta", TIPO_AGUA, QTE_IMPACTO, 12, EFEITO_PROJETIL, 1, STATUS_MOLHADO, 2, 1, false),
    ATAQUE(ATAQUE_VINHA_FINA, "Vinha Fina", TIPO_PLANTA, QTE_RITMO, 12, EFEITO_CORTE, 2, STATUS_ENRAIZAR, 1, 1, true),
    ATAQUE(ATAQUE_LUZ_GUIA, "Luz Guia", TIPO_LUZ, QTE_CARGA, 13, EFEITO_FEIXE, 1, STATUS_CEGAR, 1, 2, false),
    ATAQUE(ATAQUE_SOMBRA_FRIA, "Sombra Fria", TIPO_SOMBRA, QTE_IMPACTO, 14, EFEITO_IMPACTO, 1, STATUS_ATORDOAR, 1, 2, true),
    ATAQUE(ATAQUE_PULSO_CALMO, "Pulso Calmo", TIPO_NEUTRO, QTE_RITMO, 13, EFEITO_CHUVA, 3, STATUS_NENHUM, 0, 1, false),
    ATAQUE(ATAQUE_FAGULHA, "Fagulha", TIPO_FOGO, QTE_IMPACTO, 6, EFEITO_PROJETIL, 1, STATUS_QUEIMAR, 2, 0, false),
    ATAQUE(ATAQUE_ORVALHO, "Orvalho", TIPO_AGUA, QTE_CARGA, 15, EFEITO_FEIXE, 1, STATUS_MOLHADO, 3, 2, true),
    ATAQUE(ATAQUE_ESPINHO_LUNAR, "Espinho Lunar", TIPO_PLANTA, QTE_RITMO, 15, EFEITO_CHUVA, 3, STATUS_ENRAIZAR, 1, 2, true),
    ATAQUE(ATAQUE_SINO_VAZIO, "Sino Vazio", TIPO_LUZ, QTE_IMPACTO, 16, EFEITO_IMPACTO, 1, STATUS_ATORDOAR, 1, 3, true)
};

void JogadorInicializar(AtributosJogador *jogador)
{
    memset(jogador, 0, sizeof(*jogador));
    snprintf(jogador->nome, sizeof(jogador->nome), "Ari");
    jogador->vida_maxima = 40;
    jogador->hp = jogador->vida_maxima;
    jogador->nivel = 1;
    jogador->multiplicador_dano_recebido = 1.0f;
    jogador->tipo_defesa = TIPO_NEUTRO;

    jogador->ataques[0] = RECOMPENSA_ATAQUES[ATAQUE_BRASA];
    jogador->ataques[1] = RECOMPENSA_ATAQUES[ATAQUE_GOTA_CLARA];
    jogador->ataques[2] = RECOMPENSA_ATAQUES[ATAQUE_RAIZ_RAPIDA];
    jogador->total_ataques = 3;

    jogador->inventario[0] = (ItemInventario){ITEM_CHA, 2};
    jogador->inventario[1] = (ItemInventario){ITEM_VERMELHA_FITA, 1};
    jogador->inventario[2] = (ItemInventario){ITEM_MOLHADO_MANTO, 1};
    jogador->inventario[3] = (ItemInventario){ITEM_SOLAR_BROCHE, 1};
}

bool JogadorAdicionarItem(AtributosJogador *jogador, IdItem id, int quantidade)
{
    for (int indice = 0; indice < MAXIMO_INVENTARIO_ITENS; indice++) {
        if (jogador->inventario[indice].quantidade > 0 && jogador->inventario[indice].id == id) {
            jogador->inventario[indice].quantidade += quantidade;
            return true;
        }
    }
    for (int indice = 0; indice < MAXIMO_INVENTARIO_ITENS; indice++) {
        if (jogador->inventario[indice].quantidade == 0) {
            jogador->inventario[indice] = (ItemInventario){id, quantidade};
            return true;
        }
    }
    return false;
}

bool JogadorPossuiAtaque(const AtributosJogador *jogador, Ataque ataque)
{
    for (int indice = 0; indice < jogador->total_ataques; indice++) {
        if (strcmp(jogador->ataques[indice].nome, ataque.nome) == 0) return true;
    }
    return false;
}

int JogadorTotalAtaquesRecompensa(void)
{
    return (int)(sizeof(RECOMPENSA_ATAQUES) / sizeof(RECOMPENSA_ATAQUES[0]));
}

Ataque JogadorAtaqueRecompensa(int indice)
{
    if (indice < 0 || indice >= JogadorTotalAtaquesRecompensa()) return RECOMPENSA_ATAQUES[0];
    return RECOMPENSA_ATAQUES[indice];
}

bool JogadorAdicionarAtaque(AtributosJogador *jogador, Ataque ataque, int indice_substituicao)
{
    if (JogadorPossuiAtaque(jogador, ataque)) return false;
    if (jogador->total_ataques < MAXIMO_EQUIPADOS_ATAQUES) {
        jogador->ataques[jogador->total_ataques++] = ataque;
        return true;
    }
    if (indice_substituicao < 0 || indice_substituicao >= MAXIMO_EQUIPADOS_ATAQUES) return false;
    jogador->ataques[indice_substituicao] = ataque;
    return true;
}

bool JogadorGanharXp(AtributosJogador *jogador, int xp)
{
    const int nivel_anterior = jogador->nivel;

    jogador->xp += xp;
    while (jogador->xp >= jogador->nivel * 20) {
        jogador->xp -= jogador->nivel * 20;
        jogador->nivel++;
        jogador->vida_maxima += 5;
        jogador->hp += 5;
        jogador->bonus_dano_permanente++;
    }
    return jogador->nivel > nivel_anterior;
}

bool JogadorAtaqueDisponivel(const AtributosJogador *jogador, const Ataque *ataque, char *motivo, int tamanho_motivo)
{
    const int id = (int)ataque->id;

    if (id < 0 || id >= JOGADOR_ATAQUE_TOTAL) return false;
    if (jogador->recargas_ataques[id] > 0) {
        snprintf(motivo, (size_t)tamanho_motivo, "Espere %d", jogador->recargas_ataques[id]);
        return false;
    }
    return true;
}

void JogadorUsarAtaque(AtributosJogador *jogador, const Ataque *ataque)
{
    const int id = (int)ataque->id;
    if (id < 0 || id >= JOGADOR_ATAQUE_TOTAL) return;
    jogador->recargas_ataques[id] = ataque->turnos_recarga;
}

void JogadorAvancarRecargasAtaques(AtributosJogador *jogador)
{
    for (int indice = 0; indice < JOGADOR_ATAQUE_TOTAL; indice++) {
        if (jogador->recargas_ataques[indice] > 0) jogador->recargas_ataques[indice]--;
    }
}

const char *JogadorNomeStatus(EfeitoStatus status)
{
    static const char *const names[] = {"", "QUEIMAR", "MOLHAR", "ENRAIZAR", "CEGAR", "ATORDOAR"};
    return status >= STATUS_NENHUM && status <= STATUS_ATORDOAR ? names[status] : "";
}

bool JogadorUsarItem(AtributosJogador *jogador, int indice_inventario, const DefinicaoItem **item_usado,
                   int *valor_aplicado)
{
    ItemInventario *espaco;
    const DefinicaoItem *item;

    if (indice_inventario < 0 || indice_inventario >= MAXIMO_INVENTARIO_ITENS) return false;
    espaco = &jogador->inventario[indice_inventario];
    if (espaco->quantidade <= 0) return false;

    item = ItemObterDefinicao(espaco->id);
    espaco->quantidade--;
    *item_usado = item;
    *valor_aplicado = item->valor;
    if (item->efeito == ITEM_EFEITO_CURAR) {
        *valor_aplicado = JogadorCurar(jogador, item->valor);
    } else if (item->efeito == ITEM_EFEITO_DANO_BUFF) {
        jogador->percentual_buff_dano = item->valor;
        jogador->turnos_buff_dano = item->duracao;
    } else if (item->efeito == ITEM_EFEITO_DEFESA_TIPO) {
        jogador->tipo_defesa = item->tipo_defesa;
    }
    return true;
}

float JogadorMultiplicadorBuffAtaque(const AtributosJogador *jogador)
{
    return 1.0f + (float)jogador->percentual_buff_dano / 100.0f;
}

void JogadorConsumirBuffAtaque(AtributosJogador *jogador)
{
    if (jogador->turnos_buff_dano <= 0) return;
    jogador->turnos_buff_dano--;
    if (jogador->turnos_buff_dano == 0) jogador->percentual_buff_dano = 0;
}

int JogadorCurar(AtributosJogador *jogador, int quantidade)
{
    const int vida_anterior = jogador->hp;

    jogador->hp += quantidade;
    if (jogador->hp > jogador->vida_maxima) jogador->hp = jogador->vida_maxima;
    return jogador->hp - vida_anterior;
}
