#include "battle.h"
#include "input.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#define SHOW_ENEMY_HP_BEFORE_ANALYZE false

static bool ConfirmPressed(void)
{
    return input_confirm_pressed();
}

static Rectangle GetTextBox(void)
{
    return (Rectangle){42.0f, 228.0f, 556.0f, 94.0f};
}

static Rectangle GetMainButton(int button_index)
{
    return (Rectangle){38.0f + button_index * 143.0f, 398.0f, 132.0f, 40.0f};
}

static Rectangle GetActionButton(int option_index)
{
    return (Rectangle){72.0f, 260.0f + option_index * 20.0f, 390.0f, 20.0f};
}

static Rectangle GetItemButton(int option_index)
{
    return (Rectangle){64.0f + (option_index / 4) * 260.0f,
                       257.0f + (option_index % 4) * 19.0f, 240.0f, 18.0f};
}

static void ShowMessage(Battle *battle, TypewriterText *dialogue, const char *message,
                        BattlePhase next_phase);

static float EffectivenessAgainstEnemy(Type attack_type, const Enemy *enemy)
{
    float multiplier = type_effectiveness(attack_type, enemy->type);

    if (enemy->has_type2) multiplier *= type_effectiveness(attack_type, enemy->type2);
    return multiplier;
}

static Type FindEnemyWeakness(const Enemy *enemy)
{
    Type best_type = TYPE_NEUTRAL;
    float best_multiplier = 1.0f;

    for (Type candidate = TYPE_FLAME; candidate < TYPE_COUNT; candidate++) {
        const float multiplier = EffectivenessAgainstEnemy(candidate, enemy);
        if (multiplier > best_multiplier) {
            best_multiplier = multiplier;
            best_type = candidate;
        }
    }
    return best_type;
}

static void ResolvePlayerAttack(Battle *battle, PlayerStats *player, Enemy *enemy,
                                TypewriterText *dialogue, QteResult qte_result)
{
    const Attack *attack = &player->attacks[battle->qte_attack_index];
    const float type_multiplier = EffectivenessAgainstEnemy(attack->type, enemy);
    const float buff_multiplier = PlayerAttackBuffMultiplier(player);
    const float total_multiplier = type_multiplier * qte_multiplier(qte_result) * buff_multiplier;
    const int damage = (int)ceilf((float)(attack->base_damage + player->permanent_damage_bonus) * total_multiplier);
    const bool was_spareable = enemy->spareable;
    const bool was_phase_two = enemy->phase_two;
    char message[DIALOGUE_CAPACITY];

    EnemyTakeDamage(enemy, damage, type_multiplier);
    battle->floating_damage = damage;
    battle->floating_damage_timer = 0.85f;
    battle->last_effectiveness = type_multiplier;
    if (qte_result == QTE_PERFECT) battle->screen_shake_timer = 0.18f;
    PlayerConsumeAttackBuff(player);

    if (type_multiplier >= 2.0f) {
        snprintf(message, sizeof(message), "%s %s! %d dano. Super efetivo!", qte_result_name(qte_result),
                 attack->name, damage);
    } else if (type_multiplier <= 0.5f) {
        snprintf(message, sizeof(message), "%s %s! %d dano. Nao foi muito efetivo...",
                 qte_result_name(qte_result), attack->name, damage);
    } else {
        snprintf(message, sizeof(message), "%s %s! %d dano.", qte_result_name(qte_result),
                 attack->name, damage);
    }
    if (!was_spareable && enemy->spareable && enemy->hp > 0) {
        strncat(message, " O inimigo esta exausto.", sizeof(message) - strlen(message) - 1);
    }
    if (!was_phase_two && enemy->phase_two && enemy->hp > 0) {
        strncat(message, " O chefe mudou de fase!", sizeof(message) - strlen(message) - 1);
    }
    ShowMessage(battle, dialogue, message, enemy->hp == 0 ? BATTLE_VICTORY : BATTLE_ENEMY_TURN);
}

static void EnterPlayerMenu(Battle *battle, TypewriterText *dialogue)
{
    battle->phase = BATTLE_PLAYER_MENU;
    battle->action_menu = ACTION_NONE;
    battle->action_selection = 0;
    UiSetText(dialogue, "O que voce fara?");
}

