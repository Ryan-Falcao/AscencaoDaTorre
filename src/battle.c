#include "battle.h"
#include "input.h"
#include "patterns.h"
#include "raymath.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

static bool ConfirmacaoPressionada(void)
{
    return entrada_confirmar_pressionada();
}

static Rectangle ObterCaixaTexto(void)
{
    return (Rectangle){42.0f, 228.0f, 556.0f, 94.0f};
}

static Rectangle ObterBotaoPrincipal(int indice_botao)
{
    return (Rectangle){38.0f + indice_botao * 143.0f, 398.0f, 132.0f, 40.0f};
}

static Rectangle ObterBotaoAcao(int indice_opcao)
{
    return (Rectangle){54.0f, 237.0f + indice_opcao * 21.0f, 532.0f, 20.0f};
}

static Rectangle ObterBotaoItem(int indice_opcao)
{
    return (Rectangle){54.0f + (indice_opcao / 4) * 268.0f,
                       237.0f + (indice_opcao % 4) * 21.0f, 260.0f, 20.0f};
}

static void MostrarMensagem(Batalha *batalha, TextoDigitado *dialogo, const char *mensagem,
                        FaseBatalha proxima_fase);

static float EfetividadeContraInimigo(Tipo tipo_ataque, const Inimigo *inimigo)
{
    float multiplicador = efetividade_tipo(tipo_ataque, inimigo->tipo);

    if (inimigo->possui_tipo2) multiplicador *= efetividade_tipo(tipo_ataque, inimigo->tipo2);
    return multiplicador;
}

static void ResolverAtaqueJogador(Batalha *batalha, AtributosJogador *jogador, Inimigo *inimigo,
                                TextoDigitado *dialogo, ResultadoQte resultado_qte, EstadoAudio *audio)
{
    const Ataque *ataque = &jogador->ataques[batalha->indice_ataque_qte];
    float multiplicador_tipo = EfetividadeContraInimigo(ataque->tipo, inimigo);
    const float multiplicador_buff = JogadorMultiplicadorBuffAtaque(jogador);
    if (inimigo->status == STATUS_MOLHADO && (ataque->tipo == TIPO_PLANTA || ataque->tipo == TIPO_LUZ)) {
        multiplicador_tipo *= 1.25f;
    }
    const float multiplicador_total = multiplicador_tipo * qte_multiplicador(resultado_qte) * multiplicador_buff;
    const int dano = (int)ceilf((float)(ataque->dano_base + jogador->bonus_dano_permanente) *
                                  ataque->golpes * multiplicador_total);
    const bool era_poupavel = inimigo->poupavel;
    const bool estava_segunda_fase = inimigo->segunda_fase;
    char mensagem[DIALOGO_CAPACIDADE];

    InimigoReceberDano(inimigo, dano, multiplicador_tipo);
    if (ataque->status != STATUS_NENHUM && resultado_qte != QTE_ERRO) {
        const bool apply_status = !ataque->critico_no_perfeito || resultado_qte == QTE_PERFEITO ||
                                  GetRandomValue(0, 99) < 45;
        if (apply_status) InimigoAplicarStatus(inimigo, ataque->status, ataque->duracao_status);
    }
    EfeitoIniciar(&batalha->efeito_ataque, ataque, resultado_qte);
    batalha->dano_flutuante = dano;
    batalha->temporizador_dano_flutuante = 0.85f;
    batalha->ultima_efetividade = multiplicador_tipo;
    if (resultado_qte == QTE_PERFEITO) batalha->temporizador_tremor_tela = 0.18f;
    if (resultado_qte == QTE_PERFEITO || multiplicador_tipo >= 2.0f) batalha->temporizador_pausa_impacto = 0.055f;
    for (int indice = 0; indice < MAXIMO_BATALHA_PARTICULAS; indice++) {
        ParticulaBatalha *particle = &batalha->particulas[indice];
        if (!particle->ativo) {
            particle->ativo = true;
            particle->posicao = (Vector2){320.0f, 132.0f};
            particle->velocidade = (Vector2){(float)GetRandomValue(-70, 70), (float)GetRandomValue(-95, -20)};
            particle->cor = cor_tipo(ataque->tipo);
            particle->tempo_vida = 0.45f;
        }
    }
    JogadorConsumirBuffAtaque(jogador);
    JogadorUsarAtaque(jogador, ataque);
    AudioTocar(audio, AUDIO_DANO_CAUSADO);
    if (multiplicador_tipo >= 2.0f) AudioTocar(audio, AUDIO_SUPER_EFETIVO);
    else if (multiplicador_tipo <= 0.5f) AudioTocar(audio, AUDIO_NAO_EFETIVO);

    if (multiplicador_tipo >= 2.0f) {
        snprintf(mensagem, sizeof(mensagem), "%s %s! %d dano. Super efetivo!", qte_nome_resultado(resultado_qte),
                 ataque->nome, dano);
    } else if (multiplicador_tipo <= 0.5f) {
        snprintf(mensagem, sizeof(mensagem), "%s %s! %d dano. Nao foi muito efetivo...",
                 qte_nome_resultado(resultado_qte), ataque->nome, dano);
    } else {
        snprintf(mensagem, sizeof(mensagem), "%s %s! %d dano.", qte_nome_resultado(resultado_qte),
                 ataque->nome, dano);
    }
    if (!era_poupavel && inimigo->poupavel && inimigo->hp > 0) {
        strncat(mensagem, " O inimigo esta exausto.", sizeof(mensagem) - strlen(mensagem) - 1);
    }
    if (!estava_segunda_fase && inimigo->segunda_fase && inimigo->hp > 0) {
        strncat(mensagem, " O chefe mudou de fase!", sizeof(mensagem) - strlen(mensagem) - 1);
    }
    MostrarMensagem(batalha, dialogo, mensagem, inimigo->hp == 0 ? BATALHA_VITORIA : BATALHA_INIMIGO_TURNO);
}

static void EntrarMenuJogador(Batalha *batalha, TextoDigitado *dialogo)
{
    batalha->fase = BATALHA_JOGADOR_MENU;
    batalha->menu_acao = ACAO_NENHUM;
    batalha->selecao_acao = 0;
    InterfaceDefinirTexto(dialogo, "O que voce fara?");
}

static void MostrarMensagem(Batalha *batalha, TextoDigitado *dialogo, const char *mensagem,
                        FaseBatalha proxima_fase)
{
    batalha->fase = BATALHA_JOGADOR_ACAO;
    batalha->menu_acao = ACAO_MENSAGEM;
    batalha->proxima_fase = proxima_fase;
    InterfaceDefinirTexto(dialogo, mensagem);
}

