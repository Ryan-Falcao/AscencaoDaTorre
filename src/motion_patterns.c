#include "motion_patterns.h"
#include "raymath.h"
#include "sprites.h"
#include <math.h>
#include <string.h>

#define AQUECIMENTO .72f
#define VELOCIDADE_MOVIMENTO VELOCIDADE_MOVIMENTO_DEFESA
#define GRAVIDADE 620.0f
#define VELOCIDADE_SALTO 250.0f

typedef enum { FISICA_LIVRE, FISICA_SALTO, FISICA_PLATAFORMA, FISICA_EMPURRAO } ModoFisica;

typedef enum {
    BRECHA_ESQUERDA,
    BRECHA_DIREITA,
    BRECHA_BAIXO,
    BRECHA_CIMA
} LadoBrecha;

bool PadraoMovimentoEh(IdPadraoProjetil id) { return id >= PADRAO_MARIPOSA_TRAMA && id < PADRAO_TOTAL; }

ModoMovimento ModoDoPadraoMovimento(IdPadraoProjetil id)
{
    switch (id) {
        case PADRAO_MARIPOSA_TRAMA: return MOVIMENTO_MARIPOSA_VENEZIANAS;
        case PADRAO_MARIPOSA_CORRENTE: return MOVIMENTO_MARIPOSA_POEIRA;
        case PADRAO_SAPO_BARREIRAS: return MOVIMENTO_SAPO_RIPPLES;
        case PADRAO_SAPO_VITORIAS_REGIAS: return MOVIMENTO_SAPO_VITORIAS_REGIAS;
        case PADRAO_PASSARO_FOGO_CRUZADO: return MOVIMENTO_PASSARO_FLOCK;
        case PADRAO_PASSARO_TUNEL: return MOVIMENTO_PASSARO_FIRE_TRAIL;
        case PADRAO_LIMO_MARE: return MOVIMENTO_LIMO_MARE;
        case PADRAO_LIMO_BALSAS: return MOVIMENTO_LIMO_BALSAS;
        case PADRAO_FAGULHA_CANHOES: return MOVIMENTO_FAGULHA_FAROL;
        case PADRAO_FAGULHA_ORBITA: return MOVIMENTO_FAGULHA_ORBITA;
        case PADRAO_LEBRE_BARREIRAS: return MOVIMENTO_LEBRE_BARREIRAS;
        case PADRAO_LEBRE_PARKOUR: return MOVIMENTO_LEBRE_BURROWS;
        case PADRAO_GATO_IMPACTO: return MOVIMENTO_GATO_BOTE;
        case PADRAO_GATO_PERSEGUICAO: return MOVIMENTO_GATO_GARRAS;
        case PADRAO_CAVALEIRO_LANCAS: return MOVIMENTO_CAVALEIRO_LANCAS;
        case PADRAO_CAVALEIRO_DESAFIO: return MOVIMENTO_CAVALEIRO_JUSTA;
        case PADRAO_SINO_CANHOES: return MOVIMENTO_SINO_CARRILHAO;
        case PADRAO_SINO_IMPACTO: return MOVIMENTO_SINO_PESO;
        case PADRAO_SINO_DESAFIO: return MOVIMENTO_SINO_REQUIEM;
        case PADRAO_TORRE_PARKOUR: return MOVIMENTO_TORRE_ESCADARIA;
        case PADRAO_TORRE_CANHOES: return MOVIMENTO_TORRE_BASTIAO;
        case PADRAO_TORRE_DESAFIO: return MOVIMENTO_TORRE_COLAPSO;
        case PADRAO_SINO_GAS_SOMBRA: return MOVIMENTO_SINO_GAS_SOMBRA;
        case PADRAO_CHUVA_FOGO: return MOVIMENTO_PASSARO_CHUVA_FOGO;
        case PADRAO_SARCAS_PORTOES: return MOVIMENTO_SARCAS_PORTOES;
        case PADRAO_HERBALISTA_FRASCOS: return MOVIMENTO_HERBALISTA_FRASCOS;
        case PADRAO_LEGIONARIO_ESCUDOS: return MOVIMENTO_LEGIONARIO_ESCUDOS;
        case PADRAO_BRUXA_CALDEIRAO: return MOVIMENTO_BRUXA_CALDEIRAO;
        case PADRAO_REVENANTE_PRISAO: return MOVIMENTO_REVENANTE_PRISAO;
        case PADRAO_ESQUELETO_ESTOUPIDO: return MOVIMENTO_ESQUELETO_ESTOUPIDO;
        case PADRAO_ACENDEDOR_MARTELO: return MOVIMENTO_ACENDEDOR_MARTELO;
        case PADRAO_PENITENTE_INCENSARIO: return MOVIMENTO_PENITENTE_INCENSARIO;
        case PADRAO_LADRAO_REFLEXO: return MOVIMENTO_LADRAO_REFLEXO;
        case PADRAO_ACOLITO_VITRAL: return MOVIMENTO_ACOLITO_VITRAL;
        case PADRAO_GOBLIN_ESTOCADAS: return MOVIMENTO_GOBLIN_ESTOCADAS;
        case PADRAO_AUTOMATO_ENGRENAGENS: return MOVIMENTO_AUTOMATO_ENGRENAGENS;
        case PADRAO_MERCENARIO_RICOCHETE: return MOVIMENTO_MERCENARIO_RICOCHETE;
        case PADRAO_CARCEREIRO_GRADE: return MOVIMENTO_CARCEREIRO_GRADE;
        case PADRAO_CULTISTA_LABIRINTO: return MOVIMENTO_CULTISTA_LABIRINTO;
        case PADRAO_AFOGADO_MARE_CORAL: return MOVIMENTO_AFOGADO_MARE_CORAL;
        case PADRAO_MAGO_ESPIRAL_CINZAS: return MOVIMENTO_MAGO_ESPIRAL_CINZAS;
        case PADRAO_GOBLIN_EXERCITO: return MOVIMENTO_GOBLIN_EXERCITO;
        case PADRAO_INQUISIDOR_VEREDITO: return MOVIMENTO_INQUISIDOR_VEREDITO;
        default: return MOVIMENTO_INVALIDO;
    }
}

static ModoFisica Fisica(IdPadraoProjetil id)
{
    switch (id) {
        case PADRAO_SAPO_BARREIRAS: case PADRAO_LIMO_MARE:
        case PADRAO_LEBRE_BARREIRAS: case PADRAO_CAVALEIRO_DESAFIO: return FISICA_SALTO;
        case PADRAO_SAPO_VITORIAS_REGIAS: case PADRAO_LIMO_BALSAS:
        case PADRAO_LEBRE_PARKOUR: case PADRAO_TORRE_PARKOUR: return FISICA_PLATAFORMA;
        case PADRAO_GATO_IMPACTO: case PADRAO_SINO_IMPACTO: return FISICA_EMPURRAO;
        default: return FISICA_LIVRE;
    }
}

bool PadraoMovimentoUsaSalto(IdPadraoProjetil id)
{
    ModoFisica fisica = Fisica(id);
    return fisica == FISICA_SALTO || fisica == FISICA_PLATAFORMA;
}

bool PadraoMovimentoUsaPlataformas(IdPadraoProjetil id) { return Fisica(id) == FISICA_PLATAFORMA; }

