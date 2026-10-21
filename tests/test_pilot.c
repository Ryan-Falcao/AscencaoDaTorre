#include "route_validator.h"
#include "water_patterns.h"
#include "balance.h"
#include "tower.h"
#include <assert.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
static void VerificarProtecao(void)
{
    AtributosJogador j;JogadorInicializar(&j);OrcamentoDano o;
    DanoIniciarRodada(&o,40,8,35);
    assert(DanoAplicar(&o,&j,100,TIPO_NEUTRO)==8);
    assert(DanoAplicar(&o,&j,100,TIPO_NEUTRO)==0);
    assert(fabsf(o.invulnerabilidade-.85f)<.00001f);
    o.invulnerabilidade=0;assert(DanoAplicar(&o,&j,100,TIPO_NEUTRO)==6);
    o.invulnerabilidade=0;assert(DanoAplicarResidual(&o,&j,100)==0&&o.recebido==14);assert(j.hp==26);
    DanoIniciarRodada(&o,40,8,45);
    assert(DanoAplicarResidual(&o,&j,100)==8);
    assert(DanoAplicarResidual(&o,&j,100)==0);
    o.invulnerabilidade=0;assert(DanoAplicarResidual(&o,&j,100)==8);
    o.invulnerabilidade=0;assert(DanoAplicarResidual(&o,&j,100)==2&&o.recebido==18);
    DanoIniciarRodada(&o,41,8,35);assert(o.limite_rodada==14);
    assert(PercursoAtingeRetangulo((Vector2){0,10},(Vector2){32,10},4,(Rectangle){14,0,2,20}));
    assert(PercursoAtingeCirculo((Vector2){0,10},(Vector2){32,10},4,(Vector2){16,10},2));
    assert(!PercursoAtingeCirculo((Vector2){0,30},(Vector2){32,30},4,(Vector2){16,10},2));
    assert(BalanceamentoMultiplicadorDanoAndar(1)==1&&BalanceamentoMultiplicadorDanoAndar(2)==1);
    EstadoDefesa status;DefesaInicializar(&status);
    DefesaIniciar(&status,PADRAO_PILOTO_ADAGA_RETORNO,(Rectangle){42,228,556,94},50,0,.75f,.70f);
    assert(status.padrao.dano==3&&fabsf(status.fator_movimento_ataque-.70f)<.0001f);
    Torre t;TorreInicializar(&t,42);assert(TorreInimigoDoAndar(&t,20)==INIMIGO_CHEFE_SINEIRO);
}

static void VerificarDashContinuo(void)
{
    EstadoDefesa d;AtributosJogador j;
    JogadorInicializar(&j);DefesaInicializar(&d);
    DefesaIniciar(&d,PADRAO_CAVALEIRO_DUELO,(Rectangle){42,228,556,94},50,0,1,1);
    d.fase=DEFESA_ATIVA;d.caixa=d.caixa_alvo;
    d.posicao_alma=(Vector2){300,230};d.temporizador_invencibilidade=0;
    d.total_criacoes=1;d.ciclo_ataque=10;
    PerigosAdicionarRetangulo(&d.perigos,(Rectangle){313,210,2,40},0,1,.1f,TIPO_NEUTRO,4,true);
    d.perigos.perigos[0].fase=PERIGO_ATIVA;
    d.projeteis[0]=(Projetil){.ativo=true,.posicao={313,230},.raio=2,.tipo=TIPO_NEUTRO,.dano=4};
    DefesaAtualizarComEntrada(&d,&j,.10f,(EntradaDefesa){.movimento={1,0},.dash_pressionado=true});
    assert(d.posicao_alma.x>332); /* end lies beyond wall: endpoint-only would miss it */
    assert(j.hp==36&&d.orcamento.recebido==4); /* wall + bullet share the same protection */
    assert(d.temporizador_invencibilidade>.7f);
}
static void VerificarTetosPorAtaque(void)
{
    int hps[]={40,41,80};
    for(int id=PADRAO_PILOTO_POEIRA_ASA;id<PADRAO_TOTAL;id++)
    for(int h=0;h<3;h++)for(int tipo=0;tipo<TIPO_TOTAL;tipo++){
        AtributosJogador j;JogadorInicializar(&j);j.hp=j.vida_maxima=hps[h];
        j.tipo_defesa=(Tipo)tipo;j.multiplicador_dano_recebido=1.25f;
        OrcamentoDano o;
        int percentual=BalanceamentoFinal((IdPadraoProjetil)id)?45:35;
        DanoIniciarRodada(&o,j.vida_maxima,BalanceamentoTetoImpacto((IdPadraoProjetil)id),percentual);
        for(int acerto=0;acerto<100;acerto++){
            o.invulnerabilidade=0;
            int dano=DanoAplicar(&o,&j,10000,TIPO_AGUA);
            assert(dano<=BalanceamentoTetoImpacto((IdPadraoProjetil)id));
        }
        DanoAplicarResidual(&o,&j,9999);
        assert(o.recebido==hps[h]*percentual/100);
        assert(hps[h]-j.hp==o.recebido);
    }
}

