#ifndef GAME_H
#define GAME_H

#include "raylib.h"

#include "battle.h"
#include "tower.h"
#include "save.h"
#include "render.h"
#include "audio.h"
#include "playtest.h"
#include "sprites.h"

#define LOGICAL_WIDTH 640
#define LOGICAL_HEIGHT 480

typedef enum {
    TITLE,
    BATTLE,
    REWARD,
    REST,
    SHOP,
    PAUSE,
    GAME_OVER,
    VICTORY,
    RELICS,
    OPTIONS,
    DELETE_CONFIRM
    ,HOW_TO_PLAY, DEBUG_FX
} GameScreen;

typedef struct {
    GameScreen screen;
    GameScreen screen_before_pause;
    int title_selection;
    float elapsed_time;
    bool quit_requested;
    PlayerStats player;
    Enemy enemy;
    Battle battle;
    TypewriterText dialogue;
    Qte qte;
    BattleResult last_battle_result;
    Vector2 mouse_position;
    bool mouse_pressed;
    EnemyId next_test_enemy;
    Tower tower;
    int reward_selection;
    int replacement_selection;
    bool replacing_attack;
    int rest_selection;
    int shop_selection;
    int end_selection;
    float transition_timer;
    unsigned int run_counter;
    char screen_message[96];
    SaveData save;
    RenderState render;
    int relic_selection;
    int option_selection;
    bool delete_yes;
    int pause_selection;
    bool pause_confirm_quit;
    int tutorial_tip;
    float tutorial_timer;
    int tutorial_wait_phase;
    int last_typewriter_character;
    bool telemetry_enabled;
    float run_started_at;
    AudioState audio;
    SpriteSystem sprites;
    int fx_debug_attack;
    int fx_debug_result;
    int pattern_debug_enemy;
    int pattern_debug_id;
} GameState;

void GameInit(GameState *game);
void GameUpdate(GameState *game);
void GameDraw(const GameState *game);

#endif
