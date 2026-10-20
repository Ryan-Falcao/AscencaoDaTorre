#include "game.h"
#include "input.h"
#include "balance.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

static Color CorRaridade(RaridadeItem raridade);

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
    const bool tem_fundo_titulo = sprite_obter(&jogo->sprites, SPRITE_FUNDO_TITULO) != NULL;

    if (tem_fundo_titulo) {
        SpriteDesenharAjustado(&jogo->sprites, SPRITE_FUNDO_TITULO,
                               (Rectangle){0.0f, 0.0f, LOGICO_LARGURA, LOGICO_ALTURA}, WHITE);
    } else {
        DrawCircle(92, 90, 44, (Color){178, 220, 239, 255});
        DrawCircle(551, 101, 30, (Color){255, 207, 144, 255});
    }
    if (!tem_fundo_titulo && sprite_obter(&jogo->sprites, SPRITE_TITULO_TORRE) != NULL) {
        SpriteDesenharAjustado(&jogo->sprites, SPRITE_TITULO_TORRE,
                               (Rectangle){250, 12, 140, 210}, WHITE);
    } else if (!tem_fundo_titulo) {
        DesenharTorre(jogo->tempo_decorrido);
    }

    if (tem_fundo_titulo) DrawRectangle(156, 218, 328, 84, (Color){255, 249, 236, 188});
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
    if (recompensa->categoria == RECOMPENSA_EQUIPAMENTO) return ItemObterDefinicao((IdItem)recompensa->id)->nome;
    if (recompensa->categoria == RECOMPENSA_ITEM) return ItemObterDefinicao((IdItem)recompensa->id)->nome;
    return recompensa->categoria == RECOMPENSA_VIDA ? "+ HP MAX" : "+ DANO BASE";
}

static Rectangle AreaRecompensa(int indice)
{
    return (Rectangle){52.0f, 148.0f + indice * 84.0f, 536.0f, 76.0f};
}

static bool RecompensaEhItem(const RecompensaTorre *recompensa)
{
    return recompensa->categoria == RECOMPENSA_EQUIPAMENTO || recompensa->categoria == RECOMPENSA_ITEM;
}