const char *InstrucaoPadraoMovimento(IdPadraoProjetil id)
{
    switch (id) {
        case PADRAO_MARIPOSA_TRAMA: return "MARIPOSA - ASAS: serpenteie pelas venezianas";
        case PADRAO_MARIPOSA_CORRENTE: return "MARIPOSA - POEIRA: azul PARE; laranja MOVA";
        case PADRAO_SAPO_BARREIRAS: return "SAPO - MAROLAS: pule cada onda; A/D controla";
        case PADRAO_SAPO_VITORIAS_REGIAS: return "SAPO - VITORIAS-REGIAS: salte antes que afundem";
        case PADRAO_PASSARO_FOGO_CRUZADO: return "PASSARO - REVOADA: troque de faixa nas lacunas";
        case PADRAO_PASSARO_TUNEL: return "PASSARO - MERGULHO: saia da mira; evite a brasa";
        case PADRAO_CHUVA_FOGO: return "PASSARO - CHUVA DE FOGO: desvie das chamas que caem";
        case PADRAO_LIMO_MARE: return "LIMO - MARE PEGAJOSA: saltos curtos e longos";
        case PADRAO_LIMO_BALSAS: return "LIMO - DIVISAO: as ilhas se separam sob seus pes";
        case PADRAO_FAGULHA_CANHOES: return "FAGULHA - FAROL: rode entre os feixes cardeais";
        case PADRAO_FAGULHA_ORBITA: return "FAGULHA - ORBITA: atravesse o anel pela estrela ausente";
        case PADRAO_LEBRE_BARREIRAS: return "LEBRE - CORRIDA: encadeie saltos pelas cercas";
        case PADRAO_LEBRE_PARKOUR: return "LEBRE - TOCAS: alcance a proxima saida elevada";
        case PADRAO_GATO_IMPACTO: return "GATO - BOTE: reaja contra a parede marcada";
        case PADRAO_GATO_PERSEGUICAO: return "GATO - GARRAS: leia a ordem dos tres cortes";
        case PADRAO_CAVALEIRO_LANCAS: return "CAVALEIRO - LANCAS: ocupe a unica faixa vazia";
        case PADRAO_CAVALEIRO_DESAFIO: return "CAVALEIRO - JUSTA: salte a lamina, pouse sob a alta";
        case PADRAO_SINO_CANHOES: return "SINEIRO - CARRILHAO: atravesse os badalos alternados";
        case PADRAO_SINO_IMPACTO: return "SINEIRO - PESO: resista a gravidade do badalo";
        case PADRAO_SINO_DESAFIO: return "SINEIRO - REQUIEM: mude de casa no compasso";
        case PADRAO_TORRE_PARKOUR: return "TORRE - ESCADARIA: suba enquanto os degraus caem";
        case PADRAO_TORRE_CANHOES: return "TORRE - BASTIAO: leia o lado e atravesse a seteira";
        case PADRAO_TORRE_DESAFIO: return "TORRE - COLAPSO: fuja da coluna marcada e dos escombros";
        case PADRAO_SINO_GAS_SOMBRA: return "SINEIRO - GAS DAS SOMBRAS: corra para a brecha; SHIFT da dash";
        case PADRAO_SARCAS_PORTOES: return "SARCAS: atravesse as tampas pela abertura movel";
        case PADRAO_HERBALISTA_FRASCOS: return "PESTE: nao fique sob os frascos marcados";
        case PADRAO_LEGIONARIO_ESCUDOS: return "FALANGE: acompanhe a unica fileira sem escudos";
        case PADRAO_BRUXA_CALDEIRAO: return "CALDEIRAO: troque de canto antes das bolhas crescerem";
        case PADRAO_REVENANTE_PRISAO: return "PRISAO DE GELO: saia pela parede que nao congelou";
        case PADRAO_ESQUELETO_ESTOUPIDO: return "PAVIO OSSUDO: passe pelo osso que nao acendeu";
        case PADRAO_ACENDEDOR_MARTELO: return "BIGORNA: escape do martelo e do rebote de fagulhas";
        case PADRAO_PENITENTE_INCENSARIO: return "INCENSARIO: cruze quando o pendulo mudar de lado";
        case PADRAO_LADRAO_REFLEXO: return "REFLEXO ROUBADO: fuja do golpe espelhado";
        case PADRAO_ACOLITO_VITRAL: return "VITRAL: siga a diagonal apagada";
        case PADRAO_GOBLIN_ESTOCADAS: return "ESTOCADAS: leia a sequencia curta, longa, curta";
        case PADRAO_AUTOMATO_ENGRENAGENS: return "ENGRENAGENS: gire com os dentes de pedra";
        case PADRAO_MERCENARIO_RICOCHETE: return "RICOCHETE: antecipe a bala que rebate nas paredes";
        case PADRAO_CARCEREIRO_GRADE: return "CELA: alcance a abertura antes das correntes fecharem";
        case PADRAO_CULTISTA_LABIRINTO: return "RAIZES: atravesse dois portoes em sequencia";
        case PADRAO_AFOGADO_MARE_CORAL: return "MARE DE CORAL: permaneca na faixa entre agua e teto";
        case PADRAO_MAGO_ESPIRAL_CINZAS: return "CINZAS: atravesse o braco ausente da espiral";
        case PADRAO_GOBLIN_EXERCITO: return "CRIPTA: mude de fileira entre as duas marchas";
        case PADRAO_INQUISIDOR_VEREDITO: return "VEREDITO SOLAR: ocupe o quadrante sem luz";
        default: return "WASD/setas movem; SHIFT da dash; ESPACO/W/CIMA pula";
    }
}

static void AdicionarAreaPerigosa(QuadroMovimento *f, Rectangle r)
{
    if (r.width > 0 && r.height > 0 && f->total_perigos < MAXIMO_RETANGULOS_MOVIMENTO)
        f->perigo[f->total_perigos++] = r;
}

static void AdicionarAviso(QuadroMovimento *f, Rectangle r)
{
    if (r.width > 0 && r.height > 0 && f->total_avisos < 8)
        f->avisos[f->total_avisos++] = r;
}

static void Portao(QuadroMovimento *f, Rectangle b, float eixo, float abertura, float metade, bool horizontal)
{
    if (horizontal) {
        AdicionarAreaPerigosa(f, (Rectangle){b.x, eixo, abertura - metade - b.x, 13});
        AdicionarAreaPerigosa(f, (Rectangle){abertura + metade, eixo, b.x + b.width - abertura - metade, 13});
    } else {
        AdicionarAreaPerigosa(f, (Rectangle){eixo, b.y, 13, abertura - metade - b.y});
        AdicionarAreaPerigosa(f, (Rectangle){eixo, abertura + metade, 13, b.y + b.height - abertura - metade});
    }
}

static void PreverPortao(QuadroMovimento *f, Rectangle b, float abertura, float metade, bool horizontal)
{
    if (horizontal) {
        AdicionarAviso(f, (Rectangle){b.x, b.y + b.height - 7, abertura - metade - b.x, 7});
        AdicionarAviso(f, (Rectangle){abertura + metade, b.y + b.height - 7, b.x + b.width - abertura - metade, 7});
    } else {
        AdicionarAviso(f, (Rectangle){b.x + b.width - 7, b.y, 7, abertura - metade - b.y});
        AdicionarAviso(f, (Rectangle){b.x + b.width - 7, abertura + metade, 7, b.y + b.height - abertura - metade});
    }
}

static float Periodo(IdPadraoProjetil id)
{
    switch (id) {
        case PADRAO_GATO_IMPACTO: case PADRAO_SINO_IMPACTO: return 1.55f;
        case PADRAO_PASSARO_FOGO_CRUZADO: return 1.15f;
        case PADRAO_PASSARO_TUNEL: case PADRAO_GATO_PERSEGUICAO: case PADRAO_FAGULHA_ORBITA: return 1.20f;
        case PADRAO_FAGULHA_CANHOES: case PADRAO_TORRE_CANHOES: return 1.35f;
        case PADRAO_SINO_CANHOES: case PADRAO_SINO_DESAFIO: return 1.05f;
        case PADRAO_CAVALEIRO_LANCAS: return 1.00f;
        case PADRAO_TORRE_DESAFIO: return 1.15f;
        case PADRAO_SINO_GAS_SOMBRA: return 2.00f;
        case PADRAO_SARCAS_PORTOES: return 2.40f;
        case PADRAO_CULTISTA_LABIRINTO: return 1.80f;
        case PADRAO_LEGIONARIO_ESCUDOS: case PADRAO_GOBLIN_EXERCITO: return 1.18f;
        case PADRAO_HERBALISTA_FRASCOS: case PADRAO_BRUXA_CALDEIRAO:
        case PADRAO_ESQUELETO_ESTOUPIDO: case PADRAO_MERCENARIO_RICOCHETE: return 1.10f;
        case PADRAO_REVENANTE_PRISAO: return 2.0f;
        case PADRAO_ACENDEDOR_MARTELO:
        case PADRAO_LADRAO_REFLEXO: case PADRAO_CARCEREIRO_GRADE:
            return 1.25f;
        case PADRAO_INQUISIDOR_VEREDITO: return 1.80f;
        case PADRAO_PENITENTE_INCENSARIO: return 1.50f;
        case PADRAO_ACOLITO_VITRAL: return 1.50f;
        case PADRAO_GOBLIN_ESTOCADAS: case PADRAO_AUTOMATO_ENGRENAGENS:
        case PADRAO_MAGO_ESPIRAL_CINZAS: return 1.60f;
        case PADRAO_AFOGADO_MARE_CORAL: return 1.15f;
        default: return .90f;
    }
}

void PadraoMovimentoIniciar(EstadoDefesa *d)
{
    d->batida_movimento = -1;
    d->ancora_movimento = d->posicao_alma;
    d->velocidade_vertical = 0; d->buffer_salto = 0; d->temporizador_coyote = 0; d->no_chao = false;
    ModoFisica fisica = Fisica(d->id_padrao);
    if (fisica == FISICA_SALTO || fisica == FISICA_PLATAFORMA) {
        d->posicao_alma.x = d->caixa.x + 72;
        d->posicao_alma.y = d->caixa.y + d->caixa.height - (fisica == FISICA_SALTO ? 8 : 38);
        if (fisica == FISICA_PLATAFORMA) {
            QuadroMovimento inicio; QuadroPadraoMovimento(d, &inicio);
            for (int i = 0; i < inicio.total_plataformas; i++) {
                Rectangle p = inicio.plataformas[i];
                if (d->posicao_alma.x >= p.x && d->posicao_alma.x <= p.x + p.width) {
                    d->posicao_alma.y = p.y - d->raio_alma; break;
                }
            }
        }
        d->no_chao = true;
    }
}

