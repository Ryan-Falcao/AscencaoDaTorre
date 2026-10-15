#ifndef INPUT_H
#define INPUT_H

#include <stdbool.h>

bool input_confirm_pressed(void);
bool input_confirm_down(void);
bool input_confirm_released(void);
bool input_cancel_pressed(void);

bool input_up_pressed(void);
bool input_down_pressed(void);
bool input_left_pressed(void);
bool input_right_pressed(void);
bool input_up_down(void);
bool input_down_down(void);
bool input_left_down(void);
bool input_right_down(void);

#endif