static void DesenharRecompensa(const EstadoJogo *jogo)
{
    InterfaceDesenharPainel((Rectangle){30, 34, 580, 414});
    DrawText("ITENS RECEBIDOS", 193, 56, 26, BLACK);
    DrawText(TextFormat("%d moedas  |  %d XP  |  %s", jogo->ultimo_resultado_batalha.moedas,
                        jogo->ultimo_resultado_batalha.xp,
                        jogo->ultimo_resultado_batalha.desfecho == BATALHA_OUTCOME_MISERICORDIA ? "POUPAR: BONUS" : "VITORIA"),
             102, 94, 16, DARKGRAY);
    DrawText("Escolha uma recompensa", 52, 121, 16, BLACK);
    for (int indice = 0; indice < 3; indice++) {
        const RecompensaTorre *recompensa = &jogo->torre.recompensas[indice];
        const Rectangle area = AreaRecompensa(indice);
        const bool selecionada = jogo->selecao_recompensa == indice;
        DrawRectangleRec(area, selecionada ? (Color){248, 220, 129, 255} : (Color){232, 230, 224, 255});
        DrawRectangleLinesEx(area, selecionada ? 3.0f : 1.0f, selecionada ? MAROON : DARKGRAY);

        if (RecompensaEhItem(recompensa)) {
            const DefinicaoItem *item = ItemObterDefinicao((IdItem)recompensa->id);
            if (sprite_obter(&jogo->sprites, SpriteIdItem(item->id)) != NULL)
                SpriteDesenharCentralizadoEscalado(&jogo->sprites, SpriteIdItem(item->id),
                    (Vector2){area.x + 38.0f, area.y + 38.0f}, WHITE, 0.0f, 1.65f,
                    jogo->sprites.depurar_limites);
            DrawText(item->nome, (int)area.x + 76, (int)area.y + 9, 17,
                     item->categoria == CATEGORIA_EQUIPAMENTO ? CorRaridade(item->raridade) : BLACK);
            if (item->categoria == CATEGORIA_EQUIPAMENTO) {
                const char *tipo = item->tipo_elemental != TIPO_NEUTRO ? nome_tipo(item->tipo_elemental) : NULL;
                DrawText(tipo != NULL ? TextFormat("%s | %s | TIPO %s", ItemNomeRaridade(item->raridade),
                                                   ItemNomeEspaco(item->espaco), tipo) :
                                        TextFormat("%s | %s", ItemNomeRaridade(item->raridade),
                                                   ItemNomeEspaco(item->espaco)),
                         (int)area.x + 76, (int)area.y + 31, 12, CorRaridade(item->raridade));
            } else {
                DrawText(TextFormat("CONSUMIVEL | quantidade %d", recompensa->valor),
                         (int)area.x + 76, (int)area.y + 31, 12, DARKGRAY);
            }
            DrawText(item->descricao, (int)area.x + 76, (int)area.y + 50, 12, DARKGRAY);
        } else {
            const bool vida = recompensa->categoria == RECOMPENSA_VIDA;
            if (vida) {
                DrawCircle((int)area.x + 31, (int)area.y + 32, 11, (Color){210, 67, 77, 255});
                DrawCircle((int)area.x + 45, (int)area.y + 32, 11, (Color){210, 67, 77, 255});
                DrawTriangle((Vector2){area.x + 21, area.y + 36}, (Vector2){area.x + 55, area.y + 36},
                             (Vector2){area.x + 38, area.y + 60}, (Color){210, 67, 77, 255});
            } else {
                DrawLineEx((Vector2){area.x + 25, area.y + 55}, (Vector2){area.x + 51, area.y + 20},
                           6.0f, (Color){92, 95, 105, 255});
                DrawLineEx((Vector2){area.x + 23, area.y + 43}, (Vector2){area.x + 38, area.y + 55},
                           4.0f, (Color){92, 95, 105, 255});
            }
            DrawText(NomeRecompensa(recompensa), (int)area.x + 76, (int)area.y + 13, 17, BLACK);
            DrawText(vida ? TextFormat("Vida maxima %+d", recompensa->valor) :
                            TextFormat("Poder de ataque permanente %+d", recompensa->valor / 2),
                     (int)area.x + 76, (int)area.y + 42, 13, DARKGRAY);
        }
    }
    if (jogo->mensagem_tela[0] != '\0') DrawText(jogo->mensagem_tela, 52, 406, 12, MAROON);
    InterfaceDesenharAtalho("I inventario  |  ESPACO escolher", 196, 426);
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
    DesenharEscolha(264, "Espada da Luz inicial - 10 fragmentos", jogo->selecao_reliquia == 1);
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

static Color CorRaridade(RaridadeItem raridade)
{
    static const Color cores[] = {
        {105, 108, 112, 255}, {54, 116, 196, 255},
        {145, 67, 184, 255}, {214, 143, 25, 255}
    };
    return cores[raridade >= RARIDADE_COMUM && raridade <= RARIDADE_LENDARIA ? raridade : 0];
}

static Rectangle AreaInventario(int indice)
{
    return (Rectangle){42.0f + (indice % 6) * 43.0f, 126.0f + (indice / 6) * 43.0f, 38.0f, 38.0f};
}

static Rectangle AreaEquipamento(int espaco)
{
    static const Vector2 posicoes[ESPACO_EQUIPADO_TOTAL] = {
        {401, 105}, {501, 105}, {501, 180}, {401, 180}, {451, 225},
        {373, 267}, {451, 267}, {529, 267}, {548, 225}
    };
    return (Rectangle){posicoes[espaco].x, posicoes[espaco].y, 38.0f, 38.0f};
}

static void DesenharIconeEquipamento(EspacoEquipamento espaco, Vector2 centro, Color cor)
{
    if (espaco == ESPACO_ARMADURA) {
        DrawRectangleLines((int)centro.x - 8, (int)centro.y - 8, 16, 18, cor);
        DrawLine((int)centro.x - 8, (int)centro.y - 8, (int)centro.x - 13, (int)centro.y - 3, cor);
        DrawLine((int)centro.x + 8, (int)centro.y - 8, (int)centro.x + 13, (int)centro.y - 3, cor);
    } else if (espaco == ESPACO_ESCUDO) {
        DrawPolyLines(centro, 5, 11, -90, cor);
        DrawLine((int)centro.x, (int)centro.y - 8, (int)centro.x, (int)centro.y + 8, cor);
    } else if (espaco == ESPACO_ARMAMENTO) {
        DrawLineEx((Vector2){centro.x - 8, centro.y + 9}, (Vector2){centro.x + 8, centro.y - 8}, 3, cor);
        DrawLineEx((Vector2){centro.x - 10, centro.y + 3}, (Vector2){centro.x - 3, centro.y + 10}, 2, cor);
    } else if (espaco == ESPACO_CALCA) {
        DrawLineEx((Vector2){centro.x - 6, centro.y - 9}, (Vector2){centro.x - 7, centro.y + 9}, 5, cor);
        DrawLineEx((Vector2){centro.x + 6, centro.y - 9}, (Vector2){centro.x + 7, centro.y + 9}, 5, cor);
        DrawLineEx((Vector2){centro.x - 6, centro.y - 8}, (Vector2){centro.x + 6, centro.y - 8}, 4, cor);
    } else if (espaco == ESPACO_BOTAS) {
        DrawLineEx((Vector2){centro.x - 5, centro.y - 9}, (Vector2){centro.x - 5, centro.y + 7}, 5, cor);
        DrawLineEx((Vector2){centro.x - 5, centro.y + 7}, (Vector2){centro.x + 8, centro.y + 7}, 5, cor);
    } else if (espaco == ESPACO_AMULETO_TIPO) {
        DrawPolyLines(centro, 4, 9, 45, cor);
        DrawLine((int)centro.x - 10, (int)centro.y, (int)centro.x - 14, (int)centro.y, cor);
        DrawLine((int)centro.x + 10, (int)centro.y, (int)centro.x + 14, (int)centro.y, cor);
        DrawLine((int)centro.x, (int)centro.y - 10, (int)centro.x, (int)centro.y - 14, cor);
    } else {
        DrawCircleLines((int)centro.x, (int)centro.y + 4, 6, cor);
        DrawLine((int)centro.x - 8, (int)centro.y - 9, (int)centro.x, (int)centro.y - 2, cor);
        DrawLine((int)centro.x + 8, (int)centro.y - 9, (int)centro.x, (int)centro.y - 2, cor);
    }
}

static void DesenharBonusItem(const DefinicaoItem *item, int x, int y)
{
    const BonusAtributos *b = &item->bonus;
    int linha = 0;
#define BONUS(TEXTO, VALOR) do { if ((VALOR) != 0) { \
    DrawText(TextFormat(TEXTO " %+d", (VALOR)), x, y + linha * 15, 12, DARKGRAY); linha++; } } while (0)
    BONUS("Ataque", b->ataque); BONUS("Defesa", b->defesa); BONUS("Vida", b->vida);
    BONUS("Velocidade", b->velocidade); BONUS("Mana maxima", b->mana_maxima);
    BONUS("Mana/turno", b->mana_por_turno); BONUS("Critico %%", b->chance_critica);
    BONUS("Dano critico %%", b->dano_critico); BONUS("Sangramento", b->sangramento);
    BONUS("Resist. sangramento", b->resistencia_sangramento);
