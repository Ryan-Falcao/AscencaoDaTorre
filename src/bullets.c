#include "pilot_patterns.h"
#include "ui.h"
#include "bullets.h"

#include "input.h"
#include "ui.h"
#include "sprites.h"
#include "balance.h"
#include "patterns.h"
#include "motion_patterns.h"
#include "raymath.h"

#include <math.h>
#include <string.h>

#define AGRESSIVIDADE_BAIXA_MAXIMO 33
#define AGRESSIVIDADE_ALTA_MIN 67
#define ALTA_AGRESSIVIDADE_VELOCIDADE_MULTIPLICADOR 1.15f
#define ALTA_AGRESSIVIDADE_DANO_MULTIPLICADOR 1.25f
#define DEFESA_INICIO_TOLERANCIA_SEGUNDOS 0.35f
#define RAIO_CRIACAO_SEGURA_ESPIRAL 34.0f
/* A arena sempre cabe no corredor entre o inimigo e o HUD. Padrões podem
   pedir tamanhos diferentes, mas nunca invadem a interface inferior. */
#define DEFESA_TOPO_ARENA 148.0f
#define DEFESA_LIMITE_INFERIOR 326.0f
#define DEFESA_LARGURA_MINIMA 120.0f
#define DEFESA_ALTURA_MINIMA 90.0f
#define DEFESA_LARGURA_MAXIMA 556.0f

static Rectangle CaixaDefesaAjustada(float largura_desejada, float altura_desejada)
{
    const float largura = Clamp(largura_desejada, DEFESA_LARGURA_MINIMA, DEFESA_LARGURA_MAXIMA);
    const float altura_maxima = DEFESA_LIMITE_INFERIOR - DEFESA_TOPO_ARENA;
    const float altura = Clamp(altura_desejada, DEFESA_ALTURA_MINIMA, altura_maxima);

    return (Rectangle){
        (640.0f - largura) * 0.5f,
        DEFESA_TOPO_ARENA,
        largura,
        altura
    };
}

#define PADROES BALANCEAMENTO_PADROES

static Rectangle InterpolarRetangulo(Rectangle origem, Rectangle destino, float quantidade)
{
    return (Rectangle){
        origem.x + (destino.x - origem.x) * quantidade,
        origem.y + (destino.y - origem.y) * quantidade,
        origem.width + (destino.width - origem.width) * quantidade,
        origem.height + (destino.height - origem.height) * quantidade
    };
}

static void LimparProjeteis(EstadoDefesa *defesa)
{
    for (int indice_projetil = 0; indice_projetil < MAXIMO_PROJETEIS; indice_projetil++) {
        defesa->projeteis[indice_projetil].ativo = false;
    }
}

static Rectangle RetanguloEspinhoGravidade(const EstadoDefesa *defesa)
{
    const Rectangle caixa = defesa->caixa;
    const float depth = BALANCEAMENTO_GRAVIDADE_ESPINHO_PROFUNDIDADE;
    switch (defesa->parede_gravidade) {
        case 0: return (Rectangle){caixa.x, caixa.y, caixa.width, depth};
        case 1: return (Rectangle){caixa.x, caixa.y + caixa.height - depth, caixa.width, depth};
        case 2: return (Rectangle){caixa.x, caixa.y, depth, caixa.height};
        default: return (Rectangle){caixa.x + caixa.width - depth, caixa.y, depth, caixa.height};
    }
}

static void IniciarCicloGravidade(EstadoDefesa *defesa)
{
    defesa->parede_gravidade = GetRandomValue(0, 3);
    defesa->espinhos_gravidade_criados = false;
    defesa->temporizador_especial = 0.0f;
}

static void IniciarFloracaoSombria(EstadoDefesa *defesa)
{
    const int total = defesa->repeticoes_gravidade > 2 ? 7 : 5;
    for (int indice = 0; indice < total; indice++) {
        Vector2 posicao;
        do {
            posicao = (Vector2){(float)GetRandomValue((int)defesa->caixa.x + 34,
                                                         (int)(defesa->caixa.x + defesa->caixa.width - 34)),
                                 (float)GetRandomValue((int)defesa->caixa.y + 34,
                                                         (int)(defesa->caixa.y + defesa->caixa.height - 34))};
        } while (Vector2Distance(posicao, defesa->posicao_alma) < 72.0f);
        PerigosAdicionarCirculo(&defesa->perigos, posicao,
                         BALANCEAMENTO_SOMBRA_FLORACAO_RAIO + (float)(indice % 2) * 8.0f,
                         BALANCEAMENTO_SOMBRA_FLORACAO_TEMPO_CRESCIMENTO + (float)indice * BALANCEAMENTO_SOMBRA_FLORACAO_ATRASO,
                         0.10f, 0.15f, TIPO_SOMBRA, BALANCEAMENTO_SOMBRA_FLORACAO_DANO, true);
    }
}

