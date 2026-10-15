#include "sprites.h"

#include <stdio.h>
#include <string.h>

typedef struct { SpriteId id; const char *path; float scale; Vector2 offset; } SpriteSpec;

static const SpriteSpec SPECS[] = {
    {SPRITE_ENEMY_DUST_MOTH, "assets/sprites/enemy_mariposa_poeira.png", 1, {0, 0}},
    {SPRITE_ENEMY_GLOW_FROG, "assets/sprites/enemy_sapo_luz.png", 1, {0, 0}},
    {SPRITE_ENEMY_CINDER_BIRD, "assets/sprites/enemy_passaro_brasa.png", 1, {0, 0}},
    {SPRITE_ENEMY_MINT_SLIME, "assets/sprites/enemy_slime_menta.png", 1, {0, 0}},
    {SPRITE_ENEMY_LANTERN_WISP, "assets/sprites/enemy_fogo_fatuo.png", 1, {0, 0}},
    {SPRITE_ENEMY_THORN_HARE, "assets/sprites/enemy_lebre_espinho.png", 1, {0, 0}},
    {SPRITE_ENEMY_SHADE_CAT, "assets/sprites/enemy_gato_sombra.png", 1, {0, 0}},
    {SPRITE_ENEMY_PETAL_KNIGHT, "assets/sprites/enemy_cavaleiro_petala.png", 1, {0, 0}},
    {SPRITE_BOSS_BELLKEEPER_P1, "assets/sprites/boss_sineiro_p1.png", 1, {0, 0}},
    {SPRITE_BOSS_BELLKEEPER_P2, "assets/sprites/boss_sineiro_p2.png", 1, {0, 0}},
    {SPRITE_BOSS_LAST_TOWER_P1, "assets/sprites/boss_torre_final_p1.png", 1, {0, 0}},
    {SPRITE_BOSS_LAST_TOWER_P2, "assets/sprites/boss_torre_final_p2.png", 1, {0, 0}},
    {SPRITE_SOUL, "assets/sprites/soul.png", 1, {0, 0}},
    {SPRITE_TITLE_TOWER, "assets/sprites/title_tower.png", 1, {0, 0}},
    {SPRITE_TYPE_FLAME, "assets/sprites/type_chama.png", 1, {0, 0}},
    {SPRITE_TYPE_WATER, "assets/sprites/type_agua.png", 1, {0, 0}},
    {SPRITE_TYPE_PLANT, "assets/sprites/type_planta.png", 1, {0, 0}},
    {SPRITE_TYPE_LIGHT, "assets/sprites/type_luz.png", 1, {0, 0}},
    {SPRITE_TYPE_SHADOW, "assets/sprites/type_sombra.png", 1, {0, 0}},
    {SPRITE_TYPE_NEUTRAL, "assets/sprites/type_neutro.png", 1, {0, 0}},
    {SPRITE_BULLET_FLAME, "assets/sprites/bullet_chama.png", 1, {0, 0}},
    {SPRITE_BULLET_WATER, "assets/sprites/bullet_agua.png", 1, {0, 0}},
    {SPRITE_BULLET_PLANT, "assets/sprites/bullet_planta.png", 1, {0, 0}},
    {SPRITE_BULLET_LIGHT, "assets/sprites/bullet_luz.png", 1, {0, 0}},
    {SPRITE_BULLET_SHADOW, "assets/sprites/bullet_sombra.png", 1, {0, 0}},
    {SPRITE_BULLET_NEUTRAL, "assets/sprites/bullet_neutro.png", 1, {0, 0}}
    ,{SPRITE_ITEM_TEA, "assets/sprites/item_cha.png", 1, {0, 0}}
    ,{SPRITE_ITEM_MOON_HERB, "assets/sprites/item_erva_lunar.png", 1, {0, 0}}
    ,{SPRITE_ITEM_RED_RIBBON, "assets/sprites/item_fita_vermelha.png", 1, {0, 0}}
    ,{SPRITE_ITEM_TOWER_BELL, "assets/sprites/item_sino_torre.png", 1, {0, 0}}
    ,{SPRITE_ITEM_WET_CLOAK, "assets/sprites/item_manto_umido.png", 1, {0, 0}}
    ,{SPRITE_ITEM_SOLAR_BROOCH, "assets/sprites/item_broche_solar.png", 1, {0, 0}}
    ,{SPRITE_ITEM_FIRECRACKER, "assets/sprites/item_fogos.png", 1, {0, 0}}
    ,{SPRITE_ITEM_THORN_BOMB, "assets/sprites/item_bomba_espinho.png", 1, {0, 0}}
    ,{SPRITE_BG_PASTEL, "assets/sprites/bg_pastel.png", 1, {0, 0}}
    ,{SPRITE_BG_BOSS, "assets/sprites/bg_boss.png", 1, {0, 0}}
    ,{SPRITE_ATTACK_BRASA, "assets/sprites/attack_brasa.png", 1, {0, 0}}
    ,{SPRITE_ATTACK_GOTA_CLARA, "assets/sprites/attack_gota_clara.png", 1, {0, 0}}
    ,{SPRITE_ATTACK_RAIZ_RAPIDA, "assets/sprites/attack_raiz_rapida.png", 1, {0, 0}}
    ,{SPRITE_ATTACK_RAIO_MANSO, "assets/sprites/attack_raio_manso.png", 1, {0, 0}}
    ,{SPRITE_ATTACK_ECO_SOMBRIO, "assets/sprites/attack_eco_sombrio.png", 1, {0, 0}}
    ,{SPRITE_ATTACK_GOLPE_NEUTRO, "assets/sprites/attack_golpe_neutro.png", 1, {0, 0}}
    ,{SPRITE_ATTACK_CHAMA_ALTA, "assets/sprites/attack_chama_alta.png", 1, {0, 0}}
    ,{SPRITE_ATTACK_MARE_CURTA, "assets/sprites/attack_mare_curta.png", 1, {0, 0}}
    ,{SPRITE_ATTACK_VINHA_FINA, "assets/sprites/attack_vinha_fina.png", 1, {0, 0}}
    ,{SPRITE_ATTACK_LUZ_GUIA, "assets/sprites/attack_luz_guia.png", 1, {0, 0}}
    ,{SPRITE_ATTACK_SOMBRA_FRIA, "assets/sprites/attack_sombra_fria.png", 1, {0, 0}}
    ,{SPRITE_ATTACK_PULSO_CALMO, "assets/sprites/attack_pulso_calmo.png", 1, {0, 0}}
    ,{SPRITE_ATTACK_FAGULHA, "assets/sprites/attack_fagulha.png", 1, {0, 0}}
    ,{SPRITE_ATTACK_ORVALHO, "assets/sprites/attack_orvalho.png", 1, {0, 0}}
    ,{SPRITE_ATTACK_ESPINHO_LUNAR, "assets/sprites/attack_espinho_lunar.png", 1, {0, 0}}
    ,{SPRITE_ATTACK_SINO_VAZIO, "assets/sprites/attack_sino_vazio.png", 1, {0, 0}}
    ,{SPRITE_FX_TYPE_FLAME, "assets/sprites/fx_type_chama.png", 1, {0, 0}}
    ,{SPRITE_FX_TYPE_WATER, "assets/sprites/fx_type_agua.png", 1, {0, 0}}
    ,{SPRITE_FX_TYPE_PLANT, "assets/sprites/fx_type_planta.png", 1, {0, 0}}
    ,{SPRITE_FX_TYPE_LIGHT, "assets/sprites/fx_type_luz.png", 1, {0, 0}}
    ,{SPRITE_FX_TYPE_SHADOW, "assets/sprites/fx_type_sombra.png", 1, {0, 0}}
    ,{SPRITE_FX_TYPE_NEUTRAL, "assets/sprites/fx_type_neutro.png", 1, {0, 0}}
    ,{SPRITE_FX_ATTACK_BRASA, "assets/sprites/fx_brasa.png", 1, {0, 0}}
    ,{SPRITE_FX_ATTACK_GOTA_CLARA, "assets/sprites/fx_gota_clara.png", 1, {0, 0}}
    ,{SPRITE_FX_ATTACK_RAIZ_RAPIDA, "assets/sprites/fx_raiz_rapida.png", 1, {0, 0}}
    ,{SPRITE_FX_ATTACK_RAIO_MANSO, "assets/sprites/fx_raio_manso.png", 1, {0, 0}}
    ,{SPRITE_FX_ATTACK_ECO_SOMBRIO, "assets/sprites/fx_eco_sombrio.png", 1, {0, 0}}
    ,{SPRITE_FX_ATTACK_GOLPE_NEUTRO, "assets/sprites/fx_golpe_neutro.png", 1, {0, 0}}
    ,{SPRITE_FX_ATTACK_CHAMA_ALTA, "assets/sprites/fx_chama_alta.png", 1, {0, 0}}
    ,{SPRITE_FX_ATTACK_MARE_CURTA, "assets/sprites/fx_mare_curta.png", 1, {0, 0}}
    ,{SPRITE_FX_ATTACK_VINHA_FINA, "assets/sprites/fx_vinha_fina.png", 1, {0, 0}}
    ,{SPRITE_FX_ATTACK_LUZ_GUIA, "assets/sprites/fx_luz_guia.png", 1, {0, 0}}
    ,{SPRITE_FX_ATTACK_SOMBRA_FRIA, "assets/sprites/fx_sombra_fria.png", 1, {0, 0}}
    ,{SPRITE_FX_ATTACK_PULSO_CALMO, "assets/sprites/fx_pulso_calmo.png", 1, {0, 0}}
    ,{SPRITE_FX_ATTACK_FAGULHA, "assets/sprites/fx_fagulha.png", 1, {0, 0}}
    ,{SPRITE_FX_ATTACK_ORVALHO, "assets/sprites/fx_orvalho.png", 1, {0, 0}}
    ,{SPRITE_FX_ATTACK_ESPINHO_LUNAR, "assets/sprites/fx_espinho_lunar.png", 1, {0, 0}}
    ,{SPRITE_FX_ATTACK_SINO_VAZIO, "assets/sprites/fx_sino_vazio.png", 1, {0, 0}}
    ,{SPRITE_BULLET_PATTERN_RAIN, "assets/sprites/bullet_rain.png", 1, {0, 0}}
    ,{SPRITE_BULLET_PATTERN_SIDE_WAVE, "assets/sprites/bullet_side_wave.png", 1, {0, 0}}
    ,{SPRITE_BULLET_PATTERN_SPIRAL, "assets/sprites/bullet_spiral.png", 1, {0, 0}}
    ,{SPRITE_BULLET_PATTERN_FAST_RAIN, "assets/sprites/bullet_fast_rain.png", 1, {0, 0}}
    ,{SPRITE_BULLET_PATTERN_WIDE_WAVE, "assets/sprites/bullet_wide_wave.png", 1, {0, 0}}
    ,{SPRITE_BULLET_PATTERN_TIGHT_SPIRAL, "assets/sprites/bullet_tight_spiral.png", 1, {0, 0}}
    ,{SPRITE_BULLET_PATTERN_PETAL_ORBIT, "assets/sprites/bullet_petal_orbit.png", 1, {0, 0}}
    ,{SPRITE_BULLET_PATTERN_BOUNCE_ORBS, "assets/sprites/bullet_bounce_orbs.png", 1, {0, 0}}
    ,{SPRITE_BULLET_PATTERN_MOVING_WALL, "assets/sprites/bullet_moving_wall.png", 1, {0, 0}}
    ,{SPRITE_BULLET_PATTERN_SPLIT_SHOTS, "assets/sprites/bullet_split_shots.png", 1, {0, 0}}
    ,{SPRITE_BULLET_PATTERN_CROSS_LASERS, "assets/sprites/bullet_cross_lasers.png", 1, {0, 0}}
    ,{SPRITE_BULLET_PATTERN_PAUSE_DARTS, "assets/sprites/bullet_pause_darts.png", 1, {0, 0}}
    ,{SPRITE_BULLET_PATTERN_RING_BURST, "assets/sprites/bullet_ring_burst.png", 1, {0, 0}}
    ,{SPRITE_BULLET_PATTERN_DIAGONAL_RAIN, "assets/sprites/bullet_diagonal_rain.png", 1, {0, 0}}
    ,{SPRITE_BULLET_PATTERN_CHASE_WAVE, "assets/sprites/bullet_chase_wave.png", 1, {0, 0}}
    ,{SPRITE_BULLET_PATTERN_PINWHEEL, "assets/sprites/bullet_pinwheel.png", 1, {0, 0}}
    ,{SPRITE_BULLET_PATTERN_GRAVITY_FLIP, "assets/sprites/bullet_gravity_flip.png", 1, {0, 0}}
    ,{SPRITE_BULLET_PATTERN_SHADOW_BLOOM, "assets/sprites/bullet_shadow_bloom.png", 1, {0, 0}}
    ,{SPRITE_ARENA_BORDER, "assets/sprites/arena_border.png", 1, {0, 0}}
    ,{SPRITE_ARENA_BG, "assets/sprites/arena_bg_pastel.png", 1, {0, 0}}
    ,{SPRITE_WARN_DANGER, "assets/sprites/warn_danger.png", 1, {0, 0}}
    ,{SPRITE_FX_HIT_SOUL, "assets/sprites/fx_hit_soul.png", 1, {0, 0}}
    ,{SPRITE_WARN_GRAVITY, "assets/sprites/warn_gravity.png", 1, {0, 0}}
    ,{SPRITE_GRAVITY_WALL_GLOW, "assets/sprites/gravity_wall_glow.png", 1, {0, 0}}
    ,{SPRITE_THORN_WALL, "assets/sprites/thorn_wall.png", 1, {0, 0}}
    ,{SPRITE_THORN_WALL_WARN, "assets/sprites/thorn_wall_warn.png", 1, {0, 0}}
    ,{SPRITE_SHADOW_ORB, "assets/sprites/shadow_orb.png", 1, {0, 0}}
    ,{SPRITE_SHADOW_BLOOM_WARN, "assets/sprites/shadow_bloom_warn.png", 1, {0, 0}}
    ,{SPRITE_SHADOW_BURST, "assets/sprites/shadow_burst.png", 1, {0, 0}}
};