#undef BONUS
    if (item->passiva != PASSIVA_NENHUMA)
        DrawText(TextFormat("%s: %d%%", ItemNomePassiva(item->passiva), item->valor_passiva),
                 x, y + linha * 15, 12, MAROON);
}

static void DesenharInventario(const EstadoJogo *jogo)
{
    InterfaceDesenharPainel((Rectangle){20, 28, 600, 424});
    DrawText("INVENTARIO", 38, 48, 25, BLACK);
    DrawText("24 ESPACOS", 190, 55, 13, DARKGRAY);
    for (int indice = 0; indice < MAXIMO_INVENTARIO_ITENS; indice++) {
        const Rectangle area = AreaInventario(indice);
        const ItemInventario *pilha = &jogo->jogador.inventario[indice];
        const bool selecionado = !jogo->inventario_foco_equipamento && jogo->selecao_inventario == indice;
        DrawRectangleRec(area, selecionado ? (Color){248, 220, 129, 255} : (Color){215, 213, 207, 255});
        DrawRectangleLinesEx(area, selecionado ? 3.0f : 1.0f, selecionado ? MAROON : DARKGRAY);
        if (pilha->quantidade > 0) {
            const DefinicaoItem *item = ItemObterDefinicao(pilha->id);
            if (ItemEhEquipamento(pilha->id)) DrawRectangle((int)area.x + 3, (int)area.y + 3, 5,
                                                           (int)area.height - 6, CorRaridade(item->raridade));
            if (sprite_obter(&jogo->sprites, SpriteIdItem(pilha->id)) != NULL)
                SpriteDesenharCentralizadoEscalado(&jogo->sprites, SpriteIdItem(pilha->id),
                    (Vector2){area.x + 19, area.y + 18}, WHITE, 0.0f, 0.72f, jogo->sprites.depurar_limites);
            else if (ItemEhEquipamento(pilha->id))
                DesenharIconeEquipamento(item->espaco, (Vector2){area.x + 19, area.y + 18},
                                         CorRaridade(item->raridade));
            else DrawText("+", (int)area.x + 14, (int)area.y + 8, 20, BLACK);
            DrawText(TextFormat("%d", pilha->quantidade), (int)area.x + 27, (int)area.y + 25, 10, BLACK);
        }
    }

    DrawText("EQUIPADO", 413, 48, 20, BLACK);
    DrawCircle(470, 137, 17, (Color){91, 93, 105, 255});
    DrawRectangle(453, 154, 34, 67, (Color){91, 93, 105, 255});
    DrawLineEx((Vector2){453, 165}, (Vector2){430, 200}, 8, (Color){91, 93, 105, 255});
    DrawLineEx((Vector2){487, 165}, (Vector2){510, 200}, 8, (Color){91, 93, 105, 255});
    DrawLineEx((Vector2){461, 218}, (Vector2){448, 255}, 9, (Color){91, 93, 105, 255});
    DrawLineEx((Vector2){479, 218}, (Vector2){492, 255}, 9, (Color){91, 93, 105, 255});
    DrawText("TIPO", 553, 212, 9, DARKGRAY);
    for (int espaco = 0; espaco < ESPACO_EQUIPADO_TOTAL; espaco++) {
        const Rectangle area = AreaEquipamento(espaco);
        const bool selecionado = jogo->inventario_foco_equipamento && jogo->selecao_equipamento == espaco;
        const IdItem id = jogo->jogador.equipamentos[espaco];
        DrawRectangleRec(area, selecionado ? (Color){248, 220, 129, 255} : (Color){225, 223, 217, 255});
        DrawRectangleLinesEx(area, selecionado ? 3.0f : 1.0f, selecionado ? MAROON : DARKGRAY);
        if (id != ITEM_NENHUM) {
            const DefinicaoItem *item = ItemObterDefinicao(id);
            DrawRectangle((int)area.x + 3, (int)area.y + 3, 5, (int)area.height - 6, CorRaridade(item->raridade));
            if (sprite_obter(&jogo->sprites, SpriteIdItem(id)) != NULL)
                SpriteDesenharCentralizadoEscalado(&jogo->sprites, SpriteIdItem(id),
                    (Vector2){area.x + 20, area.y + 19}, WHITE, 0.0f, 1.0f,
                    jogo->sprites.depurar_limites);
            else DesenharIconeEquipamento((EspacoEquipamento)espaco,
                    (Vector2){area.x + 20, area.y + 19}, CorRaridade(item->raridade));
        } else DrawText("+", (int)area.x + 14, (int)area.y + 7, 22, GRAY);
    }

    const DefinicaoItem *selecionado = NULL;
    if (jogo->inventario_foco_equipamento) {
        const IdItem id = jogo->jogador.equipamentos[jogo->selecao_equipamento];
        if (id != ITEM_NENHUM) selecionado = ItemObterDefinicao(id);
    } else {
        const ItemInventario *pilha = &jogo->jogador.inventario[jogo->selecao_inventario];
        if (pilha->quantidade > 0) selecionado = ItemObterDefinicao(pilha->id);
    }
    DrawRectangle(38, 312, 564, 104, (Color){239, 238, 232, 255});
    DrawRectangleLines(38, 312, 564, 104, DARKGRAY);
    if (selecionado != NULL) {
        const Color cor = selecionado->categoria == CATEGORIA_EQUIPAMENTO ? CorRaridade(selecionado->raridade) : DARKGRAY;
        DrawText(selecionado->nome, 52, 324, 17, cor);
        DrawText(selecionado->descricao, 52, 348, 12, DARKGRAY);
        if (selecionado->categoria == CATEGORIA_EQUIPAMENTO) {
            DrawText(TextFormat("%s | %s", ItemNomeRaridade(selecionado->raridade),
                                ItemNomeEspaco(selecionado->espaco)), 52, 370, 12, cor);
            DesenharBonusItem(selecionado, 310, 324);
        }
    }
    DrawText(TextFormat("ATQ %d  DEF %d  VEL %+d%%  MANA %d (+%d)",
        jogo->jogador.bonus_equipamento.ataque, jogo->jogador.bonus_equipamento.defesa,
        jogo->jogador.bonus_equipamento.velocidade, jogo->jogador.mana_maxima,
        jogo->jogador.mana_por_turno), 52, 425, 12, BLACK);
    InterfaceDesenharAtalho("Q alternar | SETAS mover | ESPACO equipar | DEL descartar | TAB fechar", 62, 439);
}