static void IniciarPadraoEspecial(EstadoDefesa *defesa)
{
    defesa->temporizador_especial = 0.0f;
    if (defesa->id_padrao == PADRAO_GRAVIDADE_INVERSAO) IniciarCicloGravidade(defesa);
    else if (defesa->id_padrao == PADRAO_SOMBRA_FLORACAO) IniciarFloracaoSombria(defesa);
}

static Projetil *CriarProjetil(EstadoDefesa *defesa, Vector2 posicao, Vector2 velocidade)
{
    for (int indice_projetil = 0; indice_projetil < MAXIMO_PROJETEIS; indice_projetil++) {
        Projetil *projetil = &defesa->projeteis[indice_projetil];

        if (!projetil->ativo) {
            *projetil = (Projetil){
                true,
                posicao,
                velocidade,
                defesa->padrao.raio_colisao,
                defesa->padrao.tipo,
                defesa->padrao.dano,
                0.0f,
                false
            };
            return projetil;
        }
    }
    return NULL;
}

static void CriarDisparoPadrao(EstadoDefesa *defesa)
{
    const Rectangle caixa = defesa->caixa;
    const PadraoProjetil *padrao = &defesa->padrao;

    if (padrao->modo_criacao == CRIACAO_CHUVA) {
        for (int indice_projetil = 0; indice_projetil < padrao->projeteis_por_criacao; indice_projetil++) {
            const float x = (float)GetRandomValue((int)caixa.x + 8, (int)(caixa.x + caixa.width - 8));
            const float diagonal = defesa->id_padrao == PADRAO_DIAGONAL_CHUVA ?
                (indice_projetil % 2 == 0 ? padrao->velocidade * 0.42f : -padrao->velocidade * 0.42f) : 0.0f;
            CriarProjetil(defesa, (Vector2){x, caixa.y - padrao->raio},
                        (Vector2){diagonal, padrao->velocidade});
        }
    } else if (padrao->modo_criacao == CRIACAO_LATERAL_ONDA) {
        const int abertura = defesa->total_criacoes % padrao->projeteis_por_criacao;
        const float espacamento = caixa.height / (float)(padrao->projeteis_por_criacao + 1);

        for (int indice_projetil = 0; indice_projetil < padrao->projeteis_por_criacao; indice_projetil++) {
            if (indice_projetil == abertura || indice_projetil == abertura + 1) continue;
            CriarProjetil(defesa,
                        (Vector2){caixa.x - padrao->raio,
                                  caixa.y + espacamento * (float)(indice_projetil + 1)},
                        (Vector2){padrao->velocidade, 0.0f});
        }
    } else if (padrao->modo_criacao == CRIACAO_ESPIRAL) {
        const Vector2 centro = {caixa.x + caixa.width * 0.5f, caixa.y + caixa.height * 0.5f};

        for (int indice_projetil = 0; indice_projetil < padrao->projeteis_por_criacao; indice_projetil++) {
            const float angulo = defesa->angulo_espiral + padrao->abertura_angular * (float)indice_projetil;
            /* Mantem uma abertura de fuga larga e fixa a direita durante toda a espiral de fogo. */
            if (defesa->id_padrao == PADRAO_ESPIRAL && cosf(angulo) > 0.70710678f) continue;
            const Vector2 velocidade = {cosf(angulo) * padrao->velocidade, sinf(angulo) * padrao->velocidade};
            const Vector2 posicao = {
                centro.x + cosf(angulo) * RAIO_CRIACAO_SEGURA_ESPIRAL,
                centro.y + sinf(angulo) * RAIO_CRIACAO_SEGURA_ESPIRAL
            };

            /* A alma comeca no centro: nunca cria um projetil espiral dentro da sua colisao. */
            CriarProjetil(defesa, posicao, velocidade);
        }
        defesa->angulo_espiral += 0.48f;
    }

    defesa->total_criacoes++;
}

void DefesaInicializar(EstadoDefesa *defesa)
{
    memset(defesa, 0, sizeof(*defesa));
    defesa->fase = DEFESA_FINALIZADA;
    defesa->raio_alma = 4.0f;
}

