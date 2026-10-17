#ifndef QTE_H
#define QTE_H

#include "raylib.h"

typedef enum {
    QTE_IMPACTO,
    QTE_RITMO,
    QTE_CARGA
} TipoQte;

typedef enum {
    QTE_PERFEITO,
    QTE_BOM,
    QTE_RUIM,
    QTE_ERRO
} ResultadoQte;

typedef struct {
    bool confirmar_pressionado;
    bool confirmar_mantido;
    bool confirmar_solto;
} EntradaQte;

typedef struct {
    bool ativo;
    bool resultado_pronto;
    TipoQte categoria;
    ResultadoQte resultado;
    int dificuldade;
    float decorrido;
    float temporizador_guarda;
    float temporizador_resultado;
    float duracao;
    float tempo_alvo;
    int total_batidas;
    int indice_batida;
    int pontos_ritmo;
    bool segurar_iniciado;
    bool segurar_pronto;
    float carga_segurada;
    float carga_solta;
    float temporizador_retorno;
    bool retorno_sucesso;
} Qte;

void qte_iniciar(Qte *qte, TipoQte categoria, int dificuldade);
void qte_atualizar(Qte *qte, float tempo_delta);
void qte_atualizar_com_entrada(Qte *qte, float tempo_delta, EntradaQte entrada);
void qte_desenhar(const Qte *qte);
bool qte_terminou(const Qte *qte);
float qte_multiplicador(ResultadoQte resultado);
const char *qte_nome_resultado(ResultadoQte resultado);
ResultadoQte qte_resultado_por_distancia_tempo(float distancia, int dificuldade);
ResultadoQte qte_resultado_por_ritmo(int pontos, int total_batidas);
bool qte_esta_em_guarda(const Qte *qte);
void qte_retomar_com_guarda(Qte *qte);
EntradaQte qte_entrada_para_teste(bool confirmar_pressionado, bool confirmar_mantido,
                                  bool confirmar_solto);

#endif
