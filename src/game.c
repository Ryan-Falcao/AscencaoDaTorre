#include "game.h"
#include "input.h"
#include "balance.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

static void DrawTower(float time)
{
    const float bob = sinf(time * 1.4f) * 2.0f;

    DrawRectangle(272, (int)(82 + bob), 96, 198, (Color){54, 59, 67, 255});
    DrawTriangle((Vector2){256, 84 + bob}, (Vector2){384, 84 + bob},
                 (Vector2){320, 28 + bob}, (Color){39, 43, 50, 255});
    DrawRectangleLines(272, (int)(82 + bob), 96, 198, BLACK);
    DrawLine(256, (int)(84 + bob), 320, (int)(28 + bob), BLACK);
    DrawLine(320, (int)(28 + bob), 384, (int)(84 + bob), BLACK);

    for (int window_index = 0; window_index < 3; window_index++) {
        const int y = (int)(112 + bob + window_index * 50);
        DrawRectangle(302, y, 36, 28, (Color){255, 218, 112, 255});
        DrawRectangleLines(302, y, 36, 28, BLACK);
    }

    DrawCircle(320, (int)(270 + bob), 12, (Color){225, 123, 145, 255});
    DrawCircleLines(320, (int)(270 + bob), 12, BLACK);
}

static Rectangle GetTitleButton(int option_index)
{
    return (Rectangle){205.0f, 326.0f + option_index * 29.0f, 230.0f, 25.0f};
}

static void DrawTitle(const GameState *game)
{
    const char *options[5] = {"NOVA RUN", "RELIQUIAS", "COMO JOGAR", "OPCOES", "SAIR"};

    DrawCircle(92, 90, 44, (Color){178, 220, 239, 255});
    DrawCircle(551, 101, 30, (Color){255, 207, 144, 255});
    if (sprite_get(&game->sprites, SPRITE_TITLE_TOWER) != NULL) {
        SpriteDrawCentered(&game->sprites, SPRITE_TITLE_TOWER, (Vector2){320, 160}, WHITE,
                           game->elapsed_time, game->sprites.debug_bounds);
    } else {
        DrawTower(game->elapsed_time);
    }

    DrawText("ASCENSAO", 194, 268, 42, BLACK);
    DrawText("DA TORRE", 238, 309, 25, (Color){73, 77, 85, 255});
    DrawText(TextFormat("melhor andar: %d  |  vitorias: %d", game->save.best_floor,
                        game->save.victories), 188, 294, 15, (Color){105, 106, 111, 255});

    for (int option_index = 0; option_index < 5; option_index++) {
        const Rectangle button = GetTitleButton(option_index);
        const bool selected = game->title_selection == option_index;

        UiDrawPanel(button);
        if (selected) {
            DrawRectangleLinesEx((Rectangle){button.x - 4, button.y - 4,
                                               button.width + 8, button.height + 8},
                                 2.0f, (Color){225, 123, 145, 255});
            DrawText(">", 178, (int)button.y + 4, 18, BLACK);
        }
        DrawText(options[option_index], (int)button.x + 24, (int)button.y + 5, 15, BLACK);
    }
}

static void DrawPlaceholder(const char *title, const char *subtitle)
{
    UiDrawPanel((Rectangle){62.0f, 122.0f, 516.0f, 238.0f});
    DrawText(title, 96, 160, 32, BLACK);
    DrawText(subtitle, 96, 218, 18, (Color){75, 78, 84, 255});
    DrawText("Aperte X para voltar ao titulo.", 96, 306, 16, (Color){105, 106, 111, 255});
}

static void DrawChoice(int y, const char *text, bool selected)
{
    if (selected) DrawText(">", 91, y, 20, (Color){225, 95, 122, 255});
    DrawText(text, 118, y, 18, BLACK);
}

static const char *RewardName(const TowerReward *reward)
{
    if (reward->kind == REWARD_ATTACK) return PlayerRewardAttack(reward->id).name;
    if (reward->kind == REWARD_ITEM) return ItemGetDefinition((ItemId)reward->id)->name;
    return reward->kind == REWARD_HP ? "+ HP MAX" : "+ DANO BASE";
}