void DefesaIniciar(EstadoDefesa *defesa, IdPadraoProjetil id_padrao, Rectangle caixa_menu,
                  int agressividade, int bonus_dano, float multiplicador_dano, float multiplicador_velocidade)
{
    if (id_padrao < PADRAO_CHUVA || id_padrao >= PADRAO_TOTAL) id_padrao = PADRAO_CHUVA;

    LimparProjeteis(defesa);
    PerigosLimpar(&defesa->perigos);
    defesa->padrao = PADROES[id_padrao];
    defesa->orcamento_iniciado = false;
    defesa->fator_movimento_ataque = fminf(1.0f,multiplicador_velocidade);
    defesa->dash_restante = 0;
    defesa->agua = (EstadoMovimentoAgua){0};
    defesa->total_eventos_acertados = 0;
    defesa->id_padrao = id_padrao;
    if (id_padrao == PADRAO_SINO_DESAFIO || id_padrao == PADRAO_TORRE_DESAFIO)
        defesa->padrao.duracao = 12.6f;
    defesa->temporizador_especial = 0;
    defesa->aviso_ataque = agressividade >= 67 ? .62f : (agressividade <= 33 ? .72f : .67f);
    if (multiplicador_velocidade < 1.0f) defesa->aviso_ataque += .2f;
    defesa->ataque_segunda_fase = false;
    /* Encerra cada recuperacao logo apos seu ultimo impacto. Cada inimigo possui
       sua propria pausa e um perigo ainda ativo nunca e removido antes da hora. */
    float cauda = .95f;
    if (id_padrao == PADRAO_SAPO_SALTOS) cauda = 1.15f;
    else if (id_padrao == PADRAO_GATO_EMBOSCADA) cauda = .65f;
    else if (id_padrao == PADRAO_FAGULHA_LUZ || id_padrao == PADRAO_TORRE_ESMAGAMENTO) cauda = .70f;
    else if (id_padrao == PADRAO_SINO_ECO) cauda = 1.0f;
    defesa->ciclo_ataque = defesa->aviso_ataque + cauda;
    defesa->origem_ataque = defesa->alvo_ataque = (Vector2){320, 248};
    if (defesa->padrao.escala_visual <= 0.0f) defesa->padrao.escala_visual = 1.0f;
    if (defesa->padrao.raio_colisao <= 0.0f) defesa->padrao.raio_colisao = defesa->padrao.raio;
    /* Valores padrao preservam dados antigos e dao uma aparencia clara a cada padrao novo. */
    defesa->padrao.rotacionar = id_padrao == PADRAO_DIAGONAL_CHUVA ||
                               id_padrao == PADRAO_PAUSA_DARDOS ||
                               id_padrao == PADRAO_PERSEGUICAO_ONDA;
    defesa->padrao.rastro = id_padrao == PADRAO_ESPIRAL || id_padrao == PADRAO_FECHADA_ESPIRAL ||
                             id_padrao == PADRAO_PETALA_ORBITA || id_padrao == PADRAO_CATA_VENTO;
    if (id_padrao == PADRAO_CRUZADOS_LASERS || id_padrao == PADRAO_MOVEL_PAREDE) {
        defesa->padrao.escala_visual = 1.35f;
    }
    if (PilotoEh(id_padrao)) {
        const ConfiguracaoPiloto *config = &BALANCEAMENTO_PILOTO[id_padrao-PADRAO_PILOTO_POEIRA_ASA];
        defesa->padrao.dano = config->dano;
        defesa->padrao.duracao = config->duracao;
        defesa->aviso_ataque = config->aviso;
    } else defesa->padrao.dano += bonus_dano;
    defesa->padrao.dano = (int)ceilf((float)defesa->padrao.dano * multiplicador_dano);
    defesa->padrao.velocidade *= multiplicador_velocidade;
    if (agressividade >= AGRESSIVIDADE_ALTA_MIN) {
        defesa->padrao.velocidade *= ALTA_AGRESSIVIDADE_VELOCIDADE_MULTIPLICADOR;
        if (!PilotoEh(id_padrao)) defesa->padrao.dano = (int)ceilf((float)defesa->padrao.dano *
                                             ALTA_AGRESSIVIDADE_DANO_MULTIPLICADOR);
    } else if (agressividade <= AGRESSIVIDADE_BAIXA_MAXIMO) {
        defesa->padrao.velocidade *= 0.90f;
    }
    defesa->caixa_inicial = caixa_menu;
    defesa->caixa_alvo = CaixaDefesaAjustada(defesa->padrao.largura_caixa,
                                              defesa->padrao.altura_caixa);
    defesa->caixa = caixa_menu;
    defesa->posicao_alma = (Vector2){
        defesa->caixa_alvo.x + defesa->caixa_alvo.width * 0.5f,
        defesa->caixa_alvo.y + defesa->caixa_alvo.height * 0.5f
    };
    for (int indice = 0; indice < 6; indice++) defesa->rastro_alma[indice] = defesa->posicao_alma;
    /* Uma pausa curta apos a abertura da caixa evita um primeiro acerto inevitavel. */
    defesa->temporizador_invencibilidade = DEFESA_INICIO_TOLERANCIA_SEGUNDOS + 0.25f;
    defesa->temporizador_fase = 0.0f;
    defesa->temporizador_padrao = 0.0f;
    defesa->temporizador_criacao = -DEFESA_INICIO_TOLERANCIA_SEGUNDOS;
    defesa->temporizador_recarga_dash = 0.0f;
    defesa->angulo_espiral = 0.0f;
    defesa->total_criacoes = 0;
    defesa->repeticoes_gravidade = 2;
    defesa->fase = DEFESA_ENTRANDO;
}

