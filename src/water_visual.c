#include "water_patterns.h"
#include "balance.h"
#include "raymath.h"
#include "ui.h"
#include "rlgl.h"
#include <math.h>

static void Tri(Vector2 a,Vector2 b,Vector2 c,Color cor)
{float cruz=(b.x-a.x)*(c.y-a.y)-(b.y-a.y)*(c.x-a.x);if(cruz>0)DrawTriangle(a,c,b,cor);else DrawTriangle(a,b,c,cor);}
static void ForaDisco(Rectangle b,Vector2 p,float r,Color cor)
{
    for(float y=b.y;y<b.y+b.height;y+=3){
        float dy=y+1.5f-p.y;if(fabsf(dy)>=r){DrawRectangleRec((Rectangle){b.x,y,b.width,3},cor);continue;}
        float x=sqrtf(fmaxf(0,r*r-dy*dy));float esquerda=Clamp(p.x-x,b.x,b.x+b.width),direita=Clamp(p.x+x,b.x,b.x+b.width);
        DrawRectangleRec((Rectangle){b.x,y,esquerda-b.x,3},cor);DrawRectangleRec((Rectangle){direita,y,b.x+b.width-direita,3},cor);
    }
}
/* Textura limitada ao poligono real. UVs movem a superficie, nao o hitbox. */
static void TexturaTri(const SpriteJogo *s,Vector2 a,Vector2 b,Vector2 c,float t)
{
    if(!s||s->peca_temporaria)return;
    if((b.x-a.x)*(c.y-a.y)-(b.y-a.y)*(c.x-a.x)>0){Vector2 troca=b;b=c;c=troca;}
    rlSetTexture(s->textura.id);rlBegin(RL_TRIANGLES);rlColor4ub(255,255,255,110);
    Vector2 pontos[]={a,b,c};
    for(int i=0;i<3;i++){rlTexCoord2f(pontos[i].x/48+t*.12f,pontos[i].y/48-t*.08f);rlVertex2f(pontos[i].x,pontos[i].y);}
    rlEnd();rlSetTexture(0);
}
static void TexturaRet(const SpriteJogo *s,Rectangle r,float t)
{
    Vector2 a={r.x,r.y},b={r.x+r.width,r.y},c={r.x+r.width,r.y+r.height},d={r.x,r.y+r.height};
    TexturaTri(s,a,b,c,t);TexturaTri(s,a,c,d,t);
}
void AguaDesenharPeca(const AmeacaPiloto *p,const SistemaSprites *sprites,float t,Rectangle b)
{
    Color azul={47,137,185,215},espuma={164,230,244,255},gelo={177,219,236,245};
    float pulso=.5f+.5f*sinf(t*12+p->evento*.2f);Vector2 centro=Vector2Lerp(p->a,p->b,.5f);
    const SpriteJogo *s=sprites?sprite_obter(sprites,p->sprite):NULL;
    if(p->regra==4){DrawRectangleRec(p->area,Fade((Color){99,123,77,255},.48f));TexturaRet(s,p->area,t);for(float y=p->area.y+5;y<p->area.y+p->area.height;y+=13)DrawLineEx((Vector2){p->area.x+4,y},(Vector2){p->area.x+p->area.width-4,y+3*sinf(t*2)},1,Fade(LIME,.24f));return;}
    if(p->regra==6){DrawRectangleRounded(p->area,.15f,4,(Color){91,108,111,255});TexturaRet(s,p->area,t);DrawRectangleLinesEx(p->area,2,espuma);for(float x=p->area.x+10;x<p->area.x+p->area.width;x+=17)DrawLineEx((Vector2){x,p->area.y+6},(Vector2){x,p->area.y+p->area.height-6},1,Fade(espuma,.24f));return;}
    if(p->sprite==SPRITE_AGUA_ESCUDO){
        if(p->forma==PILOTO_CAPSULA){DrawLineEx(p->a,p->b,7,(Color){86,104,123,255});DrawLineEx(p->a,p->b,2,espuma);for(int i=0;i<5;i++){Vector2 z=Vector2Lerp(p->a,p->b,(i+.5f)/5);DrawCircleV(z,2,Fade(GOLD,.65f+.2f*pulso));}}else{DrawCircleV(centro,p->raio,(Color){78,99,115,255});DrawCircleLines((int)centro.x,(int)centro.y,p->raio,p->ativo?ORANGE:espuma);}
        if(s&&!s->peca_temporaria){float tamanho=p->forma==PILOTO_CAPSULA?Vector2Distance(p->a,p->b):p->raio*2;SpriteDesenharCentralizadoRotacionado(sprites,p->sprite,centro,WHITE,t,p->rotacao,tamanho/fmaxf(s->textura.width,s->textura.height)/s->escala,false);}
        return;
    }
    if(p->forma==PILOTO_FORA_CIRCULO){
        if(p->ativo)ForaDisco(b,p->a,p->raio,azul);
        DrawCircleLines((int)p->a.x,(int)p->a.y,p->raio,espuma);
        DrawCircleLines((int)p->a.x,(int)p->a.y,p->raio-2,Fade(espuma,.4f+.2f*pulso));
        if(s&&!s->peca_temporaria)SpriteDesenharCentralizadoRotacionado(sprites,p->sprite,p->a,Fade(SKYBLUE,.5f),t,0,p->raio*2/fmaxf(s->textura.width,s->textura.height)/s->escala,false);
        return;
    }
    if(p->forma==PILOTO_TRIANGULO){
        if(!p->ativo){DrawCircleV(p->a,5+pulso*2,Fade(espuma,.7f));return;}
        bool neve=p->sprite==SPRITE_AGUA_NEVE;Color cor=neve?(Color){199,221,232,240}:azul;
        Tri(p->a,p->b,p->c,cor);TexturaTri(s,p->a,p->b,p->c,t);DrawLineEx(p->b,p->a,2,neve?RAYWHITE:espuma);DrawLineEx(p->a,p->c,2,neve?RAYWHITE:espuma);
        for(int i=1;i<6;i++){float u=i/6.0f;Vector2 a=Vector2Lerp(p->a,p->b,u),z=Vector2Lerp(p->a,p->c,u);DrawLineEx(a,z,1,Fade(neve?WHITE:espuma,.3f+.12f*sinf(t*7+i)));}
        if(neve){for(int i=0;i<12;i++){float u=(i+.5f)/13;Vector2 lado=Vector2Lerp(p->b,p->c,.5f+.4f*sinf(i*2.3f));Vector2 ponto=Vector2Lerp(p->a,lado,u);DrawCircleV(ponto,3+u*4,Fade(RAYWHITE,.28f+.12f*sinf(t*4+i)));}}
        return;
    }
    if(p->forma==PILOTO_RETANGULO){
        if(!p->ativo){DrawRectangleRec(p->area,Fade(azul,.12f));return;}
        bool bloco=p->sprite==SPRITE_AGUA_FISSURA;
        DrawRectangleRec(p->area,bloco?gelo:azul);DrawRectangleLinesEx(p->area,bloco?2:1,espuma);
        Vector2 canto={p->area.x,p->area.y},direita={p->area.x+p->area.width,p->area.y},baixo={p->area.x,p->area.y+p->area.height},fim={p->area.x+p->area.width,p->area.y+p->area.height};
        TexturaTri(s,canto,direita,fim,t);TexturaTri(s,canto,fim,baixo,t);
        if(bloco){Vector2 a={p->area.x+8,p->area.y+6},z={p->area.x+p->area.width-6,p->area.y+p->area.height-5};DrawLineEx(a,z,2,Fade(SKYBLUE,.8f));DrawLineEx(Vector2Lerp(a,z,.5f),(Vector2){p->area.x+12,p->area.y+p->area.height-8},1,SKYBLUE);}
        else for(float x=p->area.x+6;x<p->area.x+p->area.width;x+=17){float y=p->area.y+p->area.height*.5f+sinf(t*8+x*.08f)*2;DrawLineEx((Vector2){x,y},(Vector2){x+8,y+2},1,espuma);}
        if(p->regra==3)DrawRectangleLinesEx(p->area,2,(Color){255,216,107,255});
        return;
    }
    if(p->sprite==SPRITE_AGUA_ESTACA&&p->forma==PILOTO_CAPSULA){
        Vector2 normal={p->raio,0};DrawLineEx(p->a,p->b,p->raio*2,Fade(SKYBLUE,.36f));DrawCircleV(p->b,p->raio,Fade(SKYBLUE,.36f));Tri(p->b,Vector2Add(p->a,normal),Vector2Subtract(p->a,normal),gelo);DrawLineEx(p->a,p->b,1,RAYWHITE);
        if(s&&!s->peca_temporaria)SpriteDesenharCentralizadoRotacionado(sprites,p->sprite,centro,WHITE,t,p->rotacao,Vector2Distance(p->a,p->b)/fmaxf(s->textura.width,s->textura.height)/s->escala,false);
        return;
    }
    Color cor=p->sprite==SPRITE_AGUA_LAMA?(Color){90,121,74,220}:p->sprite==SPRITE_AGUA_BOLHA?(Color){132,89,188,220}:azul;
    if(p->ativo){DrawCircleV(centro,p->raio,cor);DrawCircleLines((int)centro.x,(int)centro.y,p->raio,espuma);}
    else{DrawCircleV(centro,p->raio,Fade(cor,.20f+.13f*pulso));DrawCircleLines((int)centro.x,(int)centro.y,p->raio,Fade(p->sprite==SPRITE_AGUA_LAMA?LIME:GOLD,.6f+.3f*pulso));}
    if(s&&!s->peca_temporaria)SpriteDesenharCentralizadoRotacionado(sprites,p->sprite,centro,WHITE,t,t*8,(p->raio*2)/fmaxf(s->textura.width,s->textura.height)/s->escala,false);
    DrawCircleV(Vector2Add(centro,(Vector2){-p->raio*.25f,-p->raio*.3f}),fmaxf(1,p->raio*.1f),Fade(espuma,.8f));
}
void AguaDesenharEstado(const EstadoDefesa *d,const QuadroPiloto *q,const SistemaSprites *sprites)
{
    (void)sprites;Rectangle b=d->caixa;
    if(q->escorregadio){for(float x=b.x+12;x<b.x+b.width;x+=29)DrawLineEx((Vector2){x,b.y+b.height-10},(Vector2){x+12,b.y+b.height-22},1,Fade(SKYBLUE,.35f));InterfaceDesenharTexto("GELO: freie antes da curva",(int)b.x+7,(int)b.y+7,11,SKYBLUE);}
    if(d->agua.gelo>0){Vector2 p=d->posicao_alma;DrawPoly(p,6,11,30,Fade(SKYBLUE,.55f));DrawPolyLines(p,6,11,30,RAYWHITE);InterfaceDesenharTexto("CONGELADO | SHIFT rompe o gelo",(int)b.x+6,(int)(b.y+b.height-17),11,SKYBLUE);}
    if(d->id_padrao==PADRAO_AGUA_REFRACAO)DrawLineEx((Vector2){b.x,b.y+b.height*.48f},(Vector2){b.x+b.width,b.y+b.height*.48f},2,Fade(SKYBLUE,.45f));
    if(q->reflete_alma){Vector2 p={d->posicao_alma.x,b.y+b.height-(d->posicao_alma.y-b.y)};if(sprite_obter(sprites,SPRITE_ALMA))SpriteDesenharCentralizado(sprites,SPRITE_ALMA,p,SKYBLUE,d->temporizador_padrao,false);else DrawCircleV(p,d->raio_alma,(Color){119,204,236,255});DrawCircleLines((int)p.x,(int)p.y,d->raio_alma+2,SKYBLUE);DrawLineEx((Vector2){b.x,b.y+b.height*.5f},(Vector2){b.x+b.width,b.y+b.height*.5f},1,Fade(SKYBLUE,.30f));}
    if(d->id_padrao==PADRAO_AGUA_MEMBRANA)InterfaceDesenharTexto("DASH atravessa a membrana",(int)b.x+7,(int)b.y+7,11,GOLD);
    if(d->id_padrao==PADRAO_AGUA_BOLHA){const ConfiguracaoPiloto *c=&BALANCEAMENTO_PILOTO[d->id_padrao-PADRAO_PILOTO_POEIRA_ASA];float local=fmodf(d->temporizador_padrao*(d->fator_movimento_ataque>0?d->fator_movimento_ataque:1),c->ciclo);InterfaceDesenharTexto(local<c->aviso+.55f?"ENTRE NA BOLHA":"SAIA: A BOLHA VAI COLAPSAR",(int)b.x+6,(int)b.y+6,11,local<c->aviso+.55f?SKYBLUE:VIOLET);}
}
