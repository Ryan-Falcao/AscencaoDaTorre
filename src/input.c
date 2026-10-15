#include "input.h"

#include "raylib.h"

bool input_confirm_pressed(void)
{
    return IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_Z);
}

bool input_confirm_down(void)
{
    return IsKeyDown(KEY_ENTER) || IsKeyDown(KEY_SPACE) || IsKeyDown(KEY_Z);
}

bool input_confirm_released(void)
{
    return IsKeyReleased(KEY_ENTER) || IsKeyReleased(KEY_SPACE) || IsKeyReleased(KEY_Z);
}

bool input_cancel_pressed(void)
{
    return IsKeyPressed(KEY_X) || IsKeyPressed(KEY_BACKSPACE);
}

bool input_up_pressed(void) { return IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W); }
bool input_down_pressed(void) { return IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S); }
bool input_left_pressed(void) { return IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A); }
bool input_right_pressed(void) { return IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D); }
bool input_up_down(void) { return IsKeyDown(KEY_UP) || IsKeyDown(KEY_W); }
bool input_down_down(void) { return IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S); }
bool input_left_down(void) { return IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A); }
bool input_right_down(void) { return IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D); }