static void IniciarTurnoInimigo(Batalha *batalha, Inimigo *inimigo, TextoDigitado *dialogo)
{
    IdPadraoProjetil id_padrao = PADRAO_CHUVA;
    int burn_damage = 0;
    const EfeitoStatus status_ativo = inimigo->status;
    float multiplicador_velocidade = 1.0f;
    float multiplicador_dano = inimigo->multiplicador_dano_projetil;

    if (InimigoAvancarStatus(inimigo, &burn_damage) && inimigo->hp == 0) {
        MostrarMensagem(batalha, dialogo, "As chamas consumiram o inimigo.", BATALHA_VITORIA);
        return;
    }
    if (status_ativo == STATUS_ATORDOAR) {
        MostrarMensagem(batalha, dialogo, "O inimigo ficou atordoado e perdeu o turno!", BATALHA_JOGADOR_MENU);
        return;
    }
    if (status_ativo == STATUS_ENRAIZAR) multiplicador_velocidade = 0.70f;
    if (status_ativo == STATUS_CEGAR) multiplicador_dano *= 0.75f;

    if (inimigo->total_padroes > 0) {
        id_padrao = inimigo->ids_padroes[inimigo->indice_proximo_padrao % inimigo->total_padroes];
        inimigo->indice_proximo_padrao++;
    }

    batalha->fase = BATALHA_INIMIGO_TURNO;
    batalha->total_turnos++;
    DefesaIniciar(&batalha->defesa, id_padrao, (Rectangle){42.0f, 228.0f, 556.0f, 94.0f},
                 inimigo->agressividade, inimigo->bonus_dano_projetil,
                 multiplicador_dano, multiplicador_velocidade);
    if (inimigo->chefe && inimigo->segunda_fase) {
        if (PadraoEhCaracteristico(id_padrao)) PadraoDefinirSegundaFase(&batalha->defesa);
        DefesaRedimensionarCaixa(&batalha->defesa, PadraoEhCaracteristico(id_padrao) ? 290.0f : 250.0f,
                          PadraoEhCaracteristico(id_padrao) ? 190.0f : 140.0f);
        if (id_padrao == PADRAO_GRAVIDADE_INVERSAO) {
            batalha->defesa.repeticoes_gravidade = 3;
            batalha->defesa.padrao.duracao = 11.0f;
        }
        if (id_padrao == PADRAO_SOMBRA_FLORACAO) batalha->defesa.repeticoes_gravidade = 3;
    }
    if (PadraoEhCaracteristico(id_padrao)) {
        InterfaceDefinirTexto(dialogo, InstrucaoPadrao(id_padrao));
    } else if (inimigo->id == INIMIGO_CHEFE_SINEIRO && id_padrao == PADRAO_GRAVIDADE_INVERSAO) {
        InterfaceDefinirTexto(dialogo, "O chao nao e seu.");
    } else if (inimigo->id == INIMIGO_CHEFE_SINEIRO && id_padrao == PADRAO_SOMBRA_FLORACAO) {
        InterfaceDefinirTexto(dialogo, "A sombra cresce.");
    } else InterfaceDefinirTexto(dialogo, inimigo->agressividade >= 67 ? "Ela avanca furiosa!" :
                        (inimigo->agressividade <= 33 ? "Ela parece calma." : "Ela observa, irritada."));
}

static ResultadoBatalha MontarResultado(const Batalha *batalha, const AtributosJogador *jogador, DesfechoBatalha desfecho,
                               int moedas, int xp)
{
    ResultadoBatalha resultado = {0};
    resultado.desfecho = desfecho;
    resultado.moedas = moedas;
    resultado.xp = xp;
    resultado.vida_perdida = batalha->vida_inicial_jogador - jogador->hp;
    resultado.turnos = batalha->total_turnos;
    resultado.duracao_segundos = batalha->tempo_decorrido;
    for (int indice = 0; indice < 4; indice++) resultado.resultados_qte[indice] = batalha->resultados_qte[indice];
    return resultado;
}

void BatalhaInicializar(Batalha *batalha, TextoDigitado *dialogo, int andar)
{
    memset(batalha, 0, sizeof(*batalha));
    batalha->fase = BATALHA_INTRODUCAO;
    batalha->proxima_fase = BATALHA_JOGADOR_MENU;
    batalha->menu_acao = ACAO_NENHUM;
    batalha->selecao_principal = MENU_ATAQUE;
    batalha->andar = andar;
    batalha->dificuldade_qte = andar;
    batalha->selecao_acao = 0;
    batalha->tempo_decorrido = 0.0f;
    batalha->dano_flutuante = 0;
    batalha->temporizador_dano_flutuante = 0.0f;
    batalha->ultima_efetividade = 1.0f;
    batalha->indice_ataque_qte = 0;
    batalha->temporizador_tremor_tela = 0.0f;
    batalha->temporizador_pausa_impacto = 0.0f;
    batalha->vida_inicial_jogador = 0;
    batalha->vida_inimigo_exibida = -1.0f;
    batalha->rastro_vida_inimigo = -1.0f;
    batalha->vida_jogador_exibida = -1.0f;
    batalha->rastro_vida_jogador = -1.0f;
    batalha->misericordia_exibida = 0.0f;
    batalha->agressividade_exibida = 0.0f;
    batalha->temporizador_mudanca_status = 0.0f;
    batalha->popup_mudanca_misericordia = 0;
    batalha->popup_mudanca_agressividade = 0;
    batalha->vitoria_por_misericordia = false;
    batalha->resultado = (ResultadoBatalha){0};
    DefesaInicializar(&batalha->defesa);
    InterfaceDefinirTexto(dialogo, "Um inimigo bloqueia a subida.");
}

