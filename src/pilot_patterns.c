#include "pilot_patterns.h"
#include "plant_patterns.h"
#include "water_patterns.h"
#include "balance.h"
#include "attack_warning.h"
#include "ui.h"
#include "raymath.h"
#include <math.h>
#include <string.h>


static const IdInimigo PILOTO_INIMIGOS[]={INIMIGO_POEIRA_MARIPOSA,INIMIGO_LADRAO_MORTALHA,INIMIGO_CHEFE_AFOGADO_CORAL,INIMIGO_SOLDADO_SARCAS,INIMIGO_HERBALISTA_PESTE,INIMIGO_MENTA_LIMO,INIMIGO_ESPINHO_LEBRE,INIMIGO_CHEFE_CULTISTA_RAIZES,INIMIGO_LEGIONARIO_AFOGADO,INIMIGO_BRUXA_LODO,INIMIGO_REVENANTE_GELO,INIMIGO_BRILHO_SAPO};
static const IdPadraoProjetil PILOTO_BASES[]={PADRAO_PILOTO_POEIRA_ASA,PADRAO_PILOTO_ADAGA_RETORNO,PADRAO_PILOTO_ANCORA_CORAL,PADRAO_PLANTA_ESTILHACO_LANCA,PADRAO_PLANTA_FRASCO_AGITADO,PADRAO_PLANTA_GOTAS_GEMEAS,PADRAO_PLANTA_SACUDIDA,PADRAO_PLANTA_CEIFA,PADRAO_AGUA_TSUNAMI,PADRAO_AGUA_PEGADAS,PADRAO_AGUA_AVALANCHE,PADRAO_AGUA_REFRACAO};
int PilotoTotalInimigos(void){return (int)(sizeof(PILOTO_INIMIGOS)/sizeof(PILOTO_INIMIGOS[0]));}
IdInimigo PilotoInimigoId(int i){return PILOTO_INIMIGOS[(i%PilotoTotalInimigos()+PilotoTotalInimigos())%PilotoTotalInimigos()];}
IdPadraoProjetil PilotoBase(int i){return PILOTO_BASES[(i%PilotoTotalInimigos()+PilotoTotalInimigos())%PilotoTotalInimigos()];}
bool PilotoBoss(IdPadraoProjetil id){return (id>=PADRAO_PILOTO_ANCORA_CORAL&&id<=PADRAO_PILOTO_RECIFE_RUPTURA)||(id>=PADRAO_PLANTA_CEIFA&&id<=PADRAO_PLANTA_RITUAL);}

static const ConfiguracaoPiloto *Config(const EstadoDefesa *d)
{ return &BALANCEAMENTO_PILOTO[d->id_padrao-PADRAO_PILOTO_POEIRA_ASA]; }
bool PilotoEh(IdPadraoProjetil id)
{ return id >= PADRAO_PILOTO_POEIRA_ASA && id < PADRAO_TOTAL; }
const char *PilotoNome(IdPadraoProjetil id)
{ return PilotoEh(id) ? BALANCEAMENTO_PILOTO[id-PADRAO_PILOTO_POEIRA_ASA].nome : ""; }
const char *PilotoInstrucao(IdPadraoProjetil id)
{
    static const char *const instrucoes[]={
        "Leia o bater das asas e passe entre os graos.",
        "AZUL: pare. LARANJA: mova. O pulso anuncia a troca.",
        "Cruze depois da asa e antes da cauda de poeira.",
        "Desvie da ida e reposicione para a volta da adaga.",
        "O espelho inverte a ORIGEM; a mira trava antes do tiro.",
        "Passe pelo lado sem fio antes de recolher as adagas.",
        "Duas ancoras: desvie do cabo e depois do leque de coral.",
        "Ziguezague entre os corais e os fragmentos que caem.",
        "Rompa o recife e atravesse entre as duas erupcoes.",
        "Desvie da ponta e mude de eixo antes das farpas.",
        "Atraia o disparo do escudo e contorne a raiz.",
        "Contorne a dobradica enquanto a outra tampa recolhe.",
        "Leia o quique; desvie das gotas depois da quebra.",
        "ROXO fere. AZUL neutraliza as gotas no encontro.",
        "O almofariz expele e depois puxa: inverta seu desvio.",
        "Desvie das gotas que se unem e depois se fragmentam.",
        "Cruze o fio frouxo; saia antes de ele esticar.",
        "Passe pelo pescoco da massa quando ela se separar.",
        "Desvie dos espinhos na subida e na queda.",
        "Siga atras da lebre; nao encontre a cabeca da trilha.",
        "Passe sob o salto e saia das farpas do pouso.",
        "Contorne o cabo da foice e leia as sementes tangentes.",
        "Atraia as pontas; cruze atras delas antes da bifurcacao.",
        "Atravesse os aneis enquanto as raizes giram ao contrario.",
        "Proteja-se na sombra do escudo, no lado oposto a onda.",
        "Contorne o escudo pelas costas enquanto o leque gira.",
        "O conves carrega voce: desvie dos jatos sem cair na agua.",
        "Nao volte sobre as propias pegadas: o lodo amadurece atras.",
        "LAMA desacelera. Escolha o terreno antes da torrente.",
        "ENTRE na bolha para o diluvio; SAIA antes de ela colapsar.",
        "Leia a neve no teto e contorne as duas frentes da avalanche.",
        "Estacas caem do teto e ficam cravadas. Contato congela.",
        "O piso desliza: freie cedo. SHIFT rompe o congelamento.",
        "O jato dobra na superficie da agua. Leia a segunda direcao.",
        "O reflexo tambem pode ser atingido. Proteja as duas almas.",
        "Use SHIFT para atravessar a membrana; andar nao protege."
    };
    return PilotoEh(id) ? instrucoes[id-PADRAO_PILOTO_POEIRA_ASA] : "";
}
static void Peca(QuadroPiloto *q, Vector2 a, Vector2 b, float raio, bool ativa,
                  IdSprite sprite, unsigned long evento, int regra)
{
    if(q->total>=MAXIMO_AMEACAS_PILOTO)return;
    AmeacaPiloto *p=&q->ameacas[q->total++];
    *p=(AmeacaPiloto){.forma=Vector2Distance(a,b)<.1f?PILOTO_CIRCULO:PILOTO_CAPSULA,
        .a=a,.b=b,.raio=raio,.ativo=ativa,.sprite=sprite,.evento=evento,.regra=regra};
    p->rotacao=atan2f(b.y-a.y,b.x-a.x)*RAD2DEG;
}
static Vector2 Pos(Rectangle b,float x,float y) {return (Vector2){b.x+b.width*x,b.y+b.height*y};}
static Vector2 Curva(Vector2 a,Vector2 c,Vector2 b,float u)
{
    u=Clamp(u,0,1);float v=1-u;
    return (Vector2){v*v*a.x+2*v*u*c.x+u*u*b.x,v*v*a.y+2*v*u*c.y+u*u*b.y};
}
static float Tempo(const EstadoDefesa *d,float t)
{return fmaxf(0,t-(d->id_padrao>=PADRAO_PILOTO_ADAGA_RETORNO?0:Config(d)->aviso))*(d->fator_movimento_ataque>0?d->fator_movimento_ataque:1.0f);}
static unsigned long Evento(int ciclo,int peca) {return (unsigned long)(1+ciclo*100+peca);}

