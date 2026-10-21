#include "water_patterns.h"
#include "balance.h"
#include "raymath.h"
#include <math.h>

static const ConfiguracaoPiloto *Cfg(const EstadoDefesa *d){return &BALANCEAMENTO_PILOTO[d->id_padrao-PADRAO_PILOTO_POEIRA_ASA];}
static float Tempo(const EstadoDefesa *d,float t){return t*(d->fator_movimento_ataque>0?d->fator_movimento_ataque:1);}
static Vector2 Pos(Rectangle b,float x,float y){return (Vector2){b.x+x*b.width,b.y+y*b.height};}
static Vector2 Dir(float a){return (Vector2){cosf(a),sinf(a)};}
static Vector2 Alvo(const EstadoDefesa *d,float m){Rectangle b=d->caixa;return (Vector2){Clamp(d->ancora_movimento.x,b.x+m,b.x+b.width-m),Clamp(d->ancora_movimento.y,b.y+m,b.y+b.height-m)};}
static unsigned long Evento(int c,int n){return 50000+(unsigned long)c*100+n;}
static AmeacaPiloto *Nova(QuadroPiloto *q,bool ativa,IdSprite s,unsigned long e)
{if(q->total>=MAXIMO_AMEACAS_PILOTO)return NULL;AmeacaPiloto *a=&q->ameacas[q->total++];*a=(AmeacaPiloto){.ativo=ativa,.sprite=s,.evento=e};return a;}
static void Disco(QuadroPiloto *q,Vector2 p,float r,bool ativa,IdSprite s,unsigned long e)
{AmeacaPiloto *a=Nova(q,ativa,s,e);if(a){a->a=a->b=p;a->raio=r;}}
static void Barra(QuadroPiloto *q,Vector2 p,Vector2 z,float r,bool ativa,IdSprite s,unsigned long e)
{AmeacaPiloto *a=Nova(q,ativa,s,e);if(a){a->forma=PILOTO_CAPSULA;a->a=p;a->b=z;a->raio=r;a->rotacao=atan2f(z.y-p.y,z.x-p.x)*RAD2DEG;}}
static void Area(QuadroPiloto *q,Rectangle r,bool ativa,IdSprite s,unsigned long e)
{if(r.width<=0||r.height<=0)return;AmeacaPiloto *a=Nova(q,ativa,s,e);if(a){a->forma=PILOTO_RETANGULO;a->area=r;a->a=(Vector2){r.x,r.y};a->b=(Vector2){r.x+r.width,r.y+r.height};}}
static void Tri(QuadroPiloto *q,Vector2 a,Vector2 b,Vector2 c,bool ativa,IdSprite s,unsigned long e)
{AmeacaPiloto *p=Nova(q,ativa,s,e);if(p){p->forma=PILOTO_TRIANGULO;p->a=a;p->b=b;p->c=c;}}
bool AguaEh(IdPadraoProjetil id){return id>=PADRAO_AGUA_TSUNAMI&&id<=PADRAO_AGUA_MEMBRANA;}

