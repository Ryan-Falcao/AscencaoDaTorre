TARGET := ascensao_torre
SRC := src/main.c src/input.c src/audio.c src/sprites.c src/fx.c src/playtest.c src/save.c src/render.c src/game.c src/tower.c src/battle.c src/player.c src/items.c src/enemy.c src/types.c src/bullets.c src/patterns.c src/motion_patterns.c src/hazards.c src/qte.c src/ui.c
INCLUDE := -Iinclude
CFLAGS := -std=c11 -Wall -Wextra -Wpedantic $(INCLUDE)
RAYLIB_CFLAGS := $(shell pkg-config --cflags raylib 2>/dev/null)
RAYLIB_LIBS := $(shell pkg-config --libs raylib 2>/dev/null)
UNAME_S := $(shell uname -s 2>/dev/null)

ifeq ($(strip $(RAYLIB_LIBS)),)
RAYLIB_LIBS := -lraylib
endif

ifneq ($(findstring MINGW,$(UNAME_S)),MINGW)
RAYLIB_LIBS += -lm
endif

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(RAYLIB_CFLAGS) $(SRC) -o $@ $(RAYLIB_LIBS)

run: $(TARGET)
	./$(TARGET)

release:
	$(MAKE) CFLAGS="$(CFLAGS) -O2 -DNDEBUG" $(TARGET)

test-types:
	$(CC) $(CFLAGS) tests/test_types.c src/types.c -o test_types
	./test_types

test-qte:
	$(CC) $(CFLAGS) $(RAYLIB_CFLAGS) tests/test_qte.c src/qte.c src/input.c -o test_qte $(RAYLIB_LIBS)
	./test_qte

test-phase6:
	$(CC) $(CFLAGS) $(RAYLIB_CFLAGS) tests/test_phase6.c src/player.c src/items.c src/enemy.c src/types.c -o test_phase6 $(RAYLIB_LIBS)
	./test_phase6

test-tower:
	$(CC) $(CFLAGS) $(RAYLIB_CFLAGS) tests/test_tower.c src/tower.c src/player.c src/items.c src/enemy.c src/types.c -o test_tower $(RAYLIB_LIBS)
	./test_tower

test-save:
	$(CC) $(CFLAGS) tests/test_save.c src/save.c src/items.c -o test_save
	./test_save

test-balance:
	$(CC) $(CFLAGS) tests/test_balance.c -o test_balance
	./test_balance

test-sprites:
	$(CC) $(CFLAGS) $(RAYLIB_CFLAGS) tests/test_sprites.c src/sprites.c src/types.c -o test_sprites $(RAYLIB_LIBS)
	./test_sprites

test-phase10:
	$(CC) $(CFLAGS) $(RAYLIB_CFLAGS) tests/test_phase10.c src/player.c src/items.c src/enemy.c src/types.c -o test_phase10 $(RAYLIB_LIBS)
	./test_phase10

clean:
	$(RM) $(TARGET) $(TARGET).exe

.PHONY: all run release test-types test-qte test-phase6 test-tower test-save test-balance test-sprites test-phase10 clean
