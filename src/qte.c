#include "qte.h"

#include "input.h"

#include <math.h>
#include <string.h>

#define QTE_GUARD_SECONDS 0.30f
#define QTE_RESULT_SECONDS 0.50f
#define QTE_TIMING_TARGET 0.78f
#define QTE_TIMING_DURATION_BASE 1.15f
#define QTE_HOLD_DURATION_BASE 1.20f

static float ClampDifficulty(int difficulty, float base, float step, float minimum)
{
    const float value = base - (float)(difficulty - 1) * step;
    return value < minimum ? minimum : value;
}

static void FinishQte(Qte *qte, QteResult result)
{
    qte->result = result;
    qte->result_ready = true;
    qte->result_timer = 0.0f;
}

static void AudioTickHook(void) { }
static void AudioErrorHook(void) { }
static void AudioPerfectHook(void) { }

QteResult qte_result_from_timing_distance(float distance, int difficulty)
{
    const float perfect_window = ClampDifficulty(difficulty, 0.055f, 0.007f, 0.020f);
    const float good_window = ClampDifficulty(difficulty, 0.130f, 0.012f, 0.060f);
    const float bad_window = ClampDifficulty(difficulty, 0.240f, 0.016f, 0.110f);

    if (distance <= perfect_window) return QTE_PERFECT;
    if (distance <= good_window) return QTE_GOOD;
    if (distance <= bad_window) return QTE_BAD;
    return QTE_ERROR;
}

QteResult qte_result_from_sequence(int correct, int total, float elapsed, float limit)
{
    if (total <= 0 || correct <= 0) return QTE_ERROR;
    if (correct == total) return elapsed <= limit * 0.55f ? QTE_PERFECT : QTE_GOOD;
    return correct * 2 >= total ? QTE_BAD : QTE_ERROR;
}

void qte_start(Qte *qte, QteKind kind, int difficulty)
{
    memset(qte, 0, sizeof(*qte));
    qte->active = true;
    qte->kind = kind;
    qte->difficulty = difficulty < 1 ? 1 : difficulty;
    qte->target_time = QTE_TIMING_TARGET;
    qte->guard_timer = QTE_GUARD_SECONDS;
    qte->hold_ready = kind != QTE_HOLD;
    qte->released_charge = -1.0f;

    if (kind == QTE_TIMING) {
        qte->duration = ClampDifficulty(qte->difficulty, QTE_TIMING_DURATION_BASE, 0.08f, 0.80f);
    } else if (kind == QTE_SEQUENCE) {
        qte->sequence_length = 2 + qte->difficulty;
        if (qte->sequence_length > QTE_MAX_SEQUENCE) qte->sequence_length = QTE_MAX_SEQUENCE;
        qte->duration = ClampDifficulty(qte->difficulty, 2.70f, 0.28f, 1.65f);
        for (int index = 0; index < qte->sequence_length; index++) {
            qte->sequence[index] = GetRandomValue(0, 3);
        }
    } else {
        qte->duration = ClampDifficulty(qte->difficulty, QTE_HOLD_DURATION_BASE, 0.08f, 0.78f);
    }
}

static int ReadDirection(void)
{
    if (input_up_pressed()) return 0;
    if (input_right_pressed()) return 1;
    if (input_down_pressed()) return 2;
    if (input_left_pressed()) return 3;
    return -1;
}

static QteResult HoldResult(const Qte *qte)
{
    const float center = 0.68f;
    const float perfect_half_width = ClampDifficulty(qte->difficulty, 0.105f, 0.013f, 0.045f);
    const float distance = fabsf(qte->hold_charge - center);

    if (distance <= perfect_half_width) return QTE_PERFECT;
    if (distance <= perfect_half_width + 0.10f) return QTE_GOOD;
    if (distance <= perfect_half_width + 0.22f) return QTE_BAD;
    return QTE_ERROR;
}

QteInput qte_input_for_test(bool confirm_pressed, bool confirm_down, bool confirm_released,
                            int direction_pressed)
{
    return (QteInput){confirm_pressed, confirm_down, confirm_released, direction_pressed};
}

static QteInput ReadInput(void)
{
    return qte_input_for_test(input_confirm_pressed(), input_confirm_down(),
                              input_confirm_released(), ReadDirection());
}

bool qte_is_in_guard(const Qte *qte)
{
    return qte->guard_timer > 0.0f;
}

void qte_resume_with_guard(Qte *qte)
{
    if (qte->active && !qte->result_ready) qte->guard_timer = QTE_GUARD_SECONDS;
}