static void DrawReward(const GameState *game)
{
    UiDrawPanel((Rectangle){62, 72, 516, 325});
    DrawText("RECOMPENSA", 223, 94, 26, BLACK);
    DrawText(TextFormat("%d moedas  |  %d XP  |  %s", game->last_battle_result.coins,
                        game->last_battle_result.xp,
                        game->last_battle_result.outcome == BATTLE_OUTCOME_MERCY ? "POUPAR: BONUS" : "VITORIA"),
             114, 130, 16, DARKGRAY);
    if (game->replacing_attack) {
        DrawText("Escolha o ataque para substituir:", 104, 172, 18, BLACK);
        DrawText("W/S ou setas: escolher | ESPACO/ENTER: confirmar | X: voltar",
                 104, 383, 13, DARKGRAY);
        for (int index = 0; index < game->player.attack_count; index++) {
            DrawChoice(208 + index * 30, game->player.attacks[index].name,
                       game->replacement_selection == index);
        }
    } else {
        DrawText("Escolha uma opcao:", 104, 172, 18, BLACK);
        for (int index = 0; index < 3; index++) {
            DrawChoice(208 + index * 42, RewardName(&game->tower.rewards[index]),
                       game->reward_selection == index);
        }
    }
}

static void DrawRest(const GameState *game)
{
    UiDrawPanel((Rectangle){62, 104, 516, 260});
    DrawText("PONTO DE DESCANSO", 175, 132, 26, BLACK);
    DrawText(TextFormat("HP %d / %d", game->player.hp, game->player.hp_max), 225, 170, 18, DARKGRAY);
    DrawChoice(222, "DESCANSAR  (+50% HP)", game->rest_selection == 0);
    DrawChoice(264, "TREINAR  (+1 dano permanente)", game->rest_selection == 1);
}

static void DrawShop(const GameState *game)
{
    UiDrawPanel((Rectangle){62, 78, 516, 322});
    DrawText("LOJA DA TORRE", 205, 104, 26, BLACK);
    DrawText(TextFormat("MOEDAS: %d", game->tower.coins), 235, 140, 18, DARKGRAY);
    for (int index = 0; index < 3; index++) {
        const ItemDefinition *item = ItemGetDefinition(game->tower.shop_items[index]);
        if (sprite_get(&game->sprites, SpriteItemId(item->id)) != NULL) {
            SpriteDrawCentered(&game->sprites, SpriteItemId(item->id),
                               (Vector2){103, 181.0f + index * 42.0f}, WHITE, 0.0f,
                               game->sprites.debug_bounds);
        }
        DrawChoice(187 + index * 42, TextFormat("%s - %d moedas", item->name,
                                                game->tower.shop_prices[index]),
                   game->shop_selection == index);
    }
    DrawChoice(325, "SAIR E SUBIR", game->shop_selection == 3);
    if (game->shop_selection < 3) {
        DrawText(ItemGetDefinition(game->tower.shop_items[game->shop_selection])->description,
                 103, 365, 14, DARKGRAY);
    }
    if (game->screen_message[0] != '\0') DrawText(game->screen_message, 103, 382, 14, MAROON);
}

static void DrawRelics(const GameState *game)
{
    UiDrawPanel((Rectangle){62, 104, 516, 270});
    DrawText("RELIQUIAS", 246, 132, 28, BLACK);
    DrawText(TextFormat("FRAGMENTOS: %d", game->save.fragments), 218, 169, 18, DARKGRAY);
    DrawChoice(220, "Erva Lunar inicial - 8 fragmentos", game->relic_selection == 0);
    DrawChoice(264, "Raio Manso inicial - 10 fragmentos", game->relic_selection == 1);
    DrawText("X/Backspace: voltar", 205, 331, 14, DARKGRAY);
}

static void DrawOptions(const GameState *game)
{
    static const char *difficulty_names[] = {"CALMO", "NORMAL", "DIFICIL"};
    UiDrawPanel((Rectangle){62, 30, 516, 420});
    DrawText("OPCOES", 260, 120, 28, BLACK);
    DrawChoice(173, TextFormat("Musica: %d%%", (int)(game->save.music_volume * 100.0f)), game->option_selection == 0);
    DrawChoice(215, TextFormat("Efeitos: %d%%", (int)(game->save.effects_volume * 100.0f)), game->option_selection == 1);
    DrawChoice(257, TextFormat("Tela cheia: %s", game->save.fullscreen ? "SIM" : "NAO"), game->option_selection == 2);
    DrawChoice(299, TextFormat("Tremor de linha: %s", game->save.line_tremor ? "SIM" : "NAO"), game->option_selection == 3);
    DrawChoice(341, TextFormat("Efeitos reduzidos: %s", game->save.reduced_effects ? "SIM" : "NAO"), game->option_selection == 4);
    DrawChoice(383, TextFormat("Dificuldade: %s", difficulty_names[game->save.difficulty]), game->option_selection == 5);
    DrawChoice(413, "APAGAR SAVE", game->option_selection == 6);
    DrawChoice(435, "REVER TUTORIAL", game->option_selection == 7);
    DrawText("F11 alterna tela cheia.", 106, 456, 14, DARKGRAY);
}