static void Chao(QuadroMovimento *f, Rectangle b)
{
    f->plataformas[f->total_plataformas++] = (Rectangle){b.x, b.y + b.height - 2, b.width, 4};
}

static void ObstaculosMoveis(const EstadoDefesa *d, QuadroMovimento *f, float t, float velocidade,
                            float espacamento, int estilo)
{
    Rectangle b = d->caixa;
    float andar = b.y + b.height - 2;
    for (int onda = 0; onda < 22; onda++) {
        float x = b.x + b.width + 20 - (t - onda * espacamento) * velocidade;
        if (x < b.x - 45 || x > b.x + b.width + 24) continue;
        float height = estilo == 0 ? (onda % 3 == 1 ? 34 : 23) :
                       estilo == 1 ? (onda % 4 == 2 ? 36 : 20) :
                                    (onda % 3 == 0 ? 38 : 29);
        float width = estilo == 0 ? 15 : estilo == 1 ? (onda % 2 ? 27 : 18) : 20;
        AdicionarAreaPerigosa(f, (Rectangle){x, andar - height, width, height});
        /* Ondas de limo variam em largura e altura; ao contrario das cercas,
           nao escondem uma segunda colisao no ponto de pouso. */
    }
    Chao(f, b); f->velocidade_rolagem = velocidade;
}

static void PercursoPlataformas(const EstadoDefesa *d, QuadroMovimento *f, float t, float velocidade, int estilo)
{
    Rectangle b = d->caixa;
    float base = b.y + b.height - 30;
    f->velocidade_rolagem = velocidade;
    for (int bloco = -2; bloco < 36; bloco++) {
        float passo = estilo == 0 ? 64 : estilo == 1 ? 78 : estilo == 2 ? 72 : 64;
        float width = estilo == 0 ? 54 : estilo == 1 ? 62 : estilo == 2 ? 56 : 42;
        float x = b.x + 48 + bloco * passo - t * velocidade;
        if (x < b.x - width || x > b.x + b.width) continue;
        float elevacao = estilo == 0 ? (bloco % 4 == 1 ? 25 : bloco % 4 == 2 ? 8 : 0) :
                     estilo == 1 ? (bloco % 3 == 1 ? 14 : 0) :
                     estilo == 2 ? (bloco % 3 == 0 ? 42 : bloco % 3 == 1 ? 18 : 4) :
                                  (float)((bloco % 5 + 5) % 5) * 10;
        if (estilo == 0) elevacao += sinf(t * 3 + bloco) * 5;
        if (estilo == 1 && bloco % 4 == 0) elevacao += 18;          /* balsa de limo separada */
        if (estilo == 2 && bloco % 5 == 3) elevacao += 24;
        if (estilo == 2) elevacao = fminf(elevacao, 44.0f);            /* toda toca e alcancavel */
        if (f->total_plataformas < 12)
            f->plataformas[f->total_plataformas++] = (Rectangle){x, base - elevacao, width, 7};
    }
    AdicionarAreaPerigosa(f, (Rectangle){b.x, b.y + b.height - 11, b.width, 11});
}

