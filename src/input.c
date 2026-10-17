#include "input.h"

#include "raylib.h"

bool entrada_confirmar_pressionada(void)
{
    return IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_Z);
}

bool entrada_confirmar_mantida(void)
{
    return IsKeyDown(KEY_ENTER) || IsKeyDown(KEY_SPACE) || IsKeyDown(KEY_Z);
}

bool entrada_confirmar_solta(void)
{
    return IsKeyReleased(KEY_ENTER) || IsKeyReleased(KEY_SPACE) || IsKeyReleased(KEY_Z);
}

bool entrada_cancelar_pressionada(void)
{
    return IsKeyPressed(KEY_X) || IsKeyPressed(KEY_BACKSPACE);
}

bool entrada_cima_pressionada(void) { return IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W); }
bool entrada_baixo_pressionada(void) { return IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S); }
bool entrada_esquerda_pressionada(void) { return IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A); }
bool entrada_direita_pressionada(void) { return IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D); }
bool entrada_cima_mantida(void) { return IsKeyDown(KEY_UP) || IsKeyDown(KEY_W); }
bool entrada_baixo_mantida(void) { return IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S); }
bool entrada_esquerda_mantida(void) { return IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A); }
bool entrada_direita_mantida(void) { return IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D); }