int DificuldadePadraoProjetil(IdPadraoProjetil id_padrao)
{
    if (id_padrao < PADRAO_CHUVA || id_padrao >= PADRAO_TOTAL) return 1;
    return PADROES[id_padrao].dificuldade;
}

void DefesaRedimensionarCaixa(EstadoDefesa *defesa, float width, float height)
{
    defesa->caixa_alvo = CaixaDefesaAjustada(width, height);
}

static void AtualizarAlma(EstadoDefesa *defesa, AtributosJogador *jogador,
                          EntradaDefesa entrada, float tempo_delta)
{
    Vector2 movimento = entrada.movimento;
    const float velocidade = JogadorVelocidadeDefesa(jogador);

    if (movimento.x != 0.0f && movimento.y != 0.0f) movimento = Vector2Scale(movimento, 0.7071f);

    for (int indice = 5; indice > 0; indice--) defesa->rastro_alma[indice] = defesa->rastro_alma[indice - 1];
    defesa->rastro_alma[0] = defesa->posicao_alma;
    defesa->posicao_alma = Vector2Add(defesa->posicao_alma, Vector2Scale(movimento, velocidade * tempo_delta));
    if (defesa->id_padrao == PADRAO_GRAVIDADE_INVERSAO && !defesa->espinhos_gravidade_criados) {
        const float puxao = BALANCEAMENTO_GRAVIDADE_FORCA * tempo_delta;
        if (defesa->parede_gravidade == 0) defesa->posicao_alma.y -= puxao;
        else if (defesa->parede_gravidade == 1) defesa->posicao_alma.y += puxao;
        else if (defesa->parede_gravidade == 2) defesa->posicao_alma.x -= puxao;
        else defesa->posicao_alma.x += puxao;
    }
    defesa->posicao_alma.x = Clamp(defesa->posicao_alma.x, defesa->caixa.x + defesa->raio_alma,
                                     defesa->caixa.x + defesa->caixa.width - defesa->raio_alma);
    defesa->posicao_alma.y = Clamp(defesa->posicao_alma.y, defesa->caixa.y + defesa->raio_alma,
                                     defesa->caixa.y + defesa->caixa.height - defesa->raio_alma);
}

static void AtualizarPadraoEspecial(EstadoDefesa *defesa, AtributosJogador *jogador, float tempo_delta)
{
    if (defesa->id_padrao == PADRAO_GRAVIDADE_INVERSAO) {
        const float fuga = defesa->repeticoes_gravidade > 2 ? BALANCEAMENTO_GRAVIDADE_FUGA_TEMPO_FASE_DOIS :
                                                            BALANCEAMENTO_GRAVIDADE_FUGA_TEMPO;
        defesa->temporizador_especial += tempo_delta;
        if (!defesa->espinhos_gravidade_criados && defesa->temporizador_especial >= fuga) {
            PerigosAdicionarRetangulo(&defesa->perigos, RetanguloEspinhoGravidade(defesa), 0.0f,
                           BALANCEAMENTO_GRAVIDADE_ESPINHO_ATIVA_TEMPO, BALANCEAMENTO_GRAVIDADE_ESPINHO_DESVANECER_TEMPO, defesa->padrao.tipo,
                           defesa->padrao.dano, true);
            defesa->espinhos_gravidade_criados = true;
        }
        if (defesa->espinhos_gravidade_criados && defesa->temporizador_especial >=
            fuga + BALANCEAMENTO_GRAVIDADE_ESPINHO_ATIVA_TEMPO + BALANCEAMENTO_GRAVIDADE_ESPINHO_DESVANECER_TEMPO) {
            defesa->repeticao_gravidade++;
            if (defesa->repeticao_gravidade < defesa->repeticoes_gravidade) IniciarCicloGravidade(defesa);
            else defesa->temporizador_padrao = defesa->padrao.duracao;
        }
    }
    const int vida_antes = jogador->hp;
    defesa->perigos.orcamento = &defesa->orcamento;
    defesa->perigos.tem_percurso = true;
    defesa->perigos.origem_percurso = defesa->inicio_movimento;
    PerigosAtualizar(&defesa->perigos, jogador, defesa->posicao_alma, defesa->raio_alma, tempo_delta,
                  1.0f, defesa->depurar_invencivel || defesa->temporizador_invencibilidade > 0.0f);
    if (jogador->hp < vida_antes) {
        defesa->ultimo_dano = vida_antes - jogador->hp;
        defesa->multiplicador_ultimo_dano = JogadorEfetividadeDanoRecebido(jogador, defesa->padrao.tipo);
        defesa->temporizador_popup_dano = 0.75f;
        defesa->temporizador_invencibilidade = defesa->orcamento.invulnerabilidade;
    }
}

