#ifndef ITENS_H
#define ITENS_H

#include "types.h"

typedef enum {
    ITEM_CHA,
    ITEM_LUA_ERVA,
    ITEM_VERMELHA_FITA,
    ITEM_TORRE_SINO,
    ITEM_MOLHADO_MANTO,
    ITEM_SOLAR_BROCHE,
    ITEM_ESTALINHO,
    ITEM_ESPINHO_BOMBA,
    ITEM_TOTAL
} IdItem;

typedef enum {
    ITEM_EFEITO_CURAR,
    ITEM_EFEITO_DANO_BUFF,
    ITEM_EFEITO_DEFESA_TIPO,
    ITEM_EFEITO_SURPRESA_DANO
} EfeitoItem;

typedef struct {
    IdItem id;
    const char *nome;
    const char *descricao;
    EfeitoItem efeito;
    int valor;
    int duracao;
    Tipo tipo_defesa;
} DefinicaoItem;

const DefinicaoItem *ItemObterDefinicao(IdItem id);

#endif