bool SpriteParseMetadataText(const char *text, SpriteMetadata *metadata)
{
    SpriteMetadata parsed = {1, 1, 0, 0};
    int read = sscanf(text, "frames=%d fps=%d frame_w=%d frame_h=%d", &parsed.frames, &parsed.fps,
                      &parsed.frame_width, &parsed.frame_height);
    if (read != 4 || parsed.frames < 1 || parsed.fps < 1 || parsed.frame_width < 1 || parsed.frame_height < 1) return false;
    *metadata = parsed;
    return true;
}

static void LoadOne(SpriteSystem *sprites, const SpriteSpec *spec)
{
    Sprite *sprite = &sprites->sprites[spec->id];
    char metadata_path[128];
    char sheet_path[128];
    const char *texture_path = spec->path;
    FILE *file;

    *sprite = (Sprite){0};
    sprite->scale = spec->scale;
    sprite->offset = spec->offset;
    sprite->metadata = (SpriteMetadata){1, 1, 0, 0};
    snprintf(sheet_path, sizeof(sheet_path), "%s", spec->path);
    char *dot = strrchr(sheet_path, '.');
    if (dot != NULL) strcpy(dot, "_sheet.png");
    if (FileExists(sheet_path)) texture_path = sheet_path;
    if (!FileExists(texture_path)) {
#ifndef NDEBUG
        TraceLog(LOG_DEBUG, "Sprite opcional ausente: %s", spec->path);
#endif
        return;
    }
    sprite->texture = LoadTexture(texture_path);
    sprite->loaded = sprite->texture.id != 0;
    if (!sprite->loaded) return;
    SetTextureFilter(sprite->texture, TEXTURE_FILTER_POINT);
    snprintf(metadata_path, sizeof(metadata_path), "%s", spec->path);
    dot = strrchr(metadata_path, '.');
    if (dot != NULL) strcpy(dot, ".txt");
    file = fopen(metadata_path, "r");
    if (file != NULL) {
        char text[128] = {0};
        if (fgets(text, sizeof(text), file)) SpriteParseMetadataText(text, &sprite->metadata);
        fclose(file);
    }
}

