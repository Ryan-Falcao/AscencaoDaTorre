#ifndef SAVE_H
#define SAVE_H

#include <stdbool.h>
#include <stdint.h>

#include "items.h"

#define SAVE_VERSION 1u
#define SAVE_ATTACK_BITS 16

typedef enum {
    RELIC_NONE,
    RELIC_ITEM,
    RELIC_ATTACK
} RelicChoiceKind;

typedef struct {
    int best_floor;
    int total_runs;
    int victories;
    int mercy_wins;
    int damage_wins;
    int fragments;
    uint32_t unlocked_items;
    uint32_t unlocked_attacks;
    float music_volume;
    float effects_volume;
    bool fullscreen;
    bool line_tremor;
    RelicChoiceKind relic_kind;
    int relic_id;
} SaveData;

void SaveSetDefaults(SaveData *save);
bool SaveLoad(SaveData *save);
bool SaveLoadPath(SaveData *save, const char *path);
bool SaveWrite(const SaveData *save);
bool SaveWritePath(const SaveData *save, const char *path);
void SaveApplyUnlocks(SaveData *save);
bool SaveIsItemUnlocked(const SaveData *save, ItemId id);
bool SaveIsAttackUnlocked(const SaveData *save, int attack_id);
bool SaveBuyRelic(SaveData *save, RelicChoiceKind kind, int id, int price);

#endif
