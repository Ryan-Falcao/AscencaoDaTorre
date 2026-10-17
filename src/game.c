#include "game.h"
#include "input.h"
#include "balance.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

static void DesenharTorre(float tempo)
{
    const float oscilacao = sinf(tempo * 1.4f) * 2.0f;

    DrawRectangle(272, (int)(82 + oscilacao), 96, 198, (Color){54, 59, 67, 255});
    DrawTriangle((Vector2){256, 84 + oscilacao}, (Vector2){384, 84 + oscilacao},
                 (Vector2){320, 28 + oscilacao}, (Color){39, 43, 50, 255});
    DrawRectangleLines(272, (int)(82 + oscilacao), 96, 198, BLACK);
    DrawLine(256, (int)(84 + oscilacao), 320, (int)(28 + oscilacao), BLACK);
    DrawLine(320, (int)(28 + oscilacao), 384, (int)(84 + oscilacao), BLACK);

    for (int indice_janela = 0; indice_janela < 3; indice_janela++) {
        const int y = (int)(112 + oscilacao + indice_janela * 50);
        DrawRectangle(302, y, 36, 28, (Color){255, 218, 112, 255});
        DrawRectangleLines(302, y, 36, 28, BLACK);
    }

    DrawCircle(320, (int)(270 + oscilacao), 12, (Color){225, 123, 145, 255});
    DrawCircleLines(320, (int)(270 + oscilacao), 12, BLACK);
}

static Rectangle ObterBotaoTitulo(int indice_opcao)
{
    return (Rectangle){190.0f, 318.0f + indice_opcao * 29.0f, 260.0f, 26.0f};
}

static void DesenharTitulo(const EstadoJogo *jogo)
{
    const char *opcoes[5] = {"NOVA RUN", "RELIQUIAS", "COMO JOGAR", "OPCOES", "SAIR"};

    DrawCircle(92, 90, 44, (Color){178, 220, 239, 255});
    DrawCircle(551, 101, 30, (Color){255, 207, 144, 255});
    if (sprite_obter(&jogo->sprites, SPRITE_TITULO_TORRE) != NULL) {
        SpriteDesenharAjustado(&jogo->sprites, SPRITE_TITULO_TORRE,
                               (Rectangle){250, 12, 140, 210}, WHITE);
    } else {
        DesenharTorre(jogo->tempo_decorrido);
    }

    const char *titulo = "ASCENSAO";
    const char *subtitulo = "DA TORRE";
    DrawText(titulo, (LOGICO_LARGURA - MeasureText(titulo, 42)) / 2, 225, 42, BLACK);
    DrawText(subtitulo, (LOGICO_LARGURA - MeasureText(subtitulo, 23)) / 2, 266, 23,
             (Color){73, 77, 85, 255});
    DrawText(TextFormat("melhor andar: %d  |  vitorias: %d", jogo->salvamento.melhor_andar,
                        jogo->salvamento.vitorias), 188, 298, 13, (Color){86, 88, 96, 255});

    for (int indice_opcao = 0; indice_opcao < 5; indice_opcao++) {
        const Rectangle botao = ObterBotaoTitulo(indice_opcao);
        const bool selecionado = jogo->selecao_titulo == indice_opcao;

        InterfaceDesenharBotao(botao, opcoes[indice_opcao], selecionado, true,
                               (Color){225, 95, 122, 255});
    }
}

static void DesenharMarcador(const char *titulo, const char *subtitulo)
{
    InterfaceDesenharPainel((Rectangle){62.0f, 122.0f, 516.0f, 238.0f});
    DrawText(titulo, 96, 160, 32, BLACK);
    DrawText(subtitulo, 96, 218, 18, (Color){75, 78, 84, 255});
    DrawText("Aperte X para voltar ao titulo.", 96, 306, 16, (Color){105, 106, 111, 255});
}

static Rectangle ObterAreaEscolha(int y)
{
    return (Rectangle){90, (float)y - 5, 460, 29};
}

static Rectangle ObterAreaEscolhaCompacta(int y)
{
    return (Rectangle){164, (float)y - 5, 312, 29};
}

static void DesenharEscolha(int y, const char *texto, bool selecionado)
{
    const Rectangle area = ObterAreaEscolha(y);
    InterfaceDesenharLinhaOpcao(area, selecionado, true, (Color){225, 95, 122, 255});
    DrawText(texto, 108, y, 17, BLACK);
}

static void DesenharEscolhaCompacta(int y, const char *texto, bool selecionado)
{
    const Rectangle area = ObterAreaEscolhaCompacta(y);
    InterfaceDesenharLinhaOpcao(area, selecionado, true, (Color){225, 95, 122, 255});
    DrawText(texto, 182, y, 17, BLACK);
}

static bool AtualizarSelecaoMouse(const EstadoJogo *jogo, int *selecao, int total,
                                  int y_inicial, int passo, bool compacta)
{
    for (int indice = 0; indice < total; indice++) {
        const int y = y_inicial + indice * passo;
        const Rectangle area = compacta ? ObterAreaEscolhaCompacta(y) : ObterAreaEscolha(y);
        if (CheckCollisionPointRec(jogo->posicao_mouse, area)) {
            *selecao = indice;
            return jogo->mouse_pressionado;
        }
    }
    return false;
}

static const char *NomeRecompensa(const RecompensaTorre *recompensa)
{
    if (recompensa->categoria == RECOMPENSA_ATAQUE) return JogadorAtaqueRecompensa(recompensa->id).nome;
    if (recompensa->categoria == RECOMPENSA_ITEM) return ItemObterDefinicao((IdItem)recompensa->id)->nome;
    return recompensa->categoria == RECOMPENSA_VIDA ? "+ HP MAX" : "+ DANO BASE";
}

static void DesenharRecompensa(const EstadoJogo *jogo)
{
    InterfaceDesenharPainel((Rectangle){62, 72, 516, 325});
    DrawText("RECOMPENSA", 223, 94, 26, BLACK);
    DrawText(TextFormat("%d moedas  |  %d XP  |  %s", jogo->ultimo_resultado_batalha.moedas,
                        jogo->ultimo_resultado_batalha.xp,
                        jogo->ultimo_resultado_batalha.desfecho == BATALHA_OUTCOME_MISERICORDIA ? "POUPAR: BONUS" : "VITORIA"),
             114, 130, 16, DARKGRAY);
    if (jogo->substituindo_ataque) {
        DrawText("Escolha o ataque para substituir:", 104, 172, 18, BLACK);
        DrawText("W/S ou setas: escolher | ESPACO/ENTER: confirmar | X: voltar",
                 104, 383, 13, DARKGRAY);
        for (int indice = 0; indice < jogo->jogador.total_ataques; indice++) {
            DesenharEscolha(208 + indice * 30, jogo->jogador.ataques[indice].nome,
                       jogo->selecao_substituicao == indice);
        }
    } else {
        DrawText("Escolha uma opcao:", 104, 172, 18, BLACK);
        for (int indice = 0; indice < 3; indice++) {
            DesenharEscolha(208 + indice * 42, NomeRecompensa(&jogo->torre.recompensas[indice]),
                       jogo->selecao_recompensa == indice);
        }
    }
}