static void AtualizarMenuJogador(Batalha *batalha, AtributosJogador *jogador, Inimigo *inimigo,
                             TextoDigitado *dialogo, Vector2 posicao_mouse,
                             bool mouse_pressionado)
{
    bool confirmado = ConfirmacaoPressionada();

    if (entrada_esquerda_pressionada()) {
        batalha->selecao_principal = (batalha->selecao_principal + 3) % 4;
    }
    if (entrada_direita_pressionada()) {
        batalha->selecao_principal = (batalha->selecao_principal + 1) % 4;
    }
    for (int indice_botao = 0; indice_botao < 4; indice_botao++) {
        if (CheckCollisionPointRec(posicao_mouse, ObterBotaoPrincipal(indice_botao))) {
            batalha->selecao_principal = indice_botao;
            if (mouse_pressionado) confirmado = true;
        }
    }
    if (!confirmado) return;

    switch ((OpcaoMenuPrincipal)batalha->selecao_principal) {
        case MENU_ATAQUE:
            batalha->fase = BATALHA_JOGADOR_ACAO;
            batalha->menu_acao = ACAO_ATAQUES;
            batalha->selecao_acao = 0;
            InterfaceDefinirTexto(dialogo, "Escolha um ataque.");
            break;
        case MENU_ACT:
            batalha->fase = BATALHA_JOGADOR_ACAO;
            batalha->menu_acao = ACAO_ACOES;
            batalha->selecao_acao = 0;
            InterfaceDefinirTexto(dialogo, "Como voce vai agir?");
            break;
        case MENU_ITEM:
            batalha->fase = BATALHA_JOGADOR_ACAO;
            batalha->menu_acao = ACAO_ITENS;
            batalha->selecao_acao = 0;
            InterfaceDefinirTexto(dialogo, "Escolha um item.");
            break;
        case MENU_SPARE:
            if (InimigoPodePoupar(inimigo)) {
                batalha->vitoria_por_misericordia = true;
                MostrarMensagem(batalha, dialogo, "Voce poupou o inimigo com gentileza.", BATALHA_VITORIA);
            } else if (inimigo->poupavel) {
                MostrarMensagem(batalha, dialogo, "Ele esta furioso demais para te ouvir.", BATALHA_INIMIGO_TURNO);
            } else {
                MostrarMensagem(batalha, dialogo, "Ainda nao confia em voce...", BATALHA_INIMIGO_TURNO);
            }
            break;
        default:
            (void)jogador;
            break;
    }
}

static void AtualizarListaAcoes(Batalha *batalha, AtributosJogador *jogador, Inimigo *inimigo,
                             TextoDigitado *dialogo, Vector2 posicao_mouse,
                             bool mouse_pressionado, Qte *qte, EstadoAudio *audio)
{
    int option_count = 0;
    bool confirmado = ConfirmacaoPressionada();

    if (batalha->menu_acao == ACAO_ATAQUES) option_count = jogador->total_ataques;
    if (batalha->menu_acao == ACAO_ACOES) option_count = inimigo->total_acoes;
    if (batalha->menu_acao == ACAO_ITENS) option_count = MAXIMO_INVENTARIO_ITENS;

    if (entrada_cima_pressionada()) {
        batalha->selecao_acao = (batalha->selecao_acao + option_count - 1) % option_count;
    }
    if (entrada_baixo_pressionada()) {
        batalha->selecao_acao = (batalha->selecao_acao + 1) % option_count;
    }
    if (entrada_cancelar_pressionada()) {
        EntrarMenuJogador(batalha, dialogo);
        return;
    }
    for (int indice_opcao = 0; indice_opcao < option_count; indice_opcao++) {
        const Rectangle option = batalha->menu_acao == ACAO_ITENS ?
                                 ObterBotaoItem(indice_opcao) : ObterBotaoAcao(indice_opcao);
        if (CheckCollisionPointRec(posicao_mouse, option)) {
            batalha->selecao_acao = indice_opcao;
            if (mouse_pressionado) confirmado = true;
        }
    }
    if (!confirmado) return;

    if (batalha->menu_acao == ACAO_ATAQUES) {
        const Ataque *ataque = &jogador->ataques[batalha->selecao_acao];
        char motivo[32] = "";

        if (!JogadorAtaqueDisponivel(jogador, ataque, motivo, (int)sizeof(motivo))) {
            InterfaceDefinirTexto(dialogo, motivo);
            batalha->menu_acao = ACAO_MENSAGEM;
            batalha->proxima_fase = BATALHA_JOGADOR_ACAO;
            return;
        }

        batalha->indice_ataque_qte = batalha->selecao_acao;
        batalha->fase = BATALHA_JOGADOR_QTE;
        qte_iniciar(qte, ataque->tipo_qte, batalha->dificuldade_qte);
    } else if (batalha->menu_acao == ACAO_ACOES) {
        const AcaoInimigo *acao = &inimigo->acoes[batalha->selecao_acao];
        char mensagem[DIALOGO_CAPACIDADE];

        if (!InimigoAplicarAcao(inimigo, batalha->selecao_acao, mensagem, sizeof(mensagem))) {
            MostrarMensagem(batalha, dialogo, mensagem, BATALHA_JOGADOR_MENU);
            return;
        }
        if (acao->efeito == INIMIGO_ACAO_ANALISAR) {
            if (inimigo->possui_tipo2) {
                snprintf(mensagem, sizeof(mensagem), "Tipos revelados: %s e %s. Novas acoes de AGIR foram liberadas.",
                         nome_tipo(inimigo->tipo), nome_tipo(inimigo->tipo2));
            } else {
                snprintf(mensagem, sizeof(mensagem), "Tipo revelado: %s. Novas acoes de AGIR foram liberadas.",
                         nome_tipo(inimigo->tipo));
            }
        } else {
            batalha->popup_mudanca_misericordia = inimigo->ultima_variacao_misericordia;
            batalha->popup_mudanca_agressividade = inimigo->ultima_variacao_agressividade;
            batalha->temporizador_mudanca_status = 0.8f;
        }
        MostrarMensagem(batalha, dialogo, mensagem, BATALHA_INIMIGO_TURNO);
    } else if (batalha->menu_acao == ACAO_ITENS) {
        const DefinicaoItem *item;
        int valor_aplicado;
        char mensagem[DIALOGO_CAPACIDADE];

        if (!JogadorUsarItem(jogador, batalha->selecao_acao, &item, &valor_aplicado)) {
            MostrarMensagem(batalha, dialogo, "Nao sobrou nenhum desse item.", BATALHA_JOGADOR_MENU);
            return;
        }
        if (item->efeito == ITEM_EFEITO_DEFESA_TIPO) {
            snprintf(mensagem, sizeof(mensagem), "Voce usou %s. Defesa agora: %s.", item->nome,
                     nome_tipo(jogador->tipo_defesa));
        } else if (item->efeito == ITEM_EFEITO_CURAR) {
            AudioTocar(audio, AUDIO_CURAR);
            snprintf(mensagem, sizeof(mensagem), "Voce usou %s e recuperou %d HP.", item->nome,
                     valor_aplicado);
        } else if (item->efeito == ITEM_EFEITO_DANO_BUFF) {
            snprintf(mensagem, sizeof(mensagem), "%s: dano +%d%% por %d ataque(s).", item->nome,
                     item->valor, item->duracao);
        } else {
            const bool era_poupavel = inimigo->poupavel;
            InimigoReceberDano(inimigo, valor_aplicado, 1.0f);
            batalha->dano_flutuante = valor_aplicado;
            batalha->temporizador_dano_flutuante = 0.85f;
            batalha->ultima_efetividade = 1.0f;
            snprintf(mensagem, sizeof(mensagem), "%s acertou e causou %d dano.", item->nome, valor_aplicado);
            if (!era_poupavel && inimigo->poupavel && inimigo->hp > 0) {
                strncat(mensagem, " O inimigo esta exausto.", sizeof(mensagem) - strlen(mensagem) - 1);
            }
        }
        MostrarMensagem(batalha, dialogo, mensagem, inimigo->hp == 0 ? BATALHA_VITORIA : BATALHA_INIMIGO_TURNO);
    }
}