static void ShowMessage(Battle *battle, TypewriterText *dialogue, const char *message,
                        BattlePhase next_phase)
{
    battle->phase = BATTLE_PLAYER_ACTION;
    battle->action_menu = ACTION_MESSAGE;
    battle->next_phase = next_phase;
    UiSetText(dialogue, message);
}

static void StartEnemyTurn(Battle *battle, Enemy *enemy, TypewriterText *dialogue)
{
    BulletPatternId pattern_id = PATTERN_RAIN;
    const int desired_difficulty = enemy->aggression < 34 ? 1 :
                                   (enemy->aggression < 67 ? 2 : 3);

    if (enemy->pattern_count > 0) {
        for (int index = 0; index < enemy->pattern_count; index++) {
            const BulletPatternId candidate =
                enemy->pattern_ids[(enemy->next_pattern_index + index) % enemy->pattern_count];

            if (BulletPatternDifficulty(candidate) == desired_difficulty) {
                pattern_id = candidate;
                break;
            }
        }
        enemy->next_pattern_index++;
    }

    battle->phase = BATTLE_ENEMY_TURN;
    battle->enemy_turn_timer = 0.0f;
    DefenseStart(&battle->defense, pattern_id, (Rectangle){42.0f, 228.0f, 556.0f, 94.0f},
                 enemy->aggression, enemy->projectile_damage_bonus);
    if (enemy->boss && enemy->phase_two) DefenseResizeBox(&battle->defense, 250.0f, 140.0f);
    UiSetText(dialogue, enemy->aggression >= 67 ? "Ela avanca furiosa!" :
                        (enemy->aggression <= 33 ? "Ela parece calma." : "Ela observa, irritada."));
}

void BattleInit(Battle *battle, TypewriterText *dialogue, int floor)
{
    battle->phase = BATTLE_INTRO;
    battle->next_phase = BATTLE_PLAYER_MENU;
    battle->action_menu = ACTION_NONE;
    battle->main_selection = MENU_ATTACK;
    battle->floor = floor;
    battle->action_selection = 0;
    battle->enemy_turn_timer = 0.0f;
    battle->elapsed_time = 0.0f;
    battle->floating_damage = 0;
    battle->floating_damage_timer = 0.0f;
    battle->last_effectiveness = 1.0f;
    battle->qte_attack_index = 0;
    battle->screen_shake_timer = 0.0f;
    battle->displayed_mercy = 0.0f;
    battle->displayed_aggression = 0.0f;
    battle->stat_change_timer = 0.0f;
    battle->mercy_change_popup = 0;
    battle->aggression_change_popup = 0;
    battle->victory_by_mercy = false;
    battle->result = (BattleResult){BATTLE_OUTCOME_NONE, 0, 0};
    DefenseInit(&battle->defense);
    UiSetText(dialogue, "Um inimigo bloqueia a subida.");
}

static void UpdatePlayerMenu(Battle *battle, PlayerStats *player, Enemy *enemy,
                             TypewriterText *dialogue, Vector2 mouse_position,
                             bool mouse_pressed)
{
    bool confirmed = ConfirmPressed();

    if (input_left_pressed()) {
        battle->main_selection = (battle->main_selection + 3) % 4;
    }
    if (input_right_pressed()) {
        battle->main_selection = (battle->main_selection + 1) % 4;
    }
    if (mouse_pressed) {
        for (int button_index = 0; button_index < 4; button_index++) {
            if (CheckCollisionPointRec(mouse_position, GetMainButton(button_index))) {
                battle->main_selection = button_index;
                confirmed = true;
            }
        }
    }
    if (!confirmed) return;

    switch ((MainMenuOption)battle->main_selection) {
        case MENU_ATTACK:
            battle->phase = BATTLE_PLAYER_ACTION;
            battle->action_menu = ACTION_ATTACKS;
            battle->action_selection = 0;
            UiSetText(dialogue, "Escolha um ataque.");
            break;
        case MENU_ACT:
            battle->phase = BATTLE_PLAYER_ACTION;
            battle->action_menu = ACTION_ACTS;
            battle->action_selection = 0;
            UiSetText(dialogue, "Como voce vai agir?");
            break;
        case MENU_ITEM:
            battle->phase = BATTLE_PLAYER_ACTION;
            battle->action_menu = ACTION_ITEMS;
            battle->action_selection = 0;
            UiSetText(dialogue, "Escolha um item.");
            break;
        case MENU_SPARE:
            if (EnemyCanSpare(enemy)) {
                battle->victory_by_mercy = true;
                ShowMessage(battle, dialogue, "Voce poupou o inimigo com gentileza.", BATTLE_VICTORY);
            } else if (enemy->spareable) {
                ShowMessage(battle, dialogue, "Ele esta furioso demais para te ouvir.", BATTLE_ENEMY_TURN);
            } else {
                ShowMessage(battle, dialogue, "Ainda nao confia em voce...", BATTLE_ENEMY_TURN);
            }
            break;
        default:
            (void)player;
            break;
    }
}