static void AtualizarProjeteis(EstadoDefesa *defesa, AtributosJogador *jogador, float tempo_delta)
{
    for (int indice_projetil = 0; indice_projetil < MAXIMO_PROJETEIS; indice_projetil++) {
        Projetil *projetil = &defesa->projeteis[indice_projetil];

        if (!projetil->ativo) continue;

        projetil->tempo_vida += tempo_delta;
        if (defesa->id_padrao == PADRAO_REBOTE_ORBES) {
            const float next_x = projetil->posicao.x + projetil->velocidade.x * tempo_delta;
            const float next_y = projetil->posicao.y + projetil->velocidade.y * tempo_delta;
            if (next_x < defesa->caixa.x + projetil->raio ||
                next_x > defesa->caixa.x + defesa->caixa.width - projetil->raio) projetil->velocidade.x *= -1.0f;
            if (next_y < defesa->caixa.y + projetil->raio ||
                next_y > defesa->caixa.y + defesa->caixa.height - projetil->raio) projetil->velocidade.y *= -1.0f;
        } else if (defesa->id_padrao == PADRAO_PAUSA_DARDOS) {
            if (projetil->tempo_vida > 0.42f && projetil->tempo_vida < 0.82f) {
                projetil->velocidade = Vector2Scale(projetil->velocidade, 0.82f);
            } else if (projetil->tempo_vida >= 0.82f) {
                Vector2 direcao = Vector2Subtract(defesa->posicao_alma, projetil->posicao);
                if (Vector2Length(direcao) > 0.01f) {
                    direcao = Vector2Normalize(direcao);
                    projetil->velocidade = Vector2Scale(direcao, defesa->padrao.velocidade * 1.65f);
                }
            }
        } else if (defesa->id_padrao == PADRAO_PERSEGUICAO_ONDA) {
            Vector2 direcao = Vector2Subtract(defesa->posicao_alma, projetil->posicao);
            if (Vector2Length(direcao) > 0.01f) {
                direcao = Vector2Normalize(direcao);
                projetil->velocidade = Vector2Lerp(projetil->velocidade,
                    Vector2Scale(direcao, defesa->padrao.velocidade), 0.025f);
            }
        } else if (defesa->id_padrao == PADRAO_PETALA_ORBITA) {
            const Vector2 centro = {defesa->caixa.x + defesa->caixa.width * 0.5f,
                                    defesa->caixa.y + defesa->caixa.height * 0.5f};
            Vector2 radial = Vector2Subtract(projetil->posicao, centro);
            if (Vector2Length(radial) > 0.01f) {
                radial = Vector2Normalize(radial);
                projetil->velocidade = Vector2Scale((Vector2){-radial.y, radial.x}, defesa->padrao.velocidade);
            }
        }
        projetil->posicao = Vector2Add(projetil->posicao, Vector2Scale(projetil->velocidade, tempo_delta));

        if (defesa->id_padrao == PADRAO_DIVISAO_DISPAROS && !projetil->divisao_feita &&
            projetil->tempo_vida >= 0.65f) {
            const float angulo_base = atan2f(projetil->velocidade.y, projetil->velocidade.x);
            projetil->divisao_feita = true;
            for (int fragmento = -1; fragmento <= 1; fragmento++) {
                const float angulo = angulo_base + (float)fragmento * 0.42f;
                Projetil *projetil_fragmento = CriarProjetil(defesa, projetil->posicao,
                    (Vector2){cosf(angulo) * defesa->padrao.velocidade,
                              sinf(angulo) * defesa->padrao.velocidade});
                if (projetil_fragmento != NULL) projetil_fragmento->divisao_feita = true;
            }
            projetil->ativo = false;
            continue;
        }
        if (projetil->posicao.x < defesa->caixa.x - 24.0f ||
            projetil->posicao.x > defesa->caixa.x + defesa->caixa.width + 24.0f ||
            projetil->posicao.y < defesa->caixa.y - 24.0f ||
            projetil->posicao.y > defesa->caixa.y + defesa->caixa.height + 24.0f) {
            projetil->ativo = false;
            continue;
        }

        const float soul_distance = Vector2Distance(defesa->posicao_alma, projetil->posicao);
        if (soul_distance <= projetil->raio + defesa->raio_alma + 5.0f &&
            soul_distance > projetil->raio + defesa->raio_alma) {
            defesa->temporizador_popup_raspar = 0.22f;
        }
        if (!defesa->depurar_invencivel && defesa->temporizador_invencibilidade <= 0.0f &&
            PercursoAtingeCirculo(defesa->inicio_movimento, defesa->posicao_alma, defesa->raio_alma,
                                  projetil->posicao, projetil->raio)) {
            DefesaAplicarDano(defesa, jogador, projetil->dano, projetil->tipo);
        }
    }
}