static void DesenharPausa(const EstadoJogo *jogo)
{
    const char *opcoes[] = {"CONTINUAR", "INVENTARIO", "OPCOES", "COMO JOGAR", "DESISTIR DA RUN", "TITULO"};
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
    for (int indice = 0; indice < 6; indice++)
        DesenharEscolhaCompacta(158 + indice * 34, opcoes[indice], jogo->selecao_pausa == indice);
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
    JogadorIniciarBatalha(&jogo->jogador);
    JogadorIniciarTurno(&jogo->jogador);
    jogo->batalha.dificuldade_qte = BalanceamentoDificuldadeQte(jogo->torre.andar, jogo->salvamento.dificuldade);
    jogo->temporizador_transicao = 0.70f;
    jogo->tela = BATALHA;
}

static void IniciarPartida(EstadoJogo *jogo)
{
    jogo->contador_partidas++;
    jogo->partida_iniciada_em = (float)GetTime();
    JogadorInicializar(&jogo->jogador);
    jogo->jogador.multiplicador_dificuldade = BalanceamentoMultiplicadorDanoRecebido(jogo->salvamento.dificuldade);
    JogadorRecalcularAtributos(&jogo->jogador);
    if (jogo->salvamento.tipo_reliquia == RELIQUIA_ITEM) JogadorAdicionarItem(&jogo->jogador, (IdItem)jogo->salvamento.id_reliquia, 1);
    if (jogo->salvamento.tipo_reliquia == RELIQUIA_ARMAMENTO)
        JogadorAdicionarItem(&jogo->jogador,
            jogo->salvamento.id_reliquia == ATAQUE_CHAMA_ALTA ? ITEM_ESPADA_FOGO : ITEM_ESPADA_LUZ, 1);
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

    if (recompensa.categoria == RECOMPENSA_EQUIPAMENTO || recompensa.categoria == RECOMPENSA_ITEM) {
        if (!JogadorAdicionarItem(&jogo->jogador, (IdItem)recompensa.id, recompensa.valor)) {
            snprintf(jogo->mensagem_tela, sizeof(jogo->mensagem_tela),
                     "Inventario cheio. Abra espaco antes de escolher.");
            return;
        }
    } else if (recompensa.categoria == RECOMPENSA_VIDA) {
        jogo->jogador.vida_base += recompensa.valor;
        JogadorRecalcularAtributos(&jogo->jogador);
        JogadorCurar(&jogo->jogador, recompensa.valor * 2);
    } else {
        jogo->jogador.bonus_dano_permanente += recompensa.valor / 2;
    }
    ContinuarAposRecompensa(jogo);
}