void QuadroPadraoMovimento(const EstadoDefesa *d, QuadroMovimento *f)
{
    memset(f, 0, sizeof(*f));
    Rectangle b = d->caixa;
    float relogio = d->temporizador_padrao, t = fmaxf(0, relogio - AQUECIMENTO);
    float aumento = d->ataque_segunda_fase ? 1.10f : 1.0f;
    int batida = (int)(t / Periodo(d->id_padrao));
    float tempo_local = fmodf(t, Periodo(d->id_padrao));
    f->modo = ModoDoPadraoMovimento(d->id_padrao);
    f->guia = (Vector2){b.x + b.width / 2, b.y + b.height / 2};
    switch (d->id_padrao) {
        case PADRAO_MARIPOSA_TRAMA: {
            static const float gaps[] = {.22f, .73f, .38f, .82f, .55f};
            float velocidade = 172 * aumento;
            for (int onda = 0; onda < 20; onda++) {
                float abertura = b.y + 18 + gaps[onda % 5] * (b.height - 36);
                float arrival = onda * .91f - t;
                if (arrival > 0 && arrival < .72f) PreverPortao(f, b, abertura, 19, false);
                float x = b.x + b.width + 10 - (t - onda * .91f) * velocidade;
                if (x >= b.x - 16 && x <= b.x + b.width + 12) Portao(f, b, x, abertura, 19, false);
                if (onda == (int)(t / .91f)) f->guia = (Vector2){x, abertura};
            }
            break;
        }
        case PADRAO_MARIPOSA_CORRENTE: {
            f->regra_movimento = batida % 2 ? 2 : 1;
            for (int mote = 0; mote < 30; mote++) {
                float age = t - mote * .38f;
                if (age < 0) continue;
                float x = b.x + b.width + 12 - age * 116 * aumento;
                float y = b.y + b.height / 2 + sinf(age * 4.6f + mote * 1.9f) * b.height * .31f;
                if (x > b.x - 12 && x < b.x + b.width + 12) AdicionarAreaPerigosa(f, (Rectangle){x - 4, y - 4, 8, 8});
            }
            f->guia.y = b.y + b.height * (.5f + sinf(t * 1.15f) * .30f);
            break;
        }
        case PADRAO_SAPO_BARREIRAS: ObstaculosMoveis(d, f, t, 170 * aumento, .94f, 0); break;
        case PADRAO_SAPO_VITORIAS_REGIAS: PercursoPlataformas(d, f, t, 55 * aumento, 0); break;
        case PADRAO_PASSARO_FOGO_CRUZADO: {
            static const int aberturas[] = {1, 2, 1, 0, 1, 2, 3, 2};
            int aberta = aberturas[batida % 8];
            float altura_faixa = b.height / 4;
            if (tempo_local < .50f || relogio < AQUECIMENTO) {
                for (int faixa = 0; faixa < 4; faixa++) if (faixa != aberta)
                    AdicionarAviso(f, (Rectangle){b.x, b.y + faixa * altura_faixa + 3, b.width, altura_faixa - 6});
            } else {
                float progresso = Clamp((tempo_local - .50f) / .55f, 0, 1);
                for (int faixa = 0; faixa < 4; faixa++) if (faixa != aberta) {
                    for (int feather = 0; feather < 4; feather++) {
                        float espacamento = b.width / 4;
                        float x = faixa % 2 ? b.x - 20 + progresso * (b.width + 40) + feather * espacamento - b.width * .5f :
                                                   b.x + b.width + 20 - progresso * (b.width + 40) - feather * espacamento + b.width * .5f;
                        x = b.x + fmodf(x - b.x + b.width * 4, b.width + 26) - 13;
                        AdicionarAreaPerigosa(f, (Rectangle){x, b.y + faixa * altura_faixa + 1, 22, altura_faixa - 2});
                    }
                }
            }
            f->guia.y = b.y + (aberta + .5f) * b.height / 4;
            break;
        }
        case PADRAO_PASSARO_TUNEL: {
            float y = Clamp(d->ancora_movimento.y, b.y + 12, b.y + b.height - 12);
            Rectangle flight = {b.x, y - 10, b.width, 20};
            if (tempo_local < .50f || relogio < AQUECIMENTO) AdicionarAviso(f, flight);
            else if (tempo_local < .82f) AdicionarAreaPerigosa(f, flight);
            if (tempo_local >= .78f) for (int ember = 0; ember < 3; ember++)
                AdicionarAreaPerigosa(f, (Rectangle){b.x + b.width * (.24f + ember * .26f) - 7, y - 18, 14, 36});
            f->guia.y = y < b.y + b.height / 2 ? b.y + b.height - 18 : b.y + 18;
            break;
        }
        case PADRAO_LIMO_MARE: ObstaculosMoveis(d, f, t, 158 * aumento, 1.16f, 1); break;
        case PADRAO_LIMO_BALSAS: PercursoPlataformas(d, f, t, 48 * aumento, 1); break;
        case PADRAO_FAGULHA_CANHOES: {
            bool vertical = batida % 2 == 0;
            static const float offsets[] = {-.18f, 0, .18f, 0};
            float deslocamento = offsets[batida % 4];
            float eixo = (vertical ? b.x + b.width / 2 : b.y + b.height / 2) + deslocamento * (vertical ? b.width : b.height);
            Rectangle beam = vertical ? (Rectangle){eixo - 16, b.y, 32, b.height} : (Rectangle){b.x, eixo - 16, b.width, 32};
            if (tempo_local < .43f || relogio < AQUECIMENTO) AdicionarAviso(f, beam); else if (tempo_local < 1.18f) AdicionarAreaPerigosa(f, beam);
            f->guia = vertical ? (Vector2){eixo < b.x + b.width / 2 ? b.x + b.width * .75f : b.x + b.width * .25f, b.y + b.height / 2} :
                                  (Vector2){b.x + b.width / 2, eixo < b.y + b.height / 2 ? b.y + b.height * .78f : b.y + b.height * .22f};
            break;
        }
        case PADRAO_FAGULHA_ORBITA: {
            float center_x = b.x + b.width / 2, center_y = b.y + b.height / 2;
            float angulo = t * 1.8f;
            int ausente = batida % 8;
            for (int orbe = 0; orbe < 8; orbe++) if (orbe != ausente) {
                float a = angulo + orbe * PI / 4;
                AdicionarAreaPerigosa(f, (Rectangle){center_x + cosf(a) * b.width * .28f - 6,
                                      center_y + sinf(a) * b.height * .32f - 6, 12, 12});
            }
            float angulo_aberto = angulo + ausente * PI / 4;
            f->guia = (Vector2){center_x + cosf(angulo_aberto) * b.width * .28f,
                                 center_y + sinf(angulo_aberto) * b.height * .32f};
            if (batida % 2 == 0) {
                Rectangle nucleo = {center_x - 45, center_y - 30, 90, 60};
                if (tempo_local < .52f) AdicionarAviso(f, nucleo); else AdicionarAreaPerigosa(f, nucleo);
            } else {
                Rectangle bordas[] = {
                    {b.x, b.y, 55, b.height}, {b.x + b.width - 55, b.y, 55, b.height},
                    {b.x + 55, b.y, b.width - 110, 36},
                    {b.x + 55, b.y + b.height - 36, b.width - 110, 36}
                };
                for (int i = 0; i < 4; i++) if (tempo_local < .52f) AdicionarAviso(f, bordas[i]); else AdicionarAreaPerigosa(f, bordas[i]);
                f->guia = (Vector2){center_x, center_y};
            }
            break;
        }
        case PADRAO_LEBRE_BARREIRAS: ObstaculosMoveis(d, f, t, 205 * aumento, .92f, 2); break;
        case PADRAO_LEBRE_PARKOUR: PercursoPlataformas(d, f, t, 68 * aumento, 2); break;
        case PADRAO_GATO_IMPACTO:
        case PADRAO_SINO_IMPACTO: {
            bool bell = d->id_padrao == PADRAO_SINO_IMPACTO;
            f->direcao = bell ? 2 + batida % 2 : batida % 4;
            float depth = bell ? 42 : 51;
            Rectangle edge = f->direcao == 0 ? (Rectangle){b.x, b.y, depth, b.height} :
                             f->direcao == 1 ? (Rectangle){b.x + b.width - depth, b.y, depth, b.height} :
                             f->direcao == 2 ? (Rectangle){b.x, b.y, b.width, depth} :
                                                 (Rectangle){b.x, b.y + b.height - depth, b.width, depth};
            AdicionarAviso(f, edge); if (tempo_local > .82f && tempo_local < 1.45f) AdicionarAreaPerigosa(f, edge);
            break;
        }
        case PADRAO_GATO_PERSEGUICAO: {
            int corte = batida % 3;
            float x = Clamp(d->ancora_movimento.x, b.x + 28, b.x + b.width - 28);
            float y = Clamp(d->ancora_movimento.y, b.y + 22, b.y + b.height - 22);
            Rectangle garra = corte == 0 ? (Rectangle){x - 18, b.y, 36, b.height} :
                             corte == 1 ? (Rectangle){b.x, y - 14, b.width, 28} : (Rectangle){x - 31, y - 24, 62, 48};
            if (tempo_local < .46f || relogio < AQUECIMENTO) AdicionarAviso(f, garra); else if (tempo_local < .86f) AdicionarAreaPerigosa(f, garra);
            f->guia.x = x < b.x + b.width / 2 ? b.x + b.width - 22 : b.x + 22;
            f->guia.y = y < b.y + b.height / 2 ? b.y + b.height - 18 : b.y + 18;
            break;
        }
        case PADRAO_CAVALEIRO_LANCAS: {
            static const int aberturas[] = {1, 2, 1, 0, 1, 2, 3, 2};
            int aberta = aberturas[batida % 8];
            float altura_faixa = b.height / 4;
            if (tempo_local < .44f) {
                for (int faixa = 0; faixa < 4; faixa++) if (faixa != aberta)
                    AdicionarAviso(f, (Rectangle){batida % 2 ? b.x : b.x + b.width - 8, b.y + faixa * altura_faixa + 3, 8, altura_faixa - 6});
            } else {
                float progresso = Clamp((tempo_local - .44f) / .42f, 0, 1);
                for (int faixa = 0; faixa < 4; faixa++) if (faixa != aberta) {
                    float width = b.width * progresso;
                    AdicionarAreaPerigosa(f, (Rectangle){batida % 2 ? b.x : b.x + b.width - width,
                                          b.y + faixa * altura_faixa + 3, width, altura_faixa - 6});
                }
            }
            f->guia.y = b.y + (aberta + .5f) * altura_faixa;
            break;
        }
        case PADRAO_CAVALEIRO_DESAFIO: {
            ObstaculosMoveis(d, f, t, 190 * aumento, .88f, 0);
            for (int onda = 0; onda < 18; onda++) if (onda % 3 == 2) {
                float x = b.x + b.width + 20 - (t - onda * .88f) * 190 * aumento;
                if (x > b.x - 30 && x < b.x + b.width + 20) {
                    /* Troca a lanca baixa desta onda por uma alta. Nunca exige
                       que o jogador pule e fique abaixado no mesmo x. */
                    for (int i = 0; i < f->total_perigos; i++) {
                        Rectangle r = f->perigo[i];
                        if (fabsf(r.x - x) < 2 && r.y > b.y + b.height / 2) {
                            for (int j = i; j + 1 < f->total_perigos; j++) f->perigo[j] = f->perigo[j + 1];
                            f->total_perigos--; break;
                        }
                    }
                    AdicionarAreaPerigosa(f, (Rectangle){x, b.y, 22, b.height - 38});
                }
            }
            break;
        }
        case PADRAO_SINO_CANHOES: {
            float direcao = batida % 2 ? -1.0f : 1.0f;
            float x1 = Clamp(d->ancora_movimento.x, b.x + 16, b.x + b.width - 16);
            float x2 = Clamp(x1 + direcao * 58, b.x + 16, b.x + b.width - 16);
            Rectangle r1 = {x1 - 7, b.y, 14, b.height}, r2 = {x2 - 7, b.y, 14, b.height};
            if (tempo_local < .45f || relogio < AQUECIMENTO) { AdicionarAviso(f, r1); AdicionarAviso(f, r2); } else { AdicionarAreaPerigosa(f, r1); AdicionarAreaPerigosa(f, r2); }
            f->guia.x = Clamp(x1 - direcao * 55, b.x + 12, b.x + b.width - 12);
            break;
        }
        case PADRAO_SINO_DESAFIO: {
            int paridade = batida % 2;
            float largura_celula = b.width / 6, altura_celula = b.height / 3;
            for (int y = 0; y < 3; y++) for (int x = 0; x < 6; x++) if ((x + y) % 2 == paridade) {
                Rectangle bloco = {b.x + x * largura_celula + 2, b.y + y * altura_celula + 2, largura_celula - 4, altura_celula - 4};
                if (tempo_local < .45f || relogio < AQUECIMENTO) AdicionarAviso(f, bloco); else AdicionarAreaPerigosa(f, bloco);
            }
            int seguro_x = (int)Clamp(floorf((d->ancora_movimento.x - b.x) / largura_celula), 0, 5);
            int seguro_y = (int)Clamp(floorf((d->ancora_movimento.y - b.y) / altura_celula), 0, 2);
            if ((seguro_x + seguro_y) % 2 == paridade) {
                int direcao = batida % 2 ? -1 : 1;
                if (seguro_x + direcao < 0 || seguro_x + direcao > 5) direcao = -direcao;
                seguro_x += direcao;
            }
            f->guia = (Vector2){b.x + (seguro_x + .5f) * largura_celula, b.y + (seguro_y + .5f) * altura_celula};
            break;
        }
        case PADRAO_TORRE_PARKOUR: PercursoPlataformas(d, f, t, 72 * aumento, 3); break;
        case PADRAO_TORRE_CANHOES: {
            bool horizontal = batida % 2 != 0;
            float direcao = batida % 4 < 2 ? 1.0f : -1.0f;
            float abertura = horizontal ? Clamp(d->ancora_movimento.x + direcao * 45, b.x + 24, b.x + b.width - 24) :
                                     Clamp(d->ancora_movimento.y + direcao * 45, b.y + 24, b.y + b.height - 24);
            float metade = d->ataque_segunda_fase ? 17 : 20;
            QuadroMovimento disparo = {0};
            float progresso = Clamp((tempo_local - .46f) / .76f, 0, 1);
            float eixo = horizontal ? b.y - 13 + progresso * (b.height + 26) :
                                      b.x - 13 + progresso * (b.width + 26);
            Portao(&disparo, b, eixo, abertura, metade, horizontal);
            if (tempo_local < .46f || relogio < AQUECIMENTO) for (int i = 0; i < disparo.total_perigos; i++) AdicionarAviso(f, disparo.perigo[i]);
            else for (int i = 0; i < disparo.total_perigos; i++) AdicionarAreaPerigosa(f, disparo.perigo[i]);
            f->guia = horizontal ? (Vector2){abertura, b.y + b.height / 2} : (Vector2){b.x + b.width / 2, abertura};
            f->direcao = horizontal;
            break;
        }
        case PADRAO_TORRE_DESAFIO: {
            float x = Clamp(d->ancora_movimento.x, b.x + 28, b.x + b.width - 28);
            Rectangle coluna = {x - 25, b.y, 50, b.height};
            Rectangle escombro = {Clamp(x + (batida % 2 ? -72 : 72), b.x + 12, b.x + b.width - 42), b.y + b.height - 26, 30, 26};
            if (tempo_local < .48f || relogio < AQUECIMENTO) AdicionarAviso(f, coluna); else if (tempo_local < .88f) AdicionarAreaPerigosa(f, coluna);
            if (tempo_local > .62f) AdicionarAreaPerigosa(f, escombro); else AdicionarAviso(f, escombro);
            f->guia.x = x < b.x + b.width / 2 ? b.x + b.width - 20 : b.x + 20;
            f->guia.y = b.y + b.height * .35f;
            break;
        }
        case PADRAO_SINO_GAS_SOMBRA: {
            static const LadoBrecha paredes_brecha[] = {
                BRECHA_BAIXO,
                BRECHA_ESQUERDA,
                BRECHA_CIMA,
                BRECHA_DIREITA
            };

            int total_posicoes = sizeof(paredes_brecha) / sizeof(paredes_brecha[0]);

            float largura_brecha = d->ataque_segunda_fase ? 28.0f : 38.0f;

            LadoBrecha lado_parede = paredes_brecha[batida % total_posicoes];

            float duracao_expansao = d->ataque_segunda_fase ? 0.80f : 1.00f;

            float duracao_sem_dano = duracao_expansao + 0.50f;

            float duracao_dano = 0.50f;

            bool gas_nocivo =
                tempo_local >= duracao_sem_dano &&
                tempo_local < duracao_sem_dano + duracao_dano;

            float progresso_gas = Clamp(tempo_local / duracao_expansao, 0.0f, 1.0f);

            Rectangle gas = {0};

            switch (lado_parede){
                case BRECHA_BAIXO: {
                float altura_final = b.height - largura_brecha;
                float altura_atual = altura_final * progresso_gas;
                gas = (Rectangle){
                    b.x,
                    b.y,
                    b.width,
                    altura_atual
                };
                f->guia = (Vector2){
                    b.x + b.width / 2.0f,
                    b.y + b.height - largura_brecha / 2.0f
                };
                break;
                }
                case BRECHA_CIMA: {
                float altura_final = b.height - largura_brecha;
                float altura_atual = altura_final * progresso_gas;
                gas = (Rectangle){
                    b.x,
                    b.y + b.height - altura_atual,
                    b.width,
                    altura_atual
                };
                f->guia = (Vector2){
                    b.x + b.width / 2.0f,
                    b.y + largura_brecha / 2.0f
                };
                break;
                }
                case BRECHA_DIREITA: {
                float largura_final = b.width - largura_brecha;
                float largura_atual = largura_final * progresso_gas;
                gas = (Rectangle){
                    b.x,
                    b.y,
                    largura_atual,
                    b.height
                };
                f->guia = (Vector2){
                    b.x + b.width - largura_brecha / 2.0f,
                    b.y + b.height / 2.0f
                };
                break;
                }
                case BRECHA_ESQUERDA: {
                float largura_final = b.width - largura_brecha;
                float largura_atual = largura_final * progresso_gas;
                gas = (Rectangle){
                    b.x + b.width - largura_atual,
                    b.y,
                    largura_atual,
                    b.height
                };
                f->guia = (Vector2){
                    b.x + largura_brecha / 2.0f,
                    b.y + b.height / 2.0f
                };
                break;
                }
                default:
                    break;
            }
            if (gas_nocivo) {
                AdicionarAreaPerigosa(f, gas);
            } else {
                AdicionarAviso(f, gas);
            }
            break;
        }
        case PADRAO_CHUVA_FOGO: {
            static const float posicoes_x[] = {
                0.50f, 
                0.59f, 
                0.10f, 
                0.90f,
                0.25f, 
                0.75f, 
                0.15f, 
                0.85f
            };
            const int total_posicoes = sizeof(posicoes_x) / sizeof(posicoes_x[0]);
            const int total_chamas = 32;
            const float intervalo = 0.32f;
            const float velocidade_queda = 150.0f * aumento;
            const float tempo_travessia = (b.height + 40.0f) / velocidade_queda;
            int primeira_chama = (int)floorf((t - tempo_travessia) / intervalo);
            int ultima_chama = (int)floorf(t / intervalo);

            if (primeira_chama < 0) primeira_chama = 0;
            if (ultima_chama >= total_chamas) ultima_chama = total_chamas - 1;
            if (ultima_chama >= 0) {
                const float proxima_posicao = posicoes_x[ultima_chama % total_posicoes];
                f->guia.x = b.x + b.width * (proxima_posicao < 0.5f ? 0.75f : 0.25f);
            }

            for (int indice = primeira_chama; indice <= ultima_chama; indice++) {
                const float momento_nascimento = indice * intervalo;
                const float idade_chama = t - momento_nascimento;
                const float proporcao_x = posicoes_x[indice % total_posicoes];
                const float x = b.x + 12.0f + proporcao_x * (b.width - 24.0f);
                const float y = b.y - 20.0f + idade_chama * velocidade_queda;
                const Rectangle chama = {x - 7.0f, y - 10.0f, 14.0f, 20.0f};

                if (idade_chama >= 0.0f && y <= b.y + b.height + 20.0f) {
                    const float inicio_trajetoria = fmaxf(y + 10.0f, b.y);
                    AdicionarAviso(f, (Rectangle){
                        x - 9.0f,
                        inicio_trajetoria,
                        18.0f,
                        b.y + b.height - inicio_trajetoria
                    });
                    AdicionarAreaPerigosa(f, chama);
                }
            }
            break;
        }
        case PADRAO_SARCAS_PORTOES: {
            static const float brechas[] = {.24f,.72f,.42f,.82f,.58f};
            float abertura = b.y + 16 + brechas[batida % 5] * (b.height - 32);
            float x = b.x + b.width + 10 - fmaxf(0, tempo_local - .42f) * 160;
            QuadroMovimento portao = {0}; Portao(&portao, b, x, abertura, 27, false);
            for (int i=0;i<portao.total_perigos;i++)
                if (tempo_local < .42f) AdicionarAviso(f, portao.perigo[i]); else AdicionarAreaPerigosa(f, portao.perigo[i]);
            f->guia=(Vector2){x,abertura}; break;
        }
        case PADRAO_HERBALISTA_FRASCOS: {
            for(int i=0;i<1;i++){
                float x=Clamp(d->ancora_movimento.x,b.x+12,b.x+b.width-12);
                Rectangle queda={x-5,b.y,10,b.height};
                if(tempo_local<.45f) AdicionarAviso(f,queda);
                else { float y=b.y+Clamp((tempo_local-.45f)/.65f,0,1)*b.height; AdicionarAreaPerigosa(f,(Rectangle){x-5,y-8,10,16}); }
            }
            f->guia.x=d->ancora_movimento.x<b.x+b.width/2?b.x+b.width*.78f:b.x+b.width*.22f; break;
        }
        case PADRAO_LEGIONARIO_ESCUDOS: {
            static const int livres[]={1,2,3,2,1,0,1,2};
            int livre=livres[batida%8]; float faixa=b.height/4;
            for(int i=0;i<4;i++) if(i!=livre){ Rectangle r={b.x,b.y+i*faixa+2,b.width,faixa-4};
                if(tempo_local<.43f) AdicionarAviso(f,r); else AdicionarAreaPerigosa(f,r); }
            f->guia.y=b.y+(livre+.5f)*faixa; break;
        }
        case PADRAO_BRUXA_CALDEIRAO: {
            static const int ordem_segura[]={0,1,3,2};
            int seguro=ordem_segura[batida%4]; float w=b.width*.31f,h=b.height*.42f;
            Rectangle cantos[4]={{b.x,b.y,w,h},{b.x+b.width-w,b.y,w,h},
                {b.x,b.y+b.height-h,w,h},{b.x+b.width-w,b.y+b.height-h,w,h}};
            for(int i=0;i<4;i++) if(i!=seguro) {
                if(tempo_local<.48f) AdicionarAviso(f,cantos[i]); else AdicionarAreaPerigosa(f,cantos[i]);
            }
            Rectangle caldeirao={b.x+b.width/2-38,b.y+b.height/2-26,76,52};
            if(tempo_local<.48f) AdicionarAviso(f,caldeirao); else AdicionarAreaPerigosa(f,caldeirao);
            Rectangle s=cantos[seguro]; f->guia=(Vector2){s.x+s.width/2,s.y+s.height/2}; break;
        }
        case PADRAO_REVENANTE_PRISAO: {
            static const int saidas[]={0,2,1,3};
            int saida=saidas[batida%4]; float grossura=18+Clamp((tempo_local-.72f)/.70f,0,1)*28;
            Rectangle paredes[4]={{b.x,b.y,grossura,b.height},{b.x+b.width-grossura,b.y,grossura,b.height},
                {b.x,b.y,b.width,grossura},{b.x,b.y+b.height-grossura,b.width,grossura}};
            for(int i=0;i<4;i++) if(i!=saida) {
                if(tempo_local<.72f) AdicionarAviso(f,paredes[i]); else AdicionarAreaPerigosa(f,paredes[i]);
            }
            Rectangle nucleo={b.x+b.width/2-38,b.y+b.height/2-26,76,52};
            if(tempo_local<.72f) AdicionarAviso(f,nucleo); else AdicionarAreaPerigosa(f,nucleo);
            f->guia=saida==0?(Vector2){b.x+12,b.y+b.height/2}:saida==1?(Vector2){b.x+b.width-12,b.y+b.height/2}:
                     saida==2?(Vector2){b.x+b.width/2,b.y+12}:(Vector2){b.x+b.width/2,b.y+b.height-12}; break;
        }
        case PADRAO_ESQUELETO_ESTOUPIDO: {
            static const int falhas[]={1,2,3,4,5,6,7,6,5,4,3,2};
            int falha=falhas[batida%12]; float y=b.y+b.height*.5f, passo=b.width/9;
            for(int i=0;i<9;i++) if(i!=falha){ Rectangle osso={b.x+i*passo+2,y-8,passo-4,16};
                if(tempo_local<.44f) AdicionarAviso(f,osso); else AdicionarAreaPerigosa(f,osso); }
            f->guia=(Vector2){b.x+(falha+.5f)*passo,y}; break;
        }
        case PADRAO_ACENDEDOR_MARTELO: {
            float x=Clamp(d->ancora_movimento.x,b.x+42,b.x+b.width-42);
            Rectangle martelo={x-38,b.y,76,b.height};
            if(tempo_local<.48f) AdicionarAviso(f,martelo); else if(tempo_local<.88f) AdicionarAreaPerigosa(f,martelo);
            if(tempo_local>=.70f){
                Rectangle fagulha1={b.x,b.y+b.height*.22f,b.width,10},fagulha2={b.x,b.y+b.height*.72f,b.width,10};
                AdicionarAreaPerigosa(f,fagulha1); AdicionarAreaPerigosa(f,fagulha2);
            }
            f->guia.x=x<b.x+b.width/2?b.x+b.width-24:b.x+24; break;
        }
        case PADRAO_PENITENTE_INCENSARIO: {
            float x=Clamp(d->ancora_movimento.x,b.x+18,b.x+b.width-18);
            Rectangle corrente={x-4,b.y,8,b.height*.62f};
            Rectangle peso={x-15,b.y+b.height*.55f,30,26};
            if(tempo_local<.85f){AdicionarAviso(f,corrente);AdicionarAviso(f,peso);}
            else {AdicionarAreaPerigosa(f,corrente);AdicionarAreaPerigosa(f,peso);}
            f->guia.x=x<b.x+b.width/2?b.x+b.width*.78f:b.x+b.width*.22f; break;
        }
        case PADRAO_LADRAO_REFLEXO: {
            float x=b.x+b.width-(d->ancora_movimento.x-b.x), y=b.y+b.height-(d->ancora_movimento.y-b.y);
            Rectangle sombra={Clamp(x-28,b.x,b.x+b.width-56),Clamp(y-20,b.y,b.y+b.height-40),56,40};
            Rectangle corte={b.x,y-8,b.width,16};
            if(tempo_local<.46f){AdicionarAviso(f,sombra);AdicionarAviso(f,corte);} else {AdicionarAreaPerigosa(f,sombra);AdicionarAreaPerigosa(f,corte);}
            f->guia.x=batida%2?b.x+18:b.x+b.width-18;
            f->guia.y=(batida/2)%2?b.y+16:b.y+b.height-16; break;
        }
        case PADRAO_ACOLITO_VITRAL: {
            static const int apagados[]={1,2,3,4,3,2,1,0};
            int apagado=apagados[batida%8]; float largura=b.width/5;
            for(int x=0;x<5;x++) if(x!=apagado){
                Rectangle vidro={b.x+x*largura+3,b.y,largura-6,b.height};
                if(tempo_local<.70f) AdicionarAviso(f,vidro); else AdicionarAreaPerigosa(f,vidro);
            }
            f->guia=(Vector2){b.x+(apagado+.5f)*largura,b.y+b.height/2}; break;
        }
        case PADRAO_GOBLIN_ESTOCADAS: {
            static const int livres[]={2,3,4,3,2,1,0,1};
            int livre=livres[batida%8]; float faixa=b.height/5;
            for(int i=0;i<5;i++) if(i!=livre){
                float alcance=b.width*(i%2?.72f:.92f); Rectangle lanca={b.x+b.width-alcance,b.y+i*faixa+5,alcance,faixa-10};
                if(tempo_local<.40f) AdicionarAviso(f,lanca); else AdicionarAreaPerigosa(f,lanca);
            }
            f->guia.y=b.y+(livre+.5f)*faixa; break;
        }
        case PADRAO_AUTOMATO_ENGRENAGENS: {
            float cx=b.x+b.width/2,cy=b.y+b.height/2;
            AdicionarAreaPerigosa(f,(Rectangle){cx-30,cy-20,60,40});
            for(int i=0;i<6;i++){float a=t*1.3f+i*PI/3; float raio=i%2?b.width*.30f:b.width*.18f;
                AdicionarAreaPerigosa(f,(Rectangle){cx+cosf(a)*raio-6,cy+sinf(a)*b.height*.34f-6,12,12});}
            f->guia=(Vector2){cx+cosf(t*1.3f+PI)*b.width*.24f,cy+sinf(t*1.3f+PI)*b.height*.27f}; break;
        }
        case PADRAO_MERCENARIO_RICOCHETE: {
            float fase=fmodf(t*.72f,2.0f); float triangular=1.0f-fabsf(fase-1.0f);
            float fase_y=fmodf(t*1.07f+.4f,2.0f),tri_y=1.0f-fabsf(fase_y-1.0f);
            float x=b.x+12+triangular*(b.width-24),y=b.y+12+tri_y*(b.height-24);
            for(int i=0;i<4;i++) AdicionarAreaPerigosa(f,(Rectangle){x-i*18*(fase<1?1:-1)-8,y-i*8*(fase_y<1?1:-1)-8,16,16});
            f->guia=(Vector2){b.x+b.width-x+b.x,b.y+b.height-y+b.y}; break;
        }
        case PADRAO_CARCEREIRO_GRADE: {
            float x=Clamp(d->ancora_movimento.x,b.x+18,b.x+b.width-18);
            float y=Clamp(d->ancora_movimento.y,b.y+18,b.y+b.height-18);
            Rectangle vertical={x-7,b.y,14,b.height},horizontal={b.x,y-7,b.width,14};
            if(tempo_local<.55f){AdicionarAviso(f,vertical);AdicionarAviso(f,horizontal);}
            else {AdicionarAreaPerigosa(f,vertical);AdicionarAreaPerigosa(f,horizontal);}
            f->guia=(Vector2){x<b.x+b.width/2?b.x+b.width-20:b.x+20,
                              y<b.y+b.height/2?b.y+b.height-18:b.y+18}; break;
        }
        case PADRAO_CULTISTA_LABIRINTO: {
            float abertura1=b.y+b.height*(batida%2?.28f:.72f), abertura2=b.x+b.width*(batida%3?.68f:.32f);
            bool primeiro=tempo_local<.90f;
            if(primeiro) Portao(f,b,b.x+b.width*.58f,abertura1,24,false);
            else Portao(f,b,b.y+b.height*.52f,abertura2,24,true);
            bool aviso=tempo_local<.45f||(tempo_local>=.90f&&tempo_local<1.35f);
            if(aviso){for(int i=0;i<f->total_perigos;i++)AdicionarAviso(f,f->perigo[i]);f->total_perigos=0;}
            f->guia=primeiro?(Vector2){b.x+b.width/2,abertura1}:(Vector2){abertura2,b.y+b.height/2}; break;
        }
        case PADRAO_AFOGADO_MARE_CORAL: {
            float centro=b.y+b.height*(.5f+sinf(t*2.2f)*.18f), metade=22;
            Rectangle agua={b.x,centro+metade,b.width,b.y+b.height-centro-metade};
            Rectangle coral={b.x,b.y,b.width,centro-metade-b.y};
            if(tempo_local<.40f){AdicionarAviso(f,agua);AdicionarAviso(f,coral);}else{AdicionarAreaPerigosa(f,agua);AdicionarAreaPerigosa(f,coral);}
            f->guia=(Vector2){b.x+b.width/2,centro}; break;
        }
        case PADRAO_MAGO_ESPIRAL_CINZAS: {
            float cx=b.x+b.width/2,cy=b.y+b.height/2; int ausente=batida%8;
            for(int i=0;i<8;i++) if(i!=ausente&&i!=(ausente+1)%8){float a=t*.55f+i*PI/4;float raio=25+i*4;
                AdicionarAreaPerigosa(f,(Rectangle){cx+cosf(a)*raio-3,cy+sinf(a)*raio*.62f-3,6,6});}
            if(batida%2==0){Rectangle nucleo={cx-28,cy-20,56,40};if(tempo_local<.55f)AdicionarAviso(f,nucleo);else AdicionarAreaPerigosa(f,nucleo);}
            float a=t*.55f+(ausente+.5f)*PI/4;f->guia=(Vector2){cx+cosf(a)*58,cy+sinf(a)*36};break;
        }
        case PADRAO_GOBLIN_EXERCITO: {
            int livre=(batida*2)%4;float faixa=b.height/4;float progresso=Clamp((tempo_local-.38f)/.55f,0,1);
            for(int i=0;i<4;i++)if(i!=livre){float largura=b.width*.52f*progresso;
                Rectangle esq={b.x,b.y+i*faixa+2,largura,faixa-4},dir={b.x+b.width-largura,b.y+i*faixa+2,largura,faixa-4};
                if(tempo_local<.38f){AdicionarAviso(f,esq);AdicionarAviso(f,dir);}else{AdicionarAreaPerigosa(f,esq);AdicionarAreaPerigosa(f,dir);}}
            f->guia.y=b.y+(livre+.5f)*faixa;break;
        }
        case PADRAO_INQUISIDOR_VEREDITO: {
            int salvo=(batida*3+1)%4;float hw=b.width/2,hh=b.height/2;
            Rectangle quadrantes[4]={{b.x,b.y,hw,hh},{b.x+hw,b.y,hw,hh},{b.x,b.y+hh,hw,hh},{b.x+hw,b.y+hh,hw,hh}};
            for(int i=0;i<4;i++)if(i!=salvo){Rectangle r=quadrantes[i];
                if(tempo_local<.90f)AdicionarAviso(f,r);else AdicionarAreaPerigosa(f,r);}
            Rectangle s=quadrantes[salvo];f->guia=(Vector2){s.x+hw/2,s.y+hh/2};break;
        }
        default: break;
    }

    if (relogio < AQUECIMENTO) {
        for (int i = 0; i < f->total_perigos && f->total_avisos < 8; i++) AdicionarAviso(f, f->perigo[i]);
        f->total_perigos = 0;
    }
}