void SpritesInit(SpriteSystem *sprites)
{
    memset(sprites, 0, sizeof(*sprites));
    for (size_t index = 0; index < sizeof(SPECS) / sizeof(SPECS[0]); index++) LoadOne(sprites, &SPECS[index]);
}

void SpritesUnload(SpriteSystem *sprites)
{
    for (int index = 0; index < SPRITE_COUNT; index++) {
        if (sprites->sprites[index].loaded) UnloadTexture(sprites->sprites[index].texture);
    }
    memset(sprites, 0, sizeof(*sprites));
}

void SpritesReload(SpriteSystem *sprites) { SpritesUnload(sprites); SpritesInit(sprites); }
const Sprite *sprite_get(const SpriteSystem *sprites, SpriteId id)
{
    return id >= 0 && id < SPRITE_COUNT && sprites->sprites[id].loaded ? &sprites->sprites[id] : NULL;
}
SpriteId SpriteEnemyId(EnemyId enemy, bool phase_two)
{
    if (enemy == ENEMY_BOSS_BELLKEEPER) return phase_two ? SPRITE_BOSS_BELLKEEPER_P2 : SPRITE_BOSS_BELLKEEPER_P1;
    if (enemy == ENEMY_BOSS_LAST_TOWER) return phase_two ? SPRITE_BOSS_LAST_TOWER_P2 : SPRITE_BOSS_LAST_TOWER_P1;
    return (SpriteId)(SPRITE_ENEMY_DUST_MOTH + enemy);
}
SpriteId SpriteTypeId(Type type) { return (SpriteId)(SPRITE_TYPE_FLAME + type); }
SpriteId SpriteBulletId(Type type) { return (SpriteId)(SPRITE_BULLET_FLAME + type); }
SpriteId SpriteItemId(ItemId item)
{
    return item >= ITEM_TEA && item < ITEM_COUNT ? (SpriteId)(SPRITE_ITEM_TEA + item) : SPRITE_ITEM_TEA;
}
SpriteId SpriteBulletPatternId(BulletPatternId pattern)
{
    return pattern >= PATTERN_RAIN && pattern < PATTERN_COUNT ?
        (SpriteId)(SPRITE_BULLET_PATTERN_RAIN + pattern) : SPRITE_BULLET_PATTERN_RAIN;
}
SpriteId SpriteAttackId(AttackId attack) { return (SpriteId)(SPRITE_ATTACK_BRASA + attack); }
SpriteId SpriteFxAttackId(AttackId attack) { return (SpriteId)(SPRITE_FX_ATTACK_BRASA + attack); }
SpriteId SpriteFxTypeId(Type type) { return (SpriteId)(SPRITE_FX_TYPE_FLAME + type); }