/* Um reagente neutralizado nunca pode voltar a aparecer nem voltar a ferir. */
static void VerificarReceitaSemReaparecer(void)
{
    const int taxas[]={30,60,120,144};
    for(int arena=0;arena<2;arena++)for(int taxa=0;taxa<4;taxa++){
        EstadoDefesa d={0};d.id_padrao=PADRAO_PLANTA_RECEITA_REVERSA;
        d.caixa=(Rectangle){160,148,arena?290:320,178};d.fator_movimento_ataque=1;
        unsigned char estado[22][3]={{0}};int retiradas=0;
        for(int f=0;f<=taxas[taxa]*6;f++){
            bool visto[22][3]={{false}};QuadroPiloto q;PilotoQuadro(&d,(float)f/taxas[taxa],&q);
            for(int i=0;i<q.total;i++){
                const AmeacaPiloto *a=&q.ameacas[i];
                if(a->sprite==SPRITE_PLANTA_ANTIDOTO)assert(!a->ativo);
                if(a->sprite!=SPRITE_PLANTA_VENENO||!a->ativo)continue;
                int onda=(int)(a->evento-20000)/100,linha=(int)(a->evento-20000)%100;
                assert(onda>=0&&onda<22&&linha>=0&&linha<3);visto[onda][linha]=true;
            }
            for(int onda=0;onda<22;onda++)for(int linha=0;linha<3;linha++){
                if(visto[onda][linha]){
                    if(estado[onda][linha]==2)fprintf(stderr,"Gota roxa reapareceu: fps=%d onda=%d linha=%d t=%.3f\n",taxas[taxa],onda,linha,(float)f/taxas[taxa]);
                    assert(estado[onda][linha]!=2);estado[onda][linha]=1;
                }else if(estado[onda][linha]==1){estado[onda][linha]=2;retiradas++;}
            }
        }
        assert(retiradas>0);
    }
    puts("Receita: neutralizacao permanente e antidoto inofensivo em 30/60/120/144 FPS.");
}
int main(int argc, char **argv)
{

    if(argc>1&&(strcmp(argv[1],"--idle")==0||strcmp(argv[1],"--idle-plants")==0||strcmp(argv[1],"--idle-water")==0)) {
        int falhas=0;
        for(int id=strcmp(argv[1],"--idle-water")==0?PADRAO_AGUA_TSUNAMI:strcmp(argv[1],"--idle-plants")==0?PADRAO_PLANTA_ESTILHACO_LANCA:PADRAO_PILOTO_POEIRA_ASA;id<(strcmp(argv[1],"--idle-plants")==0?PADRAO_PLANTA_RITUAL+1:PADRAO_TOTAL);id++){
            printf("%s:",PilotoNome((IdPadraoProjetil)id));
            for(int p=0;p<5;p++){
                EstadoDefesa d;AtributosJogador j;DefesaInicializar(&d);JogadorInicializar(&j);
                DefesaIniciar(&d,(IdPadraoProjetil)id,(Rectangle){42,228,556,94},50,0,1,1);
                d.fase=DEFESA_ATIVA;d.caixa=d.caixa_alvo;
                d.posicao_alma=p==0?(Vector2){320,237}:(Vector2){p%2?176:464,p<=2?164:310};
                for(int f=0;f<900&&d.fase!=DEFESA_FINALIZADA;f++)
                    DefesaAtualizarComEntrada(&d,&j,1.0f/60,(EntradaDefesa){0});
                printf(" %d",40-j.hp);
                if(j.hp==40){fprintf(stderr,"IDLE SEGURO id=%d ponto=%d\n",id,p);falhas++;}
            }
            puts("");fflush(stdout);
        }
        return falhas?1:0;
    }

    if(argc>1&&strcmp(argv[1],"--recipe")==0){VerificarReceitaSemReaparecer();return 0;}
    VerificarProtecao();VerificarDashContinuo();VerificarTetosPorAtaque();VerificarReceitaSemReaparecer();
    if(argc>1&&strcmp(argv[1],"--checks")==0){puts("Tetos, sangramento, i-frames, dash e campanha verificados.");return 0;}
    int fpss[]={30,60,120,144};
    int total=0;
    const char *destino=getenv("PILOT_ROUTE_CSV");
    FILE *csv=fopen(destino?destino:"pilot-routes.csv","w");assert(csv);
    int ultimo=argc>2?atoi(argv[2]):(argc>1&&strcmp(argv[1],"--plants")==0?PADRAO_PLANTA_RITUAL:PADRAO_TOTAL-1);
    fprintf(csv,"ataque;fps;inicio_x;inicio_y;fase;rota;dano\n");
    for(int id=argc>1?(strcmp(argv[1],"--water")==0?PADRAO_AGUA_TSUNAMI:strcmp(argv[1],"--plants")==0?PADRAO_PLANTA_ESTILHACO_LANCA:atoi(argv[1])):PADRAO_PILOTO_POEIRA_ASA;id<=ultimo;id++){
        int fases=PilotoBoss((IdPadraoProjetil)id)?2:1;
        for(int fase=0;fase<fases;fase++)for(int f=0;f<4;f++)for(int p=0;p<5;p++){
            Rectangle caixa=fase?(Rectangle){175,148,290,178}:(Rectangle){160,148,320,178};
            Vector2 inicio=p==0?(Vector2){320,237}:
                (Vector2){p%2?caixa.x+16:caixa.x+caixa.width-16,p<=2?164:310};
            RotaValidada rota;
            bool ok=PilotoEncontrarRota((IdPadraoProjetil)id,caixa,inicio,fpss[f],&rota);
            if(!ok){fprintf(stderr,"SEM ROTA: id=%d fps=%d ponto=%d fase=%d\n",id,fpss[f],p,fase);fclose(csv);return 1;}
            int dano=-1;bool real=PilotoReproduzirRota((IdPadraoProjetil)id,&rota,40,fase,&dano);
            fprintf(csv,"%s;%d;%.0f;%.0f;%d;%d;%d\n",PilotoNome((IdPadraoProjetil)id),fpss[f],inicio.x,inicio.y,fase,real,dano);fflush(csv);
            if(!real){fprintf(stderr,"REPLAY FALHOU: id=%d fps=%d ponto=%d fase=%d dano=%d\n",id,fpss[f],p,fase,dano);fclose(csv);return 1;}
            total++;
        }
        printf("%s: rotas reproduzidas sem dano em 30/60/120/144 FPS.\n",PilotoNome((IdPadraoProjetil)id));fflush(stdout);
    }
    fclose(csv);printf("%d replays; ataques selecionados, 5 inicios, 4 FPS; chefes tambem na arena de fase 2.\n",total);return 0;
}