static void UpdateActionList(Battle *battle, PlayerStats *player, Enemy *enemy,
                             TypewriterText *dialogue, Vector2 mouse_position,
                             bool mouse_pressed, Qte *qte)
{
    int option_count = 0;
    bool confirmed = ConfirmPressed();

    if (battle->action_menu == ACTION_ATTACKS) option_count = player->attack_count;
    if (battle->action_menu == ACTION_ACTS) option_count = enemy->action_count;
    if (battle->action_menu == ACTION_ITEMS) option_count = MAX_INVENTORY_ITEMS;

    if (input_up_pressed()) {
        battle->action_selection = (battle->action_selection + option_count - 1) % option_count;
    }
    if (input_down_pressed()) {
        battle->action_selection = (battle->action_selection + 1) % option_count;
    }
    if (input_cancel_pressed()) {
        EnterPlayerMenu(battle, dialogue);
        return;
    }
    if (mouse_pressed) {
        for (int option_index = 0; option_index < option_count; option_index++) {
            const Rectangle option = battle->action_menu == ACTION_ITEMS ?
                                     GetItemButton(option_index) : GetActionButton(option_index);
            if (CheckCollisionPointRec(mouse_position, option)) {
                battle->action_selection = option_index;
                confirmed = true;
            }
        }
    }
    if (!confirmed) return;

    if (battle->action_menu == ACTION_ATTACKS) {
        const Attack *attack = &player->attacks[battle->action_selection];

        battle->qte_attack_index = battle->action_selection;
        battle->phase = BATTLE_PLAYER_QTE;
        qte_start(qte, attack->qte_kind, battle->floor);
    } else if (battle->action_menu == ACTION_ACTS) {
        const EnemyAction *action = &enemy->actions[battle->action_selection];
        char message[DIALOGUE_CAPACITY];

        if (action->effect == ENEMY_ACTION_ANALYZE) {
            const Type weakness = FindEnemyWeakness(enemy);
            EnemyApplyAction(enemy, battle->action_selection, message, sizeof(message));
            if (enemy->has_type2) {
                snprintf(message, sizeof(message), "%s: %s/%s. Fraco contra: %s. %s", enemy->name,
                         type_name(enemy->type), type_name(enemy->type2), type_name(weakness),
                         enemy->description);
            } else {
                snprintf(message, sizeof(message), "%s: %s. Fraco contra: %s. %s", enemy->name,
                         type_name(enemy->type), type_name(weakness), enemy->description);
            }
        } else if (EnemyApplyAction(enemy, battle->action_selection, message, sizeof(message))) {
            battle->mercy_change_popup = enemy->last_mercy_delta;
            battle->aggression_change_popup = enemy->last_aggression_delta;
            battle->stat_change_timer = 0.8f;
        } else {
            ShowMessage(battle, dialogue, message, BATTLE_PLAYER_MENU);
            return;
        }
        ShowMessage(battle, dialogue, message, BATTLE_ENEMY_TURN);
    } else if (battle->action_menu == ACTION_ITEMS) {
        const ItemDefinition *item;
        int applied_value;
        char message[DIALOGUE_CAPACITY];

        if (!PlayerUseItem(player, battle->action_selection, &item, &applied_value)) {
            ShowMessage(battle, dialogue, "Nao sobrou nenhum desse item.", BATTLE_PLAYER_MENU);
            return;
        }
        if (item->effect == ITEM_EFFECT_DEFENSE_TYPE) {
            snprintf(message, sizeof(message), "Voce usou %s. Defesa agora: %s.", item->name,
                     type_name(player->defense_type));
        } else if (item->effect == ITEM_EFFECT_HEAL) {
            snprintf(message, sizeof(message), "Voce usou %s e recuperou %d HP.", item->name,
                     applied_value);
        } else if (item->effect == ITEM_EFFECT_DAMAGE_BUFF) {
            snprintf(message, sizeof(message), "%s: dano +%d%% por %d ataque(s).", item->name,
                     item->value, item->duration);
        } else {
            const bool was_spareable = enemy->spareable;
            EnemyTakeDamage(enemy, applied_value, 1.0f);
            battle->floating_damage = applied_value;
            battle->floating_damage_timer = 0.85f;
            battle->last_effectiveness = 1.0f;
            snprintf(message, sizeof(message), "%s acertou e causou %d dano.", item->name, applied_value);
            if (!was_spareable && enemy->spareable && enemy->hp > 0) {
                strncat(message, " O inimigo esta exausto.", sizeof(message) - strlen(message) - 1);
            }
        }
        ShowMessage(battle, dialogue, message, enemy->hp == 0 ? BATTLE_VICTORY : BATTLE_ENEMY_TURN);
    }
}

