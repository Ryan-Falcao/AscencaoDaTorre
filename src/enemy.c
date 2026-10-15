#include "enemy.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#define AGGRESSION_HIGH 67

static int ClampEnemyValue(int value)
{
    if (value < 0) return 0;
    if (value > 100) return 100;
    return value;
}

static EnemyAction MakeAction(const char *name, const char *response, EnemyActionEffect effect,
                              int mercy_delta, int aggression_delta, int max_uses,
                              int cooldown_turns, float repeat_penalty,
                              int max_aggression_required)
{
    return (EnemyAction){name, response, effect, mercy_delta, aggression_delta, max_uses,
                         cooldown_turns, repeat_penalty, max_aggression_required,
                         false, TYPE_NEUTRAL, 1.0f, 50, 1.0f, 1.0f};
}

void EnemyRefreshSpareable(Enemy *enemy)
{
    enemy->exhausted = enemy->hp * 4 < enemy->hp_max;
    enemy->spareable = enemy->mercy >= 100 || enemy->exhausted;
}

bool EnemyCanSpare(const Enemy *enemy)
{
    return enemy->spareable && enemy->aggression < AGGRESSION_HIGH;
}

Enemy EnemyCreate(EnemyId id)
{
    Enemy enemy;

    memset(&enemy, 0, sizeof(enemy));
    enemy.id = id;
    enemy.last_action_index = -1;
    for (int index = 0; index < MAX_ENEMY_ACTIONS; index++) enemy.action_last_turn[index] = -1;
    switch (id) {
        case ENEMY_DUST_MOTH:
            snprintf(enemy.name, sizeof(enemy.name), "Mariposa de Poeira");
            enemy.hp_max = 36;
            enemy.hp = enemy.hp_max;
            enemy.type = TYPE_SHADOW;
            enemy.aggression = 40;
            enemy.pattern_ids[0] = PATTERN_RAIN;
            enemy.pattern_ids[1] = PATTERN_SIDE_WAVE;
            enemy.pattern_ids[2] = PATTERN_SPIRAL;
            enemy.pattern_count = 3;
            snprintf(enemy.description, sizeof(enemy.description),
                     "Ela ronda lampadas apagadas e parece esperar um elogio.");
            enemy.actions[0] = MakeAction("Analisar", "", ENEMY_ACTION_ANALYZE, 0, 0, 0, 0, 1.0f, 100);
            enemy.actions[1] = MakeAction("Elogiar", "As asas vibram, felizes.", ENEMY_ACTION_SOCIAL, 30, -5, 2, 1, 0.50f, 100);
            enemy.actions[2] = MakeAction("Provocar", "As asas batem com raiva.", ENEMY_ACTION_SOCIAL, -10, 25, 1, 2, 1.0f, 100);
            enemy.actions[3] = MakeAction("Acalmar", "A poeira se acomoda devagar.", ENEMY_ACTION_SOCIAL, 15, -20, 3, 1, 0.50f, 100);
            enemy.actions[3].has_type_bonus = true;
            enemy.actions[3].bonus_type = TYPE_SHADOW;
            enemy.actions[3].type_multiplier = 1.30f;
            enemy.actions[3].aggression_threshold = 55;
            enemy.actions[3].calm_multiplier = 1.15f;
            enemy.actions[3].angry_multiplier = 0.45f;
            enemy.action_count = 4;
            break;
        case ENEMY_CINDER_BIRD:
            snprintf(enemy.name, sizeof(enemy.name), "Passaro de Cinza");
            enemy.hp_max = 38; enemy.hp = enemy.hp_max; enemy.type = TYPE_FLAME; enemy.aggression = 48;
            enemy.pattern_ids[0] = PATTERN_FAST_RAIN; enemy.pattern_ids[1] = PATTERN_SPIRAL;
            enemy.pattern_ids[2] = PATTERN_RAIN; enemy.pattern_count = 3;
            snprintf(enemy.description, sizeof(enemy.description), "Uma ave quente demais para pousar.");
            enemy.actions[0] = MakeAction("Analisar", "", ENEMY_ACTION_ANALYZE, 0, 0, 0, 0, 1.0f, 100);
            enemy.actions[1] = MakeAction("Assobiar", "O canto perde a pressa.", ENEMY_ACTION_SOCIAL, 25, -15, 2, 1, .5f, 100);
            enemy.actions[2] = MakeAction("Ameacar", "As penas viram brasas.", ENEMY_ACTION_SOCIAL, -15, 30, 1, 2, 1.0f, 100);
            enemy.actions[3] = MakeAction("Acalmar", "A brasa diminui.", ENEMY_ACTION_SOCIAL, 15, -20, 3, 1, .5f, 100);
            enemy.action_count = 4; break;
        case ENEMY_MINT_SLIME:
            snprintf(enemy.name, sizeof(enemy.name), "Limo de Menta");
            enemy.hp_max = 34; enemy.hp = enemy.hp_max; enemy.type = TYPE_PLANT; enemy.aggression = 28;
            enemy.pattern_ids[0] = PATTERN_RAIN; enemy.pattern_ids[1] = PATTERN_WIDE_WAVE;
            enemy.pattern_ids[2] = PATTERN_SIDE_WAVE; enemy.pattern_count = 3;
            snprintf(enemy.description, sizeof(enemy.description), "Ele cresce onde ninguem olha.");
            enemy.actions[0] = MakeAction("Analisar", "", ENEMY_ACTION_ANALYZE, 0, 0, 0, 0, 1.0f, 100);
            enemy.actions[1] = MakeAction("Regar", "O limo balanca contente.", ENEMY_ACTION_SOCIAL, 28, -8, 2, 1, .5f, 100);
            enemy.actions[2] = MakeAction("Pisar", "O limo espirra de raiva.", ENEMY_ACTION_SOCIAL, -15, 28, 1, 2, 1.0f, 100);
            enemy.actions[3] = MakeAction("Esperar", "Ele para de se esticar.", ENEMY_ACTION_SOCIAL, 16, -18, 3, 1, .5f, 100);
            enemy.action_count = 4; break;
        case ENEMY_LANTERN_WISP:
            snprintf(enemy.name, sizeof(enemy.name), "Fagulha de Lanterna");
            enemy.hp_max = 40; enemy.hp = enemy.hp_max; enemy.type = TYPE_LIGHT; enemy.aggression = 50;
            enemy.pattern_ids[0] = PATTERN_FAST_RAIN; enemy.pattern_ids[1] = PATTERN_TIGHT_SPIRAL;
            enemy.pattern_ids[2] = PATTERN_SPIRAL; enemy.pattern_count = 3;
            snprintf(enemy.description, sizeof(enemy.description), "Uma luz que quer ser seguida.");
            enemy.actions[0] = MakeAction("Analisar", "", ENEMY_ACTION_ANALYZE, 0, 0, 0, 0, 1.0f, 100);
            enemy.actions[1] = MakeAction("Ouvir", "A luz fica mais baixa.", ENEMY_ACTION_SOCIAL, 26, -12, 2, 1, .5f, 100);
            enemy.actions[2] = MakeAction("Apagar", "Ela brilha num salto.", ENEMY_ACTION_SOCIAL, -18, 32, 1, 2, 1.0f, 100);
            enemy.actions[3] = MakeAction("Guiar", "A fagulha encontra descanso.", ENEMY_ACTION_SOCIAL, 16, -18, 3, 1, .5f, 100);
            enemy.action_count = 4; break;
        case ENEMY_THORN_HARE:
            snprintf(enemy.name, sizeof(enemy.name), "Lebre de Espinhos");
            enemy.hp_max = 44; enemy.hp = enemy.hp_max; enemy.type = TYPE_PLANT; enemy.aggression = 58;
            enemy.pattern_ids[0] = PATTERN_WIDE_WAVE; enemy.pattern_ids[1] = PATTERN_SIDE_WAVE;
            enemy.pattern_ids[2] = PATTERN_FAST_RAIN; enemy.pattern_count = 3;
            snprintf(enemy.description, sizeof(enemy.description), "Ela se esconde atras dos espinhos.");
            enemy.actions[0] = MakeAction("Analisar", "", ENEMY_ACTION_ANALYZE, 0, 0, 0, 0, 1.0f, 100);
            enemy.actions[1] = MakeAction("Oferecer folha", "Ela mastiga devagar.", ENEMY_ACTION_SOCIAL, 25, -15, 2, 1, .5f, 100);
            enemy.actions[2] = MakeAction("Correr atras", "Ela mostra os espinhos.", ENEMY_ACTION_SOCIAL, -12, 30, 1, 2, 1.0f, 100);
            enemy.actions[3] = MakeAction("Afastar", "Ela percebe que tem espaco.", ENEMY_ACTION_SOCIAL, 18, -18, 3, 1, .5f, 100);
            enemy.action_count = 4; break;
        case ENEMY_SHADE_CAT:
            snprintf(enemy.name, sizeof(enemy.name), "Gato de Sombra");
            enemy.hp_max = 46; enemy.hp = enemy.hp_max; enemy.type = TYPE_SHADOW; enemy.aggression = 62;
            enemy.pattern_ids[0] = PATTERN_TIGHT_SPIRAL; enemy.pattern_ids[1] = PATTERN_SPIRAL;
            enemy.pattern_ids[2] = PATTERN_SIDE_WAVE; enemy.pattern_count = 3;
            snprintf(enemy.description, sizeof(enemy.description), "Ele rosna para o proprio reflexo.");
            enemy.actions[0] = MakeAction("Analisar", "", ENEMY_ACTION_ANALYZE, 0, 0, 0, 0, 1.0f, 100);
            enemy.actions[1] = MakeAction("Fazer carinho", "O rabo para de balancar.", ENEMY_ACTION_SOCIAL, 27, -16, 2, 1, .5f, 100);
            enemy.actions[2] = MakeAction("Encurralar", "O pelo se arrepia.", ENEMY_ACTION_SOCIAL, -18, 33, 1, 2, 1.0f, 100);
            enemy.actions[3] = MakeAction("Sentar", "Ele observa em silencio.", ENEMY_ACTION_SOCIAL, 17, -18, 3, 1, .5f, 100);
            enemy.action_count = 4; break;
        case ENEMY_PETAL_KNIGHT:
            snprintf(enemy.name, sizeof(enemy.name), "Cavaleiro de Petalas");
            enemy.hp_max = 50; enemy.hp = enemy.hp_max; enemy.type = TYPE_LIGHT; enemy.aggression = 60;
            enemy.pattern_ids[0] = PATTERN_WIDE_WAVE; enemy.pattern_ids[1] = PATTERN_FAST_RAIN;
            enemy.pattern_ids[2] = PATTERN_TIGHT_SPIRAL; enemy.pattern_count = 3;
            snprintf(enemy.description, sizeof(enemy.description), "Sua armadura cheira a jardim depois da chuva.");
            enemy.actions[0] = MakeAction("Analisar", "", ENEMY_ACTION_ANALYZE, 0, 0, 0, 0, 1.0f, 100);
            enemy.actions[1] = MakeAction("Saudar", "Ele baixa a espada.", ENEMY_ACTION_SOCIAL, 26, -15, 2, 1, .5f, 100);
            enemy.actions[2] = MakeAction("Desafiar", "Ele aceita com rigor.", ENEMY_ACTION_SOCIAL, -15, 30, 1, 2, 1.0f, 100);
            enemy.actions[3] = MakeAction("Conversar", "Uma petala cai devagar.", ENEMY_ACTION_SOCIAL, 18, -20, 3, 1, .5f, 100);
            enemy.action_count = 4; break;
        case ENEMY_BOSS_BELLKEEPER:
            snprintf(enemy.name, sizeof(enemy.name), "Sineiro Vazio");
            enemy.hp_max = 100; enemy.hp = enemy.hp_max; enemy.type = TYPE_SHADOW; enemy.type2 = TYPE_LIGHT;
            enemy.has_type2 = true; enemy.boss = true; enemy.aggression = 55;
            for (int i = 0; i < 6; i++) enemy.pattern_ids[i] = (BulletPatternId)i;
            enemy.pattern_count = 6;
            snprintf(enemy.description, sizeof(enemy.description), "Um sino sem som guarda o quinto andar.");
            enemy.actions[0] = MakeAction("Analisar", "", ENEMY_ACTION_ANALYZE, 0, 0, 0, 0, 1.0f, 100);
            enemy.actions[1] = MakeAction("Escutar", "O metal para de tremer.", ENEMY_ACTION_SOCIAL, 28, -16, 3, 1, .5f, 100);
            enemy.actions[2] = MakeAction("Bater no sino", "O eco o enfurece.", ENEMY_ACTION_SOCIAL, -15, 30, 1, 2, 1.0f, 100);
            enemy.actions[3] = MakeAction("Respirar", "O vazio parece menos pesado.", ENEMY_ACTION_SOCIAL, 18, -20, 3, 1, .5f, 100);
            enemy.action_count = 4; break;
        case ENEMY_BOSS_LAST_TOWER:
            snprintf(enemy.name, sizeof(enemy.name), "A Ultima Torre");
            enemy.hp_max = 145; enemy.hp = enemy.hp_max; enemy.type = TYPE_NEUTRAL; enemy.boss = true; enemy.aggression = 62;
            for (int i = 0; i < 6; i++) enemy.pattern_ids[i] = (BulletPatternId)i;
            enemy.pattern_count = 6;
            snprintf(enemy.description, sizeof(enemy.description), "A torre responde com a sua propria voz.");
            enemy.actions[0] = MakeAction("Analisar", "", ENEMY_ACTION_ANALYZE, 0, 0, 0, 0, 1.0f, 100);
            enemy.actions[1] = MakeAction("Lembrar", "As paredes ficam mais leves.", ENEMY_ACTION_SOCIAL, 26, -16, 3, 1, .5f, 100);
            enemy.actions[2] = MakeAction("Gritar", "A escada se fecha em volta.", ENEMY_ACTION_SOCIAL, -20, 35, 1, 2, 1.0f, 100);
            enemy.actions[3] = MakeAction("Perdoar", "Uma janela se abre no alto.", ENEMY_ACTION_SOCIAL, 20, -22, 3, 1, .5f, 100);
            enemy.action_count = 4; break;
        case ENEMY_GLOW_FROG:
        default:
            snprintf(enemy.name, sizeof(enemy.name), "Sapo de Vidro");
            enemy.hp_max = 42;
            enemy.hp = enemy.hp_max;
            enemy.type = TYPE_WATER;
            enemy.type2 = TYPE_LIGHT;
            enemy.has_type2 = true;
            enemy.aggression = 55;
            enemy.pattern_ids[0] = PATTERN_SIDE_WAVE;
            enemy.pattern_ids[1] = PATTERN_SPIRAL;
            enemy.pattern_ids[2] = PATTERN_RAIN;
            enemy.pattern_count = 3;
            snprintf(enemy.description, sizeof(enemy.description),
                     "Seu brilho gentil esconde uma pressa estranha.");
            enemy.actions[0] = MakeAction("Analisar", "", ENEMY_ACTION_ANALYZE, 0, 0, 0, 0, 1.0f, 100);
            enemy.actions[1] = MakeAction("Cantar", "O brilho pulsa no ritmo.", ENEMY_ACTION_SOCIAL, 28, -10, 2, 1, 0.50f, 100);
            enemy.actions[1].aggression_threshold = 45;
            enemy.actions[1].calm_multiplier = 1.25f;
            enemy.actions[1].angry_multiplier = 0.40f;
            enemy.actions[2] = MakeAction("Ameacar", "O sapo recua, ofendido.", ENEMY_ACTION_SOCIAL, -20, 35, 1, 2, 1.0f, 100);
            enemy.actions[3] = MakeAction("Acalmar", "A agua volta a ficar lisa.", ENEMY_ACTION_SOCIAL, 18, -20, 3, 1, 0.50f, 100);
            enemy.actions[3].has_type_bonus = true;
            enemy.actions[3].bonus_type = TYPE_WATER;
            enemy.actions[3].type_multiplier = 1.20f;
            enemy.action_count = 4;
            break;
    }
    return enemy;
}

