#include "balance.h"

#include <stdio.h>

#define SIMULATED_RUNS 1000
#define TURN_LIMIT 18

typedef struct { float win_rate; float average_hp; } Simulation;

static float RandomUnit(unsigned int *seed)
{
    *seed = *seed * 1664525u + 1013904223u;
    return (float)(*seed & 0xffffu) / 65535.0f;
}

/* A headless turn simulation: HP, damage and a turn cap all decide the outcome. */
static Simulation SimulateBattles(int floor, int style, GameDifficulty difficulty)
{
    const bool boss = floor == 5 || floor == 10;
    const float enemy_hp = (boss ? 76.0f : 29.0f) + (float)(floor - 1) *
                           (boss ? BALANCE_BOSS_HP_PER_FLOOR : BALANCE_COMMON_HP_PER_FLOOR);
    const float player_damage = style == 0 ? 8.3f + (float)floor * 0.35f :
                                (style == 1 ? 10.0f + (float)floor * 1.5f :
                                              7.4f + (float)floor * 0.25f);
    const float player_hp_max = 40.0f + (float)(floor - 1) * 2.0f;
    const float received_per_turn = (2.1f + (float)floor * 0.36f +
                                     (boss ? BALANCE_BOSS_PRESSURE * 12.0f : 0.0f)) *
                                    BalanceIncomingDamageMultiplier(difficulty) *
                                    BalanceFloorDamageMultiplier(floor);
    unsigned int seed = (unsigned int)(floor * 313 + style * 97 + difficulty * 7919);
    int wins = 0;
    float hp_sum = 0.0f;

    for (int run = 0; run < SIMULATED_RUNS; run++) {
        float player_hp = player_hp_max;
        float target_hp = enemy_hp;
        for (int turn = 0; turn < TURN_LIMIT && player_hp > 0.0f && target_hp > 0.0f; turn++) {
            const float qte = style == 0 ? 0.30f + RandomUnit(&seed) * 0.55f :
                              (style == 1 ? 1.00f + RandomUnit(&seed) * 0.36f :
                                            0.75f + RandomUnit(&seed) * 0.35f);
            target_hp -= player_damage * qte;
            if (target_hp <= 0.0f) break;
            player_hp -= received_per_turn * (0.82f + RandomUnit(&seed) * 0.36f);
        }
        if (target_hp <= 0.0f && player_hp > 0.0f) {
            wins++;
            hp_sum += player_hp;
        }
    }
    return (Simulation){(float)wins / SIMULATED_RUNS, wins > 0 ? hp_sum / wins : 0.0f};
}

int main(void)
{
    static const char *styles[] = {"aleatorio", "tipos", "poupador"};
    static const char *difficulties[] = {"Calmo", "Normal", "Dificil"};

    for (int difficulty = DIFFICULTY_CALM; difficulty <= DIFFICULTY_HARD; difficulty++) {
        printf("%s\n", difficulties[difficulty]);
        for (int floor = 1; floor <= 10; floor++) {
            printf("Andar %d:", floor);
            for (int style = 0; style < 3; style++) {
                const Simulation result = SimulateBattles(floor, style, (GameDifficulty)difficulty);
                printf(" %s %.0f%%/HP %.1f", styles[style], result.win_rate * 100.0f,
                       result.average_hp);
            }
            puts("");
        }
    }
    puts("Derrota simulada: HP <= 0 ou alvo vivo apos 18 turnos.");
    return 0;
}
