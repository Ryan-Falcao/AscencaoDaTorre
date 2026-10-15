#include "playtest.h"

#include <stdio.h>

#define PLAYTEST_PATH "playtest_log.csv"

static void WriteHeaderIfNeeded(FILE *file)
{
    if (ftell(file) == 0) {
        fputs("kind,floor,enemy,outcome,hp_lost,turns,duration_seconds,qte_perfect,qte_good,qte_bad,qte_error,coins\n", file);
    }
}

void PlaytestLogBattle(int floor, const Enemy *enemy, const BattleResult *result)
{
    FILE *file = fopen(PLAYTEST_PATH, "a+");
    if (file == NULL) return;
    WriteHeaderIfNeeded(file);
    fprintf(file, "battle,%d,%s,%d,%d,%d,%.2f,%d,%d,%d,%d,%d\n", floor, enemy->name,
            result->outcome, result->hp_lost, result->turns, result->duration_seconds,
            result->qte_results[0], result->qte_results[1], result->qte_results[2],
            result->qte_results[3], result->coins);
    fclose(file);
}

void PlaytestLogRun(int floor, int coins, bool victory, float duration_seconds)
{
    FILE *file = fopen(PLAYTEST_PATH, "a+");
    if (file == NULL) return;
    WriteHeaderIfNeeded(file);
    fprintf(file, "run,%d,,%s,0,0,%.2f,0,0,0,0,%d\n", floor,
            victory ? "victory" : "defeat", duration_seconds, coins);
    fclose(file);
}
