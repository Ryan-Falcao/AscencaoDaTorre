#include "plant_patterns.h"
#include "balance.h"
#include "raymath.h"
#include <math.h>

static Vector2 Pos(Rectangle b,float x,float y){return (Vector2){b.x+b.width*x,b.y+b.height*y};}
static Vector2 Dir(float a){return (Vector2){cosf(a),sinf(a)};}
static Vector2 Curva(Vector2 a,Vector2 c,Vector2 b,float u)
{u=Clamp(u,0,1);return Vector2Add(Vector2Add(Vector2Scale(a,(1-u)*(1-u)),Vector2Scale(c,2*u*(1-u))),Vector2Scale(b,u*u));}
static const ConfiguracaoPiloto *Cfg(const EstadoDefesa *d){return &BALANCEAMENTO_PILOTO[d->id_padrao-PADRAO_PILOTO_POEIRA_ASA];}
static float Tempo(const EstadoDefesa *d,float t){return t*(d->fator_movimento_ataque>0?d->fator_movimento_ataque:1);}
static unsigned long Evento(int ciclo,int n){return 20000+(unsigned long)ciclo*100+n;}
static Vector2 Alvo(const EstadoDefesa *d,float margem)
{Rectangle b=d->caixa;return (Vector2){Clamp(d->ancora_movimento.x,b.x+margem,b.x+b.width-margem),Clamp(d->ancora_movimento.y,b.y+margem,b.y+b.height-margem)};}
static void Peca(QuadroPiloto *q,Vector2 a,Vector2 b,float r,bool ativa,IdSprite s,unsigned long e)
{
    if(q->total>=MAXIMO_AMEACAS_PILOTO)return;
    q->ameacas[q->total++]=(AmeacaPiloto){.forma=Vector2Distance(a,b)<.1f?PILOTO_CIRCULO:PILOTO_CAPSULA,
        .a=a,.b=b,.raio=r,.ativo=ativa,.sprite=s,.evento=e,.rotacao=atan2f(b.y-a.y,b.x-a.x)*RAD2DEG};
}
static void Grao(QuadroPiloto *q,Vector2 p,float r,bool ativa,IdSprite s,unsigned long e)
{Peca(q,p,p,r,ativa,s,e);}