enum { TUTORIAL_MENU = 1, TUTORIAL_TYPES = 2, TUTORIAL_MERCY = 4,
       TUTORIAL_DEFENSE = 8, TUTORIAL_QTE = 16, TUTORIAL_ALL = 31 };

static const char *TutorialText(int tip)
{
    static const char *const tips[] = {
        "Use WASD ou setas para escolher ATACAR, AGIR, ITEM e POUPAR.",
        "AGIR > Analisar revela o tipo e a fraqueza do inimigo.",
        "Mercy permite poupar; agressividade alta deixa a defesa mais perigosa.",
        "Na defesa, mova a alma com WASD ou setas e fuja dos projeteis.",
        "Nos QTEs, use ESPACO/ENTER/Z. Sequencias usam W, A, S e D."
    };
    return tips[tip];
}

static bool UpdateTutorial(GameState *game)
{
    int required = 0;

    if (game->save.tutorial_done) return false;
    if (game->tutorial_timer < 0.0f) {
        if (game->tutorial_wait_phase == (int)game->battle.phase) return false;
        game->tutorial_timer = 0.0f;
        game->tutorial_wait_phase = -1;
    }
    if (game->tutorial_timer > 0.0f) {
        if (input_cancel_pressed()) {
            /* X/Backspace is an explicit "close all tips" command. */
            game->save.tutorial_done = true;
            game->tutorial_timer = 0.0f;
        } else if (input_confirm_pressed()) {
            /* Confirm closes only this tip; the next waits for another battle state. */
            game->tutorial_timer = -1.0f;
            game->tutorial_wait_phase = (int)game->battle.phase;
        }
        else game->tutorial_timer -= GetFrameTime();
        return true;
    }
    if (game->battle.phase == BATTLE_PLAYER_MENU) {
        if ((game->save.tutorial_seen & TUTORIAL_MENU) == 0) required = TUTORIAL_MENU;
        else if ((game->save.tutorial_seen & TUTORIAL_TYPES) == 0) required = TUTORIAL_TYPES;
        else if ((game->save.tutorial_seen & TUTORIAL_MERCY) == 0) required = TUTORIAL_MERCY;
    } else if (game->battle.phase == BATTLE_ENEMY_TURN && (game->save.tutorial_seen & TUTORIAL_DEFENSE) == 0) {
        required = TUTORIAL_DEFENSE;
    } else if (game->battle.phase == BATTLE_PLAYER_QTE && (game->save.tutorial_seen & TUTORIAL_QTE) == 0) {
        required = TUTORIAL_QTE;
    }
    if (required != 0) {
        int tip = 0;
        while ((1 << tip) != required) tip++;
        game->tutorial_tip = tip;
        game->tutorial_timer = 2.4f;
        game->save.tutorial_seen |= (unsigned int)required;
        if (game->save.tutorial_seen == TUTORIAL_ALL) game->save.tutorial_done = true;
        return true;
    }
    return false;
}

static void DrawTutorial(const GameState *game)
{
    const Rectangle box = {54, 180, 532, 58};
    UiDrawPanel(box);
    DrawText("DICA", 72, 191, 14, (Color){203, 84, 104, 255});
    DrawText(TutorialText(game->tutorial_tip), 72, 209, 12, BLACK);
    DrawText("ESPACO/ENTER: avancar  |  X: fechar tutorial", 280, 225, 11, DARKGRAY);
}

static void DrawHowToPlay(void)
{
    UiDrawPanel((Rectangle){48, 54, 544, 370});
    DrawText("COMO JOGAR", 232, 76, 26, BLACK);
    DrawText("Menu: WASD/setas | confirmar: ESPACO, ENTER ou Z | voltar: X", 72, 121, 14, BLACK);
    DrawText("Combate: ataque usa QTE; AGIR revela tipos e cria misericordia.", 72, 155, 14, BLACK);
    DrawText("Defesa: mova a alma com WASD ou setas e desvie dos projeteis.", 72, 187, 14, BLACK);
    DrawText("QTE: ESPACO no timing/segurar; sequencia usa W, A, S e D.", 72, 219, 14, BLACK);
    DrawText("Tipos: Chama > Planta > Agua > Chama | Luz <-> Sombra.", 72, 251, 14, BLACK);
    DrawText("Poupar exige misericordia alta e agressividade baixa.", 72, 283, 14, BLACK);
    DrawText("ESC: pausa. X/Backspace: voltar.", 72, 345, 15, DARKGRAY);
}