static void DesenharDescanso(const EstadoJogo *jogo)
{
    InterfaceDesenharPainel((Rectangle){62, 104, 516, 260});
    DrawText("PONTO DE DESCANSO", 175, 132, 26, BLACK);
    DrawText(TextFormat("HP %d / %d", jogo->jogador.hp, jogo->jogador.vida_maxima), 225, 170, 18, DARKGRAY);
    DesenharEscolha(222, "DESCANSAR  (+50% HP)", jogo->selecao_descanso == 0);
    DesenharEscolha(264, "TREINAR  (+1 dano permanente)", jogo->selecao_descanso == 1);
}

static void DesenharLoja(const EstadoJogo *jogo)
{
    InterfaceDesenharPainel((Rectangle){62, 78, 516, 322});
    DrawText("LOJA DA TORRE", 205, 104, 26, BLACK);
    DrawText(TextFormat("MOEDAS: %d", jogo->torre.moedas), 235, 140, 18, DARKGRAY);
    for (int indice = 0; indice < 3; indice++) {
        const DefinicaoItem *item = ItemObterDefinicao(jogo->torre.itens_loja[indice]);
        if (sprite_obter(&jogo->sprites, SpriteIdItem(item->id)) != NULL) {
            SpriteDesenharCentralizado(&jogo->sprites, SpriteIdItem(item->id),
                               (Vector2){103, 181.0f + indice * 42.0f}, WHITE, 0.0f,
                               jogo->sprites.depurar_limites);
        }
        DesenharEscolha(187 + indice * 42, TextFormat("%s - %d moedas", item->nome,
                                                jogo->torre.precos_loja[indice]),
                   jogo->selecao_loja == indice);
    }
    DesenharEscolha(325, "SAIR E SUBIR", jogo->selecao_loja == 3);
    if (jogo->selecao_loja < 3) {
        DrawText(ItemObterDefinicao(jogo->torre.itens_loja[jogo->selecao_loja])->descricao,
                 103, 365, 14, DARKGRAY);
    }
    if (jogo->mensagem_tela[0] != '\0') DrawText(jogo->mensagem_tela, 103, 382, 14, MAROON);
}

static void DesenharReliquias(const EstadoJogo *jogo)
{
    InterfaceDesenharPainel((Rectangle){62, 104, 516, 270});
    DrawText("RELIQUIAS", 246, 132, 28, BLACK);
    DrawText(TextFormat("FRAGMENTOS: %d", jogo->salvamento.fragmentos), 218, 169, 18, DARKGRAY);
    DesenharEscolha(220, "Erva Lunar inicial - 8 fragmentos", jogo->selecao_reliquia == 0);
    DesenharEscolha(264, "Raio Manso inicial - 10 fragmentos", jogo->selecao_reliquia == 1);
    DrawText("X/Backspace: voltar", 205, 331, 14, DARKGRAY);
}

static void DesenharOpcoes(const EstadoJogo *jogo)
{
    static const char *difficulty_names[] = {"CALMO", "NORMAL", "DIFICIL"};
    InterfaceDesenharPainel((Rectangle){62, 30, 516, 420});
    DrawText("OPCOES", 260, 55, 28, BLACK);
    DesenharEscolha(105, TextFormat("Musica: %d%%", (int)(jogo->salvamento.volume_musica * 100.0f)), jogo->selecao_opcao == 0);
    DesenharEscolha(141, TextFormat("Efeitos: %d%%", (int)(jogo->salvamento.volume_efeitos * 100.0f)), jogo->selecao_opcao == 1);
    DesenharEscolha(177, TextFormat("Tela cheia: %s", jogo->salvamento.tela_cheia ? "SIM" : "NAO"), jogo->selecao_opcao == 2);
    DesenharEscolha(213, TextFormat("Tremor de linha: %s", jogo->salvamento.tremor_linha ? "SIM" : "NAO"), jogo->selecao_opcao == 3);
    DesenharEscolha(249, TextFormat("Efeitos reduzidos: %s", jogo->salvamento.efeitos_reduzidos ? "SIM" : "NAO"), jogo->selecao_opcao == 4);
    DesenharEscolha(285, TextFormat("Dificuldade: %s", difficulty_names[jogo->salvamento.dificuldade]), jogo->selecao_opcao == 5);
    DesenharEscolha(321, "APAGAR SAVE", jogo->selecao_opcao == 6);
    DesenharEscolha(357, "REVER TUTORIAL", jogo->selecao_opcao == 7);
    DesenharEscolha(393, TextFormat("Dicas: %s", jogo->salvamento.tutorial_concluido ? "DESATIVADAS" : "ATIVADAS"), jogo->selecao_opcao == 8);
    InterfaceDesenharAtalho("ENTER alterar  |  X voltar  |  F11 tela cheia", 174, 422);
}

enum { TUTORIAL_MENU = 1, TUTORIAL_TIPOS = 2, TUTORIAL_MERCY = 4,
       TUTORIAL_DEFESA = 8, TUTORIAL_QTE = 16, TUTORIAL_ALL = 31 };

static const char *TextoTutorial(int dica)
{
    static const char *const dicas[] = {
        "Use WASD ou setas para escolher ATACAR, AGIR, ITEM e POUPAR.",
        "AGIR > Analisar revela o tipo e libera as outras acoes de AGIR.",
        "MERCI permite poupar; ESTRESSE alto deixa a defesa mais perigosa.",
        "Defesa: WASD/setas movem e SHIFT executa um dash. Com gravidade, ESPACO/W/CIMA pula.",
        "Nos QTEs, use ESPACO/ENTER/Z. Ritmo pede tres acertos com a mesma tecla."
    };
    return dicas[dica];
}

