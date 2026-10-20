#include "items.h"

#define B(A,D,V,VEL,MM,MT,C,DC,S,RS) {A,D,V,VEL,MM,MT,C,DC,S,RS}
#define CONSUMIVEL(ID,NOME,DESC,EFEITO,VALOR,DURACAO,TIPO) \
    [ID] = {ID,NOME,DESC,EFEITO,VALOR,DURACAO,TIPO,CATEGORIA_CONSUMIVEL,ESPACO_NENHUM, \
            RARIDADE_COMUM,B(0,0,0,0,0,0,0,0,0,0),PASSIVA_NENHUMA,0,FAMILIA_DROP_GERAL,TIPO_NEUTRO}
#define EQUIPAMENTO(ID,NOME,DESC,ESPACO,RARIDADE,BONUS,PASSIVA,VALOR_PASSIVA,FAMILIA) \
    [ID] = {ID,NOME,DESC,ITEM_EFEITO_EQUIPAMENTO,0,0,TIPO_NEUTRO,CATEGORIA_EQUIPAMENTO, \
            ESPACO,RARIDADE,BONUS,PASSIVA,VALOR_PASSIVA,FAMILIA,TIPO_NEUTRO}
#define AMULETO_TIPO(ID,NOME,DESC,RARIDADE,BONUS,TIPO,FAMILIA) \
    [ID] = {ID,NOME,DESC,ITEM_EFEITO_EQUIPAMENTO,0,0,TIPO_NEUTRO,CATEGORIA_EQUIPAMENTO, \
            ESPACO_AMULETO_TIPO,RARIDADE,BONUS,PASSIVA_NENHUMA,0,FAMILIA,TIPO}

