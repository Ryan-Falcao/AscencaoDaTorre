#include "game.h"

#include <string.h>

static Rectangle ObterDestinoLetterbox(void)
{
    const float horizontal_scale = (float)GetScreenWidth() / LOGICO_LARGURA;
    const float vertical_scale = (float)GetScreenHeight() / LOGICO_ALTURA;
    const float escala = horizontal_scale < vertical_scale ? horizontal_scale : vertical_scale;
    const float width = LOGICO_LARGURA * escala;
    const float height = LOGICO_ALTURA * escala;

    return (Rectangle){
        ((float)GetScreenWidth() - width) * 0.5f,
        ((float)GetScreenHeight() - height) * 0.5f,
        width,
        height
    };
}

static Vector2 ObterPosicaoLogicaMouse(void)
{
    const Rectangle destino = ObterDestinoLetterbox();
    const Vector2 posicao_mouse = GetMousePosition();

    return (Vector2){
        (posicao_mouse.x - destino.x) * LOGICO_LARGURA / destino.width,
        (posicao_mouse.y - destino.y) * LOGICO_ALTURA / destino.height
    };
}

int main(int argc, char **argv)
{
    EstadoJogo jogo;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(960, 720, "Ascensao da Torre");
    SetWindowMinSize(LOGICO_LARGURA, LOGICO_ALTURA);
    SetTargetFPS(60);

    RenderTexture2D tela_virtual = LoadRenderTexture(LOGICO_LARGURA, LOGICO_ALTURA);
    JogoInicializar(&jogo);
    for (int indice = 1; indice < argc; indice++) {
        if (strcmp(argv[indice], "--telemetry") == 0) jogo.telemetria_ativada = true;
        if (strcmp(argv[indice], "--export-placeholders") == 0) {
            SpritesExportarModelos();
            SpritesDescarregar(&jogo.sprites);
            AudioDescarregar(&jogo.audio);
            UnloadRenderTexture(tela_virtual);
            CloseWindow();
            return 0;
        }
    }

    while (!WindowShouldClose() && !jogo.saida_solicitada) {
        jogo.posicao_mouse = ObterPosicaoLogicaMouse();
        jogo.mouse_pressionado = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
        JogoAtualizar(&jogo);

        BeginTextureMode(tela_virtual);
        ClearBackground((Color){248, 241, 229, 255});
        JogoDesenhar(&jogo);
        EndTextureMode();

        BeginDrawing();
        ClearBackground((Color){24, 25, 30, 255});

        /* Desenha em uma tela virtual fixa para manter o layout em qualquer tamanho de janela. */
        DrawTexturePro(tela_virtual.texture,
                       (Rectangle){0.0f, 0.0f, LOGICO_LARGURA, -LOGICO_ALTURA},
                       ObterDestinoLetterbox(), (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
        EndDrawing();
    }

    SalvamentoGravar(&jogo.salvamento);
    SpritesDescarregar(&jogo.sprites);
    AudioDescarregar(&jogo.audio);
    UnloadRenderTexture(tela_virtual);
    CloseWindow();
    return 0;
}