static bool AtualizarTutorial(EstadoJogo *jogo)
{
    int exigido = 0;

    /* Consome tambem a soltura para fechar uma dica sem iniciar ou concluir um QTE. */
    if (jogo->temporizador_tutorial == -2.0f) {
        if (!IsKeyDown(KEY_SPACE)) jogo->temporizador_tutorial = -1.0f;
        return true;
    }
    if (jogo->salvamento.tutorial_concluido) return false;
    if (jogo->temporizador_tutorial < 0.0f) {
        if (jogo->fase_espera_tutorial == (int)jogo->batalha.fase) return false;
        jogo->temporizador_tutorial = 0.0f;
        jogo->fase_espera_tutorial = -1;
    }
    if (jogo->temporizador_tutorial > 0.0f) {
        if (IsKeyPressed(KEY_SPACE)) {
            jogo->salvamento.tutorial_visto |= 1u << jogo->dica_tutorial;
            jogo->salvamento.tutorial_concluido = jogo->salvamento.tutorial_visto == TUTORIAL_ALL;
            jogo->temporizador_tutorial = -2.0f;
            jogo->fase_espera_tutorial = (int)jogo->batalha.fase;
            SalvamentoGravar(&jogo->salvamento);
        }
        return true;
    }
    if (jogo->batalha.fase == BATALHA_JOGADOR_MENU) {
        if ((jogo->salvamento.tutorial_visto & TUTORIAL_MENU) == 0) exigido = TUTORIAL_MENU;
        else if ((jogo->salvamento.tutorial_visto & TUTORIAL_TIPOS) == 0) exigido = TUTORIAL_TIPOS;
        else if ((jogo->salvamento.tutorial_visto & TUTORIAL_MERCY) == 0) exigido = TUTORIAL_MERCY;
    } else if (jogo->batalha.fase == BATALHA_INIMIGO_TURNO && (jogo->salvamento.tutorial_visto & TUTORIAL_DEFESA) == 0) {
        exigido = TUTORIAL_DEFESA;
    } else if (jogo->batalha.fase == BATALHA_JOGADOR_QTE && (jogo->salvamento.tutorial_visto & TUTORIAL_QTE) == 0) {
        exigido = TUTORIAL_QTE;
    }
    if (exigido != 0) {
        int dica = 0;
        while ((1 << dica) != exigido) dica++;
        jogo->dica_tutorial = dica;
        jogo->temporizador_tutorial = 1.0f; /* Modal: permanece aberto ate ESPACO ser pressionado. */
        return true;
    }
    return false;
}

static void DesenharTutorial(const EstadoJogo *jogo)
{
    const Rectangle caixa = {54, 166, 532, 92};
    DrawRectangle(0, 0, LOGICO_LARGURA, LOGICO_ALTURA, (Color){0, 0, 0, 140});
    InterfaceDesenharPainel(caixa);
    DrawText("DICA", 72, 179, 14, (Color){203, 84, 104, 255});
    InterfaceDesenharTextoQuebrado(TextoTutorial(jogo->dica_tutorial),
                (Rectangle){72, 199, 496, 35}, 14, 2, BLACK);
    InterfaceDesenharAtalho("ESPACO  fechar", 444, 234);
}

static void DesenharComoJogar(void)
{
    static const char *titulos[] = {"MENU", "COMBATE", "DEFESA", "QTE", "TIPOS", "POUPAR"};
    static const char *textos[] = {
        "WASD ou setas navegam. ESPACO, ENTER ou Z confirmam. X volta.",
        "ATACAR inicia o QTE. AGIR revela tipos e aumenta a misericordia.",
        "WASD ou setas movem a alma. SHIFT da dash. Com gravidade, ESPACO pula.",
        "Impacto pede precisao, Ritmo pede 3 acertos e Carga pede segurar e soltar.",
        "Chama vence Planta; Planta vence Agua; Agua vence Chama. Luz e Sombra se enfrentam.",
        "MERCI alta e ESTRESSE baixo permitem encerrar a luta sem atacar."
    };
    InterfaceDesenharPainel((Rectangle){48, 38, 544, 404});
    DrawText("COMO JOGAR", 232, 57, 26, BLACK);
    for (int indice = 0; indice < 6; indice++) {
        int y = 98 + indice * 49;
        DrawText(titulos[indice], 72, y, 12, (Color){203, 84, 104, 255});
        InterfaceDesenharTextoQuebrado(textos[indice],
                    (Rectangle){72, (float)y + 15, 496, 31}, 14, 2, BLACK);
    }
    InterfaceDesenharAtalho("X  voltar", 478, 410);
}

static void DesenharDepuracaoEfeitos(const EstadoJogo *jogo)
{
    const IdSprite enemy_id = SpriteIdInimigo((IdInimigo)jogo->inimigo_depuracao_padrao, false);

    DrawText("TESTE DE PADROES", 195, 28, 25, BLACK);
    DrawText(TextFormat("Padrao %d | inimigo %d", jogo->id_depuracao_padrao, jogo->inimigo_depuracao_padrao),
             70, 66, 18, DARKGRAY);
    DrawText(TextFormat("HP %d/%d | dano: %s", jogo->jogador.hp, jogo->jogador.vida_maxima,
                        jogo->batalha.defesa.depurar_invencivel ? "ignorado" : "ativo"),
             70, 90, 14, DARKGRAY);
    DrawText("WASD/setas: mover | SHIFT: dash | ESPACO: pular | R: reiniciar | G: Gas Sombrio", 20, 438, 11, DARKGRAY);
    DrawText("F9/F10: padrao | PgUp/PgDn: inimigo | I: alternar dano | X: voltar", 35, 454, 11, DARKGRAY);
    if (sprite_obter(&jogo->sprites, enemy_id) != NULL) {
        SpriteDesenharCentralizado(&jogo->sprites, enemy_id, (Vector2){320, 190}, WHITE, jogo->tempo_decorrido,
                           jogo->sprites.depurar_limites);
    } else {
        DrawCircle(320, 190, 35, cor_tipo(TIPO_SOMBRA));
        DrawCircleLines(320, 190, 35, BLACK);
    }
    DefesaDesenhar(&jogo->batalha.defesa, !jogo->salvamento.efeitos_reduzidos, &jogo->sprites);
}

static void DesenharPausa(const EstadoJogo *jogo)
{
    const char *opcoes[] = {"CONTINUAR", "OPCOES", "COMO JOGAR", "DESISTIR DA RUN", "TITULO"};
    InterfaceDesenharPainel((Rectangle){136, 92, 368, 292});
    DrawText("PAUSADO", 248, 119, 27, BLACK);
    if (jogo->pausa_confirmar_saida) {
        const Rectangle botao_desistir = {164, 218, 146, 38};
        const Rectangle botao_cancelar = {330, 218, 146, 38};
        const bool sobre_desistir = CheckCollisionPointRec(jogo->posicao_mouse, botao_desistir);
        const bool sobre_cancelar = CheckCollisionPointRec(jogo->posicao_mouse, botao_cancelar);
        DrawText("Desistir desta run?", 210, 178, 18, MAROON);
        InterfaceDesenharBotao(botao_desistir, "DESISTIR", sobre_desistir, true,
                               (Color){197, 69, 78, 255});
        InterfaceDesenharBotao(botao_cancelar, "CANCELAR", sobre_cancelar || !sobre_desistir, true,
                               (Color){83, 132, 171, 255});
        InterfaceDesenharAtalho("ESPACO confirmar  |  X cancelar", 206, 278);
        return;
    }
    for (int indice = 0; indice < 5; indice++)
        DesenharEscolhaCompacta(170 + indice * 34, opcoes[indice], jogo->selecao_pausa == indice);
}