ResultadoBatalha BatalhaAtualizar(Batalha *batalha, AtributosJogador *jogador, Inimigo *inimigo,
                          TextoDigitado *dialogo, Vector2 posicao_mouse,
                          bool mouse_pressionado, Qte *qte, EstadoAudio *audio)
{
    const float tempo_delta = GetFrameTime();

    batalha->tempo_decorrido += tempo_delta;
    EfeitoAtualizar(&batalha->efeito_ataque, tempo_delta);
    if (batalha->vida_inicial_jogador == 0) batalha->vida_inicial_jogador = jogador->hp;
    if (inimigo->temporizador_flash_acerto > 0.0f) inimigo->temporizador_flash_acerto -= tempo_delta;
    if (batalha->temporizador_dano_flutuante > 0.0f) batalha->temporizador_dano_flutuante -= tempo_delta;
    if (batalha->temporizador_tremor_tela > 0.0f) batalha->temporizador_tremor_tela -= tempo_delta;
    if (batalha->vida_inimigo_exibida < 0.0f) batalha->vida_inimigo_exibida = (float)inimigo->hp;
    if (batalha->rastro_vida_inimigo < 0.0f) batalha->rastro_vida_inimigo = (float)inimigo->hp;
    if (batalha->vida_jogador_exibida < 0.0f) batalha->vida_jogador_exibida = (float)jogador->hp;
    if (batalha->rastro_vida_jogador < 0.0f) batalha->rastro_vida_jogador = (float)jogador->hp;
    batalha->vida_inimigo_exibida = InterfaceAproximar(batalha->vida_inimigo_exibida, (float)inimigo->hp, 95.0f, tempo_delta);
    batalha->vida_jogador_exibida = InterfaceAproximar(batalha->vida_jogador_exibida, (float)jogador->hp, 110.0f, tempo_delta);
    batalha->rastro_vida_inimigo = InterfaceAproximar(batalha->rastro_vida_inimigo, batalha->vida_inimigo_exibida, 35.0f, tempo_delta);
    batalha->rastro_vida_jogador = InterfaceAproximar(batalha->rastro_vida_jogador, batalha->vida_jogador_exibida, 42.0f, tempo_delta);
    for (int indice = 0; indice < MAXIMO_BATALHA_PARTICULAS; indice++) {
        ParticulaBatalha *particle = &batalha->particulas[indice];
        if (!particle->ativo) continue;
        particle->posicao = Vector2Add(particle->posicao, Vector2Scale(particle->velocidade, tempo_delta));
        particle->velocidade.y += 120.0f * tempo_delta;
        particle->tempo_vida -= tempo_delta;
        if (particle->tempo_vida <= 0.0f) particle->ativo = false;
    }
    batalha->misericordia_exibida = InterfaceAproximar(batalha->misericordia_exibida, (float)inimigo->misericordia, 145.0f, tempo_delta);
    batalha->agressividade_exibida = InterfaceAproximar(batalha->agressividade_exibida,
                                               (float)inimigo->agressividade, 145.0f, tempo_delta);
    if (batalha->temporizador_mudanca_status > 0.0f) batalha->temporizador_mudanca_status -= tempo_delta;
    InterfaceAtualizarTexto(dialogo, tempo_delta);

    if (batalha->fase == BATALHA_INTRODUCAO) {
        const bool confirm = ConfirmacaoPressionada() ||
                             (mouse_pressionado && CheckCollisionPointRec(posicao_mouse, ObterCaixaTexto()));
        if (confirm && InterfaceConfirmarTexto(dialogo)) EntrarMenuJogador(batalha, dialogo);
    } else if (batalha->fase == BATALHA_JOGADOR_MENU) {
        AtualizarMenuJogador(batalha, jogador, inimigo, dialogo, posicao_mouse, mouse_pressionado);
    } else if (batalha->fase == BATALHA_JOGADOR_ACAO) {
        if (batalha->menu_acao == ACAO_MENSAGEM) {
            const bool confirm = ConfirmacaoPressionada() ||
                                 (mouse_pressionado && CheckCollisionPointRec(posicao_mouse, ObterCaixaTexto()));
            const bool retry_attack = batalha->proxima_fase == BATALHA_JOGADOR_ACAO;
            if ((retry_attack && entrada_cancelar_pressionada()) || (confirm && InterfaceConfirmarTexto(dialogo))) {
                if (batalha->proxima_fase == BATALHA_INIMIGO_TURNO) {
                    IniciarTurnoInimigo(batalha, inimigo, dialogo);
                } else if (batalha->proxima_fase == BATALHA_JOGADOR_MENU) {
                    EntrarMenuJogador(batalha, dialogo);
                } else if (retry_attack) {
                    batalha->menu_acao = ACAO_ATAQUES;
                    InterfaceDefinirTexto(dialogo, "Escolha outro ataque. X: voltar.");
                } else {
                    batalha->fase = batalha->proxima_fase;
                }
            }
        } else {
            AtualizarListaAcoes(batalha, jogador, inimigo, dialogo, posicao_mouse, mouse_pressionado, qte, audio);
        }
    } else if (batalha->fase == BATALHA_JOGADOR_QTE) {
        qte_atualizar(qte, tempo_delta);
        if (qte_terminou(qte)) {
            batalha->resultados_qte[qte->resultado]++;
            AudioTocar(audio, qte->resultado == QTE_PERFEITO ? AUDIO_QTE_PERFEITO :
                      (qte->resultado == QTE_ERRO ? AUDIO_QTE_ERRO : AUDIO_QTE_BATIDA));
            ResolverAtaqueJogador(batalha, jogador, inimigo, dialogo, qte->resultado, audio);
        }
    } else if (batalha->fase == BATALHA_INIMIGO_TURNO) {
        const int vida_antes = jogador->hp;
        const ResultadoDefesa resultado = DefesaAtualizar(&batalha->defesa, jogador, tempo_delta);

        if (jogador->hp < vida_antes) AudioTocar(audio, AUDIO_DANO_RECEBIDO);

        if (resultado == DEFESA_JOGADOR_DERROTADO) {
            MostrarMensagem(batalha, dialogo, "Sua alma se desfez no escuro.", BATALHA_DERROTA);
        } else if (resultado == DEFESA_CONCLUIDA) {
            InimigoAvancarTurno(inimigo);
            JogadorAvancarRecargasAtaques(jogador);
            MostrarMensagem(batalha, dialogo, "O turno da mariposa terminou.", BATALHA_JOGADOR_MENU);
        }
    } else if (batalha->fase == BATALHA_VITORIA) {
        const bool confirm = ConfirmacaoPressionada() ||
                             (mouse_pressionado && CheckCollisionPointRec(posicao_mouse, ObterCaixaTexto()));
        if (confirm && InterfaceConfirmarTexto(dialogo)) {
            return batalha->vitoria_por_misericordia ?
                   MontarResultado(batalha, jogador, BATALHA_OUTCOME_MISERICORDIA, 10, 15) :
                   MontarResultado(batalha, jogador, BATALHA_OUTCOME_DANO, 5, 8);
        }
    } else if (batalha->fase == BATALHA_DERROTA) {
        const bool confirm = ConfirmacaoPressionada() ||
                             (mouse_pressionado && CheckCollisionPointRec(posicao_mouse, ObterCaixaTexto()));
        if (confirm && InterfaceConfirmarTexto(dialogo)) return MontarResultado(batalha, jogador, BATALHA_OUTCOME_DERROTA, 0, 0);
    }

    return (ResultadoBatalha){0};
}