static EspacoEquipamento EspacoPreferidoParaItem(const EstadoJogo *jogo, IdItem id)
{
    const DefinicaoItem *item = ItemObterDefinicao(id);
    if (item->espaco == ESPACO_AMULETO_TIPO) return ESPACO_AMULETO_TIPO;
    if (item->espaco != ESPACO_AMULETO_1) return item->espaco;
    for (int espaco = ESPACO_AMULETO_1; espaco <= ESPACO_AMULETO_3; espaco++)
        if (jogo->jogador.equipamentos[espaco] == ITEM_NENHUM) return (EspacoEquipamento)espaco;
    if (jogo->selecao_equipamento >= ESPACO_AMULETO_1 && jogo->selecao_equipamento <= ESPACO_AMULETO_3)
        return (EspacoEquipamento)jogo->selecao_equipamento;
    return ESPACO_AMULETO_1;
}

static void AtualizarInventario(EstadoJogo *jogo)
{
    if (IsKeyPressed(KEY_TAB)) {
        jogo->tela = jogo->tela_antes_inventario;
        return;
    }
    if (IsKeyPressed(KEY_Q)) jogo->inventario_foco_equipamento = !jogo->inventario_foco_equipamento;
    for (int indice = 0; indice < MAXIMO_INVENTARIO_ITENS; indice++) {
        if (CheckCollisionPointRec(jogo->posicao_mouse, AreaInventario(indice))) {
            jogo->selecao_inventario = indice;
            jogo->inventario_foco_equipamento = false;
            if (jogo->mouse_pressionado) {
                const ItemInventario *pilha = &jogo->jogador.inventario[indice];
                if (pilha->quantidade > 0 && ItemEhEquipamento(pilha->id))
                    JogadorEquiparDoInventario(&jogo->jogador, indice, EspacoPreferidoParaItem(jogo, pilha->id));
            }
        }
    }
    for (int espaco = 0; espaco < ESPACO_EQUIPADO_TOTAL; espaco++) {
        if (CheckCollisionPointRec(jogo->posicao_mouse, AreaEquipamento(espaco))) {
            jogo->selecao_equipamento = espaco;
            jogo->inventario_foco_equipamento = true;
            if (jogo->mouse_pressionado) JogadorDesequipar(&jogo->jogador, (EspacoEquipamento)espaco);
        }
    }
    if (jogo->inventario_foco_equipamento) {
        if (entrada_cima_pressionada() || entrada_esquerda_pressionada())
            jogo->selecao_equipamento = (jogo->selecao_equipamento + ESPACO_EQUIPADO_TOTAL - 1) % ESPACO_EQUIPADO_TOTAL;
        if (entrada_baixo_pressionada() || entrada_direita_pressionada())
            jogo->selecao_equipamento = (jogo->selecao_equipamento + 1) % ESPACO_EQUIPADO_TOTAL;
        if (entrada_confirmar_pressionada())
            JogadorDesequipar(&jogo->jogador, (EspacoEquipamento)jogo->selecao_equipamento);
    } else {
        if (entrada_esquerda_pressionada()) jogo->selecao_inventario =
            (jogo->selecao_inventario + MAXIMO_INVENTARIO_ITENS - 1) % MAXIMO_INVENTARIO_ITENS;
        if (entrada_direita_pressionada()) jogo->selecao_inventario =
            (jogo->selecao_inventario + 1) % MAXIMO_INVENTARIO_ITENS;
        if (entrada_cima_pressionada()) jogo->selecao_inventario =
            (jogo->selecao_inventario + MAXIMO_INVENTARIO_ITENS - 6) % MAXIMO_INVENTARIO_ITENS;
        if (entrada_baixo_pressionada()) jogo->selecao_inventario =
            (jogo->selecao_inventario + 6) % MAXIMO_INVENTARIO_ITENS;
        if (entrada_confirmar_pressionada()) {
            const ItemInventario *pilha = &jogo->jogador.inventario[jogo->selecao_inventario];
            if (pilha->quantidade > 0 && ItemEhEquipamento(pilha->id))
                JogadorEquiparDoInventario(&jogo->jogador, jogo->selecao_inventario,
                                           EspacoPreferidoParaItem(jogo, pilha->id));
        }
    }
    if (!jogo->inventario_foco_equipamento && IsKeyPressed(KEY_DELETE))
        JogadorDescartarItem(&jogo->jogador, jogo->selecao_inventario);
    if (entrada_cancelar_pressionada() || IsKeyPressed(KEY_ESCAPE)) jogo->tela = jogo->tela_antes_inventario;
}