static void EstilhacoLanca(const EstadoDefesa *d,float t,QuadroPiloto *q)
{
    const ConfiguracaoPiloto *c=Cfg(d);Rectangle b=d->caixa;float rel=Tempo(d,t),local=fmodf(rel,c->ciclo);
    int ciclo=(int)(rel/c->ciclo),sentido=ciclo%2?-1:1;Vector2 alvo=Alvo(d,14);
    Vector2 origem={sentido>0?b.x-24:b.x+b.width+24,alvo.y};
    float idade=local-c->aviso;
    if(idade<0){Grao(q,(Vector2){Clamp(origem.x,b.x+7,b.x+b.width-7),alvo.y},6,false,SPRITE_PLANTA_LANCA,Evento(ciclo,0));return;}
    float u=idade/.75f;float x=origem.x+sentido*(b.width+48)*(u<.72f?u/.72f:(1-u)/.28f);
    if(u<1)Peca(q,(Vector2){x-sentido*38,alvo.y},(Vector2){x,alvo.y},3.5f,true,SPRITE_PLANTA_LANCA,Evento(ciclo,1));
    float a=idade-.48f;
    if(a>=0)for(int i=0;i<c->quantidade;i++){
        Vector2 p=Vector2Add(alvo,Vector2Scale(Dir(-PI*.75f+i*PI/(c->quantidade-1)),a*c->velocidade));
        Grao(q,p,3,true,SPRITE_PLANTA_FARPA,Evento(ciclo,10+i));
    }
}
static void EscudoEnraizado(const EstadoDefesa *d,float t,QuadroPiloto *q)
{
    const ConfiguracaoPiloto *c=Cfg(d);Rectangle b=d->caixa;float rel=Tempo(d,t),local=fmodf(rel,c->ciclo);int ciclo=(int)(rel/c->ciclo);
    Vector2 alvo=Alvo(d,16),escudo=Pos(b,ciclo%2?.72f:.28f,.22f+.56f*((ciclo%3)/2.0f));
    bool ativo=local>=c->aviso;
    Peca(q,(Vector2){escudo.x,escudo.y-12},(Vector2){escudo.x,escudo.y+12},6,ativo,SPRITE_PLANTA_ESCUDO,Evento(ciclo,0));
    if(!ativo)return;
    float a=local-c->aviso;
    Vector2 raiz={escudo.x+(ciclo%2?-22:22),b.y+b.height};
    Peca(q,escudo,Vector2Lerp(escudo,raiz,Clamp(a/.35f,0,1)),2.6f,true,SPRITE_PLANTA_RAIZ,Evento(ciclo,1));
    float ang=atan2f(alvo.y-escudo.y,alvo.x-escudo.x);
    for(int onda=0;onda<2;onda++)for(int i=0;i<c->quantidade;i++){
        float idade=a-onda*c->intervalo;if(idade<0)continue;
        Vector2 p=Vector2Add(escudo,Vector2Scale(Dir(ang+(i-(c->quantidade-1)*.5f)*.27f),12+idade*c->velocidade));
        Grao(q,p,3,true,SPRITE_PLANTA_SEMENTE,Evento(ciclo,10+onda*8+i));
    }
}
static void TampasDobradica(const EstadoDefesa *d,float t,QuadroPiloto *q)
{
    const ConfiguracaoPiloto *c=Cfg(d);Rectangle b=d->caixa;float rel=Tempo(d,t),local=fmodf(rel,c->ciclo);int ciclo=(int)(rel/c->ciclo);
    float a=local-c->aviso;
    for(int lado=0;lado<2;lado++){
        Vector2 alvo=Alvo(d,10);
        Vector2 eixo={Clamp(alvo.x+(lado?52:-52),b.x+3,b.x+b.width-3),Clamp(alvo.y+(lado?22:-22),b.y+3,b.y+b.height-3)};
        float u=Clamp((a-lado*c->intervalo)/.85f,0,1);
        float ang=(lado?PI:0)+(ciclo%2?-1:1)*(-.8f+u*2.1f);
        Vector2 ponta=Vector2Add(eixo,Vector2Scale(Dir(ang),b.height*.43f));
        Peca(q,eixo,ponta,5,a>=lado*c->intervalo&&a<lado*c->intervalo+.85f,SPRITE_PLANTA_TABUA,Evento(ciclo,lado));
        if(a<0)Grao(q,eixo,7,false,SPRITE_PLANTA_TABUA,Evento(ciclo,80+lado));
    }
}
static void FrascoAgitado(const EstadoDefesa *d,float t,QuadroPiloto *q)
{
    const ConfiguracaoPiloto *c=Cfg(d);Rectangle b=d->caixa;float rel=Tempo(d,t),local=fmodf(rel,c->ciclo);int ciclo=(int)(rel/c->ciclo);
    Vector2 origem=Pos(b,ciclo%2?.94f:.06f,.15f),alvo=Alvo(d,18);float a=local-c->aviso;
    if(a<0){Grao(q,origem,6,false,SPRITE_PLANTA_FRASCO,Evento(ciclo,0));return;}
    Vector2 chao={alvo.x,b.y+b.height-8};
    if(a<.50f){Vector2 p=Curva(origem,(Vector2){alvo.x,b.y-70},chao,a/.50f);Grao(q,p,6,true,SPRITE_PLANTA_FRASCO,Evento(ciclo,1));}
    else if(a<.82f){float u=(a-.50f)/.32f;Vector2 p=Vector2Lerp(chao,alvo,u);p.y-=sinf(u*PI)*36;Grao(q,p,6,true,SPRITE_PLANTA_FRASCO,Evento(ciclo,2));}
    else for(int i=0;i<c->quantidade;i++){
        float idade=a-.82f;Vector2 p=Vector2Add(alvo,Vector2Scale(Dir(PI*2*i/c->quantidade),idade*c->velocidade));p.y+=idade*idade*35;
        Grao(q,p,3.5f,true,SPRITE_PLANTA_GOTA,Evento(ciclo,10+i));
    }
}
/* Reconstrucao deterministica do encontro: a neutralizacao permanece nos quadros seguintes.
   Os dois fluxos tem X linear. So e preciso testar a pequena janela de cruzamento. */