/* A parede de agua e recortada pela sombra DIRECIONAL do quebra-mar. */
static void TsunamiFalange(const EstadoDefesa *d,float t,QuadroPiloto *q)
{
    Rectangle b=d->caixa;const ConfiguracaoPiloto *c=Cfg(d);float rel=Tempo(d,t),local=fmodf(rel,c->ciclo);int ciclo=(int)(rel/c->ciclo);
    bool subir=ciclo%2;float meio=b.x+b.width*.5f;
    float sentido=d->ancora_movimento.x<meio?1:d->ancora_movimento.x>meio?-1:(ciclo%2?-1:1);
    float x=Clamp(d->ancora_movimento.x+sentido*60,b.x+45,b.x+b.width-45),y=b.y+b.height*.5f;
    Barra(q,(Vector2){x-42,y},(Vector2){x+42,y},4,false,SPRITE_AGUA_ESCUDO,Evento(ciclo,0));q->ameacas[q->total-1].regra=5;
    float a=local-c->aviso;if(a<0)return;
    float frente=subir?b.y+b.height+30-a*c->velocidade:b.y-30+a*c->velocidade;
    Rectangle onda={b.x,frente-15,b.width,30};
    float protegido=subir?fminf(onda.y+onda.height,y):fmaxf(onda.y,y);
    if(subir){
        float inicio=fmaxf(onda.y,y);
        Area(q,(Rectangle){b.x,inicio,b.width,fmaxf(0,onda.y+30-inicio)},true,SPRITE_AGUA_ONDA,Evento(ciclo,1));
        Area(q,(Rectangle){b.x,onda.y,x-42-b.x,fmaxf(0,protegido-onda.y)},true,SPRITE_AGUA_ONDA,Evento(ciclo,2));
        Area(q,(Rectangle){x+42,onda.y,b.x+b.width-x-42,fmaxf(0,protegido-onda.y)},true,SPRITE_AGUA_ONDA,Evento(ciclo,3));
    }else{
        Area(q,(Rectangle){b.x,onda.y,b.width,fmaxf(0,fminf(y,onda.y+30)-onda.y)},true,SPRITE_AGUA_ONDA,Evento(ciclo,1));
        Area(q,(Rectangle){b.x,protegido,x-42-b.x,onda.y+30-protegido},true,SPRITE_AGUA_ONDA,Evento(ciclo,2));
        Area(q,(Rectangle){x+42,protegido,b.x+b.width-x-42,onda.y+30-protegido},true,SPRITE_AGUA_ONDA,Evento(ciclo,3));
    }
}
static void JatoQuadrante(const EstadoDefesa *d,float t,QuadroPiloto *q)
{
    Rectangle b=d->caixa;const ConfiguracaoPiloto *c=Cfg(d);float rel=Tempo(d,t),local=fmodf(rel,c->ciclo);int ciclo=(int)(rel/c->ciclo);
    Vector2 origem=Pos(b,.5f+.15f*sinf(ciclo*1.7f),.5f);Vector2 alvo=Alvo(d,10);
    Disco(q,origem,10,local>=c->aviso,SPRITE_AGUA_ESCUDO,Evento(ciclo,0));
    if(local<c->aviso)return;
    float a=local-c->aviso,ang=atan2f(alvo.y-origem.y,alvo.x-origem.x)-.70f+a*1.15f;
    float alcance=b.width+b.height;
    Tri(q,origem,Vector2Add(origem,Vector2Scale(Dir(ang-.26f),alcance)),Vector2Add(origem,Vector2Scale(Dir(ang+.26f),alcance)),true,SPRITE_AGUA_JATO,Evento(ciclo,1));
}
static Rectangle Conves(Rectangle b,float rel,Vector2 *vel)
{
    float a=fmaxf(0,rel-.8f);Vector2 centro=Pos(b,.5f,.5f);
    centro.x+=sinf(a*.9f)*64;centro.y+=sinf(a*1.4f)*21;
    if(vel)*vel=(Vector2){rel>.8f?cosf(a*.9f)*57.6f:0,rel>.8f?cosf(a*1.4f)*29.4f:0};
    return (Rectangle){centro.x-80,centro.y-53,160,106};
}
static void ConvesDeriva(const EstadoDefesa *d,float t,QuadroPiloto *q)
{
    Rectangle b=d->caixa;const ConfiguracaoPiloto *c=Cfg(d);float rel=Tempo(d,t),local=fmodf(rel,c->ciclo);int ciclo=(int)(rel/c->ciclo);
    Vector2 vel;Rectangle ilha=Conves(b,rel,&vel);bool ativa=rel>=c->aviso;
    Area(q,(Rectangle){b.x,b.y,b.width,ilha.y-b.y},ativa,SPRITE_AGUA_ONDA,Evento(ciclo,1));
    Area(q,(Rectangle){b.x,ilha.y+ilha.height,b.width,b.y+b.height-ilha.y-ilha.height},ativa,SPRITE_AGUA_ONDA,Evento(ciclo,2));
    Area(q,(Rectangle){b.x,ilha.y,ilha.x-b.x,ilha.height},ativa,SPRITE_AGUA_ONDA,Evento(ciclo,3));
    Area(q,(Rectangle){ilha.x+ilha.width,ilha.y,b.x+b.width-ilha.x-ilha.width,ilha.height},ativa,SPRITE_AGUA_ONDA,Evento(ciclo,4));
    Area(q,ilha,false,SPRITE_AGUA_DIQUE,Evento(ciclo,0));q->ameacas[q->total-1].regra=6;
    if(CheckCollisionPointRec(d->posicao_alma,ilha))q->corrente=vel;
    Rectangle antes=Conves(b,ciclo*c->ciclo,NULL);
    Vector2 alvo={ilha.x+80+Clamp(d->ancora_movimento.x-antes.x-80,-57,57),ilha.y+53+Clamp(d->ancora_movimento.y-antes.y-53,-31,31)};
    float aviso_jato=c->aviso+BALANCEAMENTO_CONVES_PREPARO;
    if(local<aviso_jato+.65f)Disco(q,alvo,19,local>=aviso_jato,SPRITE_AGUA_JATO,Evento(ciclo,10));
}
void AguaRastroAtualizar(EstadoDefesa *d)
{
    if(d->id_padrao!=PADRAO_AGUA_PEGADAS)return;
    float t=Tempo(d,d->temporizador_padrao);EstadoMovimentoAgua *s=&d->agua;
    if(s->total==0||t+.00001f>=s->proxima){
        if(s->total==0)s->origem_rastro=d->posicao_alma;
        int i=s->proximo;s->posicoes[i]=d->posicao_alma;s->tempos[i]=t;s->ids[i]=++s->sequencia;
        s->proximo=(i+1)%12;if(s->total<12)s->total++;s->proxima=t+BALANCEAMENTO_LODO_AMOSTRA;
    }
}
static void PegadasBrejo(const EstadoDefesa *d,float t,QuadroPiloto *q)
{
    float rel=Tempo(d,t);
    for(int i=0;i<d->agua.total;i++){
        float a=rel-d->agua.tempos[i];if(a<0||a>1.85f)continue;
        Disco(q,d->agua.posicoes[i],11,a>=.72f,SPRITE_AGUA_LAMA,80000+d->agua.ids[i]);
    }
}
static void BancosViscosos(const EstadoDefesa *d,float t,QuadroPiloto *q)
{
    Rectangle b=d->caixa;const ConfiguracaoPiloto *c=Cfg(d);float rel=Tempo(d,t),local=fmodf(rel,c->ciclo);int ciclo=(int)(rel/c->ciclo);
    for(int i=0;i<3;i++){
        Rectangle lama={b.x+22+i*(b.width-78)/3,b.y+(i%2?b.height*.40f:0),54,b.height*.60f};
        Area(q,lama,false,SPRITE_AGUA_LAMA,Evento(ciclo,i));q->ameacas[q->total-1].regra=4;
    }
    float a=local-c->aviso;Vector2 alvo=Alvo(d,14);
    if(a<0){Disco(q,(Vector2){alvo.x,b.y+3},6,false,SPRITE_AGUA_JATO,Evento(ciclo,10));return;}
    float y=b.y-36+a*c->velocidade;
    bool direita=alvo.x>b.x+b.width*.5f;
    float origem=direita?b.x+b.width:b.x;
    float limite=Clamp(alvo.x+(direita?-42:42),b.x+48,b.x+b.width-48);
    Tri(q,(Vector2){origem,y-22},(Vector2){limite,y+32},(Vector2){limite,y+56},true,SPRITE_AGUA_JATO,Evento(ciclo,11));
    Tri(q,(Vector2){origem,y-22},(Vector2){limite,y+56},(Vector2){origem,y+2},true,SPRITE_AGUA_JATO,Evento(ciclo,11));
}
static void BolhaInversa(const EstadoDefesa *d,float t,QuadroPiloto *q)
{
    Rectangle b=d->caixa;const ConfiguracaoPiloto *c=Cfg(d);float rel=Tempo(d,t),local=fmodf(rel,c->ciclo);int ciclo=(int)(rel/c->ciclo);
    Vector2 centro=Alvo(d,52);centro.x=Clamp(centro.x+(ciclo%2?-36:36),b.x+48,b.x+b.width-48);
    if(local<c->aviso+.55f){
        AmeacaPiloto *p=Nova(q,local>=c->aviso,SPRITE_AGUA_BOLHA,Evento(ciclo,0));
        if(p){p->forma=PILOTO_FORA_CIRCULO;p->a=p->b=centro;p->raio=43;}
    }else{
        float a=local-c->aviso-.55f;
        if(a<1.15f)Disco(q,centro,a<.45f?46:fmaxf(12,46-(a-.45f)*36),a>=.45f,SPRITE_AGUA_BOLHA,Evento(ciclo,1));
    }
}
static void AvalancheObliqua(const EstadoDefesa *d,float t,QuadroPiloto *q)
{
    Rectangle b=d->caixa;const ConfiguracaoPiloto *c=Cfg(d);float rel=Tempo(d,t),local=fmodf(rel,c->ciclo);int ciclo=(int)(rel/c->ciclo);
    Vector2 alvo=Alvo(d,15);
    for(int onda=0;onda<2;onda++){
        float a=local-c->aviso-onda*c->intervalo;
        float x=Clamp(alvo.x+(onda?40:-40),b.x+26,b.x+b.width-26);
        if(a<0){Disco(q,(Vector2){x,b.y+5},7,false,SPRITE_AGUA_NEVE,Evento(ciclo,onda));continue;}
        float y=b.y-45+a*c->velocidade;x+=(ciclo%2?-1:1)*a*25;
        Tri(q,(Vector2){x,y+65},(Vector2){x-64,y-48},(Vector2){x+64,y-48},true,SPRITE_AGUA_NEVE,Evento(ciclo,onda));
    }
}
static void EstacasGeada(const EstadoDefesa *d,float t,QuadroPiloto *q)
{
    Rectangle b=d->caixa;const ConfiguracaoPiloto *c=Cfg(d);float rel=Tempo(d,t),local=fmodf(rel,c->ciclo);int ciclo=(int)(rel/c->ciclo);
    Vector2 alvo=Alvo(d,12);
    for(int i=0;i<c->quantidade;i++){
        float x=Clamp(alvo.x+(i-(c->quantidade-1)*.5f)*34,b.x+8,b.x+b.width-8);
        float a=local-c->aviso-i*c->intervalo;
        if(a<0){Disco(q,(Vector2){x,b.y+5},4,false,SPRITE_AGUA_ESTACA,Evento(ciclo,i));continue;}
        if(a>1.26f)continue;
        float y=fminf(b.y+b.height-4,b.y-30+a*a*370);
        Barra(q,(Vector2){x,y-30},(Vector2){x,y},4,true,SPRITE_AGUA_ESTACA,Evento(ciclo,i));
    }
}
static void PrisaoDeslizante(const EstadoDefesa *d,float t,QuadroPiloto *q)
{
    Rectangle b=d->caixa;const ConfiguracaoPiloto *c=Cfg(d);float rel=Tempo(d,t);q->escorregadio=true;
    for(int onda=0;onda<8;onda++){
        float a=rel-c->aviso-onda*c->intervalo;if(a<0||a>3.4f)continue;
        bool lado=onda%2;float x=lado?b.x-100+a*c->velocidade:b.x+b.width+10-a*c->velocidade;
        float y=b.y+b.height*(lado?.66f:.34f)+sinf(a*2.0f+onda)*31-25;
        Area(q,(Rectangle){x,y,88,50},true,SPRITE_AGUA_FISSURA,Evento(onda,0));
    }
}
static void JatoRefratado(const EstadoDefesa *d,float t,QuadroPiloto *q)
{
    Rectangle b=d->caixa;const ConfiguracaoPiloto *c=Cfg(d);float rel=Tempo(d,t),local=fmodf(rel,c->ciclo);int ciclo=(int)(rel/c->ciclo);
    float linha=b.y+b.height*.48f;Vector2 alvo=Alvo(d,10),origem={Clamp(alvo.x+(ciclo%2?24:-24),b.x+10,b.x+b.width-10),b.y+b.height*.02f};
    Disco(q,origem,9,false,SPRITE_AGUA_PRISMA,Evento(ciclo,0));q->ameacas[q->total-1].regra=5;
    if(local<c->aviso)return;
    float a=local-c->aviso;float x=Clamp(alvo.x+sinf(a*2.4f)*42,b.x+16,b.x+b.width-16);
    Vector2 ponto={x,linha};Vector2 dir=Vector2Normalize(Vector2Subtract(ponto,origem));Vector2 normal={-dir.y,dir.x};
    Tri(q,origem,Vector2Add(ponto,Vector2Scale(normal,10)),Vector2Subtract(ponto,Vector2Scale(normal,10)),true,SPRITE_AGUA_JATO,Evento(ciclo,1));
    float refracao=Clamp((origem.x-x)*.65f,-140,140);
    Vector2 fim={x+refracao,b.y+b.height+20};
    Tri(q,Vector2Add(ponto,(Vector2){-10,0}),Vector2Add(ponto,(Vector2){10,0}),Vector2Add(fim,(Vector2){24,0}),true,SPRITE_AGUA_JATO,Evento(ciclo,2));
    Tri(q,Vector2Add(ponto,(Vector2){-10,0}),Vector2Add(fim,(Vector2){24,0}),Vector2Add(fim,(Vector2){-24,0}),true,SPRITE_AGUA_JATO,Evento(ciclo,2));
}
static void ReflexoLago(const EstadoDefesa *d,float t,QuadroPiloto *q)
{
    Rectangle b=d->caixa;const ConfiguracaoPiloto *c=Cfg(d);float rel=Tempo(d,t),local=fmodf(rel,c->ciclo);int ciclo=(int)(rel/c->ciclo);
    q->reflete_alma=true;q->caixa_reflexo=b;Vector2 alvo=Alvo(d,18);
    for(int i=0;i<2;i++){
        float a=local-c->aviso-i*c->intervalo;
        Vector2 p={Clamp(alvo.x+(i?42:-42),b.x+24,b.x+b.width-24),Clamp(alvo.y+(i?-25:25),b.y+24,b.y+b.height-24)};
        float raio=a<0?26:Clamp(26+a*45,26,58);
        if(a<.95f)Disco(q,p,raio,a>=0,SPRITE_AGUA_ONDA,Evento(ciclo,i));
    }
}
static void MembranaImpulso(const EstadoDefesa *d,float t,QuadroPiloto *q)
{
    Rectangle b=d->caixa;const ConfiguracaoPiloto *c=Cfg(d);float rel=Tempo(d,t);
    for(int onda=0;onda<6;onda++){
        float a=rel-c->aviso-onda*c->intervalo;if(a<0)continue;
        bool horizontal=onda%2;float deslocamento=a*c->velocidade;
        Rectangle r=horizontal?(Rectangle){b.x,b.y-16+deslocamento,b.width,10}:(Rectangle){b.x-16+deslocamento,b.y,10,b.height};
        if((horizontal&&r.y>b.y+b.height+12)||(!horizontal&&r.x>b.x+b.width+12))continue;
        Area(q,r,true,SPRITE_AGUA_MEMBRANA,Evento(onda,0));q->ameacas[q->total-1].regra=3;
    }
}
void AguaQuadro(const EstadoDefesa *d,float t,QuadroPiloto *q)
{
    switch(d->id_padrao){
        case PADRAO_AGUA_TSUNAMI:TsunamiFalange(d,t,q);break;
        case PADRAO_AGUA_JATO_QUADRANTE:JatoQuadrante(d,t,q);break;
        case PADRAO_AGUA_CONVES:ConvesDeriva(d,t,q);break;
        case PADRAO_AGUA_PEGADAS:PegadasBrejo(d,t,q);break;
        case PADRAO_AGUA_VISCOSIDADE:BancosViscosos(d,t,q);break;
        case PADRAO_AGUA_BOLHA:BolhaInversa(d,t,q);break;
        case PADRAO_AGUA_AVALANCHE:AvalancheObliqua(d,t,q);break;
        case PADRAO_AGUA_ESTACAS:EstacasGeada(d,t,q);break;
        case PADRAO_AGUA_PRISAO:PrisaoDeslizante(d,t,q);break;
        case PADRAO_AGUA_REFRACAO:JatoRefratado(d,t,q);break;
        case PADRAO_AGUA_REFLEXO:ReflexoLago(d,t,q);break;
        case PADRAO_AGUA_MEMBRANA:MembranaImpulso(d,t,q);break;
        default:break;
    }
}
void AguaMover(EstadoDefesa *d,const QuadroPiloto *q,Vector2 mov,float velocidade,float dt,bool dash)
{
    EstadoMovimentoAgua *s=&d->agua;s->gelo=fmaxf(0,s->gelo-dt);if(dash)s->gelo=0;
    float fator=1;
    for(int i=0;i<q->total;i++)if(q->ameacas[i].regra==4&&CheckCollisionPointRec(d->posicao_alma,q->ameacas[i].area))fator=BALANCEAMENTO_LODO_VELOCIDADE;
    Vector2 desejada=Vector2Scale(mov,velocidade*fator);
    if(s->gelo>0){s->velocidade=(Vector2){0};desejada=(Vector2){0};}
    else if(q->escorregadio){
        Vector2 delta=Vector2Subtract(desejada,s->velocidade);float n=Vector2Length(delta);
        float acel=Vector2Length(mov)>.01f?BALANCEAMENTO_GELO_ACELERACAO:BALANCEAMENTO_GELO_FREIO;
        if(n>acel*dt)delta=Vector2Scale(delta,acel*dt/n);
        s->velocidade=Vector2Add(s->velocidade,delta);
    }else s->velocidade=desejada;
    d->posicao_alma=Vector2Add(d->posicao_alma,Vector2Scale(Vector2Add(s->velocidade,q->corrente),dt));
}
void AguaCongelarNoAcerto(EstadoDefesa *d)
{
    if(d->id_padrao>=PADRAO_AGUA_AVALANCHE&&d->id_padrao<=PADRAO_AGUA_PRISAO){d->agua.gelo=BALANCEAMENTO_GELO_CONTATO;d->agua.velocidade=(Vector2){0};}
}
static float Cruz(Vector2 a,Vector2 b,Vector2 c){return (b.x-a.x)*(c.y-a.y)-(b.y-a.y)*(c.x-a.x);}
static bool DentroTri(Vector2 p,const AmeacaPiloto *a)
{float x=Cruz(a->a,a->b,p),y=Cruz(a->b,a->c,p),z=Cruz(a->c,a->a,p);return !((x<0||y<0||z<0)&&(x>0||y>0||z>0));}
float PilotoDistanciaAmeaca(const AmeacaPiloto *p,Vector2 pos)
{
    if(p->forma==PILOTO_FORA_CIRCULO)return p->raio-Vector2Distance(pos,p->a);
    if(p->forma==PILOTO_TRIANGULO){float d=fminf(DistanciaPontoSegmento(pos,p->a,p->b),fminf(DistanciaPontoSegmento(pos,p->b,p->c),DistanciaPontoSegmento(pos,p->c,p->a)));return DentroTri(pos,p)?-d:d;}
    if(p->forma==PILOTO_RETANGULO){Rectangle r=p->area;float dx=fmaxf(fmaxf(r.x-pos.x,0),pos.x-r.x-r.width),dy=fmaxf(fmaxf(r.y-pos.y,0),pos.y-r.y-r.height);return CheckCollisionPointRec(pos,r)?-fminf(fminf(pos.x-r.x,r.x+r.width-pos.x),fminf(pos.y-r.y,r.y+r.height-pos.y)):sqrtf(dx*dx+dy*dy);}
    return DistanciaPontoSegmento(pos,p->a,p->b)-p->raio;
}