static void Atingir(EstadoDefesa *d, AtributosJogador *p)
{
    if (d->depurar_invencivel || d->temporizador_invencibilidade > 0) return;
    d->multiplicador_ultimo_dano = JogadorEfetividadeDanoRecebido(p, d->padrao.tipo);
    int dano = (int)ceilf(d->padrao.dano * d->multiplicador_ultimo_dano * p->multiplicador_dano_recebido);
    if (dano < 1) dano = 1;
    p->hp = p->hp > dano ? p->hp - dano : 0;
    d->ultimo_dano = dano; d->temporizador_popup_dano = .75f; d->temporizador_invencibilidade = .65f;
}

void PadraoMovimentoAtualizar(EstadoDefesa *d, AtributosJogador *jogador, EntradaDefesa entrada, float tempo_delta)
{
    Rectangle b = d->caixa;
    float t = fmaxf(0, d->temporizador_padrao - AQUECIMENTO), periodo = Periodo(d->id_padrao);
    int batida = (int)(t / periodo);
    if (d->batida_movimento != batida) {
        d->batida_movimento = batida; d->total_criacoes++; d->ancora_movimento = d->posicao_alma;
    }
    QuadroMovimento f; QuadroPadraoMovimento(d, &f);
    for (int i = 5; i > 0; i--) d->rastro_alma[i] = d->rastro_alma[i - 1];
    d->rastro_alma[0] = d->posicao_alma;
    Vector2 movimento = entrada.movimento;
    if (Vector2Length(movimento) > 1) movimento = Vector2Normalize(movimento);
    ModoFisica fisica = Fisica(d->id_padrao);
    if (fisica == FISICA_SALTO || fisica == FISICA_PLATAFORMA) {
        if (entrada.salto_pressionado) d->buffer_salto = .12f; else d->buffer_salto = fmaxf(0, d->buffer_salto - tempo_delta);
        if (d->no_chao) d->temporizador_coyote = .08f; else d->temporizador_coyote = fmaxf(0, d->temporizador_coyote - tempo_delta);
        if (d->buffer_salto > 0 && d->temporizador_coyote > 0) {
            d->velocidade_vertical = -VELOCIDADE_SALTO; d->buffer_salto = 0; d->temporizador_coyote = 0; d->no_chao = false;
        }
        float base_anterior = d->posicao_alma.y + d->raio_alma;
        if (d->no_chao && fisica == FISICA_PLATAFORMA) d->posicao_alma.x -= f.velocidade_rolagem * tempo_delta;
        d->posicao_alma.x += movimento.x * JogadorVelocidadeDefesa(jogador) * tempo_delta;
        d->velocidade_vertical += GRAVIDADE * tempo_delta; d->posicao_alma.y += d->velocidade_vertical * tempo_delta; d->no_chao = false;
        if (d->velocidade_vertical >= 0) for (int i = 0; i < f.total_plataformas; i++) {
            Rectangle plataforma = f.plataformas[i];
            const float margem_pouso = 7.0f;
            if (d->posicao_alma.x + d->raio_alma > plataforma.x - margem_pouso &&
                d->posicao_alma.x - d->raio_alma < plataforma.x + plataforma.width + margem_pouso &&
                base_anterior <= plataforma.y + 2 && d->posicao_alma.y + d->raio_alma >= plataforma.y) {
                d->posicao_alma.y = plataforma.y - d->raio_alma; d->velocidade_vertical = 0; d->no_chao = true; break;
            }
        }
        if (d->posicao_alma.y - d->raio_alma < b.y) {
            d->posicao_alma.y = b.y + d->raio_alma; d->velocidade_vertical = fmaxf(0, d->velocidade_vertical);
        }
    } else {
        d->posicao_alma = Vector2Add(d->posicao_alma, Vector2Scale(movimento, JogadorVelocidadeDefesa(jogador) * tempo_delta));
        if (fisica == FISICA_EMPURRAO && d->temporizador_padrao >= AQUECIMENTO) {
            float tempo_local = fmodf(t, periodo);
            if (tempo_local >= .20f && tempo_local < .43f) {
                Vector2 forca = f.direcao == 0 ? (Vector2){-1, 0} : f.direcao == 1 ? (Vector2){1, 0} :
                                f.direcao == 2 ? (Vector2){0, -1} : (Vector2){0, 1};
                float intensidade = d->id_padrao == PADRAO_SINO_IMPACTO ? 1350 : 1750;
                d->posicao_alma = Vector2Add(d->posicao_alma, Vector2Scale(forca, intensidade * tempo_delta));
            }
        }
    }
    d->posicao_alma.x = Clamp(d->posicao_alma.x, b.x + d->raio_alma, b.x + b.width - d->raio_alma);
    d->posicao_alma.y = Clamp(d->posicao_alma.y, b.y + d->raio_alma, b.y + b.height - d->raio_alma);
    for (int i = 0; i < f.total_perigos; i++) {
        if (!CirculoPerigoAtingeRetangulo(d->posicao_alma, d->raio_alma, f.perigo[i])) continue;
        bool nocivo = true;
        if (d->id_padrao == PADRAO_MARIPOSA_CORRENTE) {
            float intencao = Vector2Length(entrada.movimento);
            /* Uma tolerancia breve na troca de cor cobre uma entrada a 30 FPS entre batidas. */
            float regra_local = fmodf(t, Periodo(d->id_padrao));
            nocivo = regra_local > .14f && (f.regra_movimento == 1 ? intencao > .08f : intencao < .08f);
        }
        if (nocivo) { Atingir(d, jogador); break; }
    }
    if (fisica == FISICA_PLATAFORMA && d->posicao_alma.y >= b.y + b.height - 14) {
        Atingir(d, jogador);
        if (f.total_plataformas > 0) {
            Rectangle pouso = f.plataformas[f.total_plataformas / 2];
            d->posicao_alma = (Vector2){pouso.x + pouso.width / 2, pouso.y - d->raio_alma};
            d->velocidade_vertical = 0; d->no_chao = true;
        }
    }
}