static void DrawFxDebug(const GameState *game)
{
    const SpriteId enemy_id = SpriteEnemyId((EnemyId)game->pattern_debug_enemy, false);

    DrawText("TESTE DE PADROES", 195, 28, 25, BLACK);
    DrawText(TextFormat("Padrao %d | inimigo %d", game->pattern_debug_id, game->pattern_debug_enemy),
             70, 66, 18, DARKGRAY);
    DrawText("A/D: padrao | W/S: inimigo | ESPACO: reiniciar | X: voltar", 55, 440, 14, DARKGRAY);
    if (sprite_get(&game->sprites, enemy_id) != NULL) {
        SpriteDrawCentered(&game->sprites, enemy_id, (Vector2){320, 190}, WHITE, game->elapsed_time,
                           game->sprites.debug_bounds);
    } else {
        DrawCircle(320, 190, 35, type_color(TYPE_SHADOW));
        DrawCircleLines(320, 190, 35, BLACK);
    }
    DefenseDraw(&game->battle.defense, !game->save.reduced_effects, &game->sprites);
}

static void DrawPause(const GameState *game)
{
    const char *options[] = {"CONTINUAR", "OPCOES", "COMO JOGAR", "DESISTIR DA RUN", "TITULO"};
    UiDrawPanel((Rectangle){136, 92, 368, 292});
    DrawText("PAUSADO", 248, 119, 27, BLACK);
    if (game->pause_confirm_quit) {
        DrawText("Desistir desta run?", 210, 178, 18, MAROON);
        DrawText("ESPACO confirma | X cancela", 178, 226, 15, DARKGRAY);
        return;
    }
    for (int index = 0; index < 5; index++) DrawChoice(170 + index * 34, options[index], game->pause_selection == index);
}

static void DrawDeleteConfirm(const GameState *game)
{
    UiDrawPanel((Rectangle){93, 158, 454, 164});
    DrawText("Apagar todo o save?", 191, 190, 23, BLACK);
    DrawChoice(245, "SIM", game->delete_yes);
    DrawChoice(275, "NAO", !game->delete_yes);
}

static void StartFloor(GameState *game)
{
    game->enemy = EnemyCreate(TowerEnemyForFloor(&game->tower, game->tower.floor));
    TowerScaleEnemy(&game->enemy, game->tower.floor);
    RenderSetFloor(&game->render, game->tower.floor, game->enemy.boss);
    BattleInit(&game->battle, &game->dialogue, game->tower.floor);
    game->battle.qte_difficulty = BalanceQteDifficulty(game->tower.floor, game->save.difficulty);
    game->transition_timer = 0.70f;
    game->screen = BATTLE;
}

static void StartRun(GameState *game)
{
    game->run_counter++;
    game->run_started_at = (float)GetTime();
    PlayerInit(&game->player);
    game->player.incoming_damage_multiplier = BalanceIncomingDamageMultiplier(game->save.difficulty);
    if (game->save.relic_kind == RELIC_ITEM) PlayerAddItem(&game->player, (ItemId)game->save.relic_id, 1);
    if (game->save.relic_kind == RELIC_ATTACK) PlayerAddAttack(&game->player,
        PlayerRewardAttack(game->save.relic_id), -1);
    TowerInit(&game->tower, (unsigned int)(GetTime() * 1000.0) + game->run_counter * 7919u);
    game->last_battle_result = (BattleResult){0};
    game->screen_message[0] = '\0';
    StartFloor(game);
}

static void FinishRun(GameState *game, bool victory)
{
    if (game->tower.floor > game->save.best_floor) game->save.best_floor = game->tower.floor;
    game->save.total_runs++;
    if (victory) game->save.victories++;
    game->save.mercy_wins += game->tower.mercy_wins;
    game->save.damage_wins += game->tower.damage_wins;
    game->save.fragments += game->tower.floor + (victory ? 15 : 0);
    SaveApplyUnlocks(&game->save);
    SaveWrite(&game->save);
    if (game->telemetry_enabled) {
        PlaytestLogRun(game->tower.floor, game->tower.coins, victory,
                       (float)GetTime() - game->run_started_at);
    }
}

static void ResumePausedRun(GameState *game)
{
    qte_resume_with_guard(&game->qte);
    game->screen = game->screen_before_pause;
}

static void ContinueAfterReward(GameState *game)
{
    const int completed_floor = game->tower.floor;

    if (completed_floor >= TOWER_FLOORS) {
        FinishRun(game, true);
        game->end_selection = 0;
        game->screen = VICTORY;
        return;
    }
    game->tower.floor++;
    if (TowerShouldRest(completed_floor)) game->screen = REST;
    else if (TowerShouldShop(completed_floor)) game->screen = SHOP;
    else StartFloor(game);
}

