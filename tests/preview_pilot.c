#include "../src/game.c"
#include <stdio.h>
#include <stdlib.h>
int main(int argc,char **argv)
{
    SetTraceLogLevel(LOG_WARNING);SetConfigFlags(FLAG_WINDOW_HIDDEN);
    InitWindow(640,480,"Previa do piloto");InterfaceCarregarFontes();
    EstadoJogo jogo={0};SpritesInicializar(&jogo.sprites);
    RenderizacaoInicializar(&jogo.renderizacao,false);
    RenderTexture2D tela=LoadRenderTexture(640,480);
    Image folha=GenImageColor(1920,480*PilotoTotalInimigos(),RAYWHITE);
    for(int id=0;id<PilotoTotalInimigos()*3;id++){
        JogoAbrirPiloto(&jogo,id/3);jogo.piloto_ataque=id%3;ReiniciarPiloto(&jogo);
        jogo.batalha.defesa.depurar_invencivel=true;
        float tempo=argc>1?(float)atof(argv[1]):id<2?3.2f:id==8?2.85f:2.0f;
        for(int f=0;f<(int)(tempo*60);f++)DefesaAtualizarComEntrada(&jogo.batalha.defesa,&jogo.jogador,1.0f/60,(EntradaDefesa){0});
        BeginTextureMode(tela);DesenharPiloto(&jogo);EndTextureMode();
        Image img=LoadImageFromTexture(tela.texture);ImageFlipVertical(&img);
        ExportImage(img,TextFormat("qa/pilot-%d.png",id));
        ImageDraw(&folha,img,(Rectangle){0,0,640,480},(Rectangle){id%3*640,id/3*480,640,480},WHITE);UnloadImage(img);
    }
    ExportImage(folha,"qa/pilot-overview.png");UnloadImage(folha);
    UnloadRenderTexture(tela);SpritesDescarregar(&jogo.sprites);InterfaceDescarregarFontes();CloseWindow();
    return 0;
}
