#include "game.h"

static Rectangle GetLetterboxDestination(void)
{
    const float horizontal_scale = (float)GetScreenWidth() / LOGICAL_WIDTH;
    const float vertical_scale = (float)GetScreenHeight() / LOGICAL_HEIGHT;
    const float scale = horizontal_scale < vertical_scale ? horizontal_scale : vertical_scale;
    const float width = LOGICAL_WIDTH * scale;
    const float height = LOGICAL_HEIGHT * scale;

    return (Rectangle){
        ((float)GetScreenWidth() - width) * 0.5f,
        ((float)GetScreenHeight() - height) * 0.5f,
        width,
        height
    };
}

static Vector2 GetLogicalMousePosition(void)
{
    const Rectangle destination = GetLetterboxDestination();
    const Vector2 mouse_position = GetMousePosition();

    return (Vector2){
        (mouse_position.x - destination.x) * LOGICAL_WIDTH / destination.width,
        (mouse_position.y - destination.y) * LOGICAL_HEIGHT / destination.height
    };
}

int main(void)
{
    GameState game;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(960, 720, "Ascensao da Torre");
    SetWindowMinSize(LOGICAL_WIDTH, LOGICAL_HEIGHT);
    SetTargetFPS(60);

    RenderTexture2D canvas = LoadRenderTexture(LOGICAL_WIDTH, LOGICAL_HEIGHT);
    GameInit(&game);

    while (!WindowShouldClose() && !game.quit_requested) {
        game.mouse_position = GetLogicalMousePosition();
        game.mouse_pressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
        GameUpdate(&game);

        BeginTextureMode(canvas);
        ClearBackground((Color){248, 241, 229, 255});
        GameDraw(&game);
        EndTextureMode();

        BeginDrawing();
        ClearBackground((Color){24, 25, 30, 255});

        /* Draw through a fixed-size canvas so every window size keeps the same layout. */
        DrawTexturePro(canvas.texture,
                       (Rectangle){0.0f, 0.0f, LOGICAL_WIDTH, -LOGICAL_HEIGHT},
                       GetLetterboxDestination(), (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
        EndDrawing();
    }

    SaveWrite(&game.save);
    UnloadRenderTexture(canvas);
    CloseWindow();
    return 0;
}