static ResultadoDefesa PassoDefesa(EstadoDefesa *defesa, AtributosJogador *jogador, float tempo_delta,
                                 EntradaDefesa entrada)
{
    if (!defesa->orcamento_iniciado) {
        DanoIniciarRodada(&defesa->orcamento, jogador->vida_maxima,
            BalanceamentoTetoImpacto(defesa->id_padrao),
            BalanceamentoFinal(defesa->id_padrao) ? BALANCEAMENTO_TETO_FINAL_PERCENTUAL : BALANCEAMENTO_TETO_COMUM_PERCENTUAL);
        defesa->orcamento_iniciado = true;
    }
    defesa->inicio_movimento = defesa->posicao_alma;
    defesa->temporizador_fase += tempo_delta;
    if (defesa->temporizador_invencibilidade > 0.0f) defesa->temporizador_invencibilidade = fmaxf(0, defesa->temporizador_invencibilidade - tempo_delta);
    defesa->orcamento.invulnerabilidade = fmaxf(0, defesa->temporizador_invencibilidade);
    if (defesa->temporizador_popup_dano > 0.0f) defesa->temporizador_popup_dano -= tempo_delta;
    if (defesa->temporizador_popup_raspar > 0.0f) defesa->temporizador_popup_raspar -= tempo_delta;
    if (defesa->temporizador_recarga_dash > 0.0f)
        defesa->temporizador_recarga_dash -= tempo_delta;

    if (defesa->fase == DEFESA_ENTRANDO) {
        const float progresso = Clamp(defesa->temporizador_fase / 0.25f, 0.0f, 1.0f);
        defesa->caixa = InterpolarRetangulo(defesa->caixa_inicial, defesa->caixa_alvo, progresso);
        if (progresso >= 1.0f) {
            defesa->fase = DEFESA_ATIVA;
            defesa->temporizador_fase = 0.0f;
            IniciarPadraoEspecial(defesa);
            if (PadraoMovimentoEh(defesa->id_padrao)) PadraoMovimentoIniciar(defesa);
        }
    } else if (defesa->fase == DEFESA_ATIVA) {
        defesa->temporizador_padrao += tempo_delta;
        defesa->temporizador_criacao += tempo_delta;
        if (entrada.dash_pressionado && defesa->temporizador_recarga_dash <= 0.0f &&
            Vector2Length(entrada.movimento) > 0.01f) {
            defesa->direcao_dash = Vector2Normalize(entrada.movimento);
            defesa->dash_restante = DISTANCIA_DASH_DEFESA;
            defesa->temporizador_recarga_dash = JogadorRecargaDash(jogador);
        }
        if (defesa->dash_restante > 0) {
            float passo_dash = fminf(defesa->dash_restante, 400.0f * tempo_delta);
            defesa->posicao_alma = Vector2Add(defesa->posicao_alma, Vector2Scale(defesa->direcao_dash, passo_dash));
            defesa->dash_restante -= passo_dash;
        }
        if (PilotoEh(defesa->id_padrao)) PilotoAtualizar(defesa, jogador, entrada, tempo_delta);
        else if (PadraoMovimentoEh(defesa->id_padrao)) PadraoMovimentoAtualizar(defesa, jogador, entrada, tempo_delta);
        else {
            AtualizarAlma(defesa, jogador, entrada, tempo_delta);
            if (!PilotoEh(defesa->id_padrao) && PadraoEhCaracteristico(defesa->id_padrao)) PadraoAtualizar(defesa, tempo_delta);
        }
        if (!PilotoEh(defesa->id_padrao) && (PadraoEhCaracteristico(defesa->id_padrao) || defesa->id_padrao == PADRAO_GRAVIDADE_INVERSAO || defesa->id_padrao == PADRAO_SOMBRA_FLORACAO)) {
            AtualizarPadraoEspecial(defesa, jogador, tempo_delta);
        }
        while (!PadraoEhCaracteristico(defesa->id_padrao) && defesa->id_padrao != PADRAO_GRAVIDADE_INVERSAO && defesa->id_padrao != PADRAO_SOMBRA_FLORACAO &&
               defesa->temporizador_criacao >= defesa->padrao.intervalo_criacao) {
            defesa->temporizador_criacao -= defesa->padrao.intervalo_criacao;
            CriarDisparoPadrao(defesa);
        }
        AtualizarProjeteis(defesa, jogador, tempo_delta);

        if (jogador->hp == 0) {
            LimparProjeteis(defesa);
            PerigosLimpar(&defesa->perigos);
            defesa->fase = DEFESA_FINALIZADA;
            return DEFESA_JOGADOR_DERROTADO;
        }
        if (defesa->temporizador_padrao >= defesa->padrao.duracao) {
            LimparProjeteis(defesa);
            PerigosLimpar(&defesa->perigos);
            defesa->fase = DEFESA_SAINDO;
            defesa->temporizador_fase = 0.0f;
        }
    } else if (defesa->fase == DEFESA_SAINDO) {
        const float progresso = Clamp(defesa->temporizador_fase / 0.25f, 0.0f, 1.0f);
        defesa->caixa = InterpolarRetangulo(defesa->caixa_alvo, defesa->caixa_inicial, progresso);
        if (progresso >= 1.0f) {
            defesa->fase = DEFESA_FINALIZADA;
            return DEFESA_CONCLUIDA;
        }
    }

    return DEFESA_EM_ANDAMENTO;
}

