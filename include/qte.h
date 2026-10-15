#ifndef QTE_H
#define QTE_H

#include "raylib.h"

#define QTE_MAX_SEQUENCE 5

typedef enum {
    QTE_TIMING,
    QTE_SEQUENCE,
    QTE_HOLD
} QteKind;

typedef enum {
    QTE_PERFECT,
    QTE_GOOD,
    QTE_BAD,
    QTE_ERROR
} QteResult;

typedef struct {
    bool confirm_pressed;
    bool confirm_down;
    bool confirm_released;
    int direction_pressed;
} QteInput;

typedef struct {
    bool active;
    bool result_ready;
    QteKind kind;
    QteResult result;
    int difficulty;
    float elapsed;
    float guard_timer;
    float result_timer;
    float duration;
    float target_time;
    int sequence[QTE_MAX_SEQUENCE];
    int sequence_length;
    int sequence_index;
    bool hold_started;
    bool hold_ready;
    float hold_charge;
    float released_charge;
    float feedback_timer;
    bool feedback_success;
} Qte;

void qte_start(Qte *qte, QteKind kind, int difficulty);
void qte_update(Qte *qte, float delta_time);
void qte_update_with_input(Qte *qte, float delta_time, QteInput input);
void qte_draw(const Qte *qte);
bool qte_is_finished(const Qte *qte);
float qte_multiplier(QteResult result);
const char *qte_result_name(QteResult result);
QteResult qte_result_from_timing_distance(float distance, int difficulty);
QteResult qte_result_from_sequence(int correct, int total, float elapsed, float limit);
bool qte_is_in_guard(const Qte *qte);
void qte_resume_with_guard(Qte *qte);
QteInput qte_input_for_test(bool confirm_pressed, bool confirm_down, bool confirm_released,
                            int direction_pressed);

#endif
