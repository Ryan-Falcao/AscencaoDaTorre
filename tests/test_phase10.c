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
    ataque = JogadorAtaqueRecompensa(ATAQUE_RAIZ_RAPIDA);
    assert(JogadorAtaqueDisponivel(&jogador, &ataque, motivo, (int)sizeof(motivo)));
    JogadorUsarAtaque(&jogador, &ataque);
    assert(!JogadorAtaqueDisponivel(&jogador, &ataque, motivo, (int)sizeof(motivo)));
    JogadorAvancarRecargasAtaques(&jogador);
    assert(JogadorAtaqueDisponivel(&jogador, &ataque, motivo, (int)sizeof(motivo)));

    /* Todo ataque continua reutilizavel depois de centenas de ativacoes. Apenas
       sua recarga pode impedir temporariamente outra ativacao. */
    for (int id = 0; id < JogadorTotalAtaquesRecompensa(); id++) {
        JogadorInicializar(&jogador);
        ataque = JogadorAtaqueRecompensa(id);
        for (int use = 0; use < 200; use++) {
            assert(JogadorAtaqueDisponivel(&jogador, &ataque, motivo, (int)sizeof(motivo)));
            JogadorUsarAtaque(&jogador, &ataque);
            if (ataque.turnos_recarga > 0)
                assert(!JogadorAtaqueDisponivel(&jogador, &ataque, motivo, (int)sizeof(motivo)));
            for (int turn = 0; turn < ataque.turnos_recarga; turn++) JogadorAvancarRecargasAtaques(&jogador);
        }
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