static bool EhFeixe(ModoMovimento modo)
{
    return modo == MOVIMENTO_FAGULHA_FAROL || modo == MOVIMENTO_TORRE_BASTIAO ||
           modo == MOVIMENTO_SINO_CARRILHAO || modo == MOVIMENTO_CAVALEIRO_LANCAS;
}

static void DesenharSpriteNaArea(const EstadoDefesa *d, const SistemaSprites *sprites, Rectangle area)
{
    if (sprites == NULL) return;
    IdSprite id = SpriteIdPadraoProjetil(d->id_padrao);
    if (sprite_obter(sprites, id) == NULL) id = SpriteIdProjetil(d->padrao.tipo);
    if (sprite_obter(sprites, id) == NULL) return;
    const bool horizontal = area.width >= area.height;
    const float comprimento = horizontal ? area.width : area.height;
    int quantidade = (int)(comprimento / 28.0f);
    if (quantidade < 1) quantidade = 1;
    if (quantidade > 10) quantidade = 10;
    const float espessura = horizontal ? area.height : area.width;
    const float escala = Clamp(espessura / 16.0f, .65f, 1.8f);
    for (int i = 0; i < quantidade; i++) {
        const float proporcao = ((float)i + .5f) / quantidade;
        Vector2 centro = horizontal ?
            (Vector2){area.x + area.width * proporcao, area.y + area.height * .5f} :
            (Vector2){area.x + area.width * .5f, area.y + area.height * proporcao};
        SpriteDesenharCentralizadoEscalado(sprites, id, centro, WHITE,
                                           d->temporizador_padrao + i * .07f, escala,
                                           sprites->depurar_limites);
    }
}