void EnemyTakeDamage(Enemy *enemy, int damage, float effectiveness)
{
    enemy->hp -= damage;
    if (enemy->hp < 0) enemy->hp = 0;
    enemy->hit_flash_strength = effectiveness;
    enemy->hit_flash_timer = effectiveness >= 2.0f ? 0.34f : 0.18f;
    if (enemy->boss && !enemy->phase_two && enemy->hp * 2 <= enemy->hp_max) {
        enemy->phase_two = true;
        enemy->aggression = ClampEnemyValue(enemy->aggression + 20);
        enemy->next_pattern_index += 2;
    }
    EnemyRefreshSpareable(enemy);
}

static float ActionMultiplier(const Enemy *enemy, const EnemyAction *action)
{
    float multiplier = 1.0f;
    int action_index = (int)(action - enemy->actions);

    if (action->has_type_bonus && (enemy->type == action->bonus_type ||
                                   (enemy->has_type2 && enemy->type2 == action->bonus_type))) {
        multiplier *= action->type_multiplier;
    }
    multiplier *= enemy->aggression <= action->aggression_threshold ?
                  action->calm_multiplier : action->angry_multiplier;
    if (enemy->last_action_index == action_index) {
        for (int repeat = 0; repeat < enemy->repeated_action_uses; repeat++) {
            multiplier *= action->repeat_penalty;
        }
    }
    return multiplier;
}