void qte_update_with_input(Qte *qte, float delta_time, QteInput input)
{
    if (!qte->active) return;
    if (qte->feedback_timer > 0.0f) qte->feedback_timer -= delta_time;
    if (qte->result_ready) {
        qte->result_timer += delta_time;
        return;
    }

    if (qte->guard_timer > 0.0f) {
        qte->guard_timer -= delta_time;
        if (qte->guard_timer < 0.0f) qte->guard_timer = 0.0f;
        return;
    }
    if (qte->kind == QTE_HOLD && !qte->hold_ready) {
        if (!input.confirm_down) qte->hold_ready = true;
        return;
    }

    qte->elapsed += delta_time;
    if (qte->kind == QTE_TIMING) {
        if (input.confirm_pressed) {
            const float progress = qte->elapsed / qte->duration;
            const QteResult result = qte_result_from_timing_distance(
                fabsf(progress - qte->target_time), qte->difficulty);
            qte->feedback_timer = 0.20f;
            qte->feedback_success = result != QTE_ERROR;
            if (result == QTE_PERFECT) AudioPerfectHook(); else if (result == QTE_ERROR) AudioErrorHook();
            else AudioTickHook();
            FinishQte(qte, result);
        } else if (qte->elapsed >= qte->duration) {
            qte->feedback_timer = 0.20f;
            qte->feedback_success = false;
            AudioErrorHook();
            FinishQte(qte, QTE_ERROR);
        }
    } else if (qte->kind == QTE_SEQUENCE) {
        const int arrow = input.direction_pressed;

        if (arrow >= 0) {
            if (arrow != qte->sequence[qte->sequence_index]) {
                qte->feedback_timer = 0.20f;
                qte->feedback_success = false;
                AudioErrorHook();
                FinishQte(qte, QTE_ERROR);
            } else {
                qte->sequence_index++;
                qte->feedback_timer = 0.14f;
                qte->feedback_success = true;
                AudioTickHook();
                if (qte->sequence_index == qte->sequence_length) {
                    const QteResult result = qte_result_from_sequence(qte->sequence_index,
                        qte->sequence_length, qte->elapsed, qte->duration);
                    if (result == QTE_PERFECT) AudioPerfectHook();
                    FinishQte(qte, result);
                }
            }
        } else if (qte->elapsed >= qte->duration) {
            FinishQte(qte, qte_result_from_sequence(qte->sequence_index, qte->sequence_length,
                                                     qte->elapsed, qte->duration));
        }
    } else {
        if (input.confirm_pressed && !qte->hold_started) qte->hold_started = true;
        if (qte->hold_started && input.confirm_down) {
            qte->hold_charge += delta_time / qte->duration;
        } else if (qte->hold_started && input.confirm_released) {
            const QteResult result = HoldResult(qte);
            qte->released_charge = qte->hold_charge;
            qte->feedback_timer = 0.20f;
            qte->feedback_success = result != QTE_ERROR;
            if (result == QTE_PERFECT) AudioPerfectHook(); else if (result == QTE_ERROR) AudioErrorHook();
            else AudioTickHook();
            FinishQte(qte, result);
        }

        if (qte->hold_charge >= 1.0f || qte->elapsed >= qte->duration * 1.5f) {
            qte->released_charge = qte->hold_charge;
            qte->feedback_timer = 0.20f;
            qte->feedback_success = false;
            AudioErrorHook();
            FinishQte(qte, QTE_ERROR);
        }
    }
}

void qte_update(Qte *qte, float delta_time)
{
    qte_update_with_input(qte, delta_time, ReadInput());
}

bool qte_is_finished(const Qte *qte)
{
    return qte->result_ready && qte->result_timer >= QTE_RESULT_SECONDS;
}

float qte_multiplier(QteResult result)
{
    static const float multipliers[] = {1.5f, 1.0f, 0.5f, 0.25f};
    return multipliers[result];
}

const char *qte_result_name(QteResult result)
{
    static const char *names[] = {"PERFEITO!", "BOM", "RUIM", "ERRO"};
    return names[result];
}

static Color ResultColor(QteResult result)
{
    static const Color colors[] = {GREEN, SKYBLUE, ORANGE, RED};
    return colors[result];
}

static const char *KeyName(int arrow)
{
    static const char *arrows[] = {"W", "D", "S", "A"};
    return arrows[arrow];
}

static void DrawTimeBar(const Qte *qte, int x, int y, int width)
{
    const float remaining = 1.0f - qte->elapsed / qte->duration;
    const Color color = remaining > 0.30f ? (Color){95, 184, 112, 255} :
                        (remaining > 0.12f ? (Color){237, 190, 62, 255} : (Color){224, 75, 71, 255});

    DrawRectangle(x, y, width, 8, LIGHTGRAY);
    DrawRectangle(x, y, (int)(width * (remaining < 0.0f ? 0.0f : remaining)), 8, color);
    DrawRectangleLines(x, y, width, 8, BLACK);
}