static void DesenharInimigo(const Inimigo *inimigo, float tempo, float temporizador_tremor, bool tremor_linha,
                      const SistemaSprites *sprites)
{
    const float y = 105.0f + sinf(tempo * 2.0f) * 4.0f;
    const int shake_x = temporizador_tremor > 0.0f ? (int)(sinf(tempo * 100.0f) * 3.0f) :
                        (tremor_linha ? ((int)(tempo * 8.0f) % 2) : 0);
    const Color body = inimigo->temporizador_flash_acerto > 0.0f ? RAYWHITE : (Color){48, 43, 60, 255};
    const Color accent = cor_tipo(inimigo->tipo);
    IdSprite id_sprite = SpriteIdInimigo(inimigo->id, inimigo->segunda_fase);

    if (sprite_obter(sprites, id_sprite) == NULL && inimigo->chefe && inimigo->segunda_fase) {
        id_sprite = SpriteIdInimigo(inimigo->id, false);
    }

    if (sprite_obter(sprites, id_sprite) != NULL) {
        const Color tingimento = inimigo->temporizador_flash_acerto > 0.0f && inimigo->forca_flash_acerto >= 2.0f ? RED :
                           (inimigo->temporizador_flash_acerto > 0.0f ? RAYWHITE : WHITE);
        const float outline_radius = inimigo->chefe ? 76.0f : 49.0f;

        /* O aro fino do tipo preserva a pista visual mesmo com arte personalizada. */
        DrawCircleLines((int)(320.0f + shake_x), (int)(y + 32.0f), outline_radius,
                        cor_tipo(inimigo->tipo));
        SpriteDesenharCentralizado(sprites, id_sprite, (Vector2){320.0f + shake_x, y + 32.0f}, tingimento, tempo,
                           sprites->depurar_limites);
        return;
    }

    if (inimigo->chefe) {
        const int x = 320 + shake_x;
        const Color boss_body = inimigo->temporizador_flash_acerto > 0.0f ? RAYWHITE : (Color){35, 37, 43, 255};

        if (inimigo->id == INIMIGO_CHEFE_SINEIRO) {
            DrawCircle(x, (int)(y + 34), 42, boss_body);
            DrawRectangle(x - 27, (int)(y + 34), 54, 38, boss_body);
            DrawCircleLines(x, (int)(y + 34), 42, BLACK);
            DrawRectangleLines(x - 27, (int)(y + 34), 54, 38, BLACK);
            DrawCircle(x, (int)(y + 61), 8, accent);
        } else {
            DrawRectangle(x - 49, (int)(y - 5), 98, 76, boss_body);
            DrawTriangle((Vector2){x - 60, y - 5}, (Vector2){x + 60, y - 5},
                         (Vector2){x, y - 55}, boss_body);
            DrawRectangleLines(x - 49, (int)(y - 5), 98, 76, BLACK);
            DrawLine(x - 60, (int)(y - 5), x, (int)(y - 55), BLACK);
            DrawLine(x, (int)(y - 55), x + 60, (int)(y - 5), BLACK);
            DrawRectangle(x - 9, (int)(y + 25), 18, 25, accent);
        }
        if (inimigo->segunda_fase) DrawCircleLines(x, (int)(y + 30), 50, accent);
        return;
    }

    DrawEllipse(270 + shake_x, (int)(y + 25.0f), 47, 25, accent);
    DrawEllipse(323 + shake_x, (int)(y + 25.0f), 47, 25, accent);
    DrawEllipseLines(270 + shake_x, (int)(y + 25.0f), 47, 25, BLACK);
    DrawEllipseLines(323 + shake_x, (int)(y + 25.0f), 47, 25, BLACK);
    DrawCircle(320 + shake_x, (int)(y + 32.0f), 30, body);
    DrawCircleLines(320 + shake_x, (int)(y + 32.0f), 30, BLACK);
    if (inimigo->possui_tipo2) DrawCircleLines(320 + shake_x, (int)(y + 32.0f), 26, cor_tipo(inimigo->tipo2));
    if (inimigo->temporizador_flash_acerto > 0.0f) {
        DrawCircleLines(320 + shake_x, (int)(y + 32.0f), 34,
                        inimigo->forca_flash_acerto >= 2.0f ? RED : RAYWHITE);
    }
    DrawCircle(309 + shake_x, (int)(y + 27.0f), 4, RAYWHITE);
    DrawCircle(331 + shake_x, (int)(y + 27.0f), 4, RAYWHITE);
    DrawCircle(309 + shake_x, (int)(y + 27.0f), 2, BLACK);
    DrawCircle(331 + shake_x, (int)(y + 27.0f), 2, BLACK);
}

