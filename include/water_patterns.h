#ifndef WATER_PATTERNS_H
#define WATER_PATTERNS_H
#include "pilot_patterns.h"
bool AguaEh(IdPadraoProjetil id);
void AguaQuadro(const EstadoDefesa *d,float t,QuadroPiloto *q);
void AguaRastroAtualizar(EstadoDefesa *d);
void AguaMover(EstadoDefesa *d,const QuadroPiloto *q,Vector2 mov,float velocidade,float dt,bool dash);
void AguaCongelarNoAcerto(EstadoDefesa *d);
void AguaDesenharPeca(const AmeacaPiloto *p,const SistemaSprites *sprites,float t,Rectangle b);
void AguaDesenharEstado(const EstadoDefesa *d,const QuadroPiloto *q,const SistemaSprites *sprites);
float PilotoDistanciaAmeaca(const AmeacaPiloto *p,Vector2 pos);
#endif