static void DesenharConfirmacaoExclusao(const EstadoJogo *jogo)
{
    InterfaceDesenharPainel((Rectangle){93, 158, 454, 164});
    DrawText("Apagar todo o save?", 191, 190, 23, BLACK);
    DesenharEscolhaCompacta(245, "SIM", jogo->excluir_sim);
    DesenharEscolhaCompacta(275, "NAO", !jogo->excluir_sim);
}

static void IniciarAndar(EstadoJogo *jogo)
{
    jogo->inimigo = InimigoCriar(TorreInimigoDoAndar(&jogo->torre, jogo->torre.andar));
    TorreEscalarInimigo(&jogo->inimigo, jogo->torre.andar);
    RenderizacaoDefinirAndar(&jogo->renderizacao, jogo->torre.andar, jogo->inimigo.chefe);
    BatalhaInicializar(&jogo->batalha, &jogo->dialogo, jogo->torre.andar);
    jogo->batalha.dificuldade_qte = BalanceamentoDificuldadeQte(jogo->torre.andar, jogo->salvamento.dificuldade);
    jogo->temporizador_transicao = 0.70f;
    jogo->tela = BATALHA;
}

static void IniciarPartida(EstadoJogo *jogo)
{
    jogo->contador_partidas++;
    jogo->partida_iniciada_em = (float)GetTime();
    JogadorInicializar(&jogo->jogador);
    jogo->jogador.multiplicador_dano_recebido = BalanceamentoMultiplicadorDanoRecebido(jogo->salvamento.dificuldade);
    if (jogo->salvamento.tipo_reliquia == RELIQUIA_ITEM) JogadorAdicionarItem(&jogo->jogador, (IdItem)jogo->salvamento.id_reliquia, 1);
    if (jogo->salvamento.tipo_reliquia == RELIQUIA_ATAQUE) JogadorAdicionarAtaque(&jogo->jogador,
        JogadorAtaqueRecompensa(jogo->salvamento.id_reliquia), -1);
    TorreInicializar(&jogo->torre, (unsigned int)(GetTime() * 1000.0) + jogo->contador_partidas * 7919u);
    jogo->ultimo_resultado_batalha = (ResultadoBatalha){0};
    jogo->mensagem_tela[0] = '\0';
    IniciarAndar(jogo);
}

static void FinalizarPartida(EstadoJogo *jogo, bool vitoria)
{
    if (jogo->torre.andar > jogo->salvamento.melhor_andar) jogo->salvamento.melhor_andar = jogo->torre.andar;
    jogo->salvamento.total_runs++;
    if (vitoria) jogo->salvamento.vitorias++;
    jogo->salvamento.vitorias_misericordia += jogo->torre.vitorias_misericordia;
    jogo->salvamento.vitorias_dano += jogo->torre.vitorias_dano;
    jogo->salvamento.fragmentos += jogo->torre.andar + (vitoria ? 15 : 0);
    SalvamentoAplicarDesbloqueios(&jogo->salvamento);
    SalvamentoGravar(&jogo->salvamento);
    if (jogo->telemetria_ativada) {
        TesteJogoRegistrarPartida(jogo->torre.andar, jogo->torre.moedas, vitoria,
                       (float)GetTime() - jogo->partida_iniciada_em);
    }
}

static void RetomarPartidaPausada(EstadoJogo *jogo)
{
    qte_retomar_com_guarda(&jogo->qte);
    jogo->tela = jogo->tela_antes_pausa;
}

static void ContinuarAposRecompensa(EstadoJogo *jogo)
{
    const int andar_concluido = jogo->torre.andar;

    if (andar_concluido >= TOTAL_ANDARES_TORRE) {
        FinalizarPartida(jogo, true);
        jogo->selecao_final = 0;
        jogo->tela = VITORIA;
        return;
    }
    jogo->torre.andar++;
    if (TorreDeveDescansar(andar_concluido)) jogo->tela = DESCANSO;
    else if (TorreDeveTerLoja(andar_concluido)) jogo->tela = LOJA;
    else IniciarAndar(jogo);
}

static void AplicarRecompensa(EstadoJogo *jogo, int selecao)
{
    const RecompensaTorre recompensa = jogo->torre.recompensas[selecao];

    if (recompensa.categoria == RECOMPENSA_ATAQUE) {
        const Ataque ataque = JogadorAtaqueRecompensa(recompensa.id);
        if (jogo->jogador.total_ataques >= MAXIMO_EQUIPADOS_ATAQUES && !jogo->substituindo_ataque) {
            jogo->substituindo_ataque = true;
            jogo->selecao_substituicao = 0;
            return;
        }
        if (!JogadorAdicionarAtaque(&jogo->jogador, ataque,
                             jogo->jogador.total_ataques >= MAXIMO_EQUIPADOS_ATAQUES ?
                             jogo->selecao_substituicao : -1)) {
            /* Uma recompensa nunca se perde silenciosamente se o ataque ja estiver equipado. */
            snprintf(jogo->mensagem_tela, sizeof(jogo->mensagem_tela),
                     "Esse ataque ja esta equipado.");
            jogo->substituindo_ataque = false;
            return;
        }
    } else if (recompensa.categoria == RECOMPENSA_ITEM) {
        JogadorAdicionarItem(&jogo->jogador, (IdItem)recompensa.id, recompensa.valor);
    } else if (recompensa.categoria == RECOMPENSA_VIDA) {
        jogo->jogador.vida_maxima += recompensa.valor;
        JogadorCurar(&jogo->jogador, recompensa.valor * 2);
    } else {
        jogo->jogador.bonus_dano_permanente += recompensa.valor / 2;
    }
    jogo->substituindo_ataque = false;
    ContinuarAposRecompensa(jogo);
}

void JogoInicializar(EstadoJogo *jogo)
{
    memset(jogo, 0, sizeof(*jogo));
    jogo->tela = TITULO;
    jogo->fase_espera_tutorial = -1;
    jogo->tela_antes_pausa = TITULO;
    jogo->selecao_titulo = 0;
    jogo->tempo_decorrido = 0.0f;
    jogo->saida_solicitada = false;
    SalvamentoCarregar(&jogo->salvamento);
    RenderizacaoInicializar(&jogo->renderizacao, jogo->salvamento.tremor_linha);
    AudioInicializar(&jogo->audio, jogo->salvamento.volume_musica, jogo->salvamento.volume_efeitos);
    SpritesInicializar(&jogo->sprites);
    if (jogo->salvamento.tela_cheia) ToggleFullscreen();
    jogo->contador_partidas = 0;
    jogo->ultimo_resultado_batalha = (ResultadoBatalha){0};
    JogadorInicializar(&jogo->jogador);
    TorreInicializar(&jogo->torre, 1u);
    jogo->inimigo = InimigoCriar(INIMIGO_POEIRA_MARIPOSA);
    BatalhaInicializar(&jogo->batalha, &jogo->dialogo, 1);
}