static const DefinicaoItem DEFINICOES_ITENS[ITEM_TOTAL] = {
    CONSUMIVEL(ITEM_CHA, "Cha Morno", "Recupera 12 HP.", ITEM_EFEITO_CURAR, 12, 0, TIPO_NEUTRO),
    CONSUMIVEL(ITEM_LUA_ERVA, "Erva Lunar", "Recupera 20 HP e remove sangramento.", ITEM_EFEITO_CURAR_SANGRAMENTO, 20, 0, TIPO_NEUTRO),
    CONSUMIVEL(ITEM_VERMELHA_FITA, "Fita Rubra", "+50% de dano no proximo ataque.", ITEM_EFEITO_DANO_BUFF, 50, 1, TIPO_NEUTRO),
    CONSUMIVEL(ITEM_TORRE_SINO, "Sino da Torre", "+35% de dano por dois ataques.", ITEM_EFEITO_DANO_BUFF, 35, 2, TIPO_NEUTRO),
    CONSUMIVEL(ITEM_MOLHADO_MANTO, "Manto Umido", "Muda a defesa para Agua.", ITEM_EFEITO_DEFESA_TIPO, 0, 0, TIPO_AGUA),
    AMULETO_TIPO(ITEM_SOLAR_BROCHE, "Broche Solar", "Libera tres tecnicas de Luz.",
        RARIDADE_COMUM, B(0,0,0,0,0,0,0,0,0,0), TIPO_LUZ, FAMILIA_DROP_LUZ),
    CONSUMIVEL(ITEM_ESTALINHO, "Estalinho", "Causa 10 de dano sem QTE.", ITEM_EFEITO_SURPRESA_DANO, 10, 0, TIPO_NEUTRO),
    CONSUMIVEL(ITEM_ESPINHO_BOMBA, "Bomba de Espinhos", "Causa 16 de dano sem QTE.", ITEM_EFEITO_SURPRESA_DANO, 16, 0, TIPO_NEUTRO),
    EQUIPAMENTO(ITEM_ARMADURA_CASCA, "Armadura de Casca", "+4 DEF, +5 VIDA.", ESPACO_ARMADURA,
        RARIDADE_RARA, B(0,4,5,0,0,0,0,0,0,0), PASSIVA_NENHUMA, 0, FAMILIA_DROP_PLANTA),
    EQUIPAMENTO(ITEM_ESCUDO_MARE, "Escudo da Mare", "+4 DEF.", ESPACO_ESCUDO,
        RARIDADE_COMUM, B(0,4,0,0,0,0,0,0,0,0), PASSIVA_NENHUMA, 0, FAMILIA_DROP_AGUA),
    EQUIPAMENTO(ITEM_ESPADA_NATUREZA, "Espada da Natureza", "+4 ATQ. Libera tecnicas de Planta.", ESPACO_ARMAMENTO,
        RARIDADE_COMUM, B(4,0,0,0,0,0,0,0,0,0), PASSIVA_NENHUMA, 0, FAMILIA_DROP_PLANTA),
    EQUIPAMENTO(ITEM_ESPADA_LUZ, "Espada da Luz", "+4 ATQ, +25% dano critico. Tecnicas de Luz.", ESPACO_ARMAMENTO,
        RARIDADE_RARA, B(4,0,0,0,0,0,0,25,0,0), PASSIVA_NENHUMA, 0, FAMILIA_DROP_LUZ),
    EQUIPAMENTO(ITEM_ESPADA_FOGO, "Espada de Fogo", "+4 ATQ, +5 VIDA, +5 SANG.", ESPACO_ARMAMENTO,
        RARIDADE_EPICA, B(4,0,5,0,0,0,0,0,5,0), PASSIVA_NENHUMA, 0, FAMILIA_DROP_FOGO),
    EQUIPAMENTO(ITEM_CAJADO_GELO, "Cajado de Gelo", "+3 ATQ, +8 MANA, +2 MANA/turno.", ESPACO_ARMAMENTO,
        RARIDADE_EPICA, B(3,0,0,0,8,2,0,0,0,0), PASSIVA_NENHUMA, 0, FAMILIA_DROP_AGUA),
    EQUIPAMENTO(ITEM_CALCA_FERRUGEM, "Calca de Ferrugem", "+3 DEF, +6 VIDA.", ESPACO_CALCA,
        RARIDADE_RARA, B(0,3,6,0,0,0,0,0,0,0), PASSIVA_NENHUMA, 0, FAMILIA_DROP_NEUTRO),
    EQUIPAMENTO(ITEM_BOTAS_CINZA, "Botas de Cinza", "+8 VEL, +8% chance critica.", ESPACO_BOTAS,
        RARIDADE_RARA, B(0,0,0,8,0,0,8,0,0,0), PASSIVA_NENHUMA, 0, FAMILIA_DROP_FOGO),
    EQUIPAMENTO(ITEM_AMULETO_ESPELHO, "Espelho Rachado", "Devolve 8% do dano recebido.", ESPACO_AMULETO_1,
        RARIDADE_COMUM, B(0,0,0,0,0,0,0,0,0,0), PASSIVA_REFLETIR_DANO, 8, FAMILIA_DROP_SOMBRA),
    EQUIPAMENTO(ITEM_AMULETO_DENTE, "Dente Carmesim", "Criticos aplicam +3 de sangramento.", ESPACO_AMULETO_1,
        RARIDADE_COMUM, B(0,0,0,0,0,0,0,0,0,0), PASSIVA_SANGRAMENTO_CRITICO, 3, FAMILIA_DROP_FOGO),
    EQUIPAMENTO(ITEM_AMULETO_OLHO, "Olho do Conjurador", "+5 MANA; passiva gera +2 por turno.", ESPACO_AMULETO_1,
        RARIDADE_RARA, B(0,0,0,0,5,0,0,0,0,0), PASSIVA_MANA_EXTRA, 2, FAMILIA_DROP_LUZ),
    EQUIPAMENTO(ITEM_ARMADURA_ESPINHEIRO, "Armadura do Espinheiro", "+12 DEF, +10 VIDA; resiste a sangramento.", ESPACO_ARMADURA,
        RARIDADE_EPICA, B(0,12,10,0,0,0,0,0,0,0), PASSIVA_RESISTIR_SANGRAMENTO, 35, FAMILIA_DROP_PLANTA | FAMILIA_DROP_CHEFE),
    EQUIPAMENTO(ITEM_BOTAS_CARNICEIRO, "Botas do Carniceiro", "+16 VEL, +8 ATQ, +5 SANG; rouba vida.", ESPACO_BOTAS,
        RARIDADE_LENDARIA, B(8,0,0,16,0,0,0,0,5,0), PASSIVA_ROUBO_VIDA_SANGRANDO, 15, FAMILIA_DROP_SOMBRA | FAMILIA_DROP_CHEFE),
    EQUIPAMENTO(ITEM_ESCUDO_INQUISIDOR, "Escudo do Inquisidor", "+10 DEF, +6 MANA; reduz bonus de dano critico.", ESPACO_ESCUDO,
        RARIDADE_EPICA, B(0,10,0,0,6,0,0,0,0,0), PASSIVA_BLOQUEAR_CRITICO, 25, FAMILIA_DROP_LUZ | FAMILIA_DROP_CHEFE),
    EQUIPAMENTO(ITEM_CALCA_AFOGADO, "Calca do Afogado", "+9 DEF, +10 VIDA, +5 RES. SANG.", ESPACO_CALCA,
        RARIDADE_EPICA, B(0,9,10,0,0,0,0,0,0,5), PASSIVA_NENHUMA, 0, FAMILIA_DROP_AGUA),
    EQUIPAMENTO(ITEM_AMULETO_AMPULHETA, "Ampulheta Cinzenta", "Reduz em 25% a recarga do dash.", ESPACO_AMULETO_1,
        RARIDADE_COMUM, B(0,0,0,0,0,0,0,0,0,0), PASSIVA_DASH_RAPIDO, 25, FAMILIA_DROP_NEUTRO)
};

