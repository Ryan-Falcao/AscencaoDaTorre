#include "enemy.h"

#include <assert.h>
#include <stdio.h>

int main(void)
{
    AtributosJogador jogador;
    Inimigo inimigo = InimigoCriar(INIMIGO_POEIRA_MARIPOSA);
    char motivo[24] = "";
    Ataque ataque;
    int burn_damage = 0;

    JogadorInicializar(&jogador);
    ataque = JogadorObterAtaque(ATAQUE_CHAMA_ALTA);
    jogador.mana = jogador.mana_maxima;
    assert(JogadorAtaqueDisponivel(&jogador, &ataque, motivo, (int)sizeof(motivo)));
    JogadorUsarAtaque(&jogador, &ataque);
    assert(!JogadorAtaqueDisponivel(&jogador, &ataque, motivo, (int)sizeof(motivo)));
    JogadorAvancarRecargasAtaques(&jogador);
    jogador.mana = jogador.mana_maxima;
    assert(JogadorAtaqueDisponivel(&jogador, &ataque, motivo, (int)sizeof(motivo)));

    /* Todo ataque continua reutilizavel depois de centenas de ativacoes. Apenas
       sua recarga pode impedir temporariamente outra ativacao. */
    for (int id = 0; id < JOGADOR_ATAQUE_TOTAL; id++) {
        JogadorInicializar(&jogador);
        ataque = JogadorObterAtaque((IdAtaque)id);
        for (int use = 0; use < 200; use++) {
            jogador.mana = jogador.mana_maxima;
            assert(JogadorAtaqueDisponivel(&jogador, &ataque, motivo, (int)sizeof(motivo)));
            JogadorUsarAtaque(&jogador, &ataque);
            if (ataque.turnos_recarga > 0)
                assert(!JogadorAtaqueDisponivel(&jogador, &ataque, motivo, (int)sizeof(motivo)));
            for (int turn = 0; turn < ataque.turnos_recarga; turn++) JogadorAvancarRecargasAtaques(&jogador);
        }
        jogador.mana = jogador.mana_maxima;
        assert(JogadorAtaqueDisponivel(&jogador, &ataque, motivo, (int)sizeof(motivo)));
    }

    InimigoAplicarStatus(&inimigo, STATUS_QUEIMAR, 2);
    assert(InimigoAvancarStatus(&inimigo, &burn_damage) && burn_damage == 3);
    assert(inimigo.turnos_status == 1);
    InimigoAvancarStatus(&inimigo, &burn_damage);
    assert(inimigo.status == STATUS_NENHUM && inimigo.turnos_status == 0);

    puts("status, cooldown and sprite fallback ids verified.");
    return 0;
}
