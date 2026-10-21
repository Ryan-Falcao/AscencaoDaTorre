#include "plant_patterns.h"
#include "raymath.h"
#include <math.h>

/* Silhuetas temporarias, alinhadas as mesmas capsulas da colisao.
   O manifesto aceita arte isolada por peca sem alterar o comportamento. */
void PlantaDesenharPeca(const AmeacaPiloto *p,const SistemaSprites *sprites,float t)
{
    Vector2 centro=Vector2Lerp(p->a,p->b,.5f);float n=Vector2Distance(p->a,p->b),r=p->raio;
    Color verde={128,202,115,255},claro={215,240,170,255},escuro={34,64,49,255};
    float pulso=.5f+.5f*sinf(t*11+p->evento*.17f);
    Color cor=p->ativo?verde:Fade(GOLD,.50f+.3f*pulso);
    if(p->sprite==SPRITE_PLANTA_ANTIDOTO)cor=(Color){99,206,228,220};
    if(p->sprite==SPRITE_PLANTA_VENENO)cor=(Color){178,115,207,255};
    const SpriteJogo *s=sprites?sprite_obter(sprites,p->sprite):NULL;
    if(s&&!s->peca_temporaria){
        float w=s->metadados.largura_quadro>0?s->metadados.largura_quadro:s->textura.width;
        float h=s->metadados.altura_quadro>0?s->metadados.altura_quadro:s->textura.height;
        float tamanho=fmaxf(r*2+4,n+r*2);
        float rot=p->sprite==SPRITE_PLANTA_FARPA?t*80:p->rotacao;
        if(p->sprite==SPRITE_PLANTA_ESCUDO)rot-=90;
        SpriteDesenharCentralizadoRotacionado(sprites,p->sprite,centro,p->ativo?WHITE:Fade(WHITE,.6f+.2f*pulso),t,
            rot,tamanho/fmaxf(w,h)/s->escala*(1+.025f*sinf(t*8)),false);return;
    }
    if(p->sprite==SPRITE_PLANTA_RAIZ||p->sprite==SPRITE_PLANTA_FILAMENTO){
        Color raiz=p->sprite==SPRITE_PLANTA_RAIZ?(Color){164,125,75,255}:Fade(verde,p->ativo?.85f:.4f);
        if(!p->ativo)raiz=Fade(GOLD,.4f+.3f*pulso);
        DrawLineEx(p->a,p->b,r*2,raiz);DrawCircleV(p->a,r,raiz);DrawCircleV(p->b,r,raiz);
        DrawLineEx(p->a,p->b,fmaxf(1,r*.65f),Fade(claro,.65f));return;
    }
    if(!p->ativo && n<.1f && (p->sprite==SPRITE_PLANTA_LANCA||p->sprite==SPRITE_PLANTA_TABUA))DrawCircleV(centro,r+pulso*1.5f,Fade(GOLD,.5f+.3f*pulso));
    if(!p->ativo && n>12){DrawCircleV(p->a,4+pulso*2,Fade(GOLD,.5f));return;} /* Nao exibir o percurso futuro das armas. */
    if(p->sprite==SPRITE_PLANTA_LANCA||p->sprite==SPRITE_PLANTA_FOICE||p->sprite==SPRITE_PLANTA_TABUA){
        DrawLineEx(p->a,p->b,r*2+1,escuro);DrawLineEx(p->a,p->b,fmaxf(2,r*2-1),p->sprite==SPRITE_PLANTA_FOICE?claro:(Color){162,125,82,255});
        Vector2 dir=Vector2Normalize(Vector2Subtract(p->b,p->a)),normal={-dir.y,dir.x};
        if(p->sprite!=SPRITE_PLANTA_TABUA){
            Vector2 ponta=Vector2Add(p->b,Vector2Scale(dir,2));
            DrawTriangle(ponta,Vector2Subtract(Vector2Subtract(p->b,Vector2Scale(dir,8)),Vector2Scale(normal,4)),
                Vector2Add(Vector2Subtract(p->b,Vector2Scale(dir,8)),Vector2Scale(normal,4)),claro);
        }else DrawCircleV(p->a,3,GOLD);
        return;
    }
    if(p->sprite==SPRITE_PLANTA_ESCUDO){
        DrawRectangleRounded((Rectangle){centro.x-r,centro.y-n*.5f-r,r*2,n+r*2},.5f,6,escuro);
        DrawRectangleRounded((Rectangle){centro.x-r+1,centro.y-n*.5f-r+1,r*2-2,n+r*2-2},.5f,6,cor);
        DrawLineEx((Vector2){centro.x,centro.y-n*.45f},(Vector2){centro.x,centro.y+n*.45f},2,claro);return;
    }
    if(p->sprite==SPRITE_PLANTA_FRASCO){
        DrawCircleV(centro,r,escuro);DrawCircleV(centro,r-1,(Color){172,128,204,255});
        DrawRectangle((int)(centro.x-2),(int)(centro.y-r-3),4,4,claro);
        DrawCircleV(Vector2Add(centro,(Vector2){-2,-2}),1.4f,RAYWHITE);return;
    }
    if(p->sprite==SPRITE_PLANTA_TOCA){
        DrawEllipse((int)centro.x,(int)centro.y,r,r*.6f,(Color){136,105,68,255});
        DrawEllipse((int)centro.x,(int)centro.y,r*.6f,r*.3f,escuro);return;
    }
    if(p->sprite==SPRITE_PLANTA_ALMOFARIZ||p->sprite==SPRITE_PLANTA_ALTAR){
        DrawCircleV(centro,r,escuro);DrawCircleLines((int)centro.x,(int)centro.y,r,cor);
        DrawRectangleRounded((Rectangle){centro.x-r,centro.y-r*.3f,r*2,r},.4f,6,(Color){125,120,91,255});
        DrawEllipse((int)centro.x,(int)(centro.y-r*.3f),r,r*.35f,cor);return;
    }
    if(p->sprite==SPRITE_PLANTA_FARPA||p->sprite==SPRITE_PLANTA_SEMENTE){
        float a=t*2+p->evento*.3f;Vector2 eixo={cosf(a),sinf(a)},normal={-eixo.y,eixo.x};
        Vector2 ponta=Vector2Add(centro,Vector2Scale(eixo,r+1));
        Vector2 base=Vector2Subtract(centro,Vector2Scale(eixo,r));
        DrawTriangle(ponta,Vector2Subtract(base,Vector2Scale(normal,r*.65f)),Vector2Add(base,Vector2Scale(normal,r*.65f)),claro);
        if(s)SpriteDesenharCentralizadoRotacionado(sprites,p->sprite,centro,cor,t,a*RAD2DEG,(r*2+2)/fmaxf(s->textura.width,s->textura.height)/s->escala,false);
        return;
    }
    DrawCircleV(centro,r,escuro);DrawCircleV(centro,fmaxf(1,r-1),cor);
    DrawCircleV(Vector2Add(centro,(Vector2){-r*.25f,-r*.3f}),fmaxf(1,r*.22f),Fade(claro,.8f));
}
