#include "route_validator.h"
#include "water_patterns.h"
#include "balance.h"
#include "raymath.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define LARGURA_BUSCA 64
typedef struct { Vector2 pos,ancora; int ciclo; float score; int pai,acao; EstadoMovimentoAgua agua; Vector2 dash_dir;float dash,recarga; } NoRota;
typedef struct { short pai; unsigned char acao; } EloRota;
static const Vector2 CONTROLES[9]={{0,0},{1,0},{-1,0},{0,1},{0,-1},
    {.70710678f,.70710678f},{-.70710678f,.70710678f},{.70710678f,-.70710678f},{-.70710678f,-.70710678f}};
static bool PassoRotaAntigo(EstadoDefesa *d,NoRota *no,Vector2 entrada,float tempo,float dt)
{
    const ConfiguracaoPiloto *c=&BALANCEAMENTO_PILOTO[d->id_padrao-PADRAO_PILOTO_POEIRA_ASA];
    while(dt>.000001f){
        float passo=fminf(dt,BALANCEAMENTO_PASSO_FISICA);tempo+=passo;
        int ciclo=(int)(fmaxf(0,tempo-(d->id_padrao>=PADRAO_PILOTO_ADAGA_RETORNO?0:c->aviso))/c->ciclo);
        if(ciclo!=no->ciclo){no->ancora=no->pos;no->ciclo=ciclo;}
        d->ancora_movimento=no->ancora;d->posicao_alma=no->pos;
        QuadroPiloto q;PilotoQuadro(d,tempo,&q);
        Vector2 antes=no->pos;
        no->pos=Vector2Add(no->pos,Vector2Scale(Vector2Add(Vector2Scale(entrada,135),q.corrente),passo));
        Rectangle b=d->caixa;
        no->pos.x=Clamp(no->pos.x,b.x+4,b.x+b.width-4);
        no->pos.y=Clamp(no->pos.y,b.y+4,b.y+b.height-4);
        Vector2 real=Vector2Subtract(no->pos,antes);
        if(Vector2Length(real)>.001f)real=Vector2Normalize(real);
        if(PilotoPercursoColide(&q,antes,no->pos,4.4f,real,NULL))return false;
        if(d->id_padrao==PADRAO_PILOTO_PO_CONTRATEMPO) {
            QuadroPiloto margem;PilotoQuadro(d,tempo+.006f,&margem);
            if(PilotoPercursoColide(&margem,antes,no->pos,4.4f,real,NULL))return false;
        }
        dt-=passo;
    }
    return true;
}
static bool PassoRota(EstadoDefesa *d,NoRota *no,Vector2 entrada,bool impulso,float tempo,float dt)
{
    if(!AguaEh(d->id_padrao))return PassoRotaAntigo(d,no,entrada,tempo,dt);
    const ConfiguracaoPiloto *c=&BALANCEAMENTO_PILOTO[d->id_padrao-PADRAO_PILOTO_POEIRA_ASA];
    bool primeiro=true;
    while(dt>.000001f){
        float passo=fminf(dt,BALANCEAMENTO_PASSO_FISICA);tempo+=passo;Vector2 antes=no->pos;
        no->recarga=fmaxf(0,no->recarga-passo);
        if(primeiro&&impulso&&no->recarga<=0&&Vector2Length(entrada)>.01f){no->dash=32;no->recarga=.4f;no->dash_dir=Vector2Normalize(entrada);}
        bool fez_dash=no->dash>0;
        if(fez_dash){float p=fminf(no->dash,400*passo);no->pos=Vector2Add(no->pos,Vector2Scale(no->dash_dir,p));no->dash-=p;}
        int ciclo=(int)(tempo/c->ciclo);if(ciclo!=no->ciclo){no->ancora=no->pos;no->ciclo=ciclo;}
        d->ancora_movimento=no->ancora;d->posicao_alma=no->pos;d->agua=no->agua;d->temporizador_padrao=tempo;d->fator_movimento_ataque=1;
        AguaRastroAtualizar(d);QuadroPiloto q;PilotoQuadro(d,tempo,&q);q.dash_ativo=fez_dash;
        AguaMover(d,&q,entrada,135,passo,fez_dash);Rectangle b=d->caixa;
        d->posicao_alma.x=Clamp(d->posicao_alma.x,b.x+4,b.x+b.width-4);d->posicao_alma.y=Clamp(d->posicao_alma.y,b.y+4,b.y+b.height-4);
        no->pos=d->posicao_alma;no->agua=d->agua;
        Vector2 real=Vector2Subtract(no->pos,antes);if(Vector2Length(real)>.001f)real=Vector2Normalize(real);
        if(PilotoPercursoColide(&q,antes,no->pos,4.6f,real,NULL))return false;
        dt-=passo;primeiro=false;
    }
    return true;
}