ResultadoDefesa DefesaAtualizarComEntrada(EstadoDefesa *d, AtributosJogador *p, float tempo_delta, EntradaDefesa entrada)
{

    /* Preserve legacy cycle timing when no dash is travelling. All dash segments
       and pilot physics use short steps; collision still sweeps the full path. */
    if (!PilotoEh(d->id_padrao) && !PadraoMovimentoEh(d->id_padrao) &&
        !entrada.dash_pressionado && d->dash_restante <= 0)
        return PassoDefesa(d,p,tempo_delta,entrada);
    /* Passos fisicos curtos mantem saltos e portoes finos consistentes em 30/60/144 FPS. */
    ResultadoDefesa resultado = DEFESA_EM_ANDAMENTO;
    while (tempo_delta > .000001f) {
        float passo = fminf(tempo_delta, BALANCEAMENTO_PASSO_FISICA);
        resultado = PassoDefesa(d, p, passo, entrada);
        entrada.salto_pressionado = false;
        entrada.dash_pressionado = false;
        tempo_delta -= passo;
        if (resultado != DEFESA_EM_ANDAMENTO) break;
    }
    return resultado;
}

ResultadoDefesa DefesaAtualizar(EstadoDefesa *d, AtributosJogador *p, float tempo_delta)
{
    EntradaDefesa entrada = {
        .movimento = {
            (float)entrada_direita_mantida() - (float)entrada_esquerda_mantida(),
            (float)entrada_baixo_mantida() - (float)entrada_cima_mantida()
        },
        .salto_pressionado = IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP),
        .dash_pressionado = IsKeyPressed(KEY_LEFT_SHIFT) || IsKeyPressed(KEY_RIGHT_SHIFT)
    };
    return DefesaAtualizarComEntrada(d, p, tempo_delta, entrada);
}