static bool ReceitaJaNeutralizou(const ConfiguracaoPiloto *c,float largura,int onda,int outra,float rel)
{
    const float alcance=23;
    float nasce_roxo=onda*c->intervalo,nasce_azul=outra*c->intervalo;
    bool roxo_invertido=(onda/3)%2,azul_invertido=(outra/3)%2;
    float v_roxo=(roxo_invertido?1:-1)*c->velocidade;
    float v_azul=(azul_invertido?-1:1)*c->velocidade*.90f;
    float x_roxo=roxo_invertido?-12:largura+12,x_azul=azul_invertido?largura+12:-12;
    float vel_rel=v_roxo-v_azul;
    float cruzamento=-(x_roxo-v_roxo*nasce_roxo-x_azul+v_azul*nasce_azul)/vel_rel;
    float janela=alcance/fabsf(vel_rel);
    if(rel<cruzamento-janela||fmaxf(nasce_roxo,nasce_azul)>cruzamento+janela)return false;
    for(int amostra=0;amostra<=16;amostra++){
        float encontro=cruzamento-janela+2*janela*amostra/16;
        if(encontro>rel||encontro<nasce_roxo||encontro<nasce_azul)continue;
        float a=encontro-nasce_roxo,b=encontro-nasce_azul;
        float dx=x_roxo+a*v_roxo-x_azul-b*v_azul;
        float dy=(sinf(a*2.3f+onda*.8f)-sinf(b*2.3f+outra*.8f))*13;
        if(dx*dx+dy*dy<alcance*alcance)return true;
    }
    return false;
}
static void ReceitaReversa(const EstadoDefesa *d,float t,QuadroPiloto *q)
{
    const ConfiguracaoPiloto *c=Cfg(d);Rectangle b=d->caixa;float rel=fmaxf(0,Tempo(d,t)-c->aviso);
    for(int onda=0;onda<22;onda++){
        float idade=rel-onda*c->intervalo;
        if(idade<0)continue;
        bool invertido=(onda/3)%2;
        float x_roxo=invertido?b.x-12+idade*c->velocidade:b.x+b.width+12-idade*c->velocidade;
        bool neutralizado=false;
        if(x_roxo>b.x-12&&x_roxo<b.x+b.width+12)
            for(int outra=0;outra<22&&!neutralizado;outra++)
                neutralizado=ReceitaJaNeutralizou(c,b.width,onda,outra,rel);
        for(int linha=0;linha<3;linha++){
            float y=b.y+24+linha*(b.height-48)/2+sinf(idade*2.3f+onda*.8f)*13;
            Vector2 veneno={x_roxo,y};
            Vector2 antidoto={invertido?b.x+b.width+12-idade*(c->velocidade*.90f):b.x-12+idade*(c->velocidade*.90f),y};
            if(veneno.x>b.x-12&&veneno.x<b.x+b.width+12&&!neutralizado)
                Grao(q,veneno,6,t>=c->aviso,SPRITE_PLANTA_VENENO,Evento(onda,linha));
            if(antidoto.x>b.x-12&&antidoto.x<b.x+b.width+12)
                Grao(q,antidoto,4,false,SPRITE_PLANTA_ANTIDOTO,Evento(onda,20+linha));
        }
    }
}
static void AlmofarizTransbordante(const EstadoDefesa *d,float t,QuadroPiloto *q)
{
    const ConfiguracaoPiloto *c=Cfg(d);float rel=Tempo(d,t),local=fmodf(rel,c->ciclo);int ciclo=(int)(rel/c->ciclo);
    Vector2 centro=Alvo(d,36);float a=local-c->aviso;
    Grao(q,centro,10,false,SPRITE_PLANTA_ALMOFARIZ,Evento(ciclo,0));
    if(a<0)return;
    bool retorno=a>.85f;float raio=retorno?fmaxf(0,110-(a-.85f)*c->velocidade):12+a*115;
    for(int i=0;i<c->quantidade;i++){
        float ang=PI*2*i/c->quantidade+(retorno?-.35f:.35f)*a;
        Vector2 p=Vector2Add(centro,Vector2Scale(Dir(ang),raio));
        Grao(q,p,4,true,SPRITE_PLANTA_GOTA,Evento(ciclo,10+i));
    }
    if(retorno)q->corrente=Vector2Scale(Vector2Normalize(Vector2Subtract(centro,d->posicao_alma)),18);
}
static void GotasGemeas(const EstadoDefesa *d,float t,QuadroPiloto *q)
{
    const ConfiguracaoPiloto *c=Cfg(d);Rectangle b=d->caixa;float rel=Tempo(d,t),local=fmodf(rel,c->ciclo);int ciclo=(int)(rel/c->ciclo);
    Vector2 alvo=Alvo(d,22);float a=local-c->aviso;
    for(int lado=0;lado<2;lado++){
        Vector2 origem={lado?b.x+b.width+10:b.x-10,Clamp(alvo.y+(lado?25:-25),b.y+12,b.y+b.height-12)};
        if(a<.60f){Vector2 p=Vector2Lerp(origem,alvo,Clamp(a/.60f,0,1));Grao(q,p,8,a>=0,SPRITE_PLANTA_GOTA,Evento(ciclo,lado));}
    }
    if(a>=.60f)for(int i=0;i<4;i++){
        Vector2 p=Vector2Add(alvo,Vector2Scale(Dir(PI*.25f+i*PI*.5f+ciclo*.25f),(a-.60f)*c->velocidade));
        Grao(q,p,5,true,SPRITE_PLANTA_GOTA,Evento(ciclo,10+i));
    }
}
static void ArrastoMenta(const EstadoDefesa *d,float t,QuadroPiloto *q)
{
    const ConfiguracaoPiloto *c=Cfg(d);Rectangle b=d->caixa;float rel=Tempo(d,t),local=fmodf(rel,c->ciclo);int ciclo=(int)(rel/c->ciclo);
    Vector2 alvo=Alvo(d,12);float a=local-c->aviso;float distancia=48+26*sinf(fmaxf(0,a)*4);
    float ang=ciclo*.9f+fmaxf(0,a)*1.7f;Vector2 v=Vector2Scale(Dir(ang),distancia);
    Vector2 p=Vector2Add(alvo,v),z=Vector2Subtract(alvo,v);
    p.x=Clamp(p.x,b.x+8,b.x+b.width-8);p.y=Clamp(p.y,b.y+8,b.y+b.height-8);
    z.x=Clamp(z.x,b.x+8,b.x+b.width-8);z.y=Clamp(z.y,b.y+8,b.y+b.height-8);
    Grao(q,p,7,a>=0,SPRITE_PLANTA_GOTA,Evento(ciclo,0));Grao(q,z,7,a>=0,SPRITE_PLANTA_GOTA,Evento(ciclo,1));
    Peca(q,p,z,2,a>=.25f&&a<1.0f,SPRITE_PLANTA_FILAMENTO,Evento(ciclo,2));
    if(a>=0&&a<1.0f)q->corrente=Vector2Scale(Vector2Normalize(Vector2Subtract(alvo,d->posicao_alma)),22);
}
static void DivisaoCelular(const EstadoDefesa *d,float t,QuadroPiloto *q)
{
    const ConfiguracaoPiloto *c=Cfg(d);Rectangle b=d->caixa;float rel=Tempo(d,t),local=fmodf(rel,c->ciclo);int ciclo=(int)(rel/c->ciclo);
    Vector2 alvo=Alvo(d,28),origem=Pos(b,ciclo%2?.94f:.06f,.5f);float a=local-c->aviso;
    if(a<0){Grao(q,origem,18,false,SPRITE_PLANTA_GOTA,Evento(ciclo,0));return;}
    Vector2 centro=Vector2Lerp(origem,alvo,Clamp(a/.45f,0,1));float separar=Clamp((a-.45f)/.60f,0,1);
    Vector2 v=Vector2Scale(Dir(ciclo%2?PI*.5f:0),separar*75);
    Vector2 p=Vector2Add(centro,v),z=Vector2Subtract(centro,v);
    Grao(q,p,18-separar*7,true,SPRITE_PLANTA_GOTA,Evento(ciclo,1));Grao(q,z,18-separar*7,true,SPRITE_PLANTA_GOTA,Evento(ciclo,2));
    if(separar<.7f)Peca(q,p,z,6*(1-separar),true,SPRITE_PLANTA_FILAMENTO,Evento(ciclo,3));
}
static void SacudidaEspinhosa(const EstadoDefesa *d,float t,QuadroPiloto *q)
{
    const ConfiguracaoPiloto *c=Cfg(d);Rectangle b=d->caixa;float rel=Tempo(d,t),a=rel-c->aviso;
    for(int onda=0;onda<6;onda++){
        float idade=a-onda*c->intervalo;if(idade<0||idade>2.5f)continue;
        Vector2 origem=Pos(b,.22f+.56f*((onda*3)%5)/4.0f,.96f);
        for(int i=0;i<c->quantidade;i++){
            float ang=-PI*.5f+(i-(c->quantidade-1)*.5f)*.24f;
            Vector2 p=Vector2Add(origem,Vector2Scale(Dir(ang),idade*c->velocidade));p.y+=idade*idade*70;
            p.x=Clamp(p.x,b.x+16,b.x+b.width-16);
            p.y=idade>1.45f?b.y+16+(idade-1.45f)*210:fmaxf(b.y+16,p.y);
            Grao(q,p,3.6f,true,SPRITE_PLANTA_FARPA,Evento(onda,i));
        }
    }
}
static void TocasCadeia(const EstadoDefesa *d,float t,QuadroPiloto *q)
{
    const ConfiguracaoPiloto *c=Cfg(d);Rectangle b=d->caixa;float rel=Tempo(d,t),local=fmodf(rel,c->ciclo);int ciclo=(int)(rel/c->ciclo);
    Vector2 alvo=Alvo(d,16);Vector2 origem=Pos(b,ciclo%2?.88f:.12f,ciclo%3?.82f:.18f);
    float a=local-c->aviso;
    if(a<0){Grao(q,origem,10,false,SPRITE_PLANTA_TOCA,Evento(ciclo,0));return;}
    Vector2 p=Vector2Lerp(origem,alvo,Clamp(a/.65f,0,1));
    Grao(q,p,9,true,SPRITE_PLANTA_LEBRE,Evento(ciclo,1));
    if(a>.65f)for(int i=0;i<4;i++){
        Vector2 v=Vector2Scale(Dir(PI*.25f+i*PI*.5f),(a-.65f)*c->velocidade);
        Grao(q,Vector2Add(alvo,v),3.5f,true,SPRITE_PLANTA_FARPA,Evento(ciclo,10+i));
    }
}
static void DisparadaRoseira(const EstadoDefesa *d,float t,QuadroPiloto *q)
{
    const ConfiguracaoPiloto *c=Cfg(d);Rectangle b=d->caixa;float rel=Tempo(d,t),local=fmodf(rel,c->ciclo);int ciclo=(int)(rel/c->ciclo);
    float a=local-c->aviso;Vector2 origem=Pos(b,ciclo%2?.96f:.04f,.91f);
    if(a<0){Grao(q,origem,10,false,SPRITE_PLANTA_LEBRE,Evento(ciclo,0));return;}
    float u=Clamp(a/.85f,0,1);Vector2 fim=Pos(b,ciclo%2?.04f:.96f,.91f);
    Vector2 p=Vector2Lerp(origem,fim,u);p.y-=sinf(u*PI)*(b.height-36);
    if(a<.85f)Grao(q,p,10,true,SPRITE_PLANTA_LEBRE,Evento(ciclo,1));
    else for(int i=0;i<c->quantidade;i++){
        float ang=atan2f(d->ancora_movimento.y-fim.y,d->ancora_movimento.x-fim.x)+(i-(c->quantidade-1)*.5f)*.18f;
        Grao(q,Vector2Add(fim,Vector2Scale(Dir(ang),(a-.85f)*c->velocidade)),3.7f,true,SPRITE_PLANTA_FARPA,Evento(ciclo,10+i));
    }
}
static void CeifaGerminante(const EstadoDefesa *d,float t,QuadroPiloto *q)
{
    const ConfiguracaoPiloto *c=Cfg(d);float rel=Tempo(d,t),local=fmodf(rel,c->ciclo);int ciclo=(int)(rel/c->ciclo);
    Vector2 centro=Alvo(d,48);float a=local-c->aviso;
    Grao(q,centro,9,a>=0,SPRITE_PLANTA_ALTAR,Evento(ciclo,0));
    if(a<0)return;
    float ang=(ciclo%2?-1:1)*a*5.5f;
    Vector2 ponta=Vector2Add(centro,Vector2Scale(Dir(ang),60));
    Peca(q,Vector2Add(centro,Vector2Scale(Dir(ang),15)),ponta,2.5f,true,SPRITE_PLANTA_FOICE,Evento(ciclo,1));
    Peca(q,ponta,Vector2Add(ponta,Vector2Scale(Dir(ang+PI*.5f),24)),4,true,SPRITE_PLANTA_FOICE,Evento(ciclo,2));
    for(int i=0;i<c->quantidade;i++){
        float idade=a-i*c->intervalo;if(idade<0)continue;
        float theta=(ciclo%2?-1:1)*i*c->intervalo*5.5f;
        Vector2 p=Vector2Add(centro,Vector2Scale(Dir(theta),60));
        p=Vector2Add(p,Vector2Scale(Dir(theta+(ciclo%2?-1:1)*PI*.5f),idade*c->velocidade));
        Grao(q,p,3.5f,true,SPRITE_PLANTA_SEMENTE,Evento(ciclo,10+i));
    }
}
static void RaizesProcura(const EstadoDefesa *d,float t,QuadroPiloto *q)
{
    const ConfiguracaoPiloto *c=Cfg(d);Rectangle b=d->caixa;float rel=Tempo(d,t),local=fmodf(rel,c->ciclo);int ciclo=(int)(rel/c->ciclo);
    Vector2 alvo=Alvo(d,18);float a=local-c->aviso;
    for(int raiz=0;raiz<3;raiz++){
        Vector2 origem=raiz==0?Pos(b,.5f,1):Pos(b,raiz==1?0:1,.25f+.5f*(ciclo%2));
        float idade=a-raiz*c->intervalo;
        if(idade<0){Grao(q,origem,7,false,SPRITE_PLANTA_RAIZ,Evento(ciclo,raiz));continue;}
        Vector2 controle=Vector2Lerp(origem,alvo,.5f);controle.y+=(raiz%2?-24:24);
        float u=Clamp(idade/.65f,0,1);Vector2 ant=origem;
        for(int i=1;i<=6;i++){
            Vector2 pt=Curva(origem,controle,alvo,u*i/6.0f);
            Peca(q,ant,pt,3,true,SPRITE_PLANTA_RAIZ,Evento(ciclo,10+raiz));ant=pt;
        }
        if(idade>.65f)for(int lado=0;lado<2;lado++){
            float ang=atan2f(alvo.y-origem.y,alvo.x-origem.x)+(lado?1:-1)*.85f;
            Vector2 ponta=Vector2Add(alvo,Vector2Scale(Dir(ang),Clamp((idade-.65f)*c->velocidade,0,58)));
            Peca(q,alvo,ponta,2.8f,true,SPRITE_PLANTA_RAIZ,Evento(ciclo,20+raiz*2+lado));
        }
    }
}
static void RitualColheita(const EstadoDefesa *d,float t,QuadroPiloto *q)
{
    const ConfiguracaoPiloto *c=Cfg(d);float rel=Tempo(d,t),local=fmodf(rel,c->ciclo);int ciclo=(int)(rel/c->ciclo);
    Vector2 centro=Alvo(d,52);float a=local-c->aviso;
    Grao(q,centro,11,a>=0,SPRITE_PLANTA_ALTAR,Evento(ciclo,0));if(a<0)return;
    float raio=a<.85f?24+a*110:fmaxf(10,118-(a-.85f)*c->velocidade);
    for(int i=0;i<c->quantidade;i++){
        float ang=PI*2*i/c->quantidade+a*(a<.85f?1.1f:-1.6f);
        Grao(q,Vector2Add(centro,Vector2Scale(Dir(ang),raio)),4.2f,true,SPRITE_PLANTA_SEMENTE,Evento(ciclo,10+i));
    }
    for(int i=0;i<3;i++){
        float ang=PI*2*i/3-a*1.8f+ciclo*.4f;
        Vector2 p=Vector2Add(centro,Vector2Scale(Dir(ang),34)),z=Vector2Add(centro,Vector2Scale(Dir(ang+.27f),85));
        Peca(q,p,z,3.2f,true,SPRITE_PLANTA_RAIZ,Evento(ciclo,30+i));
    }
}
void PlantaQuadro(const EstadoDefesa *d,float t,QuadroPiloto *q)
{
    switch(d->id_padrao){
        case PADRAO_PLANTA_ESTILHACO_LANCA:EstilhacoLanca(d,t,q);break;
        case PADRAO_PLANTA_ESCUDO_ENRAIZADO:EscudoEnraizado(d,t,q);break;
        case PADRAO_PLANTA_TAMPAS_DOBRADICA:TampasDobradica(d,t,q);break;
        case PADRAO_PLANTA_FRASCO_AGITADO:FrascoAgitado(d,t,q);break;
        case PADRAO_PLANTA_RECEITA_REVERSA:ReceitaReversa(d,t,q);break;
        case PADRAO_PLANTA_ALMOFARIZ:AlmofarizTransbordante(d,t,q);break;
        case PADRAO_PLANTA_GOTAS_GEMEAS:GotasGemeas(d,t,q);break;
        case PADRAO_PLANTA_ARRASTO_MENTA:ArrastoMenta(d,t,q);break;
        case PADRAO_PLANTA_DIVISAO_CELULAR:DivisaoCelular(d,t,q);break;
        case PADRAO_PLANTA_SACUDIDA:SacudidaEspinhosa(d,t,q);break;
        case PADRAO_PLANTA_TOCAS_CADEIA:TocasCadeia(d,t,q);break;
        case PADRAO_PLANTA_DISPARADA:DisparadaRoseira(d,t,q);break;
        case PADRAO_PLANTA_CEIFA:CeifaGerminante(d,t,q);break;
        case PADRAO_PLANTA_RAIZES_PROCURA:RaizesProcura(d,t,q);break;
        case PADRAO_PLANTA_RITUAL:RitualColheita(d,t,q);break;
        default:break;
    }
}
