#include "items.h"

static const DefinicaoItem DEFINICOES_ITENS[ITEM_TOTAL] = {
    [ITEM_CHA] = {ITEM_CHA, "Cha Morno", "Recupera 12 HP.", ITEM_EFEITO_CURAR, 12, 0, TIPO_NEUTRO},
    [ITEM_LUA_ERVA] = {ITEM_LUA_ERVA, "Erva Lunar", "Recupera 20 HP.", ITEM_EFEITO_CURAR, 20, 0, TIPO_NEUTRO},
    [ITEM_VERMELHA_FITA] = {ITEM_VERMELHA_FITA, "Fita Rubra", "+50% de dano no proximo ataque.", ITEM_EFEITO_DANO_BUFF, 50, 1, TIPO_NEUTRO},
    [ITEM_TORRE_SINO] = {ITEM_TORRE_SINO, "Sino da Torre", "+35% de dano por dois ataques.", ITEM_EFEITO_DANO_BUFF, 35, 2, TIPO_NEUTRO},
    [ITEM_MOLHADO_MANTO] = {ITEM_MOLHADO_MANTO, "Manto Umido", "Muda a defesa para Agua.", ITEM_EFEITO_DEFESA_TIPO, 0, 0, TIPO_AGUA},
    [ITEM_SOLAR_BROCHE] = {ITEM_SOLAR_BROCHE, "Broche Solar", "Muda a defesa para Luz.", ITEM_EFEITO_DEFESA_TIPO, 0, 0, TIPO_LUZ},
    [ITEM_ESTALINHO] = {ITEM_ESTALINHO, "Estalinho", "Causa 10 de dano sem QTE.", ITEM_EFEITO_SURPRESA_DANO, 10, 0, TIPO_NEUTRO},
    [ITEM_ESPINHO_BOMBA] = {ITEM_ESPINHO_BOMBA, "Bomba de Espinhos", "Causa 16 de dano sem QTE.", ITEM_EFEITO_SURPRESA_DANO, 16, 0, TIPO_NEUTRO}
};

const DefinicaoItem *ItemObterDefinicao(IdItem id)
{
    if (id < ITEM_CHA || id >= ITEM_TOTAL) return &DEFINICOES_ITENS[ITEM_CHA];
    return &DEFINICOES_ITENS[id];
}