void PadraoMovimentoDesenhar(const EstadoDefesa *d, const SistemaSprites *sprites)
{
    QuadroMovimento f; QuadroPadraoMovimento(d, &f);
    Rectangle b = d->caixa; Color cor = cor_tipo(d->padrao.tipo);
    DrawText(InstrucaoPadraoMovimento(d->id_padrao), (int)b.x, (int)b.y - 24, 10, WHITE);
    for (int i = 0; i < f.total_avisos; i++) {
        DrawRectangleRec(f.avisos[i], Fade(YELLOW, .10f)); DrawRectangleLinesEx(f.avisos[i], 1, YELLOW);
    }
    BeginScissorMode((int)b.x + 3, (int)b.y + 3, (int)b.width - 6, (int)b.height - 6);
    for (int i = 0; i < f.total_perigos; i++) {
        Rectangle r = f.perigo[i];
        if (f.modo == MOVIMENTO_MARIPOSA_POEIRA || f.modo == MOVIMENTO_FAGULHA_ORBITA) {
            Vector2 centro = {r.x + r.width / 2, r.y + r.height / 2};
            DrawCircleV(centro, fmaxf(r.width, r.height) / 2, Fade(cor, .82f));
            DrawCircleLines((int)centro.x, (int)centro.y, fmaxf(r.width, r.height) / 2, WHITE);
        } else if (f.modo == MOVIMENTO_PASSARO_FLOCK) {
            DrawTriangle((Vector2){r.x, r.y + r.height / 2}, (Vector2){r.x + r.width, r.y},
                         (Vector2){r.x + r.width, r.y + r.height}, ORANGE);
            DrawLine((int)r.x + 4, (int)(r.y + r.height / 2), (int)(r.x + r.width), (int)(r.y + r.height / 2), WHITE);
        } else if (f.modo == MOVIMENTO_PASSARO_CHUVA_FOGO) {
            const Vector2 ponta = {r.x + r.width / 2.0f, r.y};
            const Vector2 esquerda = {r.x, r.y + r.height - 5.0f};
            const Vector2 direita = {r.x + r.width, r.y + r.height - 5.0f};
            DrawTriangle(ponta, esquerda, direita, RED);
            DrawCircle((int)(r.x + r.width / 2.0f), (int)(r.y + r.height - 6.0f),
                       r.width * 0.46f, ORANGE);
            DrawTriangle((Vector2){ponta.x, r.y + 7.0f},
                         (Vector2){r.x + 4.0f, r.y + r.height - 5.0f},
                         (Vector2){r.x + r.width - 4.0f, r.y + r.height - 5.0f}, YELLOW);
        } else if (f.modo == MOVIMENTO_SAPO_RIPPLES) {
            DrawRectangleRec(r, Fade(BLUE, .72f));
            for (float y = r.y; y < r.y + r.height; y += 9)
                DrawCircleLines((int)(r.x + r.width / 2), (int)y, fmaxf(4, r.width / 2), SKYBLUE);
        } else if (f.modo == MOVIMENTO_LIMO_MARE) {
            DrawRectangleRounded(r, .7f, 8, Fade(GREEN, .82f));
            DrawCircleV((Vector2){r.x + r.width * .35f, r.y + 7}, 3, WHITE);
        } else if (f.modo == MOVIMENTO_LEBRE_BARREIRAS) {
            DrawRectangleRec(r, BROWN);
            DrawLineEx((Vector2){r.x, r.y + r.height * .3f},
                       (Vector2){r.x + r.width, r.y + r.height * .3f}, 3, WHITE);
            DrawLineEx((Vector2){r.x, r.y + r.height * .7f},
                       (Vector2){r.x + r.width, r.y + r.height * .7f}, 3, WHITE);
        } else if (f.modo == MOVIMENTO_CAVALEIRO_JUSTA) {
            DrawRectangleRec(r, Fade(cor, .82f));
            if (r.width < r.height) DrawTriangle((Vector2){r.x, r.y}, (Vector2){r.x + r.width, r.y},
                                                 (Vector2){r.x + r.width / 2, r.y - 9}, WHITE);
            else DrawTriangle((Vector2){r.x, r.y}, (Vector2){r.x, r.y + r.height},
                              (Vector2){r.x - 10, r.y + r.height / 2}, WHITE);
        } else if (f.modo == MOVIMENTO_TORRE_COLAPSO) {
            DrawRectangleRec(r, GRAY);
            for (float y = r.y + 13; y < r.y + r.height; y += 16)
                DrawLine((int)r.x, (int)y, (int)(r.x + r.width), (int)y, LIGHTGRAY);
        } else {
            DrawRectangleRec(r, Fade(cor, EhFeixe(f.modo) ? .62f : .85f));
            DrawRectangleLinesEx(r, EhFeixe(f.modo) ? 2 : 1, WHITE);
        }
        DesenharSpriteNaArea(d, sprites, r);
    }
    for (int i = 0; i < f.total_plataformas; i++) {
        Rectangle p = f.plataformas[i];
        if (f.modo == MOVIMENTO_SAPO_VITORIAS_REGIAS) {
            DrawEllipse((int)(p.x + p.width / 2), (int)p.y, p.width / 2, 6, LIME);
            DrawLine((int)(p.x + p.width / 2), (int)p.y, (int)(p.x + p.width), (int)p.y - 3, DARKGREEN);
        } else if (f.modo == MOVIMENTO_LIMO_BALSAS) {
            DrawRectangleRounded(p, .8f, 8, GREEN); DrawCircle((int)p.x + 8, (int)p.y, 3, WHITE);
        } else if (f.modo == MOVIMENTO_LEBRE_BURROWS) {
            DrawEllipse((int)(p.x + p.width / 2), (int)p.y, p.width / 2, 6, BROWN);
            DrawRectangleLinesEx(p, 1, WHITE);
        } else {
            DrawRectangleRec(p, GRAY);
            for (float x = p.x + 12; x < p.x + p.width; x += 12)
                DrawLine((int)x, (int)p.y, (int)x, (int)(p.y + p.height), LIGHTGRAY);
        }
    }
    EndScissorMode();
    if (f.modo == MOVIMENTO_MARIPOSA_POEIRA) {
        const char *regra = f.regra_movimento == 1 ? "POEIRA AZUL: PARE" : "POEIRA LARANJA: MOVA";
        DrawText(regra, (int)b.x + 10, (int)b.y + 10, 14, f.regra_movimento == 1 ? SKYBLUE : ORANGE);
    }
    if (f.modo == MOVIMENTO_GATO_BOTE || f.modo == MOVIMENTO_SINO_PESO) {
        static const char *instructions[] = {"DIREITA >>", "<< ESQUERDA", "BAIXO v", "CIMA ^"};
        DrawText(instructions[f.direcao], (int)b.x + 12, (int)b.y + 10, 18, YELLOW);
    }
    if (d->temporizador_padrao < AQUECIMENTO) DrawText("LEIA O AVISO", (int)b.x + 10, (int)b.y + 30, 14, WHITE);
    if (d->ataque_segunda_fase) DrawRectangleLinesEx(b, 3, MAROON);
}