void JogoAtualizar(EstadoJogo *jogo)
{
    if ((jogo->tela == BATALHA || jogo->tela == DEPURAR_EFEITOS) && !IsWindowFocused()) {
        AudioAtualizar(&jogo->audio, GetFrameTime());
        return;
    }
    if (jogo->tela == BATALHA && jogo->temporizador_transicao <= 0.0f &&
        jogo->batalha.temporizador_pausa_impacto <= 0.0f && AtualizarTutorial(jogo)) {
        AudioAtualizar(&jogo->audio, GetFrameTime());
        return;
    }
    jogo->tempo_decorrido += GetFrameTime();
    if (entrada_confirmar_pressionada()) AudioTocar(&jogo->audio, AUDIO_CONFIRMAR);
    if (entrada_cancelar_pressionada()) AudioTocar(&jogo->audio, AUDIO_CANCELAR);
    if (entrada_cima_pressionada() || entrada_baixo_pressionada() || entrada_esquerda_pressionada() || entrada_direita_pressionada()) {
        AudioTocar(&jogo->audio, AUDIO_CURSOR);
    }
#ifndef NDEBUG
    if (IsKeyPressed(KEY_F8) && jogo->tela != DEPURAR_EFEITOS) {
        jogo->tela_antes_pausa = jogo->tela;
        jogo->tela = DEPURAR_EFEITOS;
        jogo->inimigo_depuracao_padrao = INIMIGO_CHEFE_SINEIRO;
        jogo->id_depuracao_padrao = PADRAO_SINO_GAS_SOMBRA;
        JogadorInicializar(&jogo->jogador);
        DefesaInicializar(&jogo->batalha.defesa);
        DefesaIniciar(&jogo->batalha.defesa, PADRAO_SINO_GAS_SOMBRA, (Rectangle){42, 228, 556, 94},
                     0, 0, 1.0f, 1.0f);
        jogo->batalha.defesa.depurar_invencivel = true;
    }
#endif

    if (jogo->tela == TITULO) {
        bool confirmado = entrada_confirmar_pressionada();

        if (entrada_cima_pressionada()) {
            jogo->selecao_titulo = (jogo->selecao_titulo + 4) % 5;
        }
        if (entrada_baixo_pressionada()) {
            jogo->selecao_titulo = (jogo->selecao_titulo + 1) % 5;
        }
        for (int indice_opcao = 0; indice_opcao < 5; indice_opcao++) {
            if (CheckCollisionPointRec(jogo->posicao_mouse, ObterBotaoTitulo(indice_opcao))) {
                jogo->selecao_titulo = indice_opcao;
                if (jogo->mouse_pressionado) confirmado = true;
            }
        }
        if (confirmado) {
            if (jogo->selecao_titulo == 0) IniciarPartida(jogo);
            else if (jogo->selecao_titulo == 1) jogo->tela = RELIQUIAS;
            else if (jogo->selecao_titulo == 2) jogo->tela = COMO_JOGAR;
            else if (jogo->selecao_titulo == 3) jogo->tela = OPCOES;
            else jogo->saida_solicitada = true;
        }
    } else if (jogo->tela == BATALHA) {
        ResultadoBatalha resultado = {0};

        if (jogo->temporizador_transicao > 0.0f) jogo->temporizador_transicao -= GetFrameTime();
        else if (jogo->batalha.temporizador_pausa_impacto > 0.0f) jogo->batalha.temporizador_pausa_impacto -= GetFrameTime();
        else resultado = BatalhaAtualizar(&jogo->batalha, &jogo->jogador, &jogo->inimigo,
                                   &jogo->dialogo, jogo->posicao_mouse,
                                   jogo->mouse_pressionado, &jogo->qte, &jogo->audio);

        if (resultado.desfecho == BATALHA_OUTCOME_DANO || resultado.desfecho == BATALHA_OUTCOME_MISERICORDIA) {
            resultado.moedas += jogo->inimigo.bonus_recompensa + (jogo->inimigo.chefe ? 8 : 0);
            resultado.xp += jogo->inimigo.bonus_recompensa + (jogo->inimigo.chefe ? 10 : 0);
            jogo->ultimo_resultado_batalha = resultado;
            jogo->torre.moedas += resultado.moedas;
            if (jogo->telemetria_ativada) TesteJogoRegistrarBatalha(jogo->torre.andar, &jogo->inimigo, &resultado);
            if (resultado.desfecho == BATALHA_OUTCOME_MISERICORDIA) jogo->torre.vitorias_misericordia++;
            else jogo->torre.vitorias_dano++;
            if (JogadorGanharXp(&jogo->jogador, resultado.xp)) AudioTocar(&jogo->audio, AUDIO_NIVEL_CIMA);
            AudioTocar(&jogo->audio, resultado.desfecho == BATALHA_OUTCOME_MISERICORDIA ? AUDIO_VITORIA : AUDIO_DANO_CAUSADO);
            TorreSortearRecompensas(&jogo->torre, &jogo->jogador);
            jogo->selecao_recompensa = 0;
            jogo->substituindo_ataque = false;
            jogo->tela = RECOMPENSA;
        }
        if (resultado.desfecho == BATALHA_OUTCOME_DERROTA) {
            if (jogo->telemetria_ativada) TesteJogoRegistrarBatalha(jogo->torre.andar, &jogo->inimigo, &resultado);
            AudioTocar(&jogo->audio, AUDIO_DERROTA);
            FinalizarPartida(jogo, false);
            jogo->selecao_final = 0;
            jogo->tela = FIM_JOGO;
        }
        if (IsKeyPressed(KEY_ESCAPE)) {
            jogo->tela_antes_pausa = jogo->tela;
            jogo->selecao_pausa = 0;
            jogo->pausa_confirmar_saida = false;
            jogo->tela = PAUSA;
        }
    } else if (jogo->tela == PAUSA) {
        if (jogo->pausa_confirmar_saida) {
            bool clicou_desistir = jogo->mouse_pressionado &&
                CheckCollisionPointRec(jogo->posicao_mouse, (Rectangle){164, 218, 146, 38});
            bool clicou_cancelar = jogo->mouse_pressionado &&
                CheckCollisionPointRec(jogo->posicao_mouse, (Rectangle){330, 218, 146, 38});
            if (entrada_cancelar_pressionada() || IsKeyPressed(KEY_ESCAPE) || clicou_cancelar)
                jogo->pausa_confirmar_saida = false;
            else if (entrada_confirmar_pressionada() || clicou_desistir) {
                FinalizarPartida(jogo, false);
                jogo->selecao_final = 0;
                jogo->tela = FIM_JOGO;
            }
        } else {
            bool confirmou_mouse = AtualizarSelecaoMouse(jogo, &jogo->selecao_pausa, 5, 170, 34, true);
            if (entrada_cima_pressionada()) jogo->selecao_pausa = (jogo->selecao_pausa + 4) % 5;
            if (entrada_baixo_pressionada()) jogo->selecao_pausa = (jogo->selecao_pausa + 1) % 5;
            if (IsKeyPressed(KEY_ESCAPE) || entrada_cancelar_pressionada()) RetomarPartidaPausada(jogo);
            if (entrada_confirmar_pressionada() || confirmou_mouse) {
                if (jogo->selecao_pausa == 0) RetomarPartidaPausada(jogo);
                else if (jogo->selecao_pausa == 1) jogo->tela = OPCOES;
                else if (jogo->selecao_pausa == 2) jogo->tela = COMO_JOGAR;
                else if (jogo->selecao_pausa == 3) jogo->pausa_confirmar_saida = true;
                else jogo->tela = TITULO;
            }
        }
    } else if (jogo->tela == RECOMPENSA) {
        if (jogo->substituindo_ataque) {
            bool confirmou_mouse = AtualizarSelecaoMouse(jogo, &jogo->selecao_substituicao,
                jogo->jogador.total_ataques, 208, 30, false);
            if (entrada_cima_pressionada() || entrada_esquerda_pressionada()) {
                jogo->selecao_substituicao = (jogo->selecao_substituicao +
                    jogo->jogador.total_ataques - 1) % jogo->jogador.total_ataques;
            }
            if (entrada_baixo_pressionada() || entrada_direita_pressionada()) {
                jogo->selecao_substituicao = (jogo->selecao_substituicao + 1) %
                    jogo->jogador.total_ataques;
            }
            if (entrada_confirmar_pressionada() || confirmou_mouse)
                AplicarRecompensa(jogo, jogo->selecao_recompensa);
            if (entrada_cancelar_pressionada()) jogo->substituindo_ataque = false;
        } else {
            bool confirmou_mouse = AtualizarSelecaoMouse(jogo, &jogo->selecao_recompensa, 3, 208, 42, false);
            if (entrada_cima_pressionada()) jogo->selecao_recompensa = (jogo->selecao_recompensa + 2) % 3;
            if (entrada_baixo_pressionada()) jogo->selecao_recompensa = (jogo->selecao_recompensa + 1) % 3;
            if (entrada_confirmar_pressionada() || confirmou_mouse)
                AplicarRecompensa(jogo, jogo->selecao_recompensa);
        }
    } else if (jogo->tela == DESCANSO) {
        bool confirmou_mouse = AtualizarSelecaoMouse(jogo, &jogo->selecao_descanso, 2, 222, 42, false);
        if (entrada_cima_pressionada() || entrada_baixo_pressionada()) jogo->selecao_descanso = 1 - jogo->selecao_descanso;
        if (entrada_confirmar_pressionada() || confirmou_mouse) {
            if (jogo->selecao_descanso == 0) {
                JogadorCurar(&jogo->jogador, jogo->jogador.vida_maxima / 2);
                AudioTocar(&jogo->audio, AUDIO_CURAR);
            }
            else jogo->jogador.bonus_dano_permanente++;
            IniciarAndar(jogo);
        }
    } else if (jogo->tela == LOJA) {
        bool confirmou_mouse = false;
        for (int indice = 0; indice < 4; indice++) {
            int y = indice < 3 ? 187 + indice * 42 : 325;
            if (CheckCollisionPointRec(jogo->posicao_mouse, ObterAreaEscolha(y))) {
                jogo->selecao_loja = indice;
                confirmou_mouse = jogo->mouse_pressionado;
            }
        }
        if (entrada_cima_pressionada()) jogo->selecao_loja = (jogo->selecao_loja + 3) % 4;
        if (entrada_baixo_pressionada()) jogo->selecao_loja = (jogo->selecao_loja + 1) % 4;
        if (entrada_confirmar_pressionada() || confirmou_mouse) {
            if (jogo->selecao_loja == 3) IniciarAndar(jogo);
            else {
                const IdItem item = jogo->torre.itens_loja[jogo->selecao_loja];
                const int preco = jogo->torre.precos_loja[jogo->selecao_loja];
                if (jogo->torre.moedas >= preco && JogadorAdicionarItem(&jogo->jogador, item, 1)) {
                    jogo->torre.moedas -= preco;
                    AudioTocar(&jogo->audio, AUDIO_COMPRAR);
                    snprintf(jogo->mensagem_tela, sizeof(jogo->mensagem_tela), "Comprou %s.",
                             ItemObterDefinicao(item)->nome);
                } else if (jogo->torre.moedas < preco) {
                    snprintf(jogo->mensagem_tela, sizeof(jogo->mensagem_tela), "Moedas insuficientes.");
                } else {
                    snprintf(jogo->mensagem_tela, sizeof(jogo->mensagem_tela), "Inventario cheio.");
                }
            }
        }
    } else if (jogo->tela == FIM_JOGO || jogo->tela == VITORIA) {
        bool confirmou_mouse = jogo->tela == FIM_JOGO ?
            AtualizarSelecaoMouse(jogo, &jogo->selecao_final, 2, 270, 40, false) :
            (jogo->mouse_pressionado && CheckCollisionPointRec(jogo->posicao_mouse,
                                                               ObterAreaEscolha(292)));
        if (entrada_cima_pressionada() || entrada_baixo_pressionada()) jogo->selecao_final = 1 - jogo->selecao_final;
        if (entrada_confirmar_pressionada() || confirmou_mouse) {
            if (jogo->selecao_final == 0 && jogo->tela == FIM_JOGO) IniciarPartida(jogo);
            else jogo->tela = TITULO;
        }
    } else if (jogo->tela == RELIQUIAS) {
        bool confirmou_mouse = AtualizarSelecaoMouse(jogo, &jogo->selecao_reliquia, 2, 220, 44, false);
        if (entrada_cima_pressionada() || entrada_baixo_pressionada()) jogo->selecao_reliquia = 1 - jogo->selecao_reliquia;
        if (entrada_confirmar_pressionada() || confirmou_mouse) {
            if (jogo->selecao_reliquia == 0) SalvamentoComprarReliquia(&jogo->salvamento, RELIQUIA_ITEM, ITEM_LUA_ERVA, 8);
            else SalvamentoComprarReliquia(&jogo->salvamento, RELIQUIA_ATAQUE, 3, 10);
            SalvamentoGravar(&jogo->salvamento);
        }
        if (entrada_cancelar_pressionada()) jogo->tela = TITULO;
    } else if (jogo->tela == OPCOES) {
        bool confirmou_mouse = AtualizarSelecaoMouse(jogo, &jogo->selecao_opcao, 9, 105, 36, false);
        if (entrada_cima_pressionada()) jogo->selecao_opcao = (jogo->selecao_opcao + 8) % 9;
        if (entrada_baixo_pressionada()) jogo->selecao_opcao = (jogo->selecao_opcao + 1) % 9;
        if (entrada_esquerda_pressionada() && jogo->selecao_opcao == 0) jogo->salvamento.volume_musica -= 0.1f;
        if (entrada_direita_pressionada() && jogo->selecao_opcao == 0) jogo->salvamento.volume_musica += 0.1f;
        if (entrada_esquerda_pressionada() && jogo->selecao_opcao == 1) jogo->salvamento.volume_efeitos -= 0.1f;
        if (entrada_direita_pressionada() && jogo->selecao_opcao == 1) jogo->salvamento.volume_efeitos += 0.1f;
        if (jogo->salvamento.volume_musica < 0.0f) jogo->salvamento.volume_musica = 0.0f;
        if (jogo->salvamento.volume_musica > 1.0f) jogo->salvamento.volume_musica = 1.0f;
        if (jogo->salvamento.volume_efeitos < 0.0f) jogo->salvamento.volume_efeitos = 0.0f;
        if (jogo->salvamento.volume_efeitos > 1.0f) jogo->salvamento.volume_efeitos = 1.0f;
        if (entrada_confirmar_pressionada() || confirmou_mouse) {
            if (jogo->selecao_opcao == 2) {
                ToggleFullscreen(); jogo->salvamento.tela_cheia = !jogo->salvamento.tela_cheia;
            } else if (jogo->selecao_opcao == 3) {
                jogo->salvamento.tremor_linha = !jogo->salvamento.tremor_linha;
            } else if (jogo->selecao_opcao == 4) {
                jogo->salvamento.efeitos_reduzidos = !jogo->salvamento.efeitos_reduzidos;
            } else if (jogo->selecao_opcao == 5) {
                jogo->salvamento.dificuldade = (jogo->salvamento.dificuldade + 1) % 3;
            } else if (jogo->selecao_opcao == 6) jogo->tela = CONFIRMAR_EXCLUSAO;
            else if (jogo->selecao_opcao == 7) {
                jogo->salvamento.tutorial_visto = 0;
                jogo->salvamento.tutorial_concluido = false;
                jogo->temporizador_tutorial = 0.0f;
                jogo->fase_espera_tutorial = -1;
                SalvamentoGravar(&jogo->salvamento);
            } else if (jogo->selecao_opcao == 8) {
                jogo->salvamento.tutorial_concluido = !jogo->salvamento.tutorial_concluido;
                if (!jogo->salvamento.tutorial_concluido) jogo->salvamento.tutorial_visto = 0;
                jogo->temporizador_tutorial = 0.0f;
                jogo->fase_espera_tutorial = -1;
                SalvamentoGravar(&jogo->salvamento);
            }
        }
        if (entrada_cancelar_pressionada()) { SalvamentoGravar(&jogo->salvamento); jogo->tela = TITULO; }
    } else if (jogo->tela == CONFIRMAR_EXCLUSAO) {
        int selecao_exclusao = jogo->excluir_sim ? 0 : 1;
        bool confirmou_mouse = AtualizarSelecaoMouse(jogo, &selecao_exclusao, 2, 245, 30, true);
        jogo->excluir_sim = selecao_exclusao == 0;
        if (entrada_esquerda_pressionada() || entrada_direita_pressionada()) jogo->excluir_sim = !jogo->excluir_sim;
        if (entrada_confirmar_pressionada() || confirmou_mouse) {
            if (jogo->excluir_sim) SalvamentoDefinirPadroes(&jogo->salvamento);
            SalvamentoGravar(&jogo->salvamento);
            jogo->tela = OPCOES;
        }
    } else if (jogo->tela == COMO_JOGAR) {
        if (entrada_cancelar_pressionada() || entrada_confirmar_pressionada()) jogo->tela = TITULO;
    } else if (jogo->tela == DEPURAR_EFEITOS) {
        bool trocar_padrao = false;
        if (IsKeyPressed(KEY_F9)) {
            jogo->id_depuracao_padrao =
                (jogo->id_depuracao_padrao + PADRAO_TOTAL - 1) % PADRAO_TOTAL;
            trocar_padrao = true;
        }
        if (IsKeyPressed(KEY_F10)) {
            jogo->id_depuracao_padrao =
                (jogo->id_depuracao_padrao + 1) % PADRAO_TOTAL;
            trocar_padrao = true;
        }
        if (IsKeyPressed(KEY_PAGE_UP)) jogo->inimigo_depuracao_padrao =
            (jogo->inimigo_depuracao_padrao + INIMIGO_TOTAL - 1) % INIMIGO_TOTAL;
        if (IsKeyPressed(KEY_PAGE_DOWN)) jogo->inimigo_depuracao_padrao =
            (jogo->inimigo_depuracao_padrao + 1) % INIMIGO_TOTAL;
        if (IsKeyPressed(KEY_G)) {
            jogo->id_depuracao_padrao = PADRAO_SINO_GAS_SOMBRA;
            jogo->inimigo_depuracao_padrao = INIMIGO_CHEFE_SINEIRO;
            trocar_padrao = true;
        }
        if (IsKeyPressed(KEY_I)) jogo->batalha.defesa.depurar_invencivel =
            !jogo->batalha.defesa.depurar_invencivel;
        if (trocar_padrao || IsKeyPressed(KEY_R)) {
            const bool ignorar_dano = jogo->batalha.defesa.depurar_invencivel;
            JogadorInicializar(&jogo->jogador);
            DefesaIniciar(&jogo->batalha.defesa, (IdPadraoProjetil)jogo->id_depuracao_padrao,
                         (Rectangle){42, 228, 556, 94}, 0, 0, 1.0f, 1.0f);
            jogo->batalha.defesa.depurar_invencivel = ignorar_dano;
        }
        ResultadoDefesa resultado_depuracao =
            DefesaAtualizar(&jogo->batalha.defesa, &jogo->jogador, GetFrameTime());
        if (resultado_depuracao != DEFESA_EM_ANDAMENTO) {
            const bool ignorar_dano = jogo->batalha.defesa.depurar_invencivel;
            JogadorInicializar(&jogo->jogador);
            DefesaIniciar(&jogo->batalha.defesa, (IdPadraoProjetil)jogo->id_depuracao_padrao,
                         (Rectangle){42, 228, 556, 94}, 0, 0, 1.0f, 1.0f);
            jogo->batalha.defesa.depurar_invencivel = ignorar_dano;
        }
        if (entrada_cancelar_pressionada()) jogo->tela = jogo->tela_antes_pausa;
    }

    if (IsKeyPressed(KEY_F11)) { ToggleFullscreen(); jogo->salvamento.tela_cheia = !jogo->salvamento.tela_cheia; }
    if (IsKeyPressed(KEY_F3)) jogo->telemetria_ativada = !jogo->telemetria_ativada;
#ifndef NDEBUG
    if (IsKeyPressed(KEY_F6)) SpritesRecarregar(&jogo->sprites);
    if (IsKeyPressed(KEY_F7)) jogo->sprites.depurar_limites = !jogo->sprites.depurar_limites;
#endif
    if (jogo->tela == BATALHA) {
        if (jogo->dialogo.caracteres_visiveis < jogo->ultimo_caractere_digitado) {
            jogo->ultimo_caractere_digitado = 0;
        }
        if (jogo->dialogo.caracteres_visiveis > jogo->ultimo_caractere_digitado) {
            AudioTocar(&jogo->audio, AUDIO_TEXTO);
        }
        jogo->ultimo_caractere_digitado = jogo->dialogo.caracteres_visiveis;
    } else jogo->ultimo_caractere_digitado = 0;
    jogo->renderizacao.tremor_linha = jogo->salvamento.tremor_linha;
    if (jogo->salvamento.efeitos_reduzidos) jogo->batalha.temporizador_tremor_tela = 0.0f;
    if (jogo->salvamento.efeitos_reduzidos) {
        jogo->batalha.temporizador_pausa_impacto = 0.0f;
        for (int indice = 0; indice < MAXIMO_BATALHA_PARTICULAS; indice++) jogo->batalha.particulas[indice].ativo = false;
    }
    RenderizacaoAtualizar(&jogo->renderizacao, GetFrameTime());
    AudioDefinirVolumes(&jogo->audio, jogo->salvamento.volume_musica, jogo->salvamento.volume_efeitos);
    AudioSolicitarFaixa(&jogo->audio, jogo->tela == LOJA || jogo->tela == DESCANSO ? MUSICA_CALMA :
                      (jogo->tela == BATALHA && jogo->inimigo.chefe ? MUSICA_CHEFE : MUSICA_COMUM));
    AudioAtualizar(&jogo->audio, GetFrameTime());
}

