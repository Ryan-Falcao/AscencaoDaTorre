#include <assert.h>
#include <stdio.h>

#include "qte.h"

int main(void)
{
    Qte qte;

    assert(qte_result_from_timing_distance(0.01f, 1) == QTE_PERFECT);
    assert(qte_result_from_timing_distance(0.09f, 1) == QTE_GOOD);
    assert(qte_result_from_timing_distance(0.20f, 1) == QTE_BAD);
    assert(qte_result_from_timing_distance(0.40f, 1) == QTE_ERROR);

    assert(qte_result_from_sequence(3, 3, 1.0f, 3.0f) == QTE_PERFECT);
    assert(qte_result_from_sequence(3, 3, 2.0f, 3.0f) == QTE_GOOD);
    assert(qte_result_from_sequence(2, 4, 1.0f, 3.0f) == QTE_BAD);
    assert(qte_result_from_sequence(1, 4, 1.0f, 3.0f) == QTE_ERROR);

    qte_start(&qte, QTE_TIMING, 1);
    qte_update_with_input(&qte, 0.10f, qte_input_for_test(true, true, false, -1));
    assert(qte_is_in_guard(&qte) && !qte.result_ready);
    qte_update_with_input(&qte, 0.21f, qte_input_for_test(true, true, false, -1));
    assert(!qte_is_in_guard(&qte) && !qte.result_ready);

    qte_start(&qte, QTE_HOLD, 1);
    qte_update_with_input(&qte, 0.31f, qte_input_for_test(true, true, false, -1));
    qte_update_with_input(&qte, 0.01f, qte_input_for_test(false, true, false, -1));
    assert(!qte.hold_ready && !qte.hold_started);
    qte_update_with_input(&qte, 0.01f, qte_input_for_test(false, false, true, -1));
    assert(qte.hold_ready && !qte.hold_started);
    qte_update_with_input(&qte, 0.10f, qte_input_for_test(false, true, false, -1));
    assert(!qte.hold_started);
    qte_update_with_input(&qte, 0.10f, qte_input_for_test(true, true, false, -1));
    assert(qte.hold_started);

    puts("Metricas, guarda e inicio seguro de QTE verificados.");
    return 0;
}
