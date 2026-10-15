#include "save.h"

#include <stdio.h>
#include <string.h>

#define SAVE_MAGIC 0x41535452u
#define SAVE_PATH "save.dat"

typedef struct {
    uint32_t magic;
    uint32_t version;
    uint32_t checksum;
} SaveHeader;

/* Exact layout of version 2: loading it preserves progress and starts the tutorial. */
typedef struct {
    int best_floor, total_runs, victories, mercy_wins, damage_wins, fragments;
    uint32_t unlocked_items, unlocked_attacks;
    float music_volume, effects_volume;
    bool fullscreen, line_tremor, reduced_effects;
    unsigned int tutorial_seen;
    GameDifficulty difficulty;
    RelicChoiceKind relic_kind;
    int relic_id;
} SaveDataV2;

typedef struct {
    int floor_required;
    int victories_required;
    int mercy_required;
    RelicChoiceKind kind;
    int id;
} UnlockRule;

static const UnlockRule UNLOCKS[] = {
    {2, 0, 0, RELIC_ITEM, ITEM_MOON_HERB}, {3, 0, 0, RELIC_ATTACK, 3},
    {5, 0, 0, RELIC_ITEM, ITEM_TOWER_BELL}, {5, 0, 0, RELIC_ATTACK, 6},
    {1, 1, 0, RELIC_ATTACK, 9}, {1, 0, 10, RELIC_ITEM, ITEM_THORN_BOMB}
};

static uint32_t ChecksumBytes(const void *data, size_t size)
{
    const unsigned char *bytes = data;
    uint32_t value = 2166136261u;

    for (size_t index = 0; index < size; index++) value = (value ^ bytes[index]) * 16777619u;
    return value;
}

static uint32_t Checksum(const SaveData *save) { return ChecksumBytes(save, sizeof(*save)); }

void SaveSetDefaults(SaveData *save)
{
    memset(save, 0, sizeof(*save));
    save->music_volume = 0.70f;
    save->effects_volume = 0.80f;
    save->line_tremor = true;
    save->difficulty = DIFFICULTY_NORMAL;
    save->unlocked_items = (1u << ITEM_TEA) | (1u << ITEM_RED_RIBBON) |
                            (1u << ITEM_WET_CLOAK) | (1u << ITEM_SOLAR_BROOCH);
    save->unlocked_attacks = (1u << 0) | (1u << 1) | (1u << 2);
}

bool SaveLoadPath(SaveData *save, const char *path)
{
    SaveHeader header;
    SaveData loaded;
    FILE *file = fopen(path, "rb");

    if (file == NULL) {
        SaveSetDefaults(save);
        return false;
    }
    const bool header_ok = fread(&header, sizeof(header), 1, file) == 1 && header.magic == SAVE_MAGIC;
    bool valid = false;
    if (header_ok && header.version == SAVE_VERSION) {
        valid = fread(&loaded, sizeof(loaded), 1, file) == 1 && header.checksum == Checksum(&loaded);
    } else if (header_ok && header.version == 2u) {
        SaveDataV2 old = {0};
        if (fread(&old, sizeof(old), 1, file) == 1 &&
            header.checksum == ChecksumBytes(&old, sizeof(old))) {
            SaveSetDefaults(&loaded);
            loaded.best_floor = old.best_floor; loaded.total_runs = old.total_runs;
            loaded.victories = old.victories; loaded.mercy_wins = old.mercy_wins;
            loaded.damage_wins = old.damage_wins; loaded.fragments = old.fragments;
            loaded.unlocked_items = old.unlocked_items; loaded.unlocked_attacks = old.unlocked_attacks;
            loaded.music_volume = old.music_volume; loaded.effects_volume = old.effects_volume;
            loaded.fullscreen = old.fullscreen; loaded.line_tremor = old.line_tremor;
            loaded.reduced_effects = old.reduced_effects; loaded.tutorial_seen = old.tutorial_seen;
            loaded.difficulty = old.difficulty; loaded.relic_kind = old.relic_kind; loaded.relic_id = old.relic_id;
            loaded.tutorial_done = false;
            valid = true;
        }
    }
    fclose(file);
    if (!valid) {
        SaveSetDefaults(save);
        return false;
    }
    *save = loaded;
    return true;
}

bool SaveLoad(SaveData *save) { return SaveLoadPath(save, SAVE_PATH); }

bool SaveWritePath(const SaveData *save, const char *path)
{
    const SaveHeader header = {SAVE_MAGIC, SAVE_VERSION, Checksum(save)};
    FILE *file = fopen(path, "wb");

    if (file == NULL) return false;
    const bool written = fwrite(&header, sizeof(header), 1, file) == 1 &&
                         fwrite(save, sizeof(*save), 1, file) == 1;
    fclose(file);
    return written;
}

bool SaveWrite(const SaveData *save) { return SaveWritePath(save, SAVE_PATH); }

void SaveApplyUnlocks(SaveData *save)
{
    for (size_t index = 0; index < sizeof(UNLOCKS) / sizeof(UNLOCKS[0]); index++) {
        const UnlockRule *rule = &UNLOCKS[index];
        if (save->best_floor >= rule->floor_required && save->victories >= rule->victories_required &&
            save->mercy_wins >= rule->mercy_required) {
            if (rule->kind == RELIC_ITEM) save->unlocked_items |= 1u << rule->id;
            else save->unlocked_attacks |= 1u << rule->id;
        }
    }
}

bool SaveIsItemUnlocked(const SaveData *save, ItemId id)
{
    return id >= 0 && id < ITEM_COUNT && (save->unlocked_items & (1u << id)) != 0;
}

bool SaveIsAttackUnlocked(const SaveData *save, int attack_id)
{
    return attack_id >= 0 && attack_id < SAVE_ATTACK_BITS &&
           (save->unlocked_attacks & (1u << attack_id)) != 0;
}

bool SaveBuyRelic(SaveData *save, RelicChoiceKind kind, int id, int price)
{
    const bool unlocked = kind == RELIC_ITEM ? SaveIsItemUnlocked(save, (ItemId)id) :
                          SaveIsAttackUnlocked(save, id);

    if (!unlocked || save->fragments < price) return false;
    save->fragments -= price;
    save->relic_kind = kind;
    save->relic_id = id;
    return true;
}
