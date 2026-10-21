#ifndef PILOT_PATTERNS_H
#define PILOT_PATTERNS_H
#include "bullets.h"
#include "sprites.h"

typedef enum { PILOTO_CIRCULO, PILOTO_CAPSULA, PILOTO_RETANGULO, PILOTO_TRIANGULO, PILOTO_FORA_CIRCULO } FormaPiloto;
typedef struct {
    FormaPiloto forma;
    Vector2 a, b, c;
    Rectangle area;
    float raio, rotacao;
    bool ativo;
    int regra;
    unsigned long evento;
    IdSprite sprite;
} AmeacaPiloto;
#define MAXIMO_AMEACAS_PILOTO 96
typedef struct {
    AmeacaPiloto ameacas[MAXIMO_AMEACAS_PILOTO];
    int total;
    Vector2 corrente;
    bool dash_ativo,escorregadio,reflete_alma;
    Rectangle caixa_reflexo;
    int regra;
    float tolerancia_regra;
} QuadroPiloto;
int PilotoTotalInimigos(void);
IdInimigo PilotoInimigoId(int indice);
IdPadraoProjetil PilotoBase(int indice);
bool PilotoBoss(IdPadraoProjetil id);
bool PilotoEh(IdPadraoProjetil id);
const char *PilotoNome(IdPadraoProjetil id);
const char *PilotoInstrucao(IdPadraoProjetil id);
void PilotoQuadro(const EstadoDefesa *d, float tempo, QuadroPiloto *q);
bool PilotoPercursoColide(const QuadroPiloto *q, Vector2 inicio, Vector2 fim,
                          float raio, Vector2 entrada, unsigned long *evento);
void PilotoAtualizar(EstadoDefesa *d, AtributosJogador *j, EntradaDefesa entrada, float dt);
void PilotoDesenhar(const EstadoDefesa *d, const SistemaSprites *sprites);
#endif