BattleResult BattleUpdate(Battle *battle, PlayerStats *player, Enemy *enemy,
                          TypewriterText *dialogue, Vector2 mouse_position,
                          bool mouse_pressed, Qte *qte)
{
    const float delta_time = GetFrameTime();

    battle->elapsed_time += delta_time;
    if (enemy->hit_flash_timer > 0.0f) enemy->hit_flash_timer -= delta_time;
    if (battle->floating_damage_timer > 0.0f) battle->floating_damage_timer -= delta_time;
    if (battle->screen_shake_timer > 0.0f) battle->screen_shake_timer -= delta_time;
    battle->displayed_mercy = UiApproach(battle->displayed_mercy, (float)enemy->mercy, 145.0f, delta_time);
    battle->displayed_aggression = UiApproach(battle->displayed_aggression,
                                               (float)enemy->aggression, 145.0f, delta_time);
    if (battle->stat_change_timer > 0.0f) battle->stat_change_timer -= delta_time;
    UiUpdateText(dialogue, delta_time);

    if (battle->phase == BATTLE_INTRO) {
        const bool confirm = ConfirmPressed() ||
                             (mouse_pressed && CheckCollisionPointRec(mouse_position, GetTextBox()));
        if (confirm && UiConfirmText(dialogue)) EnterPlayerMenu(battle, dialogue);
    } else if (battle->phase == BATTLE_PLAYER_MENU) {
        UpdatePlayerMenu(battle, player, enemy, dialogue, mouse_position, mouse_pressed);
    } else if (battle->phase == BATTLE_PLAYER_ACTION) {
        if (battle->action_menu == ACTION_MESSAGE) {
            const bool confirm = ConfirmPressed() ||
                                 (mouse_pressed && CheckCollisionPointRec(mouse_position, GetTextBox()));
            if (confirm && UiConfirmText(dialogue)) {
                if (battle->next_phase == BATTLE_ENEMY_TURN) {
                    StartEnemyTurn(battle, enemy, dialogue);
                } else if (battle->next_phase == BATTLE_PLAYER_MENU) {
                    EnterPlayerMenu(battle, dialogue);
                } else {
                    battle->phase = battle->next_phase;
                }
            }
        } else {
            UpdateActionList(battle, player, enemy, dialogue, mouse_position, mouse_pressed, qte);
        }
    } else if (battle->phase == BATTLE_PLAYER_QTE) {
        qte_update(qte, delta_time);
        if (qte_is_finished(qte)) ResolvePlayerAttack(battle, player, enemy, dialogue, qte->result);
    } else if (battle->phase == BATTLE_ENEMY_TURN) {
        const DefenseResult result = DefenseUpdate(&battle->defense, player, delta_time);

        if (result == DEFENSE_PLAYER_DEFEATED) {
            ShowMessage(battle, dialogue, "Sua alma se desfez no escuro.", BATTLE_DEFEAT);
        } else if (result == DEFENSE_COMPLETE) {
            EnemyAdvanceTurn(enemy);
            ShowMessage(battle, dialogue, "O turno da mariposa terminou.", BATTLE_PLAYER_MENU);
        }
    } else if (battle->phase == BATTLE_VICTORY) {
        const bool confirm = ConfirmPressed() ||
                             (mouse_pressed && CheckCollisionPointRec(mouse_position, GetTextBox()));
        if (confirm && UiConfirmText(dialogue)) {
            return battle->victory_by_mercy ?
                   (BattleResult){BATTLE_OUTCOME_MERCY, 10, 15} :
                   (BattleResult){BATTLE_OUTCOME_DAMAGE, 5, 8};
        }
    } else if (battle->phase == BATTLE_DEFEAT) {
        const bool confirm = ConfirmPressed() ||
                             (mouse_pressed && CheckCollisionPointRec(mouse_position, GetTextBox()));
        if (confirm && UiConfirmText(dialogue)) return (BattleResult){BATTLE_OUTCOME_DEFEAT, 0, 0};
    }

    return (BattleResult){BATTLE_OUTCOME_NONE, 0, 0};
}