static void DesenharBarraVida(Rectangle barra, float atual, float rastro, int maximo, Color preenchimento)
{
    const float ratio = maximo > 0 ? atual / maximo : 0.0f;
    const float trail_ratio = maximo > 0 ? rastro / maximo : 0.0f;

    DrawRectangleRec(barra, BLACK);
    DrawRectangle((int)barra.x + 2, (int)barra.y + 2, (int)barra.width - 4, (int)barra.height - 4,
                  (Color){211, 214, 210, 255});
    DrawRectangle((int)barra.x + 2, (int)barra.y + 2, (int)((barra.width - 4) * trail_ratio),
                  (int)barra.height - 4, (Color){211, 85, 82, 255});
    DrawRectangle((int)barra.x + 2, (int)barra.y + 2, (int)((barra.width - 4) * ratio),
                  (int)barra.height - 4, preenchimento);
}

static void DesenharIconeLupa(Vector2 centro, Color cor)
{
    DrawCircleLines((int)centro.x - 2, (int)centro.y - 2, 5.0f, cor);
    DrawLineEx((Vector2){centro.x + 2.0f, centro.y + 2.0f},
               (Vector2){centro.x + 7.0f, centro.y + 7.0f}, 2.5f, cor);
}

static void DesenharIconeBalao(Vector2 centro, Color cor)
{
    Rectangle balao = {centro.x - 7.0f, centro.y - 6.0f, 14.0f, 11.0f};
    DrawRectangleLinesEx(balao, 2.0f, cor);
    DrawTriangle((Vector2){centro.x - 3.0f, centro.y + 5.0f},
                 (Vector2){centro.x + 1.0f, centro.y + 5.0f},
                 (Vector2){centro.x - 3.0f, centro.y + 9.0f}, cor);
}

static void DesenharIconeAcao(const AcaoInimigo *acao, Vector2 centro, bool habilitada)
{
    const Color cor = !habilitada ? GRAY :
                      (acao->efeito == INIMIGO_ACAO_ANALISAR ? (Color){74, 109, 160, 255} :
                       (acao->variacao_agressividade > 0 ? (Color){196, 62, 62, 255} :
                                                          (Color){190, 145, 28, 255}));

    if (acao->efeito == INIMIGO_ACAO_ANALISAR) DesenharIconeLupa(centro, cor);
    else DesenharIconeBalao(centro, cor);
}

static void DesenharIconeAtaque(Vector2 centro, Color cor)
{
    DrawLineEx((Vector2){centro.x - 6.0f, centro.y + 6.0f},
               (Vector2){centro.x + 5.0f, centro.y - 5.0f}, 3.0f, cor);
    DrawTriangle((Vector2){centro.x + 3.0f, centro.y - 7.0f},
                 (Vector2){centro.x + 8.0f, centro.y - 8.0f},
                 (Vector2){centro.x + 7.0f, centro.y - 3.0f}, cor);
    DrawLineEx((Vector2){centro.x - 7.0f, centro.y + 3.0f},
               (Vector2){centro.x - 3.0f, centro.y + 7.0f}, 2.0f, cor);
}

static void DesenharIconeTipo(Tipo tipo, Vector2 centro, bool habilitado)
{
    const Color cor = habilitado ? cor_tipo(tipo) : GRAY;
    const Color fundo = habilitado ? ColorLerp(RAYWHITE, cor, 0.18f) : (Color){225, 225, 225, 255};

    DrawCircleV(centro, 8.0f, fundo);
    DrawCircleLines((int)centro.x, (int)centro.y, 8.0f, habilitado ? DARKGRAY : GRAY);
    switch (tipo) {
        case TIPO_FOGO:
            DrawTriangle((Vector2){centro.x, centro.y - 6.0f},
                         (Vector2){centro.x - 5.0f, centro.y + 5.0f},
                         (Vector2){centro.x + 5.0f, centro.y + 5.0f}, cor);
            DrawCircle((int)centro.x, (int)centro.y + 2, 3.0f, fundo);
            break;
        case TIPO_AGUA:
            DrawTriangle((Vector2){centro.x, centro.y - 6.0f},
                         (Vector2){centro.x - 5.0f, centro.y + 2.0f},
                         (Vector2){centro.x + 5.0f, centro.y + 2.0f}, cor);
            DrawCircle((int)centro.x, (int)centro.y + 2, 5.0f, cor);
            break;
        case TIPO_PLANTA:
            DrawEllipse((int)centro.x - 1, (int)centro.y - 1, 6.0f, 4.0f, cor);
            DrawLine((int)centro.x - 5, (int)centro.y + 5,
                     (int)centro.x + 4, (int)centro.y - 4, habilitado ? DARKGREEN : GRAY);
            break;
        case TIPO_LUZ:
            DrawCircleV(centro, 3.5f, cor);
            DrawLine((int)centro.x, (int)centro.y - 7, (int)centro.x, (int)centro.y - 5, cor);
            DrawLine((int)centro.x, (int)centro.y + 5, (int)centro.x, (int)centro.y + 7, cor);
            DrawLine((int)centro.x - 7, (int)centro.y, (int)centro.x - 5, (int)centro.y, cor);
            DrawLine((int)centro.x + 5, (int)centro.y, (int)centro.x + 7, (int)centro.y, cor);
            break;
        case TIPO_SOMBRA:
            DrawCircleV(centro, 5.5f, cor);
            DrawCircle((int)centro.x + 3, (int)centro.y - 2, 4.5f, fundo);
            break;
        case TIPO_NEUTRO:
        default:
            DrawPoly(centro, 4, 5.5f, 45.0f, cor);
            DrawPolyLines(centro, 4, 5.5f, 45.0f, DARKGRAY);
            break;
    }
}

static void DesenharIconeMenuPrincipal(int indice, Vector2 centro, Color cor)
{
    if (indice == 0) {
        DesenharIconeAtaque(centro, cor);
    } else if (indice == 1) {
        DesenharIconeBalao(centro, cor);
    } else if (indice == 2) {
        Rectangle bolsa = {centro.x - 7.0f, centro.y - 5.0f, 14.0f, 12.0f};
        DrawRectangleLinesEx(bolsa, 2.0f, cor);
        DrawLine((int)centro.x - 4, (int)centro.y - 7,
                 (int)centro.x + 4, (int)centro.y - 7, cor);
        DrawLine((int)centro.x - 4, (int)centro.y - 7,
                 (int)centro.x - 6, (int)centro.y - 4, cor);
        DrawLine((int)centro.x + 4, (int)centro.y - 7,
                 (int)centro.x + 6, (int)centro.y - 4, cor);
    } else {
        DrawCircle((int)centro.x - 4, (int)centro.y - 3, 5.0f, cor);
        DrawCircle((int)centro.x + 4, (int)centro.y - 3, 5.0f, cor);
        DrawTriangle((Vector2){centro.x - 9.0f, centro.y - 1.0f},
                     (Vector2){centro.x + 9.0f, centro.y - 1.0f},
                     (Vector2){centro.x, centro.y + 9.0f}, cor);
    }
}

