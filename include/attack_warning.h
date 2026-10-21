#ifndef ATTACK_WARNING_H
#define ATTACK_WARNING_H
#include "raylib.h"
typedef struct { float aviso, ativo, saida; } CronogramaAviso;
float AvisoProgresso(CronogramaAviso aviso, float tempo);
bool AvisoCausaDano(CronogramaAviso aviso, float tempo);
void AvisoDesenharLinha(Vector2 a, Vector2 b, float raio, float progresso, Color cor);
#endif