static void DrawEnemy(const Enemy *enemy, float time, float shake_timer, bool line_tremor)
{
    const float y = 105.0f + sinf(time * 2.0f) * 4.0f;
    const int shake_x = shake_timer > 0.0f ? (int)(sinf(time * 100.0f) * 3.0f) :
                        (line_tremor ? ((int)(time * 8.0f) % 2) : 0);
    const Color body = enemy->hit_flash_timer > 0.0f ? RAYWHITE : (Color){48, 43, 60, 255};
    const Color accent = type_color(enemy->type);

    if (enemy->boss) {
        const int x = 320 + shake_x;
        const Color boss_body = enemy->hit_flash_timer > 0.0f ? RAYWHITE : (Color){35, 37, 43, 255};

        if (enemy->id == ENEMY_BOSS_BELLKEEPER) {
            DrawCircle(x, (int)(y + 34), 42, boss_body);
            DrawRectangle(x - 27, (int)(y + 34), 54, 38, boss_body);
            DrawCircleLines(x, (int)(y + 34), 42, BLACK);
            DrawRectangleLines(x - 27, (int)(y + 34), 54, 38, BLACK);
            DrawCircle(x, (int)(y + 61), 8, accent);
        } else {
            DrawRectangle(x - 49, (int)(y - 5), 98, 76, boss_body);
            DrawTriangle((Vector2){x - 60, y - 5}, (Vector2){x + 60, y - 5},
                         (Vector2){x, y - 55}, boss_body);
            DrawRectangleLines(x - 49, (int)(y - 5), 98, 76, BLACK);
            DrawLine(x - 60, (int)(y - 5), x, (int)(y - 55), BLACK);
            DrawLine(x, (int)(y - 55), x + 60, (int)(y - 5), BLACK);
            DrawRectangle(x - 9, (int)(y + 25), 18, 25, accent);
        }
        if (enemy->phase_two) DrawCircleLines(x, (int)(y + 30), 50, accent);
        return;
    }

    DrawEllipse(270 + shake_x, (int)(y + 25.0f), 47, 25, accent);
    DrawEllipse(323 + shake_x, (int)(y + 25.0f), 47, 25, accent);
    DrawEllipseLines(270 + shake_x, (int)(y + 25.0f), 47, 25, BLACK);
    DrawEllipseLines(323 + shake_x, (int)(y + 25.0f), 47, 25, BLACK);
    DrawCircle(320 + shake_x, (int)(y + 32.0f), 30, body);
    DrawCircleLines(320 + shake_x, (int)(y + 32.0f), 30, BLACK);
    if (enemy->has_type2) DrawCircleLines(320 + shake_x, (int)(y + 32.0f), 26, type_color(enemy->type2));
    if (enemy->hit_flash_timer > 0.0f) {
        DrawCircleLines(320 + shake_x, (int)(y + 32.0f), 34,
                        enemy->hit_flash_strength >= 2.0f ? RED : RAYWHITE);
    }
    DrawCircle(309 + shake_x, (int)(y + 27.0f), 4, RAYWHITE);
    DrawCircle(331 + shake_x, (int)(y + 27.0f), 4, RAYWHITE);
    DrawCircle(309 + shake_x, (int)(y + 27.0f), 2, BLACK);
    DrawCircle(331 + shake_x, (int)(y + 27.0f), 2, BLACK);
}

