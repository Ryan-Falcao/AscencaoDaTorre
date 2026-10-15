#ifndef UI_H
#define UI_H

#include "raylib.h"


#define DIALOGUE_CAPACITY 192

typedef struct {
    char text[DIALOGUE_CAPACITY];
    int visible_characters;
    float character_timer;
    bool complete;
} TypewriterText;

void UiSetText(TypewriterText *text, const char *message);
void UiUpdateText(TypewriterText *text, float delta_time);
bool UiConfirmText(TypewriterText *text);
void UiDrawTextBox(const TypewriterText *text, Rectangle box);
void UiDrawPanel(Rectangle panel);
void UiDrawMeter(Rectangle meter, float amount, float maximum, Color fill);
float UiApproach(float current, float target, float speed, float delta_time);
#endif