void JogoInicializar(EstadoJogo *jogo)
{
    memset(jogo, 0, sizeof(*jogo));
    jogo->tela = TITULO;
    jogo->fase_espera_tutorial = -1;
    jogo->tela_antes_pausa = TITULO;
    jogo->tela_antes_inventario = TITULO;
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
    bool abriu_inventario = false;
    const Vector2 posicao_mouse_atual = jogo->posicao_mouse;
    const bool mouse_moveu = fabsf(posicao_mouse_atual.x - jogo->ultima_posicao_mouse.x) > 0.1f ||
                             fabsf(posicao_mouse_atual.y - jogo->ultima_posicao_mouse.y) > 0.1f;
    jogo->ultima_posicao_mouse = posicao_mouse_atual;
    if (!mouse_moveu && !jogo->mouse_pressionado)
        jogo->posicao_mouse = (Vector2){-1000.0f, -1000.0f};
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

    if (IsKeyPressed(KEY_TAB) &&
        (jogo->tela == BATALHA || jogo->tela == RECOMPENSA || jogo->tela == DESCANSO ||
         jogo->tela == LOJA || jogo->tela == PAUSA)) {
        jogo->tela_antes_inventario = jogo->tela;
        jogo->tela = INVENTARIO;
        abriu_inventario = true;
    }

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
            jogo->mensagem_tela[0] = '\0';
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
            bool confirmou_mouse = AtualizarSelecaoMouse(jogo, &jogo->selecao_pausa, 6, 158, 34, true);
            if (entrada_cima_pressionada()) jogo->selecao_pausa = (jogo->selecao_pausa + 5) % 6;
            if (entrada_baixo_pressionada()) jogo->selecao_pausa = (jogo->selecao_pausa + 1) % 6;
            if (IsKeyPressed(KEY_ESCAPE) || entrada_cancelar_pressionada()) RetomarPartidaPausada(jogo);
            if (entrada_confirmar_pressionada() || confirmou_mouse) {
                if (jogo->selecao_pausa == 0) RetomarPartidaPausada(jogo);
                else if (jogo->selecao_pausa == 1) {
                    jogo->tela_antes_inventario = PAUSA;
                    jogo->tela = INVENTARIO;
                }
                else if (jogo->selecao_pausa == 2) jogo->tela = OPCOES;
                else if (jogo->selecao_pausa == 3) jogo->tela = COMO_JOGAR;
                else if (jogo->selecao_pausa == 4) jogo->pausa_confirmar_saida = true;
                else jogo->tela = TITULO;
            }
        }
    } else if (jogo->tela == INVENTARIO) {
        if (!abriu_inventario) AtualizarInventario(jogo);
    } else if (jogo->tela == RECOMPENSA) {
        if (IsKeyPressed(KEY_I)) {
            jogo->tela_antes_inventario = RECOMPENSA;
            jogo->tela = INVENTARIO;
            return;
        }
        bool confirmou_mouse = false;
        for (int indice = 0; indice < 3; indice++) {
            if (CheckCollisionPointRec(jogo->posicao_mouse, AreaRecompensa(indice))) {
                jogo->selecao_recompensa = indice;
                confirmou_mouse = jogo->mouse_pressionado;
            }
        }
        if (entrada_cima_pressionada()) jogo->selecao_recompensa = (jogo->selecao_recompensa + 2) % 3;
        if (entrada_baixo_pressionada()) jogo->selecao_recompensa = (jogo->selecao_recompensa + 1) % 3;
        if (entrada_confirmar_pressionada() || confirmou_mouse)
            AplicarRecompensa(jogo, jogo->selecao_recompensa);
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
            else SalvamentoComprarReliquia(&jogo->salvamento, RELIQUIA_ARMAMENTO, ATAQUE_RAIO_MANSO, 10);
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
        if (entrada_cancelar_pressionada()) {
            SalvamentoGravar(&jogo->salvamento);
            jogo->tela = jogo->tela_antes_pausa == BATALHA ? PAUSA : TITULO;
        }
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
        if (entrada_cancelar_pressionada() || entrada_confirmar_pressionada())
            jogo->tela = jogo->tela_antes_pausa == BATALHA ? PAUSA : TITULO;
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
        const IdSprite fundo = SpriteIdFundoTipo(TorreTipoDoAndar(jogo->torre.andar));
        if (sprite_obter(&jogo->sprites, fundo) != NULL) {
            SpriteDesenharAjustado(&jogo->sprites, fundo,
                                  (Rectangle){0, 0, LOGICO_LARGURA, LOGICO_ALTURA}, WHITE);
        } else {
            const IdSprite fallback = jogo->inimigo.chefe ? SPRITE_FUNDO_CHEFE : SPRITE_FUNDO_PASTEL;
            SpriteDesenharAjustado(&jogo->sprites, fallback,
                                  (Rectangle){0, 0, LOGICO_LARGURA, LOGICO_ALTURA}, WHITE);
        }
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
    } else if (jogo->tela == INVENTARIO) {
        DesenharInventario(jogo);
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
        DrawText(TextFormat("%d andares | %d moedas", TOTAL_ANDARES_TORRE, jogo->torre.moedas), 205, 190, 18, DARKGRAY);
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