void SpriteDrawCentered(const SpriteSystem *sprites, SpriteId id, Vector2 center, Color tint,
                        float time, bool debug_bounds)
{
    const Sprite *sprite = sprite_get(sprites, id);
    if (sprite == NULL) return;
    const int frame = sprite->metadata.frames > 1 ? ((int)(time * sprite->metadata.fps) % sprite->metadata.frames) : 0;
    const int frame_w = sprite->metadata.frame_width > 0 ? sprite->metadata.frame_width : sprite->texture.width;
    const int frame_h = sprite->metadata.frame_height > 0 ? sprite->metadata.frame_height : sprite->texture.height;
    const Rectangle source = {(float)(frame * frame_w), 0, (float)frame_w, (float)frame_h};
    const Rectangle destination = {center.x - frame_w * sprite->scale * 0.5f + sprite->offset.x,
                                   center.y - frame_h * sprite->scale * 0.5f + sprite->offset.y,
                                   frame_w * sprite->scale, frame_h * sprite->scale};
    DrawTexturePro(sprite->texture, source, destination, (Vector2){0, 0}, 0, tint);
    if (debug_bounds) { DrawRectangleLinesEx(destination, 1, MAGENTA); DrawCircleV(center, 2, MAGENTA); }
}

void SpriteDrawCenteredScaled(const SpriteSystem *sprites, SpriteId id, Vector2 center, Color tint,
                              float time, float scale, bool debug_bounds)
{
    const Sprite *sprite = sprite_get(sprites, id);
    if (sprite == NULL) return;
    const int frame = sprite->metadata.frames > 1 ? ((int)(time * sprite->metadata.fps) % sprite->metadata.frames) : 0;
    const int frame_w = sprite->metadata.frame_width > 0 ? sprite->metadata.frame_width : sprite->texture.width;
    const int frame_h = sprite->metadata.frame_height > 0 ? sprite->metadata.frame_height : sprite->texture.height;
    const float full_scale = sprite->scale * scale;
    const Rectangle source = {(float)(frame * frame_w), 0, (float)frame_w, (float)frame_h};
    const Rectangle destination = {center.x - frame_w * full_scale * 0.5f + sprite->offset.x,
                                   center.y - frame_h * full_scale * 0.5f + sprite->offset.y,
                                   frame_w * full_scale, frame_h * full_scale};
    DrawTexturePro(sprite->texture, source, destination, (Vector2){0, 0}, 0, tint);
    if (debug_bounds) DrawRectangleLinesEx(destination, 1, MAGENTA);
}