static void DesenharListaAcoes(const Batalha *batalha, const AtributosJogador *jogador, const Inimigo *inimigo,
                           const SistemaSprites *sprites)
{
    const int option_count = batalha->menu_acao == ACAO_ATAQUES ? jogador->total_ataques :
                             (batalha->menu_acao == ACAO_ACOES ? inimigo->total_acoes :
                                                                  MAXIMO_INVENTARIO_ITENS);

    for (int linha = 0; linha < option_count; linha++) {
        const Rectangle area = batalha->menu_acao == ACAO_ITENS ? ObterBotaoItem(linha) :
                               ObterBotaoAcao(linha);
        const int y = (int)area.y + 3;
        const bool selecionado = batalha->selecao_acao == linha;

        if (batalha->menu_acao == ACAO_ATAQUES) {
            const Ataque *ataque = &jogador->ataques[linha];
            char motivo[32] = "";
            const bool disponivel = JogadorAtaqueDisponivel(jogador, ataque, motivo, (int)sizeof(motivo));
            InterfaceDesenharLinhaOpcao(area, selecionado, disponivel, (Color){225, 95, 122, 255});
            if (sprite_obter(sprites, SpriteIdAtaque(ataque->id)) != NULL) {
                SpriteDesenharCentralizadoEscalado(sprites, SpriteIdAtaque(ataque->id),
                    (Vector2){area.x + 19.0f, area.y + 10.0f}, WHITE, 0.0f, 0.70f,
                    sprites->depurar_limites);
            } else {
                DesenharIconeAtaque((Vector2){area.x + 19.0f, area.y + 10.0f},
                                    disponivel ? cor_tipo(ataque->tipo) : GRAY);
            }
            DesenharIconeTipo(ataque->tipo, (Vector2){area.x + 251.0f, area.y + 10.0f}, disponivel);
            DrawText(ataque->nome, (int)area.x + 36, y, 14, disponivel ? BLACK : GRAY);
            DrawText(nome_tipo(ataque->tipo), (int)area.x + 265, y + 1, 12,
                     disponivel ? cor_tipo(ataque->tipo) : GRAY);
            DrawText(TextFormat("DANO %d", ataque->dano_base), (int)area.x + 353, y + 1, 12,
                     disponivel ? BLACK : GRAY);
            if (!disponivel) {
                const int largura = MeasureText(motivo, 10) + 12;
                const int x_estado = (int)(area.x + area.width) - largura - 6;
                DrawRectangle(x_estado, y, largura, 15, (Color){211, 207, 203, 255});
                DrawRectangleLines(x_estado, y, largura, 15, GRAY);
                DrawText(motivo, x_estado + 6, y + 2, 10, DARKGRAY);
            }
        } else if (batalha->menu_acao == ACAO_ACOES) {
            char motivo[48] = "";
            const bool disponivel = InimigoAcaoDisponivel(inimigo, linha, motivo, (int)sizeof(motivo));
            const Color cor = disponivel ? BLACK : GRAY;
            const AcaoInimigo *acao = &inimigo->acoes[linha];

            InterfaceDesenharLinhaOpcao(area, selecionado, disponivel, (Color){225, 95, 122, 255});
            DesenharIconeAcao(acao, (Vector2){area.x + 19.0f, area.y + 10.0f}, disponivel);
            DrawText(acao->nome, (int)area.x + 36, y, 14, cor);
            if (acao->efeito == INIMIGO_ACAO_ANALISAR) {
                DrawText("revela o tipo e libera as acoes", (int)area.x + 174, y + 2, 11, cor);
            } else {
                const Color cor_misericordia = disponivel ? (Color){175, 128, 16, 255} : GRAY;
                const Color cor_estresse = disponivel ? (Color){181, 55, 55, 255} : GRAY;
                DrawText(TextFormat("MERCI %+d", acao->variacao_misericordia),
                         (int)area.x + 174, y + 1, 12, cor_misericordia);
                DrawText(TextFormat("EST %+d", acao->variacao_agressividade),
                         (int)area.x + 278, y + 1, 12, cor_estresse);
            }
            if (!disponivel) DrawText(motivo, (int)area.x + 407, y + 2, 11, GRAY);
        } else if (batalha->menu_acao == ACAO_ITENS) {
            const ItemInventario *item = &jogador->inventario[linha];
            const DefinicaoItem *definicao = ItemObterDefinicao(item->id);
            const bool disponivel = item->quantidade > 0;

            InterfaceDesenharLinhaOpcao(area, selecionado, disponivel,
                                        (Color){225, 95, 122, 255});
            if (sprite_obter(sprites, SpriteIdItem(item->id)) != NULL) {
                SpriteDesenharCentralizadoEscalado(sprites, SpriteIdItem(item->id),
                    (Vector2){area.x + 18.0f, area.y + 10.0f}, disponivel ? WHITE : GRAY,
                    0.0f, 0.68f, sprites->depurar_limites);
            } else {
                DesenharIconeMenuPrincipal(2, (Vector2){area.x + 18.0f, area.y + 10.0f},
                                           disponivel ? BLACK : GRAY);
            }
            DrawText(definicao->nome, (int)area.x + 34, y, 13, disponivel ? BLACK : GRAY);
            DrawText(disponivel ? TextFormat("x%d", item->quantidade) : "VAZIO",
                     (int)area.x + 215, y + 1, 12, disponivel ? DARKGRAY : GRAY);
        }
    }
}

