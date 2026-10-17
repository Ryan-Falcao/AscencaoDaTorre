#ifndef AUDIO_H
#define AUDIO_H

#include <stdbool.h>

#include "raylib.h"

typedef enum {
    AUDIO_CURSOR,
    AUDIO_CONFIRMAR,
    AUDIO_CANCELAR,
    AUDIO_TEXTO,
    AUDIO_DANO_RECEBIDO,
    AUDIO_DANO_CAUSADO,
    AUDIO_SUPER_EFETIVO,
    AUDIO_NAO_EFETIVO,
    AUDIO_QTE_BATIDA,
    AUDIO_QTE_ERRO,
    AUDIO_QTE_PERFEITO,
    AUDIO_CURAR,
    AUDIO_COMPRAR,
    AUDIO_NIVEL_CIMA,
    AUDIO_DERROTA,
    AUDIO_VITORIA,
    AUDIO_TOTAL_EFEITOS
} EfeitoAudio;

typedef enum {
    MUSICA_COMUM,
    MUSICA_CHEFE,
    MUSICA_CALMA,
    MUSICA_FAIXA_TOTAL
} FaixaMusical;

typedef struct {
    bool disponivel;
    Sound efeitos[AUDIO_TOTAL_EFEITOS];
    Sound faixas[MUSICA_FAIXA_TOTAL];
    float volume_musica;
    float volume_efeitos;
    FaixaMusical faixa_atual;
    FaixaMusical faixa_solicitada;
    float desvanecer;
} EstadoAudio;

void AudioInicializar(EstadoAudio *audio, float volume_musica, float volume_efeitos);
void AudioDescarregar(EstadoAudio *audio);
void AudioDefinirVolumes(EstadoAudio *audio, float volume_musica, float volume_efeitos);
void AudioTocar(EstadoAudio *audio, EfeitoAudio efeito);
void AudioSolicitarFaixa(EstadoAudio *audio, FaixaMusical faixa);
void AudioAtualizar(EstadoAudio *audio, float tempo_delta);

#endif