static void DrawHealthBar(Rectangle bar, int current, int maximum, Color fill)
{
    const float ratio = maximum > 0 ? (float)current / maximum : 0.0f;

    DrawRectangleRec(bar, BLACK);
    DrawRectangle((int)bar.x + 2, (int)bar.y + 2, (int)bar.width - 4, (int)bar.height - 4,
                  (Color){211, 214, 210, 255});
    DrawRectangle((int)bar.x + 2, (int)bar.y + 2, (int)((bar.width - 4) * ratio),
                  (int)bar.height - 4, fill);
}

static void DrawActionList(const Battle *battle, const PlayerStats *player, const Enemy *enemy)
{
    const int option_count = battle->action_menu == ACTION_ATTACKS ? player->attack_count :
                             (battle->action_menu == ACTION_ACTS ? enemy->action_count :
                                                                  MAX_INVENTORY_ITEMS);

    for (int row = 0; row < option_count; row++) {
        const Rectangle area = battle->action_menu == ACTION_ITEMS ? GetItemButton(row) :
                               GetActionButton(row);
        const int y = (int)area.y + 3;
        const bool selected = battle->action_selection == row;

        if (selected) DrawText(">", (int)area.x + 2, y, 16, BLACK);
        if (battle->action_menu == ACTION_ATTACKS) {
            const Attack *attack = &player->attacks[row];
            DrawText(attack->name, 94, y, 18, BLACK);
            DrawText(type_name(attack->type), 275, y, 15, type_color(attack->type));
            DrawText(TextFormat("%d", attack->base_damage), 410, y, 16, BLACK);
        } else if (battle->action_menu == ACTION_ACTS) {
            char reason[48] = "";
            const bool available = EnemyActionAvailable(enemy, row, reason, (int)sizeof(reason));
            const Color color = available ? BLACK : GRAY;

            DrawText(enemy->actions[row].name, 94, y, 18, color);
            if (!available) DrawText(reason, 250, y + 2, 13, GRAY);
        } else if (battle->action_menu == ACTION_ITEMS) {
            const InventoryItem *item = &player->inventory[row];
            const ItemDefinition *definition = ItemGetDefinition(item->id);

            DrawText(definition->name, (int)area.x + 22, y, 15, BLACK);
            DrawText(TextFormat("x%d", item->quantity), (int)area.x + 190, y, 14, DARKGRAY);
        }
    }
}