void SpriteDrawCenteredRotated(const SpriteSystem *sprites, SpriteId id, Vector2 center, Color tint,
                               float time, float rotation, float scale, bool debug_bounds)
{
    const Sprite *sprite = sprite_get(sprites, id);
    if (sprite == NULL) return;
    const int frame = sprite->metadata.frames > 1 ? ((int)(time * sprite->metadata.fps) % sprite->metadata.frames) : 0;
    const int frame_w = sprite->metadata.frame_width > 0 ? sprite->metadata.frame_width : sprite->texture.width;
    const int frame_h = sprite->metadata.frame_height > 0 ? sprite->metadata.frame_height : sprite->texture.height;
    const Rectangle source = {(float)(frame * frame_w), 0, (float)frame_w, (float)frame_h};
    const float full_scale = sprite->scale * scale;
    const Rectangle destination = {center.x + sprite->offset.x, center.y + sprite->offset.y,
                                   frame_w * full_scale, frame_h * full_scale};
    DrawTexturePro(sprite->texture, source, destination,
                   (Vector2){destination.width * 0.5f, destination.height * 0.5f}, rotation, tint);
    if (debug_bounds) DrawCircleLines((int)center.x, (int)center.y, frame_w * full_scale * 0.5f, MAGENTA);
}

void SpriteDrawFit(const SpriteSystem *sprites, SpriteId id, Rectangle destination, Color tint)
{
    const Sprite *sprite = sprite_get(sprites, id);
    if (sprite == NULL) return;
    DrawTexturePro(sprite->texture, (Rectangle){0, 0, (float)sprite->texture.width,
                   (float)sprite->texture.height}, destination, (Vector2){0, 0}, 0.0f, tint);
}