void DefesaDesenhar(const EstadoDefesa *defesa, bool desenhar_efeitos, const SistemaSprites *sprites)
{
    if (sprite_obter(sprites, SPRITE_ARENA_FUNDO) != NULL) {
        SpriteDesenharAjustado(sprites, SPRITE_ARENA_FUNDO, defesa->caixa, WHITE);
    } else {
        DrawRectangleRec(defesa->caixa, BLACK);
    }
    if (sprite_obter(sprites, SPRITE_ARENA_BORDA) != NULL) {
        SpriteDesenharAjustado(sprites, SPRITE_ARENA_BORDA, defesa->caixa, WHITE);
    } else {
        DrawRectangleLinesEx(defesa->caixa, 4.0f, RAYWHITE);
    }

    if (defesa->fase == DEFESA_ATIVA) {
        if (PilotoEh(defesa->id_padrao)) PilotoDesenhar(defesa, sprites);
        else if (PadraoEhCaracteristico(defesa->id_padrao)) PadraoDesenhar(defesa, sprites);
        else PerigosDesenhar(&defesa->perigos, !desenhar_efeitos, sprites, sprites->depurar_limites);
        if (defesa->temporizador_padrao < 0.40f && defesa->padrao.raio >= 6.0f) {
            const Vector2 aviso = {defesa->caixa.x + defesa->caixa.width * 0.5f,
                                     defesa->caixa.y + defesa->caixa.height * 0.5f};
            DrawCircleLines((int)aviso.x, (int)aviso.y, 18, (Color){245, 90, 82, 255});
            InterfaceDesenharTexto("!", (int)aviso.x - 3, (int)aviso.y - 10, 18, (Color){245, 90, 82, 255});
            if (sprite_obter(sprites, SPRITE_AVISO_PERIGO) != NULL) {
                SpriteDesenharCentralizado(sprites, SPRITE_AVISO_PERIGO, aviso, WHITE,
                                   defesa->temporizador_padrao, sprites->depurar_limites);
            }
        }
        for (int indice_projetil = 0; indice_projetil < MAXIMO_PROJETEIS; indice_projetil++) {
            const Projetil *projetil = &defesa->projeteis[indice_projetil];
            if (!projetil->ativo) continue;
            const IdSprite pattern_sprite = SpriteIdPadraoProjetil(defesa->id_padrao);
            const IdSprite id_sprite = sprite_obter(sprites, pattern_sprite) != NULL ? pattern_sprite :
                                       SpriteIdProjetil(projetil->tipo);
            if (sprite_obter(sprites, id_sprite) != NULL) {
                const float angulo = defesa->padrao.rotacionar ?
                    atan2f(projetil->velocidade.y, projetil->velocidade.x) * 57.2958f : 0.0f;
                SpriteDesenharCentralizadoRotacionado(sprites, id_sprite, projetil->posicao, WHITE,
                                          projetil->tempo_vida, angulo, defesa->padrao.escala_visual,
                                          sprites->depurar_limites);
                DrawCircleLines((int)projetil->posicao.x, (int)projetil->posicao.y,
                                projetil->raio + 1.0f, Fade(cor_tipo(projetil->tipo), 0.65f));
            } else {
                DrawCircleV(projetil->posicao, projetil->raio, cor_tipo(projetil->tipo));
            }
            if (defesa->padrao.rastro && desenhar_efeitos) {
                DrawCircleV((Vector2){projetil->posicao.x - projetil->velocidade.x * 0.05f,
                                      projetil->posicao.y - projetil->velocidade.y * 0.05f},
                            projetil->raio * 0.55f, Fade(cor_tipo(projetil->tipo), 0.35f));
            }
        }

        if (defesa->temporizador_popup_dano > 0.0f) {
            const int font_size = defesa->multiplicador_ultimo_dano >= 2.0f ? 25 : 19;
            const Color cor = defesa->multiplicador_ultimo_dano >= 2.0f ? RED : MAROON;
            InterfaceDesenharTexto(TextFormat("-%d", defesa->ultimo_dano), (int)defesa->posicao_alma.x + 9,
                     (int)defesa->posicao_alma.y - 18, font_size, cor);
        }
        if (desenhar_efeitos && defesa->temporizador_popup_raspar > 0.0f) {
            InterfaceDesenharTexto("RASPOU", (int)defesa->posicao_alma.x + 10,
                     (int)defesa->posicao_alma.y + 9, 10, (Color){125, 220, 205, 255});
        }

        if (desenhar_efeitos) {
            for (int indice = 5; indice > 0; indice--) {
                const unsigned char alpha = (unsigned char)(indice * 18);
                DrawCircleV(defesa->rastro_alma[indice], 3.0f, (Color){231, 87, 112, alpha});
            }
        }
        if (defesa->temporizador_invencibilidade <= 0.0f || ((int)(defesa->temporizador_invencibilidade * 14.0f) % 2 == 0)) {
            const Vector2 alma = defesa->posicao_alma;
            const Color heart = {231, 87, 112, 255};
            if (sprite_obter(sprites, SPRITE_ALMA) != NULL) {
                SpriteDesenharCentralizado(sprites, SPRITE_ALMA, alma, heart, defesa->temporizador_padrao,
                                   sprites->depurar_limites);
            } else {
                DrawCircle((int)alma.x - 3, (int)alma.y - 2, 4, heart);
                DrawCircle((int)alma.x + 3, (int)alma.y - 2, 4, heart);
                DrawTriangle((Vector2){alma.x - 7, alma.y}, (Vector2){alma.x + 7, alma.y},
                             (Vector2){alma.x, alma.y + 8}, heart);
            }
        }
        if (desenhar_efeitos && defesa->temporizador_popup_dano > 0.0f) {
            const float raio = 9.0f + (0.75f - defesa->temporizador_popup_dano) * 18.0f;
            DrawCircleLines((int)defesa->posicao_alma.x, (int)defesa->posicao_alma.y,
                            raio, Fade(RED, 0.75f));
            if (sprite_obter(sprites, SPRITE_EFEITO_ACERTO_ALMA) != NULL) {
                SpriteDesenharCentralizado(sprites, SPRITE_EFEITO_ACERTO_ALMA, defesa->posicao_alma, WHITE,
                                   defesa->temporizador_padrao, sprites->depurar_limites);
            }
        }
    }
}


int DefesaAplicarDano(EstadoDefesa *d, AtributosJogador *j, int base, Tipo tipo)
{
    int dano = d->depurar_invencivel ? 0 : DanoAplicar(&d->orcamento, j, base, tipo);
    if (dano > 0) {
        d->ultimo_dano = dano; d->multiplicador_ultimo_dano = JogadorEfetividadeDanoRecebido(j,tipo);
        d->temporizador_popup_dano = .75f;
        d->temporizador_invencibilidade = d->orcamento.invulnerabilidade;
    }
    return dano;
}
