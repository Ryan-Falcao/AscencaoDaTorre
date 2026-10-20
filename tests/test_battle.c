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
        assert(inimigo.total_padroes == total_esperado);
        JogadorInicializar(&jogador); BatalhaInicializar(&batalha, &dialogo, 1);
        for (int turn = 0; turn < inimigo.total_padroes * 2; turn++) {
            batalha.fase = BATALHA_JOGADOR_ACAO; batalha.menu_acao = ACAO_MENSAGEM;
            batalha.proxima_fase = BATALHA_INIMIGO_TURNO;
            InterfaceDefinirTexto(&dialogo, "Proximo turno"); InterfaceConfirmarTexto(&dialogo);
            BatalhaAtualizar(&batalha, &jogador, &inimigo, &dialogo, (Vector2){100, 240}, true, &qte, &audio);
            assert(batalha.fase == BATALHA_INIMIGO_TURNO);
            assert(batalha.defesa.id_padrao == inimigo.ids_padroes[turn % inimigo.total_padroes]);
        }
    }
    puts("Repertorios completos alternam em todos os niveis de agressividade.");
    return 0;
}
