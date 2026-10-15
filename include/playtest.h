#ifndef PLAYTEST_H
#define PLAYTEST_H

#include "battle.h"

void PlaytestLogBattle(int floor, const Enemy *enemy, const BattleResult *result);
void PlaytestLogRun(int floor, int coins, bool victory, float duration_seconds);

#endif
