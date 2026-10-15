#include "ui.h"

#include <stdio.h>
#include <string.h>

void UiSetText(TypewriterText *text, const char *message)
{
    snprintf(text->text, sizeof(text->text), "%s", message);
    text->visible_characters = 0;
    text->character_timer = 0.0f;
    text->complete = false;
}

void UiUpdateText(TypewriterText *text, float delta_time)
{
    const int length = (int)strlen(text->text);

    if (text->complete) return;

    text->character_timer += delta_time;
    while (text->character_timer >= 0.025f && text->visible_characters < length) {
        text->character_timer -= 0.025f;
        text->visible_characters++;
    }
    if (text->visible_characters >= length) text->complete = true;
}

bool UiConfirmText(TypewriterText *text)
{
    if (!text->complete) {
        text->visible_characters = (int)strlen(text->text);
        text->complete = true;
        return false;
    }

    return true;
}

void UiDrawPanel(Rectangle panel)
{
    DrawRectangleRec(panel, RAYWHITE);
    DrawRectangleLinesEx(panel, 3.0f, BLACK);
}

void UiDrawMeter(Rectangle meter, float amount, float maximum, Color fill)
{
    float ratio = maximum > 0.0f ? amount / maximum : 0.0f;

    if (ratio < 0.0f) ratio = 0.0f;
    if (ratio > 1.0f) ratio = 1.0f;
    DrawRectangleRec(meter, BLACK);
    DrawRectangle((int)meter.x + 2, (int)meter.y + 2, (int)meter.width - 4,
                  (int)meter.height - 4, RAYWHITE);
    DrawRectangle((int)meter.x + 2, (int)meter.y + 2,
                  (int)((meter.width - 4.0f) * ratio), (int)meter.height - 4, fill);
}

float UiApproach(float current, float target, float speed, float delta_time)
{
    const float distance = target - current;
    const float step = speed * delta_time;

    if (distance > step) return current + step;
    if (distance < -step) return current - step;
    return target;
}

void UiDrawTextBox(const TypewriterText *text, Rectangle box)
{
    char visible[DIALOGUE_CAPACITY];
    int character_count = text->visible_characters;

    if (character_count >= DIALOGUE_CAPACITY) character_count = DIALOGUE_CAPACITY - 1;
    memcpy(visible, text->text, (size_t)character_count);
    visible[character_count] = '\0';

    UiDrawPanel(box);
    DrawText(visible, (int)box.x + 16, (int)box.y + 15, 18, BLACK);
    if (text->complete) DrawText("ESPACO", (int)(box.x + box.width - 61),
                                 (int)(box.y + box.height - 25), 12, DARKGRAY);
}

