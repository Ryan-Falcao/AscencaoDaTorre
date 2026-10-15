#ifndef AUDIO_H
#define AUDIO_H

#include <stdbool.h>

#include "raylib.h"

typedef enum {
    AUDIO_CURSOR,
    AUDIO_CONFIRM,
    AUDIO_CANCEL,
    AUDIO_TEXT,
    AUDIO_DAMAGE_TAKEN,
    AUDIO_DAMAGE_DEALT,
    AUDIO_SUPER_EFFECTIVE,
    AUDIO_NOT_EFFECTIVE,
    AUDIO_QTE_TICK,
    AUDIO_QTE_ERROR,
    AUDIO_QTE_PERFECT,
    AUDIO_HEAL,
    AUDIO_BUY,
    AUDIO_LEVEL_UP,
    AUDIO_DEFEAT,
    AUDIO_VICTORY,
    AUDIO_EFFECT_COUNT
} AudioEffect;

typedef enum {
    MUSIC_COMMON,
    MUSIC_BOSS,
    MUSIC_CALM,
    MUSIC_TRACK_COUNT
} MusicTrack;

typedef struct {
    bool available;
    Sound effects[AUDIO_EFFECT_COUNT];
    Sound tracks[MUSIC_TRACK_COUNT];
    float music_volume;
    float effects_volume;
    MusicTrack current_track;
    MusicTrack requested_track;
    float fade;
} AudioState;

void AudioInit(AudioState *audio, float music_volume, float effects_volume);
void AudioUnload(AudioState *audio);
void AudioSetVolumes(AudioState *audio, float music_volume, float effects_volume);
void AudioPlay(AudioState *audio, AudioEffect effect);
void AudioRequestTrack(AudioState *audio, MusicTrack track);
void AudioUpdate(AudioState *audio, float delta_time);

#endif
