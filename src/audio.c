#include "audio.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#define AUDIO_SAMPLE_RATE 22050
#define AUDIO_FADE_SECONDS 0.50f

static float ClampVolume(float value)
{
    if (value < 0.0f) return 0.0f;
    if (value > 1.0f) return 1.0f;
    return value;
}

static const char *const EFFECT_FILES[AUDIO_EFFECT_COUNT] = {
    "cursor", "confirm", "cancel", "text", "damage_taken", "damage_dealt",
    "super_effective", "not_effective", "qte_tick", "qte_error", "qte_perfect",
    "heal", "buy", "level_up", "defeat", "victory"
};

static Sound GenerateTone(float frequency, float seconds, bool triangle, float volume)
{
    const unsigned int frames = (unsigned int)(AUDIO_SAMPLE_RATE * seconds);
    short *samples = MemAlloc((int)(frames * sizeof(*samples)));
    Wave wave = {0};

    if (samples == NULL) return (Sound){0};
    for (unsigned int index = 0; index < frames; index++) {
        const float progress = (float)index / (float)frames;
        const float phase = fmodf((float)index * frequency / AUDIO_SAMPLE_RATE, 1.0f);
        const float raw = triangle ? 1.0f - 4.0f * fabsf(phase - 0.5f) :
                         (phase < 0.5f ? 1.0f : -1.0f);
        const float envelope = (1.0f - progress) * (progress < 0.06f ? progress / 0.06f : 1.0f);
        samples[index] = (short)(raw * envelope * volume * 32767.0f);
    }
    wave.frameCount = frames;
    wave.sampleRate = AUDIO_SAMPLE_RATE;
    wave.sampleSize = 16;
    wave.channels = 1;
    wave.data = samples;
    Sound sound = LoadSoundFromWave(wave);
    UnloadWave(wave);
    return sound;
}

static Sound LoadEffect(int index)
{
    char path[96];
    snprintf(path, sizeof(path), "assets/audio/%s.wav", EFFECT_FILES[index]);
    if (FileExists(path)) return LoadSound(path);
    return GenerateTone(260.0f + (float)(index * 31), 0.075f + (float)(index % 3) * 0.02f,
                        index % 2 == 0, 0.22f);
}

static Sound GenerateTrack(int track)
{
    const float frequencies[] = {220.0f, 246.94f, 293.66f, 329.63f, 392.0f};
    const unsigned int frames = AUDIO_SAMPLE_RATE * 4u;
    short *samples = MemAlloc((int)(frames * sizeof(*samples)));
    Wave wave = {0};

    if (samples == NULL) return (Sound){0};
    for (unsigned int index = 0; index < frames; index++) {
        const int note = (int)((index / (AUDIO_SAMPLE_RATE / 2)) % 5u);
        const float frequency = frequencies[(note + track * 2) % 5] * (track == MUSIC_BOSS ? 0.5f : 1.0f);
        const float phase = fmodf((float)index * frequency / AUDIO_SAMPLE_RATE, 1.0f);
        const float raw = 1.0f - 4.0f * fabsf(phase - 0.5f);
        samples[index] = (short)(raw * 2600.0f);
    }
    wave.frameCount = frames;
    wave.sampleRate = AUDIO_SAMPLE_RATE;
    wave.sampleSize = 16;
    wave.channels = 1;
    wave.data = samples;
    Sound sound = LoadSoundFromWave(wave);
    UnloadWave(wave);
    return sound;
}

void AudioInit(AudioState *audio, float music_volume, float effects_volume)
{
    memset(audio, 0, sizeof(*audio));
    if (!IsAudioDeviceReady()) InitAudioDevice();
    audio->available = IsAudioDeviceReady();
    audio->current_track = MUSIC_COMMON;
    audio->requested_track = MUSIC_COMMON;
    AudioSetVolumes(audio, music_volume, effects_volume);
    if (!audio->available) return;
    for (int index = 0; index < AUDIO_EFFECT_COUNT; index++) audio->effects[index] = LoadEffect(index);
    for (int index = 0; index < MUSIC_TRACK_COUNT; index++) audio->tracks[index] = GenerateTrack(index);
    SetSoundVolume(audio->tracks[MUSIC_COMMON], audio->music_volume);
    PlaySound(audio->tracks[MUSIC_COMMON]);
}

void AudioUnload(AudioState *audio)
{
    if (!audio->available) return;
    for (int index = 0; index < AUDIO_EFFECT_COUNT; index++) UnloadSound(audio->effects[index]);
    for (int index = 0; index < MUSIC_TRACK_COUNT; index++) UnloadSound(audio->tracks[index]);
    CloseAudioDevice();
    audio->available = false;
}

void AudioSetVolumes(AudioState *audio, float music_volume, float effects_volume)
{
    audio->music_volume = ClampVolume(music_volume);
    audio->effects_volume = ClampVolume(effects_volume);
}

void AudioPlay(AudioState *audio, AudioEffect effect)
{
    if (audio->available && effect >= 0 && effect < AUDIO_EFFECT_COUNT) {
        SetSoundVolume(audio->effects[effect], audio->effects_volume);
        PlaySound(audio->effects[effect]);
    }
}

void AudioRequestTrack(AudioState *audio, MusicTrack track)
{
    if (track >= MUSIC_COMMON && track < MUSIC_TRACK_COUNT) audio->requested_track = track;
}

void AudioUpdate(AudioState *audio, float delta_time)
{
    if (!audio->available) return;
    if (audio->requested_track != audio->current_track) {
        audio->fade += delta_time;
        const float progress = ClampVolume(audio->fade / AUDIO_FADE_SECONDS);
        SetSoundVolume(audio->tracks[audio->current_track], audio->music_volume * (1.0f - progress));
        SetSoundVolume(audio->tracks[audio->requested_track], audio->music_volume * progress);
        if (!IsSoundPlaying(audio->tracks[audio->requested_track])) PlaySound(audio->tracks[audio->requested_track]);
        if (audio->fade >= AUDIO_FADE_SECONDS) {
            StopSound(audio->tracks[audio->current_track]);
            audio->current_track = audio->requested_track;
            audio->fade = 0.0f;
            SetSoundVolume(audio->tracks[audio->current_track], audio->music_volume);
            PlaySound(audio->tracks[audio->current_track]);
        }
    } else {
        SetSoundVolume(audio->tracks[audio->current_track], audio->music_volume);
        if (!IsSoundPlaying(audio->tracks[audio->current_track])) PlaySound(audio->tracks[audio->current_track]);
    }
}