static float Avaliar(EstadoDefesa *d,const NoRota *no,float tempo)
{
    d->ancora_movimento=no->ancora;d->agua=no->agua;d->posicao_alma=no->pos;float score=0;
    for(int k=0;k<3;k++){
        QuadroPiloto q;PilotoQuadro(d,tempo+k*.24f,&q);
        for(int i=0;i<q.total;i++){
            const AmeacaPiloto *p=&q.ameacas[i];
            if(p->regra==6){Vector2 seguro={Clamp(no->pos.x,p->area.x+28,p->area.x+p->area.width-28),Clamp(no->pos.y,p->area.y+25,p->area.y+p->area.height-25)};score+=Vector2Distance(no->pos,seguro)*4.5f;continue;}
            if((p->regra&&p->regra!=3)||p->sprite==SPRITE_PLANTA_ANTIDOTO)continue;
            if(d->id_padrao==PADRAO_AGUA_MEMBRANA&&p->regra==3)continue;
            float dist=PilotoDistanciaAmeaca(p,no->pos)-4;
            if(q.reflete_alma){Vector2 reflexo={no->pos.x,q.caixa_reflexo.y+q.caixa_reflexo.height-(no->pos.y-q.caixa_reflexo.y)};dist=fminf(dist,PilotoDistanciaAmeaca(p,reflexo)-4);}
            if(AguaEh(d->id_padrao)&&dist<0)score+=-dist*3.0f/(k+1);
            if(dist<55)score+=(55-fmaxf(0,dist))*(p->ativo?1:.55f)/(k+1);
        }
    }
    Vector2 centro={d->caixa.x+d->caixa.width/2,d->caixa.y+d->caixa.height/2};
    if(d->id_padrao==PADRAO_PILOTO_ROUBO_PASSAGEM) {
        Vector2 alvo={Clamp(no->ancora.x,d->caixa.x+8,d->caixa.x+d->caixa.width-8),
                      Clamp(no->ancora.y,d->caixa.y+8,d->caixa.y+d->caixa.height-8)};
        int aberto=no->ciclo%3;
        if(alvo.x<d->caixa.x+70)aberto=1;
        else if(alvo.x>d->caixa.x+d->caixa.width-70)aberto=2;
        else if(alvo.y>d->caixa.y+d->caixa.height-48)aberto=0;
        alvo=Vector2Add(alvo,aberto==0?(Vector2){0,-68}:aberto==1?(Vector2){80,40}:(Vector2){-80,40});
        score+=Vector2Distance(no->pos,alvo)*.8f;
    }
    if(d->id_padrao==PADRAO_PILOTO_RESSACA_CALCIFICADA) {
        float rel=fmaxf(0,tempo-BALANCEAMENTO_PILOTO[7].aviso);
        Vector2 alvo={centro.x,d->caixa.y+d->caixa.height*(.5f+.25f*cosf(rel*1.4f))};
        score+=Vector2Distance(no->pos,alvo)*4.0f;
    }
    if(d->id_padrao==PADRAO_AGUA_MEMBRANA)score+=Vector2Distance(no->pos,centro);
    if(d->id_padrao==PADRAO_AGUA_PRISAO){
        Vector2 v=no->agua.velocidade;
        Vector2 parada={no->pos.x+v.x*fabsf(v.x)/(2*BALANCEAMENTO_GELO_FREIO),no->pos.y+v.y*fabsf(v.y)/(2*BALANCEAMENTO_GELO_FREIO)};
        Vector2 dentro={Clamp(parada.x,d->caixa.x+20,d->caixa.x+d->caixa.width-20),Clamp(parada.y,d->caixa.y+20,d->caixa.y+d->caixa.height-20)};
        int faixa=(int)(fmaxf(0,tempo-1.0f)/1.65f); Vector2 slalom={centro.x,centro.y+(faixa%2?-65:65)};
        score+=Vector2Distance(parada,dentro)*2.0f+Vector2Distance(no->pos,slalom)*3.0f;
    }
    if(d->id_padrao==PADRAO_AGUA_REFLEXO){float lado=no->ancora.x<centro.x?1:-1;float x=Clamp(no->ancora.x+lado*135,d->caixa.x+16,d->caixa.x+d->caixa.width-16);score+=fabsf(no->pos.x-x)*3+fabsf(no->pos.y-centro.y)*.1f;}
    if(d->id_padrao==PADRAO_AGUA_ESTACAS){
        float lado=no->ancora.x<centro.x?1:-1;
        float x=Clamp(no->ancora.x+lado*105,d->caixa.x+15,d->caixa.x+d->caixa.width-15);
        score+=fabsf(no->pos.x-x)*3.0f;
    }
    if(d->id_padrao==PADRAO_AGUA_AVALANCHE){
        float x=d->caixa.x+d->caixa.width*(no->ancora.x<centro.x?.84f:.16f);
        score+=fabsf(no->pos.x-x)*4.5f;
    }
    if(d->id_padrao==PADRAO_AGUA_VISCOSIDADE){
        float lado=no->ancora.x>centro.x?-1:1;
        Vector2 saida={Clamp(no->ancora.x+lado*86,d->caixa.x+20,d->caixa.x+d->caixa.width-20),centro.y};
        score+=fabsf(no->pos.x-saida.x)*5.0f+fabsf(no->pos.y-saida.y)*.2f;
    }
    if(d->id_padrao==PADRAO_AGUA_PEGADAS){
        float rx=d->caixa.width*.35f,ry=d->caixa.height*.30f;
        float inicio=atan2f((no->agua.origem_rastro.y-centro.y)/ry,(no->agua.origem_rastro.x-centro.x)/rx);
        float a=inicio+tempo*.9f;Vector2 alvo={centro.x+rx*cosf(a),centro.y+ry*sinf(a)};
        score+=Vector2Distance(no->pos,alvo)*1.4f;
    }
    if(d->id_padrao==PADRAO_AGUA_TSUNAMI){
        const ConfiguracaoPiloto *c=&BALANCEAMENTO_PILOTO[d->id_padrao-PADRAO_PILOTO_POEIRA_ASA];
        int ciclo=(int)(tempo/c->ciclo);float lado=no->ancora.x<centro.x?1:no->ancora.x>centro.x?-1:(ciclo%2?-1:1);
        Vector2 abrigo={Clamp(no->ancora.x+lado*60,d->caixa.x+45,d->caixa.x+d->caixa.width-45),centro.y+(ciclo%2?-15:15)};
        score+=Vector2Distance(no->pos,abrigo)*1.6f;
    }
    if(d->id_padrao==PADRAO_AGUA_JATO_QUADRANTE){
        const ConfiguracaoPiloto *c=&BALANCEAMENTO_PILOTO[d->id_padrao-PADRAO_PILOTO_POEIRA_ASA];int ciclo=(int)(tempo/c->ciclo);
        Vector2 origem={centro.x+d->caixa.width*.15f*sinf(ciclo*1.7f),centro.y};
        Vector2 dir=Vector2Normalize(Vector2Subtract(no->ancora,origem));if(Vector2Length(dir)<.1f)dir=(Vector2){1,0};
        Vector2 fuga=Vector2Subtract(origem,Vector2Scale(dir,85));fuga.x=Clamp(fuga.x,d->caixa.x+15,d->caixa.x+d->caixa.width-15);fuga.y=Clamp(fuga.y,d->caixa.y+15,d->caixa.y+d->caixa.height-15);
        score+=Vector2Distance(no->pos,fuga)*.8f;
    }
    if(d->id_padrao==PADRAO_PLANTA_CEIFA){
        Vector2 pivo={Clamp(no->ancora.x,d->caixa.x+48,d->caixa.x+d->caixa.width-48),Clamp(no->ancora.y,d->caixa.y+48,d->caixa.y+d->caixa.height-48)};
        Vector2 fuga={d->caixa.x+d->caixa.width*(pivo.x<centro.x?.80f:.20f),centro.y};
        score+=Vector2Distance(no->pos,fuga)*1.5f;
    }
    if(d->id_padrao==PADRAO_PLANTA_RITUAL){
        Vector2 fuga={d->caixa.x+d->caixa.width*(no->ancora.x<centro.x?.80f:.20f),centro.y};
        score+=Vector2Distance(no->pos,fuga)*1.5f;
    }
    if(d->id_padrao==PADRAO_PLANTA_ARRASTO_MENTA)score+=fmaxf(0,72-Vector2Distance(no->pos,no->ancora))*1.5f;
    if(d->id_padrao==PADRAO_PLANTA_RECEITA_REVERSA){
        float y1=d->caixa.y+d->caixa.height*.33f,y2=d->caixa.y+d->caixa.height*.67f;
        score+=fminf(fabsf(no->pos.y-y1),fabsf(no->pos.y-y2))*2.0f;
    }
    if(d->id_padrao==PADRAO_PLANTA_TAMPAS_DOBRADICA){
        Vector2 fuga={d->caixa.x+d->caixa.width*(no->ancora.x<centro.x?.78f:.22f),centro.y};
        score+=Vector2Distance(no->pos,fuga)*1.3f;
    }
    if(d->id_padrao>=PADRAO_PLANTA_ESTILHACO_LANCA){
        float borda=fminf(fminf(no->pos.x-d->caixa.x,d->caixa.x+d->caixa.width-no->pos.x),fminf(no->pos.y-d->caixa.y,d->caixa.y+d->caixa.height-no->pos.y));
        score+=fmaxf(0,12-borda)*.7f;
    }
    return score+Vector2Distance(no->pos,centro)*.025f;
}
static int Comparar(const void *a,const void *b)
{
    float diff=((const NoRota *)a)->score-((const NoRota *)b)->score;
    return diff<0?-1:diff>0?1:0;
}
bool PilotoEncontrarRota(IdPadraoProjetil id,Rectangle caixa,Vector2 inicio,int fps,RotaValidada *rota)
{
    memset(rota,0,sizeof(*rota));rota->fps=fps;rota->inicio=inicio;rota->caixa=caixa;
    if(!PilotoEh(id)||fps<=0)return false;
    const ConfiguracaoPiloto *c=&BALANCEAMENTO_PILOTO[id-PADRAO_PILOTO_POEIRA_ASA];
    int passos=(int)ceilf(c->duracao*fps);
    if(passos>ROTA_MAXIMO_PASSOS)return false;
    EloRota *elos=calloc((size_t)passos*LARGURA_BUSCA,sizeof(*elos));if(!elos)return false;
    EstadoDefesa d={0};d.id_padrao=id;d.caixa=caixa;
    NoRota atual[LARGURA_BUSCA]={{.pos=inicio,.ancora=inicio,.ciclo=-1}};int total=1;
    for(int passo=0;passo<passos;passo++){
        NoRota candidatos[LARGURA_BUSCA*18];int quantidade=0;
        for(int pai=0;pai<total;pai++)for(int acao=0;acao<((id==PADRAO_AGUA_MEMBRANA||id==PADRAO_AGUA_PRISAO)?18:9);acao++){
            NoRota candidato=atual[pai];
            if(!PassoRota(&d,&candidato,CONTROLES[acao%9],acao>=9,(float)passo/fps,1.0f/fps))continue;
            candidato.pai=pai;candidato.acao=acao;
            candidato.score=Avaliar(&d,&candidato,(passo+1.0f)/fps)+atual[pai].score*.10f;
            candidatos[quantidade++]=candidato;
        }
        if(!quantidade){fprintf(stderr,"Busca esgotada t=%.3f pos=%.1f,%.1f ancora=%.1f,%.1f\n",(float)passo/fps,atual[0].pos.x,atual[0].pos.y,atual[0].ancora.x,atual[0].ancora.y);free(elos);return false;}
        qsort(candidatos,(size_t)quantidade,sizeof(NoRota),Comparar);total=0;
        for(int i=0;i<quantidade&&total<LARGURA_BUSCA;i++){
            bool perto=false;
            for(int j=0;j<total;j++)if(Vector2Distance(candidatos[i].pos,atual[j].pos)<1.8f&&
                Vector2Distance(candidatos[i].ancora,atual[j].ancora)<3&&Vector2Distance(candidatos[i].agua.velocidade,atual[j].agua.velocidade)<3&&fabsf(candidatos[i].recarga-atual[j].recarga)<.04f&&fabsf(candidatos[i].dash-atual[j].dash)<2){perto=true;break;}
            if(perto)continue;
            atual[total]=candidatos[i];
            elos[passo*LARGURA_BUSCA+total]=(EloRota){(short)candidatos[i].pai,(unsigned char)candidatos[i].acao};total++;
        }
    }
    int no=0;
    for(int p=passos-1;p>=0;p--){EloRota elo=elos[p*LARGURA_BUSCA+no];rota->entradas[p].movimento=CONTROLES[elo.acao%9];rota->entradas[p].dash_pressionado=elo.acao>=9;no=elo.pai;}
    free(elos);rota->total=passos;rota->encontrada=true;return true;
}
bool PilotoReproduzirRota(IdPadraoProjetil id,const RotaValidada *r,int hp,bool fase,int *dano)
{
    if(!r->encontrada)return false;
    AtributosJogador j;JogadorInicializar(&j);j.vida_maxima=hp;j.hp=hp;
    EstadoDefesa d;DefesaInicializar(&d);DefesaIniciar(&d,id,(Rectangle){42,228,556,94},50,0,1,1);
    d.fase=DEFESA_ATIVA;d.caixa=d.caixa_alvo=r->caixa;d.posicao_alma=r->inicio;
    d.ataque_segunda_fase=fase;d.temporizador_invencibilidade=0;
    for(int i=0;i<r->total;i++) {
        int antes=j.hp;
        DefesaAtualizarComEntrada(&d,&j,1.0f/r->fps,r->entradas[i]);
        if(j.hp!=antes)fprintf(stderr,"Replay hit frame=%d t=%.6f xy=%.2f,%.2f input=%.2f,%.2f\n",
            i,d.temporizador_padrao,d.posicao_alma.x,d.posicao_alma.y,r->entradas[i].movimento.x,r->entradas[i].movimento.y);
    }
    *dano=hp-j.hp;
    return *dano==0&&d.temporizador_padrao>=d.padrao.duracao-.02f;
}
