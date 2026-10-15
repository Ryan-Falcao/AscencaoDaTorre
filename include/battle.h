#ifndef BATTLE_H
#define BATTLE_H

#include "enemy.h"
#include "ui.h"

typedef enum {
    BATTLE_INTRO,
    BATTLE_PLAYER_MENU,
    BATTLE_PLAYER_ACTION,
    BATTLE_PLAYER_QTE,
    BATTLE_ENEMY_TURN,
    BATTLE_VICTORY,
    BATTLE_DEFEAT
} BattlePhase;

typedef enum {
    MENU_ATTACK,
    MENU_ACT,
    MENU_ITEM,
    MENU_SPARE
} MainMenuOption;

typedef enum {
    ACTION_NONE,
    ACTION_ATTACKS,
    ACTION_ACTS,
    ACTION_ITEMS,
    ACTION_MESSAGE
} ActionMenu;

typedef enum {
    BATTLE_OUTCOME_NONE,
    BATTLE_OUTCOME_DAMAGE,
    BATTLE_OUTCOME_MERCY,
    BATTLE_OUTCOME_DEFEAT
} BattleOutcome;

typedef struct {
    BattleOutcome outcome;
    int coins;
    int xp;
} BattleResult;

typedef struct {
    BattlePhase phase;
    BattlePhase next_phase;
    ActionMenu action_menu;
    int main_selection;
    int floor;
    int action_selection;
    float enemy_turn_timer;
    float elapsed_time;
    int floating_damage;
    float floating_damage_timer;
    float last_effectiveness;
    int qte_attack_index;
    float screen_shake_timer;
    float displayed_mercy;
    float displayed_aggression;
    float stat_change_timer;
    int mercy_change_popup;
    int aggression_change_popup;
    bool victory_by_mercy;
    BattleResult result;
    DefenseState defense;
} Battle;

void BattleInit(Battle *battle, TypewriterText *dialogue, int floor);
BattleResult BattleUpdate(Battle *battle, PlayerStats *player, Enemy *enemy,
                          TypewriterText *dialogue, Vector2 mouse_position,
                          bool mouse_pressed, Qte *qte);
void BattleDraw(const Battle *battle, const PlayerStats *player, const Enemy *enemy,
                const TypewriterText *dialogue, const Qte *qte, bool line_tremor);

#endif