void JogoDesenhar(const EstadoJogo *jogo)
{
    RenderizacaoDesenharFundo(&jogo->renderizacao, jogo->tempo_decorrido);
    if (jogo->tela != TITULO) {
        const IdSprite fundo = jogo->inimigo.chefe ? SPRITE_FUNDO_CHEFE : SPRITE_FUNDO_PASTEL;
        SpriteDesenharAjustado(&jogo->sprites, fundo, (Rectangle){0, 0, LOGICO_LARGURA, LOGICO_ALTURA}, WHITE);
    }
    if (jogo->tela == TITULO) {
        DesenharTitulo(jogo);
    } else if (jogo->tela == BATALHA) {
        BatalhaDesenhar(&jogo->batalha, &jogo->jogador, &jogo->inimigo, &jogo->dialogo, &jogo->qte,
                   jogo->renderizacao.tremor_linha && !jogo->salvamento.efeitos_reduzidos, jogo->salvamento.efeitos_reduzidos,
                   &jogo->sprites);
        if (jogo->temporizador_tutorial > 0.0f) DesenharTutorial(jogo);
        if (jogo->temporizador_transicao > 0.0f) {
            InterfaceDesenharPainel((Rectangle){198, 193, 244, 82});
            DrawText("SUBINDO...", 252, 215, 25, BLACK);
            DrawText(TextFormat("ANDAR %d", jogo->torre.andar), 276, 246, 16, DARKGRAY);
        }
    } else if (jogo->tela == PAUSA) {
        DesenharPausa(jogo);
    } else if (jogo->tela == RECOMPENSA) {
        DesenharRecompensa(jogo);
    } else if (jogo->tela == DESCANSO) {
        DesenharDescanso(jogo);
    } else if (jogo->tela == LOJA) {
        DesenharLoja(jogo);
    } else if (jogo->tela == FIM_JOGO) {
        InterfaceDesenharPainel((Rectangle){62, 120, 516, 250});
        DrawText("FIM DA RUN", 225, 152, 30, BLACK);
        DrawText(TextFormat("Andar %d  |  Moedas %d", jogo->torre.andar, jogo->torre.moedas),
                 190, 206, 18, DARKGRAY);
        DesenharEscolha(270, "TENTAR DE NOVO", jogo->selecao_final == 0);
        DesenharEscolha(310, "TITULO", jogo->selecao_final == 1);
    } else if (jogo->tela == VITORIA) {
        InterfaceDesenharPainel((Rectangle){62, 104, 516, 276});
        DrawText("TORRE CONCLUIDA", 185, 136, 29, BLACK);
        DrawText(TextFormat("10 andares | %d moedas", jogo->torre.moedas), 205, 190, 18, DARKGRAY);
        DrawText(TextFormat("Poupar: %d  |  Dano: %d", jogo->torre.vitorias_misericordia,
                            jogo->torre.vitorias_dano), 205, 218, 18, DARKGRAY);
        DesenharEscolha(292, "TITULO", true);
    } else if (jogo->tela == RELIQUIAS) {
        DesenharReliquias(jogo);
    } else if (jogo->tela == OPCOES) {
        DesenharOpcoes(jogo);
    } else if (jogo->tela == CONFIRMAR_EXCLUSAO) {
        DesenharConfirmacaoExclusao(jogo);
    } else if (jogo->tela == COMO_JOGAR) {
        DesenharComoJogar();
    } else if (jogo->tela == DEPURAR_EFEITOS) {
        DesenharDepuracaoEfeitos(jogo);
    } else {
        DesenharMarcador("EM BREVE", "Este estado sera implementado nas proximas fases.");
    }
    if ((jogo->tela == BATALHA || jogo->tela == DEPURAR_EFEITOS) && !IsWindowFocused()) {
        DrawRectangle(0, 0, LOGICO_LARGURA, LOGICO_ALTURA, (Color){0, 0, 0, 135});
        InterfaceDesenharPainel((Rectangle){142, 190, 356, 92});
        DrawText("TECLADO PAUSADO", 218, 208, 20, BLACK);
        DrawText("Clique na janela para continuar", 207, 241, 14, DARKGRAY);
    }
}