/* Cada ataque possui sua propria regra. Somente geometria e animacao sao compartilhadas. */
static void PoeiraDeAsa(const EstadoDefesa *d,float t,QuadroPiloto *q)
{
    Rectangle b=d->caixa;const ConfiguracaoPiloto *c=Config(d);
    float rel=Tempo(d,t);
    for(int onda=0;onda<20;onda++) {
        float idade=rel-onda*c->intervalo;
        float x=b.x+b.width+12-idade*c->velocidade;
        if(idade<-.50f||x<b.x-14||x>b.x+b.width+20)continue;
        for(int grao=0;grao<5;grao++){
            float y=b.y+18+grao*(b.height-36)/4+sinf(idade*3.2f+onda*.8f)*9;
            Vector2 p={x,y};
            Peca(q,p,p,c->raio,idade>=0&&t>=c->aviso,SPRITE_PILOTO_POEIRA,Evento(onda,grao),0);
        }
    }
}
static void PoEmContratempo(const EstadoDefesa *d,float t,QuadroPiloto *q)
{
    Rectangle b=d->caixa;const ConfiguracaoPiloto *c=Config(d);
    float rel=Tempo(d,t),periodo=1.45f,local=fmodf(rel,periodo);
    q->regra=((int)(rel/periodo))%2?2:1;q->tolerancia_regra=.25f;
    for(int i=0;i<18;i++){
        float idade=rel-i*.28f,x=b.x+b.width+8-idade*106;
        if(idade<-.5f||x<b.x-10||x>b.x+b.width+12)continue;
        for(int linha=0;linha<4;linha++){
            Vector2 p={x,b.y+20+linha*(b.height-40)/3+sinf(idade*3+i)*10};
            Peca(q,p,p,4,t>=c->aviso&&idade>=0&&local>.25f,SPRITE_PILOTO_POEIRA,Evento(i,linha),q->regra);
        }
    }
    if(q->regra==2) {
        Vector2 grao=d->ancora_movimento;
        grao.x+=sinf(local*4)*8;
        Peca(q,grao,grao,6,t>=c->aviso&&local>.25f,SPRITE_PILOTO_POEIRA,Evento((int)(rel/periodo),70),2);
    }
}
static Vector2 Asa(Rectangle b,float t,int lado)
{
    static const Vector2 pontos[]={{.06f,.08f},{.94f,.08f},{.94f,.92f},{.06f,.92f},{.5f,.5f}};
    float rel=fmodf(t*.85f+(lado?2.5f:0),5);if(rel<0)rel+=5;
    int i=(int)rel;float u=rel-i,u2=u*u,u3=u2*u;
    Vector2 p0=pontos[(i+4)%5],p1=pontos[i],p2=pontos[(i+1)%5],p3=pontos[(i+2)%5];
    float x=.5f*((2*p1.x)+(-p0.x+p2.x)*u+(2*p0.x-5*p1.x+4*p2.x-p3.x)*u2+(-p0.x+3*p1.x-3*p2.x+p3.x)*u3);
    float y=.5f*((2*p1.y)+(-p0.y+p2.y)*u+(2*p0.y-5*p1.y+4*p2.y-p3.y)*u2+(-p0.y+3*p1.y-3*p2.y+p3.y)*u3);
    return Pos(b,Clamp(x,.025f,.975f),Clamp(y,.045f,.955f));
}
static void RevoadaCega(const EstadoDefesa *d,float t,QuadroPiloto *q)
{
    float rel=Tempo(d,t);const ConfiguracaoPiloto *c=Config(d);
    for(int lado=0;lado<2;lado++){
        Vector2 a=Asa(d->caixa,rel,lado),b=Asa(d->caixa,rel-.14f,lado);
        int ciclo=(int)(rel/c->ciclo);
        Peca(q,a,b,7,t>=c->aviso,SPRITE_PILOTO_ASA,Evento(ciclo,lado),0);
        for(int r=1;r<=4;r++) {
            Vector2 p=Asa(d->caixa,rel-.12f*r-.12f,lado);
            Vector2 z=Asa(d->caixa,rel-.12f*r-.24f,lado);
            Peca(q,p,z,2.5f,t>=c->aviso+.45f,SPRITE_PILOTO_POEIRA,Evento(ciclo,10+lado*4+r),0);
        }
    }
}
static void AdagaRetorno(const EstadoDefesa *d,float t,QuadroPiloto *q)
{
    Rectangle b=d->caixa;const ConfiguracaoPiloto *c=Config(d);
    float rel=Tempo(d,t),local=fmodf(rel,c->ciclo);int ciclo=(int)(rel/c->ciclo);
    float y=Clamp(d->ancora_movimento.y,b.y+6,b.y+b.height-6);
    Vector2 mao=Pos(b,ciclo%2?.96f:.04f,.20f);
    Vector2 mira={d->ancora_movimento.x,y};
    Vector2 vetor=Vector2Normalize(Vector2Subtract(mira,mao));
    if(Vector2Length(vetor)<.1f)vetor=(Vector2){ciclo%2?-1:1,.3f};
    float tx=vetor.x>0?(b.x+b.width-6-mao.x)/vetor.x:(b.x+6-mao.x)/vetor.x;
    float ty=fabsf(vetor.y)<.0001f?100000:vetor.y>0?(b.y+b.height-6-mao.y)/vetor.y:(b.y+6-mao.y)/vetor.y;
    Vector2 parede=Vector2Add(mao,Vector2Scale(vetor,fminf(tx,ty)));
    Vector2 volta=Pos(b,ciclo%2?.90f:.10f,.82f);
    bool ativo=t>=c->aviso&&AvisoCausaDano((CronogramaAviso){c->aviso,.78f,.12f},local);
    if(local<c->aviso || t<c->aviso) {
        Peca(q,mao,parede,4,false,SPRITE_PILOTO_ADAGA,Evento(ciclo,0),0);
        Peca(q,parede,volta,4,false,SPRITE_PILOTO_ADAGA,Evento(ciclo,1),0);
    } else if(local<c->aviso+.78f){
        float u=(local-c->aviso)/.78f;
        Vector2 p=u<.52f?Vector2Lerp(mao,parede,u/.52f):Vector2Lerp(parede,volta,(u-.52f)/.48f);
        Vector2 dir=Vector2Normalize(u<.52f?Vector2Subtract(parede,mao):Vector2Subtract(volta,parede));
        Peca(q,Vector2Subtract(p,Vector2Scale(dir,7)),Vector2Add(p,Vector2Scale(dir,7)),4,ativo,SPRITE_PILOTO_ADAGA,Evento(ciclo,2),0);
    }
}
static void ReflexoRoubado(const EstadoDefesa *d,float t,QuadroPiloto *q)
{
    Rectangle b=d->caixa;const ConfiguracaoPiloto *c=Config(d);
    float rel=Tempo(d,t),local=fmodf(rel,c->ciclo);int ciclo=(int)(rel/c->ciclo);
    Vector2 alvo=d->ancora_movimento;
    Vector2 origem=ciclo%2?(Vector2){alvo.x,b.y+b.height-(alvo.y-b.y)}:
                                (Vector2){b.x+b.width-(alvo.x-b.x),alvo.y};
    if(Vector2Distance(origem,alvo)<70)origem=Pos(b,ciclo%2?.15f:.85f,ciclo%2?.15f:.85f);
    Vector2 direcao=Vector2Normalize(Vector2Subtract(alvo,origem));
    Vector2 destino=Vector2Add(origem,Vector2Scale(direcao,b.width*.95f));
    if(local<c->aviso||t<c->aviso) {
        Peca(q,origem,origem,10,false,SPRITE_PILOTO_ESPELHO,Evento(ciclo,0),0);
        Peca(q,origem,destino,3,false,SPRITE_PILOTO_ADAGA,Evento(ciclo,1),0);
    } else if(local<c->aviso+.62f){
        Vector2 p=Vector2Lerp(origem,destino,(local-c->aviso)/.62f);
        Peca(q,Vector2Subtract(p,Vector2Scale(direcao,8)),Vector2Add(p,Vector2Scale(direcao,8)),
             4,true,SPRITE_PILOTO_ADAGA,Evento(ciclo,2),0);
    }
}
static void RouboPassagem(const EstadoDefesa *d,float t,QuadroPiloto *q)
{
    Rectangle b=d->caixa;const ConfiguracaoPiloto *c=Config(d);
    float rel=Tempo(d,t),local=fmodf(rel,c->ciclo);int ciclo=(int)(rel/c->ciclo);
    Vector2 centro={Clamp(d->ancora_movimento.x,b.x+8,b.x+b.width-8),
                    Clamp(d->ancora_movimento.y,b.y+8,b.y+b.height-8)};
    Vector2 a={centro.x-66,centro.y-36},z={centro.x+66,centro.y-36},p={centro.x,centro.y+38};
    a.x=Clamp(a.x,b.x+1,b.x+b.width-1);z.x=Clamp(z.x,b.x+1,b.x+b.width-1);
    a.y=Clamp(a.y,b.y+1,b.y+b.height-1);z.y=a.y;p.y=Clamp(p.y,b.y+1,b.y+b.height-1);
    Vector2 pontos[3]={a,z,p};
    float u=Clamp((local-c->aviso)/.85f,0,1);
    bool ativo=t>=c->aviso&&local>=c->aviso&&local<c->aviso+.85f;
    /* Um lado permanece sem fio; o recolhimento altera a forma dos outros dois. */
    int aberto=ciclo%3;
    if(centro.x<b.x+70)aberto=1;
    else if(centro.x>b.x+b.width-70)aberto=2;
    else if(centro.y>b.y+b.height-48)aberto=0;
    for(int i=0;i<3;i++){
        Vector2 inicio=pontos[i],fim=pontos[(i+1)%3];
        if(local>=c->aviso){inicio=Vector2Lerp(inicio,centro,u*.96f);fim=Vector2Lerp(fim,centro,u*.96f);}
        if(i!=aberto)Peca(q,inicio,fim,2,ativo,SPRITE_PILOTO_FIO,Evento(ciclo,i),0);
        Peca(q,fim,fim,4,ativo,SPRITE_PILOTO_ADAGA,Evento(ciclo,10+i),0);
    }
}
/* Boss: pressao composta. O impacto da arma gera os estilhacos, sem linhas de mira. */
static void AncoraCoral(const EstadoDefesa *d,float t,QuadroPiloto *q)
{
    Rectangle b=d->caixa;const ConfiguracaoPiloto *c=Config(d);
    float rel=Tempo(d,t),local=fmodf(rel,c->ciclo);int ciclo=(int)(rel/c->ciclo);
    Vector2 alvo={Clamp(d->ancora_movimento.x,b.x+16,b.x+b.width-16),
                  Clamp(d->ancora_movimento.y,b.y+16,b.y+b.height-16)};
    for(int lado=0;lado<2;lado++) {
        float idade=local-c->aviso-lado*c->intervalo;
        if(idade<0||idade>.75f)continue;
        Vector2 origem=Pos(b,((ciclo+lado)%2)?.96f:.04f,lado?.88f:.12f);
        Vector2 controle={b.x+b.width*.5f,lado?b.y+b.height+24:b.y-24};
        float u=idade/.75f;
        Vector2 peso=Curva(origem,controle,alvo,u<.6f?u/.6f:(1-u)/.4f);
        Peca(q,peso,peso,8,true,SPRITE_PILOTO_ANCORA,Evento(ciclo,10+lado),0);
        Vector2 ant=origem;
        for(int i=1;i<=5;i++){
            Vector2 pt=Vector2Lerp(origem,peso,i/5.0f);pt.y+=sinf(i*PI/5)*(lado?-7:7);
            Peca(q,ant,pt,2.3f,true,SPRITE_PILOTO_CABO,Evento(ciclo,20+lado*5+i),0);ant=pt;
        }
    }
    /* Leque nasce do ponto atingido, depois de a primeira ancora chegar. */
    float idade=local-c->aviso-.48f;
    if(idade>=0)for(int i=0;i<c->quantidade;i++){
        float ang=PI*2*i/c->quantidade+ciclo*.47f;
        Vector2 p=Vector2Add(alvo,Vector2Scale((Vector2){cosf(ang),sinf(ang)},10+idade*c->velocidade));
        Peca(q,p,p,3.4f,true,SPRITE_PILOTO_ESTILHACO,Evento(ciclo,50+i),0);
    }
}
static void RessacaCalcificada(const EstadoDefesa *d,float t,QuadroPiloto *q)
{
    Rectangle b=d->caixa;const ConfiguracaoPiloto *c=Config(d);
    float rel=fmaxf(0,Tempo(d,t)-c->aviso);
    q->corrente=(Vector2){sinf(rel*PI/c->ciclo)*42,0};
    for(int i=0;i<18;i++){
        float x=b.x+b.width+28+i*84-rel*c->velocidade;
        if(x<b.x-25||x>b.x+b.width+35)continue;
        bool baixo=i%2==0;
        Vector2 raiz={x,baixo?b.y+b.height:b.y};
        /* O corredor se move, mas a ponta nao se fecha sobre a alma sem entrada visivel. */
        float corredor=b.y+b.height*(.5f+.25f*cosf(rel*1.4f));
        Vector2 ponta={x+9*sinf(rel*1.3f+i),corredor+(baixo?32:-32)};
        Peca(q,raiz,ponta,4,t>=c->aviso,SPRITE_PILOTO_CORAL,Evento(i,0),0);
        Vector2 ramo={ponta.x+(baixo?-13:13),ponta.y+(baixo?10:-10)};
        Peca(q,ponta,ramo,3,t>=c->aviso,SPRITE_PILOTO_CORAL,Evento(i,1),0);
    }
    /* Fragmentos caem entre os ramos: avancar na horizontal tambem exige leitura. */
    for(int i=0;i<20;i++) {
        float idade=rel-.65f-i*c->intervalo;
        if(idade<0||idade>1.65f)continue;
        Vector2 p={b.x+b.width*(.18f+.64f*((i*7)%11)/10.0f),b.y-14+idade*110};
        Peca(q,p,p,3,t>=c->aviso,SPRITE_PILOTO_ESTILHACO,Evento(i,50),0);
    }
}
static void RecifeRuptura(const EstadoDefesa *d,float t,QuadroPiloto *q)
{
    Rectangle b=d->caixa;const ConfiguracaoPiloto *c=Config(d);
    float rel=Tempo(d,t),local=fmodf(rel,c->ciclo);int ciclo=(int)(rel/c->ciclo);
    float x=b.x+b.width*(ciclo%2?.38f:.62f);
    int quebrar=(int)Clamp((d->ancora_movimento.y-b.y)/(b.height/4),0,3);
    for(int s=0;s<4;s++){
        Vector2 a={x,b.y+s*b.height/4+5},z={x,b.y+(s+1)*b.height/4-5};
        bool removido=s==quebrar&&local>=c->aviso;
        if(!removido)Peca(q,a,z,5,t>=c->aviso&&local>=c->aviso,SPRITE_PILOTO_CORAL,Evento(ciclo,s),0);
    }
    Vector2 alvo={x,b.y+(quebrar+.5f)*b.height/4};
    float idade=local-c->aviso;
    if(idade>=0){
        Vector2 origem={d->ancora_movimento.x,b.y+5};
        Vector2 arma=Vector2Lerp(origem,alvo,Clamp(idade/.25f,0,1));
        Peca(q,arma,arma,8,idade<.25f,SPRITE_PILOTO_ANCORA,Evento(ciclo,30),0);
        /* Duas erupcoes defasadas obrigam a sair do primeiro desvio. */
        for(int onda=0;onda<2;onda++) {
            float a=idade-.25f-onda*c->intervalo;
            if(a<0)continue;
            for(int i=0;i<c->quantidade;i++){
                float ang=PI*2*(i+.5f*onda)/c->quantidade;
                Vector2 p=Vector2Add(alvo,Vector2Scale((Vector2){cosf(ang),sinf(ang)},12+a*c->velocidade));
                Peca(q,p,p,3.5f,true,SPRITE_PILOTO_ESTILHACO,Evento(ciclo,40+onda*12+i),0);
            }
        }
        q->corrente=(Vector2){ciclo%2?-44:44,0};
    }
}
void PilotoQuadro(const EstadoDefesa *d,float t,QuadroPiloto *q)
{
    memset(q,0,sizeof(*q));
    switch(d->id_padrao){
        case PADRAO_PILOTO_POEIRA_ASA:PoeiraDeAsa(d,t,q);break;
        case PADRAO_PILOTO_PO_CONTRATEMPO:PoEmContratempo(d,t,q);break;
        case PADRAO_PILOTO_REVOADA_CEGA:RevoadaCega(d,t,q);break;
        case PADRAO_PILOTO_ADAGA_RETORNO:AdagaRetorno(d,t,q);break;
        case PADRAO_PILOTO_REFLEXO_ROUBADO:ReflexoRoubado(d,t,q);break;
        case PADRAO_PILOTO_ROUBO_PASSAGEM:RouboPassagem(d,t,q);break;
        case PADRAO_PILOTO_ANCORA_CORAL:AncoraCoral(d,t,q);break;
        case PADRAO_PILOTO_RESSACA_CALCIFICADA:RessacaCalcificada(d,t,q);break;
        case PADRAO_PILOTO_RECIFE_RUPTURA:RecifeRuptura(d,t,q);break;
        default:if(AguaEh(d->id_padrao))AguaQuadro(d,t,q);else PlantaQuadro(d,t,q);break;
    }
}
static float Cruz(Vector2 a,Vector2 b,Vector2 c)
{return (b.x-a.x)*(c.y-a.y)-(b.y-a.y)*(c.x-a.x);}
static bool SegmentosCruzam(Vector2 a,Vector2 b,Vector2 c,Vector2 z)
{
    float p=Cruz(a,b,c),r=Cruz(a,b,z),s=Cruz(c,z,a),v=Cruz(c,z,b);
    return p*r<0&&s*v<0;
}
static bool PercursoCapsula(Vector2 a,Vector2 b,float raio,Vector2 c,Vector2 z,float r)
{
    float total=raio+r;
    return SegmentosCruzam(a,b,c,z)||DistanciaPontoSegmento(a,c,z)<=total||
        DistanciaPontoSegmento(b,c,z)<=total||DistanciaPontoSegmento(c,a,b)<=total||
        DistanciaPontoSegmento(z,a,b)<=total;
}
static bool DentroTri(Vector2 p,const AmeacaPiloto *a)
{float x=Cruz(a->a,a->b,p),y=Cruz(a->b,a->c,p),z=Cruz(a->c,a->a,p);return !((x<0||y<0||z<0)&&(x>0||y>0||z>0));}
static bool PercursoAmeacas(const QuadroPiloto *q,Vector2 a,Vector2 b,float raio,Vector2 entrada,unsigned long *evento)
{
    for(int i=0;i<q->total;i++){
        const AmeacaPiloto *p=&q->ameacas[i];if(!p->ativo)continue;
        float mov=Vector2Length(entrada);
        if((p->regra==1&&mov<.08f)||(p->regra==2&&mov>=.08f)||(p->regra==3&&q->dash_ativo))continue;
        bool hit;
        if(p->forma==PILOTO_TRIANGULO)hit=DentroTri(a,p)||DentroTri(b,p)||PercursoCapsula(a,b,raio,p->a,p->b,0)||PercursoCapsula(a,b,raio,p->b,p->c,0)||PercursoCapsula(a,b,raio,p->c,p->a,0);
        else if(p->forma==PILOTO_FORA_CIRCULO)hit=Vector2Distance(a,p->a)>p->raio-raio||Vector2Distance(b,p->a)>p->raio-raio;
        else hit=p->forma==PILOTO_CIRCULO?PercursoAtingeCirculo(a,b,raio,p->a,p->raio):p->forma==PILOTO_CAPSULA?PercursoCapsula(a,b,raio,p->a,p->b,p->raio):PercursoAtingeRetangulo(a,b,raio,p->area);
        if(hit){if(evento)*evento=p->evento;return true;}
    }
    return false;
}
bool PilotoPercursoColide(const QuadroPiloto *q,Vector2 a,Vector2 b,float raio,Vector2 entrada,unsigned long *evento)
{
    if(PercursoAmeacas(q,a,b,raio,entrada,evento))return true;
    if(!q->reflete_alma)return false;
    Rectangle r=q->caixa_reflexo;a.y=r.y+r.height-(a.y-r.y);b.y=r.y+r.height-(b.y-r.y);
    return PercursoAmeacas(q,a,b,raio,entrada,evento);
}
void PilotoAtualizar(EstadoDefesa *d,AtributosJogador *j,EntradaDefesa entrada,float dt)
{
    const ConfiguracaoPiloto *c=Config(d);
    float rel=Tempo(d,d->temporizador_padrao);
    int ciclo=(int)(rel/c->ciclo);
    if(d->total_criacoes==0||d->batida_movimento!=ciclo){
        d->ancora_movimento=d->posicao_alma;d->batida_movimento=ciclo;d->total_criacoes++;
    }
    if(AguaEh(d->id_padrao))AguaRastroAtualizar(d);
    QuadroPiloto q;PilotoQuadro(d,d->temporizador_padrao,&q);
    q.dash_ativo=d->dash_restante>0||Vector2Distance(d->inicio_movimento,d->posicao_alma)>.001f;
    Vector2 mov=entrada.movimento;if(Vector2Length(mov)>1)mov=Vector2Normalize(mov);
    for(int i=5;i>0;i--) d->rastro_alma[i]=d->rastro_alma[i-1];
    d->rastro_alma[0]=d->posicao_alma;
    Vector2 deslocamento=Vector2Add(Vector2Scale(mov,JogadorVelocidadeDefesa(j)),q.corrente);
    if(AguaEh(d->id_padrao))AguaMover(d,&q,mov,JogadorVelocidadeDefesa(j),dt,q.dash_ativo);
    else d->posicao_alma=Vector2Add(d->posicao_alma,Vector2Scale(deslocamento,dt));
    Rectangle b=d->caixa;
    d->posicao_alma.x=Clamp(d->posicao_alma.x,b.x+d->raio_alma,b.x+b.width-d->raio_alma);
    d->posicao_alma.y=Clamp(d->posicao_alma.y,b.y+d->raio_alma,b.y+b.height-d->raio_alma);
    unsigned long evento=0;
    Vector2 intencao_real=Vector2Subtract(d->posicao_alma,d->inicio_movimento);
    if(Vector2Length(intencao_real)>.001f)intencao_real=Vector2Normalize(intencao_real);
    if(PilotoPercursoColide(&q,d->inicio_movimento,d->posicao_alma,d->raio_alma,intencao_real,&evento)){
        bool usado=false;
        for(int i=0;i<d->total_eventos_acertados;i++)if(d->eventos_acertados[i]==evento)usado=true;
        if(!usado&&DefesaAplicarDano(d,j,d->padrao.dano,d->padrao.tipo)>0) {
            if(AguaEh(d->id_padrao))AguaCongelarNoAcerto(d);
            if(d->total_eventos_acertados>=16){memmove(d->eventos_acertados,d->eventos_acertados+1,15*sizeof(unsigned long));d->total_eventos_acertados=15;}
            d->eventos_acertados[d->total_eventos_acertados++]=evento;
        }
    }
}
static void DesenharPeca(const AmeacaPiloto *p,const SistemaSprites *sprites,float t,Color cor,Rectangle caixa)
{
    if(p->sprite>=SPRITE_AGUA_ONDA&&p->sprite<=SPRITE_AGUA_MEMBRANA){AguaDesenharPeca(p,sprites,t,caixa);return;}
    if(p->sprite>=SPRITE_PLANTA_GOTA && p->sprite<=SPRITE_PLANTA_FILAMENTO){PlantaDesenharPeca(p,sprites,t);return;}
    Vector2 centro=Vector2Lerp(p->a,p->b,.5f);
    float comprimento=Vector2Distance(p->a,p->b);
    if(!p->ativo && p->sprite!=SPRITE_PILOTO_CORAL)return;
    /* The geometric silhouette is exactly the collision; the art stays within it. */
    if(comprimento>.1f)DrawLineEx(p->a,p->b,p->raio*2,Fade(cor,.44f));
    else DrawCircleV(centro,p->raio,Fade(cor,.52f));
    if(!sprites)return;
    const SpriteJogo *s=sprite_obter(sprites,p->sprite);
    if(!s)return;
    float w=s->metadados.largura_quadro>0?s->metadados.largura_quadro:s->textura.width;
    float h=s->metadados.altura_quadro>0?s->metadados.altura_quadro:s->textura.height;
    if(p->sprite==SPRITE_PILOTO_CORAL && comprimento>.1f) {
        /* Um ramo continuo: a textura fornece a superficie, sem colar uma arvore na ponta. */
        Color base=p->ativo?(Color){231,116,103,255}:Fade(GOLD,.65f);
        Color borda=p->ativo?(Color){105,54,65,255}:Fade(GOLD,.4f);
        DrawLineEx(p->a,p->b,p->raio*2,borda);
        DrawCircleV(p->a,p->raio,borda);DrawCircleV(p->b,p->raio,borda);
        float miolo=fmaxf(1,p->raio-1);
        DrawLineEx(p->a,p->b,miolo*2,base);
        DrawCircleV(p->a,miolo,base);DrawCircleV(p->b,miolo,base);
        int quadro=s->metadados.quadros>1?((int)(t*s->metadados.fps)%s->metadados.quadros):0;
        Rectangle textura={quadro*w+w*.42f,h*.10f,w*.16f,h*.80f};
        DrawTexturePro(s->textura,textura,(Rectangle){centro.x,centro.y,miolo*2,comprimento},
            (Vector2){miolo,comprimento*.5f},p->rotacao+90,Fade(WHITE,.24f));
        Vector2 normal=Vector2Scale(Vector2Normalize((Vector2){p->a.y-p->b.y,p->b.x-p->a.x}),miolo*.36f);
        DrawLineEx(Vector2Add(p->a,normal),Vector2Add(p->b,normal),1,Fade(RAYWHITE,.52f+.05f*sinf(t*5)));
        return;
    }
    if(p->sprite==SPRITE_PILOTO_CABO && comprimento>.1f) {
        DrawLineEx(p->a,p->b,p->raio*2,(Color){79,169,176,255});
        int n=(int)ceilf(comprimento/10);
        for(int i=0;i<n;i++) {
            Vector2 a=Vector2Lerp(p->a,p->b,(i+.15f)/n),z=Vector2Lerp(p->a,p->b,(i+.45f)/n);
            DrawLineEx(a,z,1.2f,Fade(RAYWHITE,.75f));
        }
        return;
    }
    float alvo=fmaxf(p->raio*2+4,comprimento+4);
    if(p->sprite==SPRITE_PILOTO_ANCORA)alvo=26;
    float escala=alvo/fmaxf(w,h)/s->escala;
    if(comprimento>30) {
        int n=(int)(comprimento/16)+1;
        for(int i=0;i<n;i++)SpriteDesenharCentralizadoRotacionado(sprites,p->sprite,
            Vector2Lerp(p->a,p->b,(i+.5f)/n),WHITE,t,p->rotacao,((p->raio*2+5)/fmaxf(w,h)/s->escala)*(1+.04f*sinf(t*7+i)),false);
    } else SpriteDesenharCentralizadoRotacionado(sprites,p->sprite,centro,p->regra?cor:WHITE,t,
        p->rotacao+(p->sprite==SPRITE_PILOTO_POEIRA?t*90:p->sprite==SPRITE_PILOTO_ADAGA?t*420:p->sprite==SPRITE_PILOTO_ESTILHACO?t*180:0),escala*(1+.04f*sinf(t*7)),false);

}
static void AtorPiloto(const SistemaSprites *sprites,IdSprite id,Vector2 pos,float tamanho,float t,float giro,Color cor)
{
    if(!sprites)return;
    const SpriteJogo *s=sprite_obter(sprites,id);if(!s)return;
    float w=s->metadados.largura_quadro>0?s->metadados.largura_quadro:s->textura.width;
    float h=s->metadados.altura_quadro>0?s->metadados.altura_quadro:s->textura.height;
    float escala=tamanho/fmaxf(w,h)/s->escala;
    SpriteDesenharCentralizadoRotacionado(sprites,id,pos,cor,t,giro,escala*(1+.06f*sinf(t*7)),false);
}
void PilotoDesenhar(const EstadoDefesa *d,const SistemaSprites *sprites)
{
    QuadroPiloto q;PilotoQuadro(d,d->temporizador_padrao,&q);
    Rectangle b=d->caixa;
    InterfaceDesenharTextoAjustado(PilotoNome(d->id_padrao),(Rectangle){b.x,b.y-31,b.width,15},14,RAYWHITE);
    InterfaceDesenharTextoAjustado(PilotoInstrucao(d->id_padrao),(Rectangle){b.x,b.y-15,b.width,12},11,RAYWHITE);
    BeginScissorMode((int)b.x,(int)b.y,(int)b.width,(int)b.height);
    float rel=Tempo(d,d->temporizador_padrao);
    int ciclo=(int)(rel/Config(d)->ciclo);
    if(d->id_padrao<=PADRAO_PILOTO_REVOADA_CEGA) {
        AtorPiloto(sprites,SPRITE_PILOTO_ASA,Pos(b,.96f,.5f+.1f*sinf(rel*4)),26,
                    d->temporizador_padrao,20*sinf(rel*7),Fade(WHITE,.85f));
    }
    if(d->id_padrao==PADRAO_PILOTO_ADAGA_RETORNO) {
        AtorPiloto(sprites,SPRITE_PILOTO_MAO,Pos(b,ciclo%2?.96f:.04f,.20f),28,
                    d->temporizador_padrao,8*sinf(rel*4),WHITE);
    }
    if(d->id_padrao==PADRAO_PILOTO_ANCORA_CORAL) {
        float local=fmodf(rel,Config(d)->ciclo);
        if(local<Config(d)->aviso)for(int lado=0;lado<2;lado++) {
            Vector2 p=Pos(b,((ciclo+lado)%2)?.96f:.04f,lado?.88f:.12f);
            AtorPiloto(sprites,SPRITE_PILOTO_ANCORA,p,26,d->temporizador_padrao,12*sinf(rel*12),WHITE);
        }
    }
    if(d->id_padrao==PADRAO_PILOTO_REFLEXO_ROUBADO) {
        Vector2 alvo=d->ancora_movimento;
        Vector2 origem=ciclo%2?(Vector2){alvo.x,b.y+b.height-(alvo.y-b.y)}:
            (Vector2){b.x+b.width-(alvo.x-b.x),alvo.y};
        if(Vector2Distance(origem,alvo)<70)origem=Pos(b,ciclo%2?.15f:.85f,ciclo%2?.15f:.85f);
        AtorPiloto(sprites,SPRITE_PILOTO_ESPELHO,origem,22,d->temporizador_padrao,6*sinf(rel*5),Fade(SKYBLUE,.8f));
    }
    if(d->id_padrao==PADRAO_PILOTO_RECIFE_RUPTURA) {
        int parte=(int)Clamp((d->ancora_movimento.y-b.y)/(b.height/4),0,3);

        InterfaceDesenharTexto(TextFormat("QUEBRA %d",parte+1),(int)b.x+8,(int)b.y+8,11,GOLD);

    }
    /* Preparacao localizada na arma/origem, sem desenhar o trajeto futuro. */
    float local=fmodf(rel,Config(d)->ciclo);
    if(d->id_padrao==PADRAO_PLANTA_GOTAS_GEMEAS && local<Config(d)->aviso){
        float alvo_y=Clamp(d->ancora_movimento.y,b.y+22,b.y+b.height-22);
        for(int lado=0;lado<2;lado++){
            Vector2 origem={lado?b.x+b.width:b.x,Clamp(alvo_y+(lado?25:-25),b.y+12,b.y+b.height-12)};
            AmeacaPiloto aviso={.a=origem,.b=origem,.raio=8,.sprite=SPRITE_PLANTA_GOTA,.ativo=false};
            PlantaDesenharPeca(&aviso,sprites,d->temporizador_padrao);
        }
    }
    if(d->id_padrao==PADRAO_PLANTA_SACUDIDA){
        int onda=(int)(fmaxf(0,rel-Config(d)->aviso)/Config(d)->intervalo);
        AtorPiloto(sprites,SPRITE_PLANTA_LEBRE,Pos(b,.22f+.56f*((onda*3)%5)/4.0f,.96f),25,d->temporizador_padrao,6*sinf(rel*9),Fade(WHITE,.8f));
    }
    if(d->id_padrao>=PADRAO_PILOTO_ADAGA_RETORNO && d->id_padrao<=PADRAO_PILOTO_RECIFE_RUPTURA && local<Config(d)->aviso) {
        Vector2 origem=Pos(b,ciclo%2?.94f:.06f,.12f);
        if(d->id_padrao==PADRAO_PILOTO_ADAGA_RETORNO)origem=Pos(b,ciclo%2?.96f:.04f,.20f);
        if(d->id_padrao==PADRAO_PILOTO_ROUBO_PASSAGEM)origem=d->ancora_movimento;
        if(d->id_padrao==PADRAO_PILOTO_RECIFE_RUPTURA)origem=(Vector2){d->ancora_movimento.x,b.y+9};
        if(d->id_padrao!=PADRAO_PILOTO_REFLEXO_ROUBADO && d->id_padrao!=PADRAO_PILOTO_RESSACA_CALCIFICADA) {
            float pulso=.5f+.5f*sinf(d->temporizador_padrao*24);
            DrawCircleV(origem,6+pulso*3,Fade(GOLD,.18f+.22f*pulso));
            if(d->id_padrao>=PADRAO_PILOTO_ANCORA_CORAL)
                AtorPiloto(sprites,SPRITE_PILOTO_ANCORA,origem,24,d->temporizador_padrao,10*sinf(rel*9),WHITE);
        }
    }
    for(int i=0;i<q.total;i++){
        Color cor=q.ameacas[i].regra==1?SKYBLUE:q.ameacas[i].regra==2?ORANGE:cor_tipo(d->padrao.tipo);
        DesenharPeca(&q.ameacas[i],sprites,d->temporizador_padrao,cor,b);
    }
    if(AguaEh(d->id_padrao))AguaDesenharEstado(d,&q,sprites);
    if(q.regra) {
        InterfaceDesenharTexto(q.regra==1?"AZUL: PARE":"LARANJA: MOVA",(int)b.x+8,(int)b.y+8,14,q.regra==1?SKYBLUE:ORANGE);
        float rel=Tempo(d,d->temporizador_padrao),local=fmodf(rel,1.45f);
        if(local<.25f)InterfaceDesenharTexto("TROCA...",(int)b.x+8,(int)b.y+26,12,GOLD);
    }
    if(fabsf(q.corrente.x)>1) {
        InterfaceDesenharTexto(q.corrente.x>0?">>> CORRENTE": "CORRENTE <<<",(int)b.x+8,(int)(b.y+b.height)-19,12,SKYBLUE);
    }
    if(sprites && sprites->depurar_limites) {
        DrawCircleLines((int)d->posicao_alma.x,(int)d->posicao_alma.y,d->raio_alma,LIME);
        for(int i=0;i<q.total;i++) if(q.ameacas[i].ativo){
            const AmeacaPiloto *a=&q.ameacas[i];
            if(a->forma==PILOTO_TRIANGULO){DrawLineEx(a->a,a->b,1,MAGENTA);DrawLineEx(a->b,a->c,1,MAGENTA);DrawLineEx(a->c,a->a,1,MAGENTA);}
            else if(a->forma==PILOTO_RETANGULO)DrawRectangleLinesEx(a->area,1,MAGENTA);
            else{DrawCircleLines((int)a->a.x,(int)a->a.y,a->raio,MAGENTA);DrawCircleLines((int)a->b.x,(int)a->b.y,a->raio,MAGENTA);}
        }
    }
    EndScissorMode();
}
