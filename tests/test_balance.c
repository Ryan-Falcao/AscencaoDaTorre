#include "balance.h"

#include <stdio.h>

#define SIMULATED_RUNS 1000
#define TURN_LIMIT 18

typedef struct { float taxa_vitoria; float vida_media; } Simulacao;

static float UnidadeAleatoria(unsigned int *semente)
{
    *semente = *semente * 1664525u + 1013904223u;
    return (float)(*semente & 0xffffu) / 65535.0f;
}

/* Simulacao de turnos sem janela: vida, dano e limite de turnos decidem o resultado. */
static Simulacao SimularBatalhas(int andar, int estilo, DificuldadeJogo dificuldade)
{
    const bool chefe = andar == 5 || andar == 10;
    const float enemy_hp = (chefe ? 76.0f : 29.0f) + (float)(andar - 1) *
                           (chefe ? BALANCEAMENTO_CHEFE_VIDA_PER_ANDAR : BALANCEAMENTO_COMUM_VIDA_PER_ANDAR);
    const float player_damage = estilo == 0 ? 8.3f + (float)andar * 0.35f :
                                (estilo == 1 ? 10.0f + (float)andar * 1.5f :
                                              7.4f + (float)andar * 0.25f);
    const float player_hp_max = 40.0f + (float)(andar - 1) * 2.0f;
    const float received_per_turn = (2.1f + (float)andar * 0.36f +
                                     (chefe ? BALANCEAMENTO_CHEFE_PRESSAO * 12.0f : 0.0f)) *
                                    BalanceamentoMultiplicadorDanoRecebido(dificuldade) *
                                    BalanceamentoMultiplicadorDanoAndar(andar);
    unsigned int semente = (unsigned int)(andar * 313 + estilo * 97 + dificuldade * 7919);
    int wins = 0;
    float hp_sum = 0.0f;

    for (int run = 0; run < SIMULATED_RUNS; run++) {
        float player_hp = player_hp_max;
        float target_hp = enemy_hp;
        for (int turn = 0; turn < TURN_LIMIT && player_hp > 0.0f && target_hp > 0.0f; turn++) {
            const float qte = estilo == 0 ? 0.30f + UnidadeAleatoria(&semente) * 0.55f :
                              (estilo == 1 ? 1.00f + UnidadeAleatoria(&semente) * 0.36f :
                                            0.75f + UnidadeAleatoria(&semente) * 0.35f);
            target_hp -= player_damage * qte;
            if (target_hp <= 0.0f) break;
            player_hp -= received_per_turn * (0.82f + UnidadeAleatoria(&semente) * 0.36f);
        }
        if (target_hp <= 0.0f && player_hp > 0.0f) {
            wins++;
            hp_sum += player_hp;
        }
    }
    return (Simulacao){(float)wins / SIMULATED_RUNS, wins > 0 ? hp_sum / wins : 0.0f};
}

int main(void)
{
    static const char *styles[] = {"aleatorio", "tipos", "poupador"};
    static const char *difficulties[] = {"Calmo", "Normal", "Dificil"};

    for (int dificuldade = DIFICULDADE_CALMA; dificuldade <= DIFICULDADE_DIFICIL; dificuldade++) {
        printf("%s\n", difficulties[dificuldade]);
        for (int andar = 1; andar <= 10; andar++) {
            printf("Andar %d:", andar);
            for (int estilo = 0; estilo < 3; estilo++) {
                const Simulacao resultado = SimularBatalhas(andar, estilo, (DificuldadeJogo)dificuldade);
                printf(" %s %.0f%%/HP %.1f", styles[estilo], resultado.taxa_vitoria * 100.0f,
                       resultado.vida_media);
            }
            puts("");
        }
    }
    puts("Derrota simulada: HP <= 0 ou alvo vivo apos 18 turnos.");
    return 0;
}