static void ApplyReward(GameState *game, int selection)
{
    const TowerReward reward = game->tower.rewards[selection];

    if (reward.kind == REWARD_ATTACK) {
        const Attack attack = PlayerRewardAttack(reward.id);
        if (game->player.attack_count >= MAX_EQUIPPED_ATTACKS && !game->replacing_attack) {
            game->replacing_attack = true;
            game->replacement_selection = 0;
            return;
        }
        if (!PlayerAddAttack(&game->player, attack,
                             game->player.attack_count >= MAX_EQUIPPED_ATTACKS ?
                             game->replacement_selection : -1)) {
            /* A reward can never silently consume itself when the attack is already equipped. */
            snprintf(game->screen_message, sizeof(game->screen_message),
                     "Esse ataque ja esta equipado.");
            game->replacing_attack = false;
            return;
        }
    } else if (reward.kind == REWARD_ITEM) {
        PlayerAddItem(&game->player, (ItemId)reward.id, reward.value);
    } else if (reward.kind == REWARD_HP) {
        game->player.hp_max += reward.value;
        PlayerHeal(&game->player, reward.value * 2);
    } else {
        game->player.permanent_damage_bonus += reward.value / 2;
    }
    game->replacing_attack = false;
    ContinueAfterReward(game);
}

void GameInit(GameState *game)
{
    memset(game, 0, sizeof(*game));
    game->screen = TITLE;
    game->tutorial_wait_phase = -1;
    game->screen_before_pause = TITLE;
    game->title_selection = 0;
    game->elapsed_time = 0.0f;
    game->quit_requested = false;
    SaveLoad(&game->save);
    RenderInit(&game->render, game->save.line_tremor);
    AudioInit(&game->audio, game->save.music_volume, game->save.effects_volume);
    SpritesInit(&game->sprites);
    if (game->save.fullscreen) ToggleFullscreen();
    game->next_test_enemy = ENEMY_DUST_MOTH;
    game->run_counter = 0;
    game->last_battle_result = (BattleResult){0};
    PlayerInit(&game->player);
    TowerInit(&game->tower, 1u);
    game->enemy = EnemyCreate(ENEMY_DUST_MOTH);
    BattleInit(&game->battle, &game->dialogue, 1);
}