bool EnemyActionAvailable(const Enemy *enemy, int action_index, char *reason, int reason_size)
{
    const EnemyAction *action;

    if (action_index < 0 || action_index >= enemy->action_count) return false;
    action = &enemy->actions[action_index];
    if (action->effect == ENEMY_ACTION_ANALYZE) return true;
    if (action->max_uses > 0 && enemy->action_uses[action_index] >= action->max_uses) {
        snprintf(reason, (size_t)reason_size, "Esgotada");
        return false;
    }
    if (enemy->action_cooldowns[action_index] > 0) {
        snprintf(reason, (size_t)reason_size, "Espere %d turno(s)", enemy->action_cooldowns[action_index]);
        return false;
    }
    if (ActionMultiplier(enemy, action) <= 0.01f) {
        snprintf(reason, (size_t)reason_size, "Ja ouviu demais");
        return false;
    }
    if (enemy->aggression > action->max_aggression_required) {
        snprintf(reason, (size_t)reason_size, "Inquieto demais");
        return false;
    }
    return true;
}

bool EnemyApplyAction(Enemy *enemy, int action_index, char *message, int message_size)
{
    EnemyAction *action;
    char reason[48] = "";
    float multiplier;

    if (!EnemyActionAvailable(enemy, action_index, reason, (int)sizeof(reason))) {
        snprintf(message, (size_t)message_size, "%s.", reason[0] != '\0' ? reason : "Acao indisponivel");
        return false;
    }
    action = &enemy->actions[action_index];
    if (action->effect == ENEMY_ACTION_ANALYZE) {
        enemy->analyzed = true;
        return true;
    }
    multiplier = ActionMultiplier(enemy, action);
    enemy->last_mercy_delta = (int)lroundf((float)action->mercy_delta * multiplier);
    enemy->last_aggression_delta = (int)lroundf((float)action->aggression_delta * multiplier);
    enemy->mercy = ClampEnemyValue(enemy->mercy + enemy->last_mercy_delta);
    enemy->aggression = ClampEnemyValue(enemy->aggression + enemy->last_aggression_delta);
    enemy->action_uses[action_index]++;
    enemy->action_cooldowns[action_index] = action->cooldown_turns + 1;
    enemy->action_last_turn[action_index] = enemy->player_turn;
    if (enemy->last_action_index == action_index) {
        enemy->repeated_action_uses++;
    } else {
        enemy->last_action_index = action_index;
        enemy->repeated_action_uses = 1;
    }
    EnemyRefreshSpareable(enemy);
    snprintf(message, (size_t)message_size, "%s", action->response);
    return true;
}

void EnemyAdvanceTurn(Enemy *enemy)
{
    enemy->player_turn++;
    for (int index = 0; index < enemy->action_count; index++) {
        if (enemy->action_cooldowns[index] > 0) enemy->action_cooldowns[index]--;
    }
}
