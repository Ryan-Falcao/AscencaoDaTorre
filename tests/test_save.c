#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "save.h"

typedef struct {
    uint32_t magic, version, checksum;
} TestHeader;

typedef struct {
    int best_floor, total_runs, victories, mercy_wins, damage_wins, fragments;
    uint32_t unlocked_items, unlocked_attacks;
    float music_volume, effects_volume;
    bool fullscreen, line_tremor, reduced_effects;
    unsigned int tutorial_seen;
    GameDifficulty difficulty;
    RelicChoiceKind relic_kind;
    int relic_id;
} TestSaveV2;

static uint32_t ChecksumV2(const TestSaveV2 *save)
{
    const unsigned char *bytes = (const unsigned char *)save;
    uint32_t value = 2166136261u;
    for (size_t index = 0; index < sizeof(*save); index++) value = (value ^ bytes[index]) * 16777619u;
    return value;
}

int main(void)
{
    const char *path = "test_save_temp.dat";
    SaveData saved;
    SaveData loaded;
    FILE *file;

    SaveSetDefaults(&saved);
    saved.best_floor = 5;
    saved.fragments = 22;
    saved.mercy_wins = 10;
    saved.reduced_effects = true;
    saved.difficulty = DIFFICULTY_HARD;
    SaveApplyUnlocks(&saved);
    assert(SaveIsAttackUnlocked(&saved, 3));
    assert(SaveIsItemUnlocked(&saved, ITEM_THORN_BOMB));
    assert(SaveWritePath(&saved, path));
    assert(SaveLoadPath(&loaded, path));
    assert(loaded.best_floor == 5 && loaded.fragments == 22);
    assert(loaded.reduced_effects && loaded.difficulty == DIFFICULTY_HARD);

    {
        TestSaveV2 old = {0};
        old.best_floor = 4;
        old.fragments = 9;
        old.music_volume = 0.5f;
        old.effects_volume = 0.6f;
        old.difficulty = DIFFICULTY_CALM;
        file = fopen(path, "wb");
        assert(file != NULL);
        const TestHeader old_header = {0x41535452u, 2u, ChecksumV2(&old)};
        assert(fwrite(&old_header, sizeof(old_header), 1, file) == 1);
        assert(fwrite(&old, sizeof(old), 1, file) == 1);
        fclose(file);
        assert(SaveLoadPath(&loaded, path));
        assert(loaded.best_floor == 4 && loaded.fragments == 9);
        assert(!loaded.tutorial_done && loaded.difficulty == DIFFICULTY_CALM);
    }

    file = fopen(path, "wb");
    assert(file != NULL);
    fputs("corrompido", file);
    fclose(file);
    assert(!SaveLoadPath(&loaded, path));
    assert(loaded.best_floor == 0);
    assert(loaded.difficulty == DIFFICULTY_NORMAL);

    file = fopen(path, "wb");
    assert(file != NULL);
    fwrite(&(uint32_t){0x41535452u}, sizeof(uint32_t), 1, file);
    fwrite(&(uint32_t){0u}, sizeof(uint32_t), 1, file);
    fclose(file);
    assert(!SaveLoadPath(&loaded, path));
    remove(path);
    puts("Save, corrupcao, versao e desbloqueios verificados.");
    return 0;
}