void GameUpdate(GameState *game)
{
    game->elapsed_time += GetFrameTime();
    if (input_confirm_pressed()) AudioPlay(&game->audio, AUDIO_CONFIRM);
    if (input_cancel_pressed()) AudioPlay(&game->audio, AUDIO_CANCEL);
    if (input_up_pressed() || input_down_pressed() || input_left_pressed() || input_right_pressed()) {
        AudioPlay(&game->audio, AUDIO_CURSOR);
    }
#ifndef NDEBUG
    if (IsKeyPressed(KEY_F8) && game->screen != DEBUG_FX) {
        game->screen_before_pause = game->screen;
        game->screen = DEBUG_FX;
        game->pattern_debug_enemy = ENEMY_DUST_MOTH;
        game->pattern_debug_id = PATTERN_RAIN;
        PlayerInit(&game->player);
        DefenseInit(&game->battle.defense);
        DefenseStart(&game->battle.defense, PATTERN_RAIN, (Rectangle){42, 228, 556, 94},
                     0, 0, 1.0f, 1.0f);
        game->battle.defense.debug_invincible = true;
    }
#endif

    if (game->screen == TITLE) {
        bool confirmed = input_confirm_pressed();

        if (input_up_pressed()) {
            game->title_selection = (game->title_selection + 4) % 5;
        }
        if (input_down_pressed()) {
            game->title_selection = (game->title_selection + 1) % 5;
        }
        if (game->mouse_pressed) {
            for (int option_index = 0; option_index < 5; option_index++) {
                if (CheckCollisionPointRec(game->mouse_position, GetTitleButton(option_index))) {
                    game->title_selection = option_index;
                    confirmed = true;
                }
            }
        }
        if (confirmed) {
            if (game->title_selection == 0) StartRun(game);
            else if (game->title_selection == 1) game->screen = RELICS;
            else if (game->title_selection == 2) game->screen = HOW_TO_PLAY;
            else if (game->title_selection == 3) game->screen = OPTIONS;
            else game->quit_requested = true;
        }
    } else if (game->screen == BATTLE) {
        BattleResult result = {0};

        if (game->transition_timer > 0.0f) game->transition_timer -= GetFrameTime();
        else if (game->battle.hitstop_timer > 0.0f) game->battle.hitstop_timer -= GetFrameTime();
        else if (!UpdateTutorial(game)) result = BattleUpdate(&game->battle, &game->player, &game->enemy,
                                   &game->dialogue, game->mouse_position,
                                   game->mouse_pressed, &game->qte, &game->audio);

        if (result.outcome == BATTLE_OUTCOME_DAMAGE || result.outcome == BATTLE_OUTCOME_MERCY) {
            result.coins += game->enemy.reward_bonus + (game->enemy.boss ? 8 : 0);
            result.xp += game->enemy.reward_bonus + (game->enemy.boss ? 10 : 0);
            game->last_battle_result = result;
            game->tower.coins += result.coins;
            if (game->telemetry_enabled) PlaytestLogBattle(game->tower.floor, &game->enemy, &result);
            if (result.outcome == BATTLE_OUTCOME_MERCY) game->tower.mercy_wins++;
            else game->tower.damage_wins++;
            if (PlayerGainXp(&game->player, result.xp)) AudioPlay(&game->audio, AUDIO_LEVEL_UP);
            AudioPlay(&game->audio, result.outcome == BATTLE_OUTCOME_MERCY ? AUDIO_VICTORY : AUDIO_DAMAGE_DEALT);
            TowerRollRewards(&game->tower, &game->player);
            game->reward_selection = 0;
            game->replacing_attack = false;
            game->screen = REWARD;
        }
        if (result.outcome == BATTLE_OUTCOME_DEFEAT) {
            if (game->telemetry_enabled) PlaytestLogBattle(game->tower.floor, &game->enemy, &result);
            AudioPlay(&game->audio, AUDIO_DEFEAT);
            FinishRun(game, false);
            game->end_selection = 0;
            game->screen = GAME_OVER;
        }
        if (IsKeyPressed(KEY_ESCAPE)) {
            game->screen_before_pause = game->screen;
            game->pause_selection = 0;
            game->pause_confirm_quit = false;
            game->screen = PAUSE;
        }
    } else if (game->screen == PAUSE) {
        if (game->pause_confirm_quit) {
            if (input_cancel_pressed() || IsKeyPressed(KEY_ESCAPE)) game->pause_confirm_quit = false;
            else if (input_confirm_pressed()) { FinishRun(game, false); game->end_selection = 0; game->screen = GAME_OVER; }
        } else {
            if (input_up_pressed()) game->pause_selection = (game->pause_selection + 4) % 5;
            if (input_down_pressed()) game->pause_selection = (game->pause_selection + 1) % 5;
            if (IsKeyPressed(KEY_ESCAPE) || input_cancel_pressed()) ResumePausedRun(game);
            if (input_confirm_pressed()) {
                if (game->pause_selection == 0) ResumePausedRun(game);
                else if (game->pause_selection == 1) game->screen = OPTIONS;
                else if (game->pause_selection == 2) game->screen = HOW_TO_PLAY;
                else if (game->pause_selection == 3) game->pause_confirm_quit = true;
                else game->screen = TITLE;
            }
        }
    } else if (game->screen == REWARD) {
        if (game->replacing_attack) {
            if (input_up_pressed() || input_left_pressed()) {
                game->replacement_selection = (game->replacement_selection +
                    game->player.attack_count - 1) % game->player.attack_count;
            }
            if (input_down_pressed() || input_right_pressed()) {
                game->replacement_selection = (game->replacement_selection + 1) %
                    game->player.attack_count;
            }
            if (input_confirm_pressed()) ApplyReward(game, game->reward_selection);
            if (input_cancel_pressed()) game->replacing_attack = false;
        } else {
            if (input_up_pressed()) game->reward_selection = (game->reward_selection + 2) % 3;
            if (input_down_pressed()) game->reward_selection = (game->reward_selection + 1) % 3;
            if (input_confirm_pressed()) ApplyReward(game, game->reward_selection);
        }
    } else if (game->screen == REST) {
        if (input_up_pressed() || input_down_pressed()) game->rest_selection = 1 - game->rest_selection;
        if (input_confirm_pressed()) {
            if (game->rest_selection == 0) {
                PlayerHeal(&game->player, game->player.hp_max / 2);
                AudioPlay(&game->audio, AUDIO_HEAL);
            }
            else game->player.permanent_damage_bonus++;
            StartFloor(game);
        }
    } else if (game->screen == SHOP) {
        if (input_up_pressed()) game->shop_selection = (game->shop_selection + 3) % 4;
        if (input_down_pressed()) game->shop_selection = (game->shop_selection + 1) % 4;
        if (input_confirm_pressed()) {
            if (game->shop_selection == 3) StartFloor(game);
            else {
                const ItemId item = game->tower.shop_items[game->shop_selection];
                const int price = game->tower.shop_prices[game->shop_selection];
                if (game->tower.coins >= price && PlayerAddItem(&game->player, item, 1)) {
                    game->tower.coins -= price;
                    AudioPlay(&game->audio, AUDIO_BUY);
                    snprintf(game->screen_message, sizeof(game->screen_message), "Comprou %s.",
                             ItemGetDefinition(item)->name);
                } else if (game->tower.coins < price) {
                    snprintf(game->screen_message, sizeof(game->screen_message), "Moedas insuficientes.");
                } else {
                    snprintf(game->screen_message, sizeof(game->screen_message), "Inventario cheio.");
                }
            }
        }
    } else if (game->screen == GAME_OVER || game->screen == VICTORY) {
        if (input_up_pressed() || input_down_pressed()) game->end_selection = 1 - game->end_selection;
        if (input_confirm_pressed()) {
            if (game->end_selection == 0 && game->screen == GAME_OVER) StartRun(game);
            else game->screen = TITLE;
        }
    } else if (game->screen == RELICS) {
        if (input_up_pressed() || input_down_pressed()) game->relic_selection = 1 - game->relic_selection;
        if (input_confirm_pressed()) {
            if (game->relic_selection == 0) SaveBuyRelic(&game->save, RELIC_ITEM, ITEM_MOON_HERB, 8);
            else SaveBuyRelic(&game->save, RELIC_ATTACK, 3, 10);
            SaveWrite(&game->save);
        }
        if (input_cancel_pressed()) game->screen = TITLE;
    } else if (game->screen == OPTIONS) {
        if (input_up_pressed()) game->option_selection = (game->option_selection + 7) % 8;
        if (input_down_pressed()) game->option_selection = (game->option_selection + 1) % 8;
        if (input_left_pressed() && game->option_selection == 0) game->save.music_volume -= 0.1f;
        if (input_right_pressed() && game->option_selection == 0) game->save.music_volume += 0.1f;
        if (input_left_pressed() && game->option_selection == 1) game->save.effects_volume -= 0.1f;
        if (input_right_pressed() && game->option_selection == 1) game->save.effects_volume += 0.1f;
        if (game->save.music_volume < 0.0f) game->save.music_volume = 0.0f;
        if (game->save.music_volume > 1.0f) game->save.music_volume = 1.0f;
        if (game->save.effects_volume < 0.0f) game->save.effects_volume = 0.0f;
        if (game->save.effects_volume > 1.0f) game->save.effects_volume = 1.0f;
        if (input_confirm_pressed()) {
            if (game->option_selection == 2) {
                ToggleFullscreen(); game->save.fullscreen = !game->save.fullscreen;
            } else if (game->option_selection == 3) {
                game->save.line_tremor = !game->save.line_tremor;
            } else if (game->option_selection == 4) {
                game->save.reduced_effects = !game->save.reduced_effects;
            } else if (game->option_selection == 5) {
                game->save.difficulty = (game->save.difficulty + 1) % 3;
            } else if (game->option_selection == 6) game->screen = DELETE_CONFIRM;
            else if (game->option_selection == 7) {
                game->save.tutorial_seen = 0;
                game->save.tutorial_done = false;
                game->tutorial_timer = 0.0f;
                game->tutorial_wait_phase = -1;
            }
        }
        if (input_cancel_pressed()) { SaveWrite(&game->save); game->screen = TITLE; }
    } else if (game->screen == DELETE_CONFIRM) {
        if (input_left_pressed() || input_right_pressed()) game->delete_yes = !game->delete_yes;
        if (input_confirm_pressed()) {
            if (game->delete_yes) SaveSetDefaults(&game->save);
            SaveWrite(&game->save);
            game->screen = OPTIONS;
        }
    } else if (game->screen == HOW_TO_PLAY) {
        if (input_cancel_pressed() || input_confirm_pressed()) game->screen = TITLE;
    } else if (game->screen == DEBUG_FX) {
        if (input_left_pressed()) game->pattern_debug_id =
            (game->pattern_debug_id + PATTERN_COUNT - 1) % PATTERN_COUNT;
        if (input_right_pressed()) game->pattern_debug_id =
            (game->pattern_debug_id + 1) % PATTERN_COUNT;
        if (input_up_pressed()) game->pattern_debug_enemy =
            (game->pattern_debug_enemy + ENEMY_COUNT - 1) % ENEMY_COUNT;
        if (input_down_pressed()) game->pattern_debug_enemy =
            (game->pattern_debug_enemy + 1) % ENEMY_COUNT;
        if (input_left_pressed() || input_right_pressed() || input_confirm_pressed()) {
            DefenseStart(&game->battle.defense, (BulletPatternId)game->pattern_debug_id,
                         (Rectangle){42, 228, 556, 94}, 0, 0, 1.0f, 1.0f);
            game->battle.defense.debug_invincible = true;
        }
        DefenseUpdate(&game->battle.defense, &game->player, GetFrameTime());
        if (input_cancel_pressed()) game->screen = game->screen_before_pause;
    }

    if (IsKeyPressed(KEY_F11)) { ToggleFullscreen(); game->save.fullscreen = !game->save.fullscreen; }
    if (IsKeyPressed(KEY_F3)) game->telemetry_enabled = !game->telemetry_enabled;
#ifndef NDEBUG
    if (IsKeyPressed(KEY_F6)) SpritesReload(&game->sprites);
    if (IsKeyPressed(KEY_F7)) game->sprites.debug_bounds = !game->sprites.debug_bounds;
#endif
    if (game->screen == BATTLE) {
        if (game->dialogue.visible_characters < game->last_typewriter_character) {
            game->last_typewriter_character = 0;
        }
        if (game->dialogue.visible_characters > game->last_typewriter_character) {
            AudioPlay(&game->audio, AUDIO_TEXT);
        }
        game->last_typewriter_character = game->dialogue.visible_characters;
    } else game->last_typewriter_character = 0;
    game->render.line_tremor = game->save.line_tremor;
    if (game->save.reduced_effects) game->battle.screen_shake_timer = 0.0f;
    if (game->save.reduced_effects) {
        game->battle.hitstop_timer = 0.0f;
        for (int index = 0; index < MAX_BATTLE_PARTICLES; index++) game->battle.particles[index].active = false;
    }
    RenderUpdate(&game->render, GetFrameTime());
    AudioSetVolumes(&game->audio, game->save.music_volume, game->save.effects_volume);
    AudioRequestTrack(&game->audio, game->screen == SHOP || game->screen == REST ? MUSIC_CALM :
                      (game->screen == BATTLE && game->enemy.boss ? MUSIC_BOSS : MUSIC_COMMON));
    AudioUpdate(&game->audio, GetFrameTime());
}