const DefinicaoItem *ItemObterDefinicao(IdItem id) {
    if (id < ITEM_CHA || id >= ITEM_TOTAL) return &DEFINICOES_ITENS[ITEM_CHA];
    return &DEFINICOES_ITENS[id];
}

bool ItemEhEquipamento(IdItem id) {
    return id >= ITEM_CHA && id < ITEM_TOTAL && DEFINICOES_ITENS[id].categoria == CATEGORIA_EQUIPAMENTO;
}

bool ItemPodeEquiparNoEspaco(IdItem id, EspacoEquipamento espaco) {
    if (!ItemEhEquipamento(id) || espaco < 0 || espaco >= ESPACO_EQUIPADO_TOTAL) return false;
    EspacoEquipamento base = DEFINICOES_ITENS[id].espaco;
    if (base == ESPACO_AMULETO_1)
        return espaco == ESPACO_AMULETO_1 || espaco == ESPACO_AMULETO_2 || espaco == ESPACO_AMULETO_3;
    return base == espaco;
}

int ItemTotalAtributos(const DefinicaoItem *item) {
    if (item == NULL || item->categoria != CATEGORIA_EQUIPAMENTO) return 0;
    const BonusAtributos *b = &item->bonus;
    int total = b->ataque != 0;
    total += b->defesa != 0; total += b->vida != 0; total += b->velocidade != 0;
    total += b->mana_maxima != 0; total += b->mana_por_turno != 0;
    total += b->chance_critica != 0; total += b->dano_critico != 0;
    total += b->sangramento != 0; total += b->resistencia_sangramento != 0;
    total += item->passiva != PASSIVA_NENHUMA;
    total += item->tipo_elemental != TIPO_NEUTRO;
    return total;
}

bool ItemRaridadeValida(const DefinicaoItem *item) {
    if (item == NULL || item->categoria != CATEGORIA_EQUIPAMENTO) return true;
    if (ItemTotalAtributos(item) != (int)item->raridade + 1) return false;
    return item->passiva == PASSIVA_NENHUMA || item->espaco == ESPACO_AMULETO_1 || item->raridade >= RARIDADE_EPICA;
}

const char *ItemNomeRaridade(RaridadeItem raridade) {
    static const char *const nomes[] = {"Comum", "Rara", "Epica", "Lendaria"};
    return raridade >= RARIDADE_COMUM && raridade <= RARIDADE_LENDARIA ? nomes[raridade] : "?";
}

const char *ItemNomeEspaco(EspacoEquipamento espaco) {
    static const char *const nomes[] = {"Armadura", "Escudo", "Armamento", "Calca", "Botas",
                                        "Amuleto", "Amuleto", "Amuleto", "Amuleto elemental"};
    return espaco >= 0 && espaco < ESPACO_EQUIPADO_TOTAL ? nomes[espaco] : "Consumivel";
}

const char *ItemNomePassiva(PassivaItem passiva) {
    static const char *const nomes[] = {"", "Refletir dano", "Sangramento critico", "Mana extra", "Resistir sangramento",
                                        "Roubo de vida", "Bloquear critico", "Dash rapido"};
    return passiva >= PASSIVA_NENHUMA && passiva <= PASSIVA_DASH_RAPIDO ? nomes[passiva] : "";
}

static unsigned int ProximoAleatorioItem(unsigned int *semente) {
    *semente = *semente * 1664525u + 1013904223u;
    return *semente;
}

IdItem ItemSortearDaFamilia(uint32_t familia, unsigned int *semente, bool incluir_equipamentos) {
    IdItem candidatos[ITEM_TOTAL]; int total = 0;
    for (int id = 0; id < ITEM_TOTAL; id++) {
        const DefinicaoItem *item = &DEFINICOES_ITENS[id];
        if (incluir_equipamentos && item->categoria != CATEGORIA_EQUIPAMENTO) continue;
        if (!incluir_equipamentos && item->categoria != CATEGORIA_CONSUMIVEL) continue;
        if ((item->familias_drop & familia) != 0 || (item->familias_drop & FAMILIA_DROP_GERAL) != 0)
            candidatos[total++] = (IdItem)id;
    }
    if (total == 0) return ITEM_CHA;
    return candidatos[ProximoAleatorioItem(semente) % (unsigned int)total];
}

#undef B
#undef CONSUMIVEL
#undef EQUIPAMENTO
#undef AMULETO_TIPO
