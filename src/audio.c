#include "audio.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#define AUDIO_AMOSTRA_TAXA 22050
#define AUDIO_DESVANECER_SEGUNDOS 0.50f

static float LimitarVolume(float valor)
{
    if (valor < 0.0f) return 0.0f;
    if (valor > 1.0f) return 1.0f;
    return valor;
}

static const char *const ARQUIVOS_EFEITOS[AUDIO_TOTAL_EFEITOS] = {
    "cursor", "confirm", "cancel", "text", "damage_taken", "damage_dealt",
    "super_effective", "not_effective", "qte_tick", "qte_error", "qte_perfect",
    "heal", "buy", "level_up", "defeat", "victory"
};

static Sound GerarTom(float frequencia, float segundos, bool triangular, float volume)
{
    const unsigned int quadros = (unsigned int)(AUDIO_AMOSTRA_TAXA * segundos);
    short *amostras = MemAlloc((int)(quadros * sizeof(*amostras)));
    Wave onda = {0};

    if (amostras == NULL) return (Sound){0};
    for (unsigned int indice = 0; indice < quadros; indice++) {
        const float progresso = (float)indice / (float)quadros;
        const float fase = fmodf((float)indice * frequencia / AUDIO_AMOSTRA_TAXA, 1.0f);
        const float raw = triangular ? 1.0f - 4.0f * fabsf(fase - 0.5f) :
                         (fase < 0.5f ? 1.0f : -1.0f);
        const float envelope = (1.0f - progresso) * (progresso < 0.06f ? progresso / 0.06f : 1.0f);
        amostras[indice] = (short)(raw * envelope * volume * 32767.0f);
    }
    onda.frameCount = quadros;
    onda.sampleRate = AUDIO_AMOSTRA_TAXA;
    onda.sampleSize = 16;
    onda.channels = 1;
    onda.data = amostras;
    Sound som = LoadSoundFromWave(onda);
    UnloadWave(onda);
    return som;
}

static Sound CarregarEfeito(int indice)
{
    char caminho[96];
    snprintf(caminho, sizeof(caminho), "assets/audio/%s.wav", ARQUIVOS_EFEITOS[indice]);
    if (FileExists(caminho)) return LoadSound(caminho);
    return GerarTom(260.0f + (float)(indice * 31), 0.075f + (float)(indice % 3) * 0.02f,
                        indice % 2 == 0, 0.22f);
}

static Sound GerarFaixa(int faixa)
{
    const float frequencies[] = {220.0f, 246.94f, 293.66f, 329.63f, 392.0f};
    const unsigned int quadros = AUDIO_AMOSTRA_TAXA * 4u;
    short *amostras = MemAlloc((int)(quadros * sizeof(*amostras)));
    Wave onda = {0};

    if (amostras == NULL) return (Sound){0};
    for (unsigned int indice = 0; indice < quadros; indice++) {
        const int note = (int)((indice / (AUDIO_AMOSTRA_TAXA / 2)) % 5u);
        const float frequencia = frequencies[(note + faixa * 2) % 5] * (faixa == MUSICA_CHEFE ? 0.5f : 1.0f);
        const float fase = fmodf((float)indice * frequencia / AUDIO_AMOSTRA_TAXA, 1.0f);
        const float raw = 1.0f - 4.0f * fabsf(fase - 0.5f);
        amostras[indice] = (short)(raw * 2600.0f);
    }
    onda.frameCount = quadros;
    onda.sampleRate = AUDIO_AMOSTRA_TAXA;
    onda.sampleSize = 16;
    onda.channels = 1;
    onda.data = amostras;
    Sound som = LoadSoundFromWave(onda);
    UnloadWave(onda);
    return som;
}

void AudioInicializar(EstadoAudio *audio, float volume_musica, float volume_efeitos)
{
    memset(audio, 0, sizeof(*audio));
    if (!IsAudioDeviceReady()) InitAudioDevice();
    audio->disponivel = IsAudioDeviceReady();
    audio->faixa_atual = MUSICA_COMUM;
    audio->faixa_solicitada = MUSICA_COMUM;
    AudioDefinirVolumes(audio, volume_musica, volume_efeitos);
    if (!audio->disponivel) return;
    for (int indice = 0; indice < AUDIO_TOTAL_EFEITOS; indice++) audio->efeitos[indice] = CarregarEfeito(indice);
    for (int indice = 0; indice < MUSICA_FAIXA_TOTAL; indice++) audio->faixas[indice] = GerarFaixa(indice);
    SetSoundVolume(audio->faixas[MUSICA_COMUM], audio->volume_musica);
    PlaySound(audio->faixas[MUSICA_COMUM]);
}

void AudioDescarregar(EstadoAudio *audio)
{
    if (!audio->disponivel) return;
    for (int indice = 0; indice < AUDIO_TOTAL_EFEITOS; indice++) UnloadSound(audio->efeitos[indice]);
    for (int indice = 0; indice < MUSICA_FAIXA_TOTAL; indice++) UnloadSound(audio->faixas[indice]);
    CloseAudioDevice();
    audio->disponivel = false;
}

void AudioDefinirVolumes(EstadoAudio *audio, float volume_musica, float volume_efeitos)
{
    audio->volume_musica = LimitarVolume(volume_musica);
    audio->volume_efeitos = LimitarVolume(volume_efeitos);
}

void AudioTocar(EstadoAudio *audio, EfeitoAudio efeito)
{
    if (audio->disponivel && efeito >= 0 && efeito < AUDIO_TOTAL_EFEITOS) {
        SetSoundVolume(audio->efeitos[efeito], audio->volume_efeitos);
        PlaySound(audio->efeitos[efeito]);
    }
}

void AudioSolicitarFaixa(EstadoAudio *audio, FaixaMusical faixa)
{
    if (faixa >= MUSICA_COMUM && faixa < MUSICA_FAIXA_TOTAL) audio->faixa_solicitada = faixa;
}

void AudioAtualizar(EstadoAudio *audio, float tempo_delta)
{
    if (!audio->disponivel) return;
    if (audio->faixa_solicitada != audio->faixa_atual) {
        audio->desvanecer += tempo_delta;
        const float progresso = LimitarVolume(audio->desvanecer / AUDIO_DESVANECER_SEGUNDOS);
        SetSoundVolume(audio->faixas[audio->faixa_atual], audio->volume_musica * (1.0f - progresso));
        SetSoundVolume(audio->faixas[audio->faixa_solicitada], audio->volume_musica * progresso);
        if (!IsSoundPlaying(audio->faixas[audio->faixa_solicitada])) PlaySound(audio->faixas[audio->faixa_solicitada]);
        if (audio->desvanecer >= AUDIO_DESVANECER_SEGUNDOS) {
            StopSound(audio->faixas[audio->faixa_atual]);
            audio->faixa_atual = audio->faixa_solicitada;
            audio->desvanecer = 0.0f;
            SetSoundVolume(audio->faixas[audio->faixa_atual], audio->volume_musica);
            PlaySound(audio->faixas[audio->faixa_atual]);
        }
    } else {
        SetSoundVolume(audio->faixas[audio->faixa_atual], audio->volume_musica);
        if (!IsSoundPlaying(audio->faixas[audio->faixa_atual])) PlaySound(audio->faixas[audio->faixa_atual]);
    }
}