void GameDraw(const GameState *game)
{
    RenderDrawBackground(&game->render, game->elapsed_time);
    if (game->screen != TITLE) {
        const SpriteId background = game->enemy.boss ? SPRITE_BG_BOSS : SPRITE_BG_PASTEL;
        SpriteDrawFit(&game->sprites, background, (Rectangle){0, 0, LOGICAL_WIDTH, LOGICAL_HEIGHT}, WHITE);
    }
    if (game->screen == TITLE) {
        DrawTitle(game);
    } else if (game->screen == BATTLE) {
        BattleDraw(&game->battle, &game->player, &game->enemy, &game->dialogue, &game->qte,
                   game->render.line_tremor && !game->save.reduced_effects, game->save.reduced_effects,
                   &game->sprites);
        if (game->tutorial_timer > 0.0f) DrawTutorial(game);
        if (game->transition_timer > 0.0f) {
            UiDrawPanel((Rectangle){198, 193, 244, 82});
            DrawText("SUBINDO...", 252, 215, 25, BLACK);
            DrawText(TextFormat("ANDAR %d", game->tower.floor), 276, 246, 16, DARKGRAY);
        }
    } else if (game->screen == PAUSE) {
        DrawPause(game);
    } else if (game->screen == REWARD) {
        DrawReward(game);
    } else if (game->screen == REST) {
        DrawRest(game);
    } else if (game->screen == SHOP) {
        DrawShop(game);
    } else if (game->screen == GAME_OVER) {
        UiDrawPanel((Rectangle){62, 120, 516, 250});
        DrawText("FIM DA RUN", 225, 152, 30, BLACK);
        DrawText(TextFormat("Andar %d  |  Moedas %d", game->tower.floor, game->tower.coins),
                 190, 206, 18, DARKGRAY);
        DrawChoice(270, "TENTAR DE NOVO", game->end_selection == 0);
        DrawChoice(310, "TITULO", game->end_selection == 1);
    } else if (game->screen == VICTORY) {
        UiDrawPanel((Rectangle){62, 104, 516, 276});
        DrawText("TORRE CONCLUIDA", 185, 136, 29, BLACK);
        DrawText(TextFormat("10 andares | %d moedas", game->tower.coins), 205, 190, 18, DARKGRAY);
        DrawText(TextFormat("Poupar: %d  |  Dano: %d", game->tower.mercy_wins,
                            game->tower.damage_wins), 205, 218, 18, DARKGRAY);
        DrawChoice(292, "TITULO", true);
    } else if (game->screen == RELICS) {
        DrawRelics(game);
    } else if (game->screen == OPTIONS) {
        DrawOptions(game);
    } else if (game->screen == DELETE_CONFIRM) {
        DrawDeleteConfirm(game);
    } else if (game->screen == HOW_TO_PLAY) {
        DrawHowToPlay();
    } else if (game->screen == DEBUG_FX) {
        DrawFxDebug(game);
    } else {
        DrawPlaceholder("EM BREVE", "Este estado sera implementado nas proximas fases.");
    }
}
