#include "water_patterns.h"
#include "balance.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>

static bool Hit(QuadroPiloto *q,Vector2 a,Vector2 b,float r)
{return PilotoPercursoColide(q,a,b,r,(Vector2){1,0},NULL);}
int main(void)
{
    QuadroPiloto q={0};q.total=1;
    q.ameacas[0]=(AmeacaPiloto){.forma=PILOTO_TRIANGULO,.a={10,10},.b={30,10},.c={20,30},.ativo=true};
    assert(Hit(&q,(Vector2){0,20},(Vector2){40,20},2));
    assert(!Hit(&q,(Vector2){0,0},(Vector2){40,0},2));
    q.ameacas[0]=(AmeacaPiloto){.forma=PILOTO_FORA_CIRCULO,.a={20,20},.raio=20,.ativo=true};
    assert(!Hit(&q,(Vector2){20,20},(Vector2){25,20},4));
    assert(Hit(&q,(Vector2){20,20},(Vector2){40,20},4));
    q.ameacas[0]=(AmeacaPiloto){.forma=PILOTO_RETANGULO,.area={18,0,8,100},.ativo=true,.regra=3};
    assert(Hit(&q,(Vector2){0,20},(Vector2){40,20},4));q.dash_ativo=true;
    assert(!Hit(&q,(Vector2){0,20},(Vector2){40,20},4));
    q.total=2;q.ameacas[1]=(AmeacaPiloto){.a={35,20},.b={35,20},.raio=3,.ativo=true};
    assert(Hit(&q,(Vector2){0,20},(Vector2){40,20},4)); /* Dash continua vulneravel aos demais ataques. */
    q=(QuadroPiloto){0};q.total=1;q.ameacas[0]=(AmeacaPiloto){.a={20,80},.b={20,80},.raio=5,.ativo=true};
    assert(!Hit(&q,(Vector2){20,20},(Vector2){20,20},4));
    q.reflete_alma=true;q.caixa_reflexo=(Rectangle){0,0,100,100};assert(Hit(&q,(Vector2){20,20},(Vector2){20,20},4));

    EstadoDefesa tsunami={0};tsunami.id_padrao=PADRAO_AGUA_TSUNAMI;tsunami.caixa=(Rectangle){160,148,320,178};
    tsunami.ancora_movimento=(Vector2){320,237};tsunami.fator_movimento_ataque=1;
    PilotoQuadro(&tsunami,1.48f,&q);
    assert(!Hit(&q,(Vector2){380,252},(Vector2){380,252},1)); /* sombra do escudo */
    assert(Hit(&q,(Vector2){320,252},(Vector2){320,252},1));
    assert(Hit(&q,(Vector2){380,234},(Vector2){380,234},1)); /* face exposta */

    AtributosJogador j;JogadorInicializar(&j);EstadoDefesa d;DefesaInicializar(&d);
    DefesaIniciar(&d,PADRAO_AGUA_ESTACAS,(Rectangle){42,228,556,94},50,0,1,1);
    d.fase=DEFESA_ATIVA;d.caixa=d.caixa_alvo;d.posicao_alma=d.ancora_movimento=(Vector2){320,237};
    d.temporizador_padrao=1.63f;d.total_criacoes=1;d.batida_movimento=0;d.temporizador_invencibilidade=0;
    DefesaAtualizarComEntrada(&d,&j,.01f,(EntradaDefesa){0});
    assert(j.hp<j.vida_maxima&&d.agua.gelo>.25f&&d.temporizador_invencibilidade>.80f);
    Vector2 antes=d.posicao_alma;int vida=j.hp;
    DefesaAtualizarComEntrada(&d,&j,.10f,(EntradaDefesa){.movimento={1,0}});
    assert(fabsf(d.posicao_alma.x-antes.x)<.01f&&j.hp==vida);
    DefesaAtualizarComEntrada(&d,&j,.04f,(EntradaDefesa){.movimento={1,0},.dash_pressionado=true});
    assert(d.agua.gelo==0&&d.posicao_alma.x>antes.x+12&&j.hp==vida);
    assert(BALANCEAMENTO_GELO_CONTATO<BALANCEAMENTO_IFRAMES-.2f);
    DefesaIniciar(&d,PADRAO_AGUA_PEGADAS,(Rectangle){42,228,556,94},50,0,1,1);
    assert(d.agua.gelo==0&&d.agua.total==0);
    puts("Agua: triangulos, bolha, sombra direcional, reflexo, dash seletivo e congelamento real verificados.");
    return 0;
}
