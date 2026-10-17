#include <assert.h>
#include <stdio.h>

#include "qte.h"

int main(void)
{
    Qte qte;

    assert(qte_resultado_por_distancia_tempo(0.01f, 1) == QTE_PERFEITO);
    assert(qte_resultado_por_distancia_tempo(0.09f, 1) == QTE_BOM);
    assert(qte_resultado_por_distancia_tempo(0.20f, 1) == QTE_RUIM);
    assert(qte_resultado_por_distancia_tempo(0.40f, 1) == QTE_ERRO);

    assert(qte_resultado_por_ritmo(9, 3) == QTE_PERFEITO);
    assert(qte_resultado_por_ritmo(6, 3) == QTE_BOM);
    assert(qte_resultado_por_ritmo(3, 3) == QTE_RUIM);
    assert(qte_resultado_por_ritmo(2, 3) == QTE_ERRO);

    qte_iniciar(&qte, QTE_IMPACTO, 1);
    qte_atualizar_com_entrada(&qte, 0.10f, qte_entrada_para_teste(true, true, false));
    assert(qte_esta_em_guarda(&qte) && !qte.resultado_pronto);
    qte_atualizar_com_entrada(&qte, 0.29f, qte_entrada_para_teste(true, true, false));
    assert(!qte_esta_em_guarda(&qte) && !qte.resultado_pronto);

    qte_iniciar(&qte, QTE_RITMO, 1);
    qte_atualizar_com_entrada(&qte, 0.39f, qte_entrada_para_teste(false, false, false));
    qte_atualizar_com_entrada(&qte, qte.duracao * 0.32f,
                              qte_entrada_para_teste(true, true, false));
    qte_atualizar_com_entrada(&qte, qte.duracao * 0.68f,
                              qte_entrada_para_teste(true, true, false));
    qte_atualizar_com_entrada(&qte, qte.duracao * 0.48f,
                              qte_entrada_para_teste(true, true, false));
    assert(qte.resultado_pronto && qte.resultado == QTE_PERFEITO);

    qte_iniciar(&qte, QTE_CARGA, 1);
    qte_atualizar_com_entrada(&qte, 0.39f, qte_entrada_para_teste(true, true, false));
    qte_atualizar_com_entrada(&qte, 0.01f, qte_entrada_para_teste(false, true, false));
    assert(!qte.segurar_pronto && !qte.segurar_iniciado);
    qte_atualizar_com_entrada(&qte, 0.01f, qte_entrada_para_teste(false, false, true));
    assert(qte.segurar_pronto && !qte.segurar_iniciado);
    qte_atualizar_com_entrada(&qte, 0.10f, qte_entrada_para_teste(false, true, false));
    assert(!qte.segurar_iniciado);
    qte_atualizar_com_entrada(&qte, 0.10f, qte_entrada_para_teste(true, true, false));
    assert(qte.segurar_iniciado);

    puts("Metricas, guarda e inicio seguro de QTE verificados.");
    return 0;
}
