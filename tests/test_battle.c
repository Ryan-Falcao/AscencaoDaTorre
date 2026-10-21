#include "pilot_patterns.h"
#include "battle.h"
#include <assert.h>
#include <stdio.h>

int main(void)
{
    {
        Batalha batalha;
        AtributosJogador jogador;
        Inimigo inimigo = InimigoCriar(INIMIGO_POEIRA_MARIPOSA);
        TextoDigitado dialogo = {0};
        Qte qte = {0};
        EstadoAudio audio = {0};
        JogadorInicializar(&jogador);
        BatalhaInicializar(&batalha, &dialogo, 1);
        batalha.fase = BATALHA_JOGADOR_ACAO;
        batalha.menu_acao = ACAO_ATAQUES;
        const Ataque *ataque = &jogador.ataques[2];
        jogador.recargas_ataques[ataque->id] = 1;

        BatalhaAtualizar(&batalha, &jogador, &inimigo, &dialogo, (Vector2){100, 289}, true, &qte, &audio);
        assert(batalha.menu_acao == ACAO_MENSAGEM);
        /* O primeiro clique revela a mensagem; o segundo a fecha. */
        BatalhaAtualizar(&batalha, &jogador, &inimigo, &dialogo, (Vector2){100, 240}, true, &qte, &audio);
        BatalhaAtualizar(&batalha, &jogador, &inimigo, &dialogo, (Vector2){100, 240}, true, &qte, &audio);
        assert(batalha.fase == BATALHA_JOGADOR_ACAO);
        assert(batalha.menu_acao == ACAO_ATAQUES);
        assert(batalha.total_turnos == 0);
        assert(jogador.hp == jogador.vida_maxima);
        BatalhaAtualizar(&batalha, &jogador, &inimigo, &dialogo, (Vector2){100, 247}, true, &qte, &audio);
        assert(batalha.fase == BATALHA_JOGADOR_QTE);
        assert(batalha.indice_ataque_qte == 0);
    }
    puts("Ataques em recarga retornam a lista sem gastar turno.");
    /* Exercita o controlador real de turnos: a agressividade nao pode reduzir
       o repertorio novamente a um unico ataque selecionado repetidamente. */
    for (int id = 0; id < INIMIGO_TOTAL; id++) for (int agressividade = 0; agressividade <= 100; agressividade += 50) {
        Batalha batalha; AtributosJogador jogador; TextoDigitado dialogo = {0};
        Qte qte = {0}; EstadoAudio audio = {0};
        Inimigo inimigo = InimigoCriar((IdInimigo)id);
        inimigo.agressividade = agressividade;
        int total_esperado = id == INIMIGO_CHEFE_ULTIMA_TORRE ? 4 :
                             (id == INIMIGO_BRASA_PASSARO ? 4 : (inimigo.chefe ? 5 : 3));
        if (PilotoEh(inimigo.ids_padroes[0])) total_esperado=3;
        assert(inimigo.total_padroes == total_esperado);
        JogadorInicializar(&jogador); BatalhaInicializar(&batalha, &dialogo, 1);
        for (int turn = 0; turn < inimigo.total_padroes * 2; turn++) {
            batalha.fase = BATALHA_JOGADOR_ACAO; batalha.menu_acao = ACAO_MENSAGEM;
            batalha.proxima_fase = BATALHA_INIMIGO_TURNO;
            InterfaceDefinirTexto(&dialogo, "Proximo turno"); InterfaceConfirmarTexto(&dialogo);
            BatalhaAtualizar(&batalha, &jogador, &inimigo, &dialogo, (Vector2){100, 240}, true, &qte, &audio);
            assert(batalha.fase == BATALHA_INIMIGO_TURNO);
            if (!PilotoEh(inimigo.ids_padroes[0]))
                assert(batalha.defesa.id_padrao == inimigo.ids_padroes[turn % inimigo.total_padroes]);
            else {
                assert(PilotoEh(batalha.defesa.id_padrao));
                if (turn == 0) assert(batalha.defesa.id_padrao == inimigo.ids_padroes[0]);
                if (inimigo.hp*100>inimigo.vida_maxima*65)
                    assert(batalha.defesa.id_padrao != inimigo.ids_padroes[2]);
            }
        }
    }
    puts("Repertorios completos alternam em todos os niveis de agressividade.");

    const IdInimigo pilotos[]={INIMIGO_POEIRA_MARIPOSA,INIMIGO_LADRAO_MORTALHA,INIMIGO_CHEFE_AFOGADO_CORAL,INIMIGO_SOLDADO_SARCAS,INIMIGO_HERBALISTA_PESTE,INIMIGO_MENTA_LIMO,INIMIGO_ESPINHO_LEBRE,INIMIGO_CHEFE_CULTISTA_RAIZES,INIMIGO_LEGIONARIO_AFOGADO,INIMIGO_BRUXA_LODO,INIMIGO_REVENANTE_GELO,INIMIGO_BRILHO_SAPO};
    const int vida_percentual[]={75,50,25};
    for(int e=0;e<(int)(sizeof(pilotos)/sizeof(pilotos[0]));e++)for(int faixa=0;faixa<3;faixa++){
        Batalha b;TextoDigitado texto={0};Qte q={0};EstadoAudio audio={0};
        Inimigo inimigo=InimigoCriar(pilotos[e]);AtributosJogador j;JogadorInicializar(&j);
        InimigoReceberDano(&inimigo,inimigo.vida_maxima*(100-vida_percentual[faixa])/100,1);
        BatalhaInicializar(&b,&texto,10);
        IdPadraoProjetil anterior=PADRAO_TOTAL;int especial_anterior=-100;bool viu_dificil=false;
        for(int turno=0;turno<12;turno++){
            b.fase=BATALHA_JOGADOR_ACAO;b.menu_acao=ACAO_MENSAGEM;b.proxima_fase=BATALHA_INIMIGO_TURNO;
            InterfaceDefinirTexto(&texto,"Ataque piloto");InterfaceConfirmarTexto(&texto);
            BatalhaAtualizar(&b,&j,&inimigo,&texto,(Vector2){100,240},true,&q,&audio);
            assert(b.fase==BATALHA_INIMIGO_TURNO);
            IdPadraoProjetil atual=b.defesa.id_padrao;
            assert(atual!=anterior);anterior=atual;
            if(turno==0)assert(atual==inimigo.ids_padroes[0]);
            if(atual==inimigo.ids_padroes[2]){
                viu_dificil=true;
                if(inimigo.chefe){assert(faixa==2);assert(turno-especial_anterior>=3);especial_anterior=turno;}
            }
        }
        if(faixa==0)assert(!viu_dificil);
        if(faixa==2)assert(viu_dificil);
    }
    puts("Piloto: primeiro padrao, progressao por HP, sem repeticao e cooldown do final verificados.");

    return 0;
}