static void ExportBlank(const char *path, int width, int height, Color color)
{
    Image image = GenImageColor(width, height, color);
    ExportImage(image, path);
    UnloadImage(image);
}

void SpritesExportPlaceholders(void)
{
    MakeDirectory("assets"); MakeDirectory("assets/sprites_template");
    static const char *const enemy_names[] = {"mariposa_poeira", "sapo_luz", "passaro_brasa",
        "slime_menta", "fogo_fatuo", "lebre_espinho", "gato_sombra", "cavaleiro_petala"};
    static const char *const type_names[] = {"chama", "agua", "planta", "luz", "sombra", "neutro"};
    for (int index = 0; index < 8; index++) {
        char path[128]; snprintf(path, sizeof(path), "assets/sprites_template/enemy_%s.png", enemy_names[index]);
        ExportBlank(path, 96, 96, BLANK);
    }
    ExportBlank("assets/sprites_template/boss_sineiro_p1.png", 160, 160, BLANK);
    ExportBlank("assets/sprites_template/boss_torre_final_p1.png", 160, 160, BLANK);
    ExportBlank("assets/sprites_template/soul.png", 16, 16, (Color){231, 87, 112, 255});
    for (int index = 0; index < TYPE_COUNT; index++) {
        char path[128]; snprintf(path, sizeof(path), "assets/sprites_template/type_%s.png", type_names[index]);
        ExportBlank(path, 24, 24, type_color((Type)index));
        snprintf(path, sizeof(path), "assets/sprites_template/fx_type_%s_sheet.png", type_names[index]);
        ExportBlank(path, 96, 96, BLANK);
        snprintf(path, sizeof(path), "assets/sprites_template/bullet_%s.png", type_names[index]);
        ExportBlank(path, 12, 12, type_color((Type)index));
    }
    static const char *const attack_names[] = {"brasa", "gota_clara", "raiz_rapida", "raio_manso",
        "eco_sombrio", "golpe_neutro", "chama_alta", "mare_curta", "vinha_fina", "luz_guia",
        "sombra_fria", "pulso_calmo", "fagulha", "orvalho", "espinho_lunar", "sino_vazio"};
    for (int index = 0; index < PLAYER_ATTACK_COUNT; index++) {
        char path[128];
        snprintf(path, sizeof(path), "assets/sprites_template/attack_%s.png", attack_names[index]);
        ExportBlank(path, 24, 24, BLANK);
        snprintf(path, sizeof(path), "assets/sprites_template/fx_%s_sheet.png", attack_names[index]);
        ExportBlank(path, 96, 96, BLANK);
    }
    static const char *const pattern_names[] = {"rain", "side_wave", "spiral", "fast_rain",
        "wide_wave", "tight_spiral", "petal_orbit", "bounce_orbs", "moving_wall",
        "split_shots", "cross_lasers", "pause_darts", "ring_burst", "diagonal_rain",
        "chase_wave", "pinwheel", "gravity_flip", "shadow_bloom"};
    for (int index = 0; index < PATTERN_COUNT; index++) {
        char path[128];
        snprintf(path, sizeof(path), "assets/sprites_template/bullet_%s.png", pattern_names[index]);
        ExportBlank(path, 16, 16, BLANK);
    }
    ExportBlank("assets/sprites_template/warn_danger.png", 32, 32, (Color){245, 90, 82, 255});
    ExportBlank("assets/sprites_template/arena_border.png", 16, 16, RAYWHITE);
    ExportBlank("assets/sprites_template/arena_bg_pastel.png", 320, 180, BLACK);
    ExportBlank("assets/sprites_template/fx_hit_soul.png", 32, 32, BLANK);
    ExportBlank("assets/sprites_template/fx_hit_soul_sheet.png", 128, 32, BLANK);
    ExportBlank("assets/sprites_template/warn_gravity.png", 32, 32, BLANK);
    ExportBlank("assets/sprites_template/gravity_wall_glow_sheet.png", 64, 16, BLANK);
    ExportBlank("assets/sprites_template/thorn_wall_sheet.png", 64, 48, BLANK);
    ExportBlank("assets/sprites_template/thorn_wall_warn.png", 16, 48, BLANK);
    ExportBlank("assets/sprites_template/shadow_orb_sheet.png", 128, 32, BLANK);
    ExportBlank("assets/sprites_template/shadow_bloom_warn.png", 64, 64, BLANK);
    ExportBlank("assets/sprites_template/shadow_burst_sheet.png", 576, 96, BLANK);
}
