#ifndef ITENS_H
#define ITENS_H

#include <stdbool.h>
#include <stdint.h>
#include "types.h"

typedef enum {
    ITEM_NENHUM = -1,
    ITEM_CHA, ITEM_LUA_ERVA, ITEM_VERMELHA_FITA, ITEM_TORRE_SINO,
    ITEM_MOLHADO_MANTO, ITEM_SOLAR_BROCHE, ITEM_ESTALINHO, ITEM_ESPINHO_BOMBA,
    ITEM_ARMADURA_CASCA, ITEM_ESCUDO_MARE,
    ITEM_ESPADA_NATUREZA, ITEM_ESPADA_LUZ, ITEM_ESPADA_FOGO, ITEM_CAJADO_GELO,
    ITEM_CALCA_FERRUGEM, ITEM_BOTAS_CINZA,
    ITEM_AMULETO_ESPELHO, ITEM_AMULETO_DENTE, ITEM_AMULETO_OLHO,
    ITEM_ARMADURA_ESPINHEIRO, ITEM_BOTAS_CARNICEIRO,
    ITEM_ESCUDO_INQUISIDOR, ITEM_CALCA_AFOGADO, ITEM_AMULETO_AMPULHETA,
    ITEM_BROCHE_INICIAL_CHAMA, ITEM_BROCHE_INICIAL_PLANTA, ITEM_BROCHE_INICIAL_AGUA,
    ITEM_ESPADA_INICIAL_CHAMA, ITEM_ESPADA_INICIAL_PLANTA, ITEM_ESPADA_INICIAL_AGUA,
    ITEM_TOTAL
} IdItem;

typedef enum {
    ITEM_EFEITO_CURAR, ITEM_EFEITO_DANO_BUFF, ITEM_EFEITO_DEFESA_TIPO,
    ITEM_EFEITO_SURPRESA_DANO, ITEM_EFEITO_CURAR_SANGRAMENTO, ITEM_EFEITO_EQUIPAMENTO
} EfeitoItem;

typedef enum { CATEGORIA_CONSUMIVEL, CATEGORIA_EQUIPAMENTO } CategoriaItem;
typedef enum { RARIDADE_COMUM, RARIDADE_RARA, RARIDADE_EPICA, RARIDADE_LENDARIA } RaridadeItem;

typedef enum {
    ESPACO_ARMADURA, ESPACO_ESCUDO, ESPACO_ARMAMENTO, ESPACO_CALCA, ESPACO_BOTAS,
    ESPACO_AMULETO_1, ESPACO_AMULETO_2, ESPACO_AMULETO_3,
    ESPACO_AMULETO_TIPO,
    ESPACO_EQUIPADO_TOTAL, ESPACO_NENHUM
} EspacoEquipamento;

typedef enum {
    PASSIVA_NENHUMA, PASSIVA_REFLETIR_DANO, PASSIVA_SANGRAMENTO_CRITICO,
    PASSIVA_MANA_EXTRA, PASSIVA_RESISTIR_SANGRAMENTO,
    PASSIVA_ROUBO_VIDA_SANGRANDO, PASSIVA_BLOQUEAR_CRITICO, PASSIVA_DASH_RAPIDO
} PassivaItem;

typedef enum {
    FAMILIA_DROP_GERAL = 1u << 0, FAMILIA_DROP_PLANTA = 1u << 1,
    FAMILIA_DROP_AGUA = 1u << 2, FAMILIA_DROP_FOGO = 1u << 3,
    FAMILIA_DROP_SOMBRA = 1u << 4, FAMILIA_DROP_LUZ = 1u << 5,
    FAMILIA_DROP_NEUTRO = 1u << 6, FAMILIA_DROP_CHEFE = 1u << 7
} FamiliaDrop;

typedef struct {
    int ataque, defesa, vida, velocidade;
    int mana_maxima, mana_por_turno;
    int chance_critica, dano_critico;
    int sangramento, resistencia_sangramento;
} BonusAtributos;

typedef struct {
    IdItem id;
    const char *nome;
    const char *descricao;
    EfeitoItem efeito;
    int valor, duracao;
    Tipo tipo_defesa;
    CategoriaItem categoria;
    EspacoEquipamento espaco;
    RaridadeItem raridade;
    BonusAtributos bonus;
    PassivaItem passiva;
    int valor_passiva;
    uint32_t familias_drop;
    Tipo tipo_elemental;
} DefinicaoItem;

const DefinicaoItem *ItemObterDefinicao(IdItem id);
bool ItemEhEquipamento(IdItem id);
bool ItemPodeEquiparNoEspaco(IdItem id, EspacoEquipamento espaco);
int ItemTotalAtributos(const DefinicaoItem *item);
bool ItemRaridadeValida(const DefinicaoItem *item);
const char *ItemNomeRaridade(RaridadeItem raridade);
const char *ItemNomeEspaco(EspacoEquipamento espaco);
const char *ItemNomePassiva(PassivaItem passiva);
IdItem ItemSortearDaFamilia(uint32_t familia, unsigned int *semente, bool incluir_equipamentos);

#endif