void BattleDraw(const Battle *battle, const PlayerStats *player, const Enemy *enemy,
                const TypewriterText *dialogue, const Qte *qte, bool line_tremor)
{
    const char *button_names[4] = {"ATACAR", "AGIR", "ITEM", "POUPAR"};
    const Rectangle text_box = GetTextBox();

    DrawEnemy(enemy, battle->elapsed_time, battle->screen_shake_timer, line_tremor);
    DrawText(TextFormat("ANDAR %d/10", battle->floor), 470, 28, 16, DARKGRAY);
    if (battle->floating_damage_timer > 0.0f) {
        const int y = (int)(102.0f - (0.85f - battle->floating_damage_timer) * 34.0f);
        const int size = battle->last_effectiveness >= 2.0f ? 28 : 20;
        const Color color = battle->last_effectiveness >= 2.0f ? RED :
                            (battle->last_effectiveness <= 0.5f ? DARKBLUE : BLACK);
        DrawText(TextFormat("-%d", battle->floating_damage), 344, y, size, color);
    }
    DrawText(enemy->name, 54, 32, 21, BLACK);
    DrawText(enemy->analyzed ? type_name(enemy->type) : "???", 54, 58, 16,
             enemy->analyzed ? type_color(enemy->type) : DARKGRAY);
    if (enemy->analyzed || SHOW_ENEMY_HP_BEFORE_ANALYZE) {
        DrawHealthBar((Rectangle){54.0f, 80.0f, 170.0f, 14.0f}, enemy->hp, enemy->hp_max,
                      type_color(enemy->type));
    }
    if (enemy->analyzed) {
        UiDrawMeter((Rectangle){104.0f, 100.0f, 120.0f, 11.0f}, battle->displayed_mercy, 100.0f,
                    (Color){241, 198, 65, 255});
        UiDrawMeter((Rectangle){104.0f, 116.0f, 120.0f, 11.0f}, battle->displayed_aggression, 100.0f,
                    (Color){220, 78, 76, 255});
        DrawText("MERCI", 54, 99, 13, BLACK);
        DrawText("AGRES", 54, 115, 13, BLACK);
        if (battle->stat_change_timer > 0.0f) {
            const int offset = (int)((0.8f - battle->stat_change_timer) * 18.0f);
            if (battle->mercy_change_popup != 0) {
                DrawText(TextFormat("%+d", battle->mercy_change_popup), 228, 97 - offset, 14,
                         (Color){198, 152, 23, 255});
            }
            if (battle->aggression_change_popup != 0) {
                DrawText(TextFormat("%+d", battle->aggression_change_popup), 228, 128 - offset, 14,
                         (Color){185, 45, 44, 255});
            }
        }
    }

    if (battle->phase == BATTLE_PLAYER_QTE) {
        UiDrawPanel(text_box);
        DrawText("Concentre-se no QTE!", 210, 264, 18, BLACK);
        qte_draw(qte);
    } else if (battle->phase == BATTLE_ENEMY_TURN) {
        DefenseDraw(&battle->defense);
        DrawText("DESVIE!  SETAS ou WASD", 198, 326, 16, BLACK);
    } else {
        UiDrawTextBox(dialogue, text_box);
    }
    if (battle->phase == BATTLE_PLAYER_ACTION && battle->action_menu != ACTION_MESSAGE) {
        DrawActionList(battle, player, enemy);
        DrawText("X/Backspace: voltar", 434, 297, 14, DARKGRAY);
        if (battle->action_menu == ACTION_ITEMS) {
            const InventoryItem *selected = &player->inventory[battle->action_selection];
            const ItemDefinition *definition = ItemGetDefinition(selected->id);
            DrawText(definition->description, 70, 309, 13, DARKGRAY);
        }
    }

    DrawText(player->name, 48, 343, 20, BLACK);
    DrawText(TextFormat("LV %d", player->level), 157, 343, 17, DARKGRAY);
    DrawText("HP", 222, 343, 16, BLACK);
    DrawHealthBar((Rectangle){252.0f, 346.0f, 150.0f, 14.0f}, player->hp, player->hp_max,
                  (Color){104, 183, 114, 255});
    DrawText(TextFormat("%d / %d", player->hp, player->hp_max), 416, 342, 17, BLACK);
    DrawText("DEF", 480, 343, 14, BLACK);
    DrawCircle(515, 351, 7, type_color(player->defense_type));
    DrawCircleLines(515, 351, 7, BLACK);
    DrawText(type_name(player->defense_type), 528, 343, 14, BLACK);
    if (player->damage_buff_turns > 0) {
        DrawRectangle(48, 367, 116, 17, (Color){243, 207, 110, 255});
        DrawRectangleLines(48, 367, 116, 17, BLACK);
        DrawText(TextFormat("DMG +%d%% (%d)", player->damage_buff_percent,
                            player->damage_buff_turns), 53, 369, 12, BLACK);
    }

    for (int button_index = 0; button_index < 4; button_index++) {
        const Rectangle button = GetMainButton(button_index);
        const bool selected = battle->phase == BATTLE_PLAYER_MENU && battle->main_selection == button_index;

        UiDrawPanel(button);
        DrawText(button_names[button_index], (int)button.x + 30, (int)button.y + 12, 16, BLACK);
        if (selected) {
            DrawCircle((int)button.x + 16, (int)button.y + 20, 7, (Color){225, 95, 122, 255});
            DrawCircleLines((int)button.x + 16, (int)button.y + 20, 7, BLACK);
        }
    }
}
