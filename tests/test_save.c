#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "save.h"

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
    SaveApplyUnlocks(&saved);
    assert(SaveIsAttackUnlocked(&saved, 3));
    assert(SaveIsItemUnlocked(&saved, ITEM_THORN_BOMB));
    assert(SaveWritePath(&saved, path));
    assert(SaveLoadPath(&loaded, path));
    assert(loaded.best_floor == 5 && loaded.fragments == 22);

    file = fopen(path, "wb");
    assert(file != NULL);
    fputs("corrompido", file);
    fclose(file);
    assert(!SaveLoadPath(&loaded, path));
    assert(loaded.best_floor == 0);

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