static void DrawKeycap(Rectangle cap, const char *label, bool current, Color fill)
{
    DrawRectangleRec(cap, fill);
    DrawRectangleLinesEx(cap, current ? 3.0f : 2.0f, BLACK);
    DrawText(label, (int)(cap.x + cap.width * 0.5f - MeasureText(label, current ? 22 : 16) * 0.5f),
             (int)(cap.y + cap.height * 0.5f - (current ? 11 : 8)), current ? 22 : 16, BLACK);
}

void qte_draw(const Qte *qte)
{
    const Rectangle panel = {188.0f, 88.0f, 264.0f, 118.0f};

    DrawRectangleRec(panel, RAYWHITE);
    DrawRectangleLinesEx(panel, 3.0f, BLACK);
    if (qte_is_in_guard(qte)) {
        const int count = (int)ceilf(qte->guard_timer / (QTE_GUARD_SECONDS / 3.0f));
        DrawText("PREPARAR...", 245, 112, 19, BLACK);
        DrawText(TextFormat("%d", count < 1 ? 1 : count), 314, 143, 34, (Color){225, 95, 122, 255});
        DrawRectangle(218, 181, 204, 8, LIGHTGRAY);
        DrawRectangle(218, 181, (int)(204.0f * (1.0f - qte->guard_timer / QTE_GUARD_SECONDS)), 8,
                      (Color){225, 95, 122, 255});
        return;
    }
    if (qte->result_ready) {
        const char *name = qte_result_name(qte->result);
        if (qte->kind == QTE_TIMING && qte->result == QTE_ERROR) {
            DrawCircleLines(320, 155, 18, BLACK);
            DrawCircleLines(320, 155, 8, RED);
        }
        if (qte->kind == QTE_HOLD && qte->released_charge >= 0.0f) {
            const int marker = 210 + (int)(220.0f * qte->released_charge);
            DrawRectangle(210, 166, 220, 14, LIGHTGRAY);
            DrawRectangleLines(210, 166, 220, 14, BLACK);
            DrawLine(marker, 161, marker, 185, RED);
        }
        DrawText(name, 320 - MeasureText(name, 29) / 2, 129, 29, ResultColor(qte->result));
        return;
    }

    if (qte->kind == QTE_TIMING) {
        const float progress = qte->elapsed / qte->duration;
        const float radius = 58.0f - progress * 46.0f;
        DrawText("Aperte na hora certa!", 233, 97, 16, BLACK);
        DrawKeycap((Rectangle){274, 108, 92, 22}, "ESPAÇO", false, RAYWHITE);
        DrawCircleLines(320, 155, 18, BLACK);
        DrawCircleLines(320, 155, radius, progress > qte->target_time ? RED : (Color){225, 95, 122, 255});
        DrawTimeBar(qte, 218, 190, 204);
    } else if (qte->kind == QTE_SEQUENCE) {
        DrawText("Repita W A S D!", 247, 97, 16, BLACK);
        for (int index = 0; index < qte->sequence_length; index++) {
            const bool current = index == qte->sequence_index;
            const int size = current ? 38 : 28;
            const int x = 218 + index * 40;
            const Color color = index < qte->sequence_index ? (Color){105, 185, 119, 255} : RAYWHITE;
            DrawKeycap((Rectangle){(float)x, 137.0f, (float)size, (float)size},
                       KeyName(qte->sequence[index]), current, color);
        }
        DrawTimeBar(qte, 218, 190, 204);
    } else {
        const float zone_width = ClampDifficulty(qte->difficulty, 0.21f, 0.026f, 0.09f);
        const int bar_x = 210;
        const int bar_y = 146;
        const int bar_width = 220;
        const int zone_x = bar_x + (int)(bar_width * (0.68f - zone_width * 0.5f));
        const int zone_pixels = (int)(bar_width * zone_width);

        DrawText(qte->hold_ready ? "Segure e solte na zona!" : "Solte a tecla para comecar!", 216, 97, 16, BLACK);
        DrawKeycap((Rectangle){274, 108, 92, 22}, "ESPAÇO", false, RAYWHITE);
        DrawRectangle(bar_x, bar_y, bar_width, 20, LIGHTGRAY);
        DrawRectangle(zone_x, bar_y, zone_pixels, 20, (Color){112, 205, 126, 255});
        DrawRectangleLines(bar_x, bar_y, bar_width, 20, BLACK);
        DrawRectangle(bar_x, bar_y, (int)(bar_width * qte->hold_charge), 20, (Color){92, 145, 220, 255});
        if (qte->released_charge >= 0.0f) DrawLine(bar_x + (int)(bar_width * qte->released_charge),
                                                    bar_y - 4, bar_x + (int)(bar_width * qte->released_charge),
                                                    bar_y + 24, RED);
        DrawTimeBar(qte, 218, 190, 204);
    }
    if (qte->feedback_timer > 0.0f) {
        const char *feedback = qte->feedback_success ? "ACERTO" : "ERRO";
        DrawText(feedback, 380, 114, 13, qte->feedback_success ? GREEN : RED);
    }
}