void BatalhaDesenhar(const Batalha *batalha, const AtributosJogador *jogador, const Inimigo *inimigo,
                const TextoDigitado *dialogo, const Qte *qte, bool tremor_linha,
                bool efeitos_reduzidos, const SistemaSprites *sprites)
{
    const char *button_names[4] = {"ATACAR", "AGIR", "ITEM", "POUPAR"};
    const Rectangle text_box = ObterCaixaTexto();

    DesenharInimigo(inimigo, batalha->tempo_decorrido, batalha->temporizador_tremor_tela, tremor_linha, sprites);
    EfeitoDesenhar(&batalha->efeito_ataque, sprites, efeitos_reduzidos);
    if (!efeitos_reduzidos) {
        for (int indice = 0; indice < MAXIMO_BATALHA_PARTICULAS; indice++) {
            const ParticulaBatalha *particle = &batalha->particulas[indice];
            if (particle->ativo) DrawCircleV(particle->posicao, 3.0f, particle->cor);
        }
    }
    DrawText(TextFormat("ANDAR %d/10", batalha->andar), 470, 28, 16, DARKGRAY);
    if (batalha->temporizador_dano_flutuante > 0.0f) {
        const int y = (int)(102.0f - (0.85f - batalha->temporizador_dano_flutuante) * 34.0f);
        const int tamanho = batalha->ultima_efetividade >= 2.0f ? 28 : 20;
        const Color cor = batalha->ultima_efetividade >= 2.0f ? RED :
                            (batalha->ultima_efetividade <= 0.5f ? DARKBLUE : BLACK);
        DrawText(TextFormat("-%d", batalha->dano_flutuante), 344, y, tamanho, cor);
    }
    DrawText(inimigo->nome, 54, 32, 21, BLACK);
    if (inimigo->analisado && inimigo->possui_tipo2) {
        DrawText(TextFormat("%s / %s", nome_tipo(inimigo->tipo), nome_tipo(inimigo->tipo2)),
                 54, 58, 14, cor_tipo(inimigo->tipo));
    } else {
        DrawText(inimigo->analisado ? nome_tipo(inimigo->tipo) : "TIPO ???", 54, 58, 14,
                 inimigo->analisado ? cor_tipo(inimigo->tipo) : DARKGRAY);
    }
    DrawText("VIDA", 54, 80, 12, BLACK);
    DesenharBarraVida((Rectangle){105.0f, 80.0f, 120.0f, 12.0f}, batalha->vida_inimigo_exibida,
                  batalha->rastro_vida_inimigo, inimigo->vida_maxima, cor_tipo(inimigo->tipo));
    DrawText(TextFormat("%d/%d", inimigo->hp, inimigo->vida_maxima), 231, 79, 12, BLACK);
    DrawText("MERCI", 54, 99, 12, BLACK);
    InterfaceDesenharMedidor((Rectangle){105.0f, 100.0f, 120.0f, 11.0f}, batalha->misericordia_exibida, 100.0f,
                (Color){241, 198, 65, 255});
    DrawText("ESTRESSE", 54, 116, 11, BLACK);
    InterfaceDesenharMedidor((Rectangle){105.0f, 117.0f, 120.0f, 11.0f}, batalha->agressividade_exibida, 100.0f,
                (Color){220, 78, 76, 255});
    if (inimigo->turnos_status > 0) {
        DrawText(TextFormat("%s %d", JogadorNomeStatus(inimigo->status), inimigo->turnos_status),
                 54, 133, 12, cor_tipo(inimigo->tipo));
    }
    if (batalha->temporizador_mudanca_status > 0.0f) {
        const int deslocamento = (int)((0.8f - batalha->temporizador_mudanca_status) * 18.0f);
        if (batalha->popup_mudanca_misericordia != 0) {
            DrawText(TextFormat("%+d", batalha->popup_mudanca_misericordia), 231, 97 - deslocamento, 14,
                     (Color){198, 152, 23, 255});
        }
        if (batalha->popup_mudanca_agressividade != 0) {
            DrawText(TextFormat("%+d", batalha->popup_mudanca_agressividade), 231, 128 - deslocamento, 14,
                     (Color){185, 45, 44, 255});
        }
    }

    if (batalha->fase == BATALHA_JOGADOR_QTE) {
        InterfaceDesenharPainel(text_box);
        qte_desenhar(qte);
    } else if (batalha->fase == BATALHA_INIMIGO_TURNO) {
        DefesaDesenhar(&batalha->defesa, !efeitos_reduzidos, sprites);
    } else {
        InterfaceDesenharCaixaTexto(dialogo, text_box);
    }
    if (batalha->fase == BATALHA_JOGADOR_ACAO && batalha->menu_acao != ACAO_MENSAGEM) {
        DesenharListaAcoes(batalha, jogador, inimigo, sprites);
        InterfaceDesenharAtalho("X  voltar", 505, 204);
        if (batalha->menu_acao == ACAO_ITENS) {
            const ItemInventario *selecionado = &jogador->inventario[batalha->selecao_acao];
            const DefinicaoItem *definicao = ItemObterDefinicao(selecionado->id);
            DrawText(definicao->descricao, 54, 323, 11, DARKGRAY);
        }
    }

    DrawRectangle(38, 336, 560, 49, Fade(RAYWHITE, 0.72f));
    DrawLine(38, 336, 598, 336, (Color){69, 70, 77, 170});
    DrawText(jogador->nome, 48, 343, 20, BLACK);
    DrawText(TextFormat("LV %d", jogador->nivel), 154, 343, 16, DARKGRAY);
    DrawText("HP", 214, 343, 16, BLACK);
    DesenharBarraVida((Rectangle){242.0f, 346.0f, 128.0f, 14.0f}, batalha->vida_jogador_exibida,
                  batalha->rastro_vida_jogador, jogador->vida_maxima,
                  (Color){104, 183, 114, 255});
    DrawText(TextFormat("%d/%d", jogador->hp, jogador->vida_maxima), 380, 343, 16, BLACK);
    DrawText("DEF", 446, 343, 14, BLACK);
    DesenharIconeTipo(jogador->tipo_defesa, (Vector2){484, 351}, true);
    DrawText(nome_tipo(jogador->tipo_defesa), 499, 343, 14, BLACK);
    if (jogador->turnos_buff_dano > 0) {
        DrawRectangle(48, 367, 116, 17, (Color){243, 207, 110, 255});
        DrawRectangleLines(48, 367, 116, 17, BLACK);
        DrawText(TextFormat("DMG +%d%% (%d)", jogador->percentual_buff_dano,
                            jogador->turnos_buff_dano), 53, 369, 12, BLACK);
    }

    if (batalha->fase == BATALHA_INIMIGO_TURNO) {
        InterfaceDesenharPainel((Rectangle){38, 398, 560, 40});
        InterfaceDesenharAtalho("WASD/setas  mover", 54, 408);
        InterfaceDesenharAtalho("SHIFT  dash", 267, 408);
        InterfaceDesenharAtalho("ESC  pausa", 473, 408);
    } else {
        for (int indice_botao = 0; indice_botao < 4; indice_botao++) {
            const Rectangle botao = ObterBotaoPrincipal(indice_botao);
            const bool selecionado = batalha->fase == BATALHA_JOGADOR_MENU && batalha->selecao_principal == indice_botao;

            InterfaceDesenharBotao(botao, button_names[indice_botao], selecionado, true,
                                   (Color){225, 95, 122, 255});
            DesenharIconeMenuPrincipal(indice_botao,
                (Vector2){botao.x + 23.0f, botao.y + botao.height / 2.0f},
                selecionado ? (Color){175, 52, 83, 255} : BLACK);
        }
    }
}
