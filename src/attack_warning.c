#include "attack_warning.h"
#include <math.h>
float AvisoProgresso(CronogramaAviso c, float t) { return c.aviso > 0 ? fmaxf(0,fminf(1,t/c.aviso)) : 1; }
bool AvisoCausaDano(CronogramaAviso c, float t) { return t >= c.aviso && t < c.aviso+c.ativo; }
void AvisoDesenharLinha(Vector2 a, Vector2 b, float r, float progresso, Color cor)
{
    Color tinta=Fade(cor,.28f+.55f*progresso);
    DrawLineEx(a,b,fmaxf(1,r*.28f),tinta);
    DrawCircleLines((int)b.x,(int)b.y,r+2+sinf(progresso*12)*1.5f,tinta);
}
