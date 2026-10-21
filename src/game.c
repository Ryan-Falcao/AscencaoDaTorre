#include "pilot_patterns.h"
#include "ui.h"
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
    return (Rectangle){170.0f, 316.0f + indice_opcao * 32.0f, 300.0f, 29.0f};
}

static void DesenharBotaoTitulo(const EstadoJogo *jogo, Rectangle botao, const char *rotulo,
                                bool selecionado)
{
    InterfaceDesenharBotao(&jogo->sprites, botao, rotulo, selecionado, true,
                           (Color){225, 95, 122, 255});
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

    if (sprite_obter(&jogo->sprites, SPRITE_LOGO_ASCENSAO_TORRE) != NULL) {
        SpriteDesenharAjustado(&jogo->sprites, SPRITE_LOGO_ASCENSAO_TORRE,
                               (Rectangle){156.0f, 54.0f, 328.0f, 142.0f}, WHITE);
    } else {
        const char *titulo = "ASCENSAO";
        const char *subtitulo = "DA TORRE";
        InterfaceDesenharTexto(titulo, (LOGICO_LARGURA - InterfaceMedirTexto(titulo, 42)) / 2, 225, 42, BLACK);
        InterfaceDesenharTexto(subtitulo, (LOGICO_LARGURA - InterfaceMedirTexto(subtitulo, 23)) / 2, 266, 23,
                 (Color){73, 77, 85, 255});
    }
    InterfaceDesenharTexto(TextFormat("melhor andar: %d  |  vitorias: %d", jogo->salvamento.melhor_andar,
                        jogo->salvamento.vitorias), 188, 298, 13, (Color){86, 88, 96, 255});

    for (int indice_opcao = 0; indice_opcao < 5; indice_opcao++) {
        const Rectangle botao = ObterBotaoTitulo(indice_opcao);
        const bool selecionado = jogo->selecao_titulo == indice_opcao;

        DesenharBotaoTitulo(jogo, botao, opcoes[indice_opcao], selecionado);
    }
}

static void DesenharMarcador(const char *titulo, const char *subtitulo)
{
    InterfaceDesenharPainel((Rectangle){62.0f, 122.0f, 516.0f, 238.0f});
    InterfaceDesenharTexto(titulo, 96, 160, 32, BLACK);
    InterfaceDesenharTextoQuebrado(subtitulo, (Rectangle){96, 218, 448, 70}, 18, 3, (Color){75, 78, 84, 255});
    InterfaceDesenharTexto("Aperte X para voltar ao titulo.", 96, 306, 16, (Color){105, 106, 111, 255});
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
    InterfaceDesenharTextoAjustado(texto, (Rectangle){112, y, 426, 19}, 17, BLACK);
}

static void DesenharEscolhaCompacta(int y, const char *texto, bool selecionado)
{
    const Rectangle area = ObterAreaEscolhaCompacta(y);
    InterfaceDesenharLinhaOpcao(area, selecionado, true, (Color){225, 95, 122, 255});
    InterfaceDesenharTextoAjustado(texto, (Rectangle){186, y, 278, 19}, 17, BLACK);
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
    InterfaceDesenharTexto("ITENS RECEBIDOS", (640 - InterfaceMedirTexto("ITENS RECEBIDOS", 26)) / 2, 56, 26, BLACK);
    InterfaceDesenharTexto(TextFormat("%d moedas  |  %d XP  |  %s", jogo->ultimo_resultado_batalha.moedas,
                        jogo->ultimo_resultado_batalha.xp,
                        jogo->ultimo_resultado_batalha.desfecho == BATALHA_OUTCOME_MISERICORDIA ? "POUPAR: BONUS" : "VITORIA"),
             102, 94, 16, DARKGRAY);
    InterfaceDesenharTexto("Escolha uma recompensa", 52, 121, 16, BLACK);
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
            InterfaceDesenharTexto(item->nome, (int)area.x + 76, (int)area.y + 9, 17,
                     item->categoria == CATEGORIA_EQUIPAMENTO ? CorRaridade(item->raridade) : BLACK);
            if (item->categoria == CATEGORIA_EQUIPAMENTO) {
                const char *tipo = item->tipo_elemental != TIPO_NEUTRO ? nome_tipo(item->tipo_elemental) : NULL;
                InterfaceDesenharTexto(tipo != NULL ? TextFormat("%s | %s | TIPO %s", ItemNomeRaridade(item->raridade),
                                                   ItemNomeEspaco(item->espaco), tipo) :
                                        TextFormat("%s | %s", ItemNomeRaridade(item->raridade),
                                                   ItemNomeEspaco(item->espaco)),
                         (int)area.x + 76, (int)area.y + 31, 12, CorRaridade(item->raridade));
            } else {
                InterfaceDesenharTexto(TextFormat("CONSUMIVEL | quantidade %d", recompensa->valor),
                         (int)area.x + 76, (int)area.y + 31, 12, DARKGRAY);
            }
            InterfaceDesenharTextoQuebrado(item->descricao, (Rectangle){area.x + 76, area.y + 48, area.width - 88, 25}, 12, 1, DARKGRAY);
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
            InterfaceDesenharTexto(NomeRecompensa(recompensa), (int)area.x + 76, (int)area.y + 13, 17, BLACK);
            InterfaceDesenharTexto(vida ? TextFormat("Vida maxima %+d", recompensa->valor) :
                            TextFormat("Poder de ataque permanente %+d", recompensa->valor / 2),
                     (int)area.x + 76, (int)area.y + 42, 13, DARKGRAY);
        }
    }
    if (jogo->mensagem_tela[0] != '\0') InterfaceDesenharTexto(jogo->mensagem_tela, 52, 406, 12, MAROON);
    InterfaceDesenharAtalho("I inventario  |  ESPACO escolher", 196, 426);
}

static void DesenharDescanso(const EstadoJogo *jogo)
{
    InterfaceDesenharPainel((Rectangle){62, 104, 516, 260});
    InterfaceDesenharTexto("PONTO DE DESCANSO", (640 - InterfaceMedirTexto("PONTO DE DESCANSO", 26)) / 2, 132, 26, BLACK);
    InterfaceDesenharTexto(TextFormat("HP %d / %d", jogo->jogador.hp, jogo->jogador.vida_maxima), 225, 170, 18, DARKGRAY);
    DesenharEscolha(222, "DESCANSAR  (+50% HP)", jogo->selecao_descanso == 0);
    DesenharEscolha(264, "TREINAR  (+1 dano permanente)", jogo->selecao_descanso == 1);
}

static void DesenharLoja(const EstadoJogo *jogo)
{
    InterfaceDesenharPainel((Rectangle){62, 78, 516, 322});
    InterfaceDesenharTexto("LOJA DA TORRE", (640 - InterfaceMedirTexto("LOJA DA TORRE", 26)) / 2, 104, 26, BLACK);
    InterfaceDesenharTexto(TextFormat("MOEDAS: %d", jogo->torre.moedas), 235, 140, 18, DARKGRAY);
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
        InterfaceDesenharTextoQuebrado(ItemObterDefinicao(jogo->torre.itens_loja[jogo->selecao_loja])->descricao,
                 (Rectangle){103, 354, 446, 26}, 13, 1, DARKGRAY);
    }
    if (jogo->mensagem_tela[0] != '\0') InterfaceDesenharTexto(jogo->mensagem_tela, 103, 382, 14, MAROON);
}

static void DesenharReliquias(const EstadoJogo *jogo)
{
    InterfaceDesenharPainel((Rectangle){62, 104, 516, 270});
    InterfaceDesenharTexto("RELIQUIAS", (640 - InterfaceMedirTexto("RELIQUIAS", 28)) / 2, 132, 28, BLACK);
    InterfaceDesenharTexto(TextFormat("FRAGMENTOS: %d", jogo->salvamento.fragmentos), 218, 169, 18, DARKGRAY);
    DesenharEscolha(220, "Erva Lunar inicial - 8 fragmentos", jogo->selecao_reliquia == 0);
    DesenharEscolha(264, "Espada da Luz inicial - 10 fragmentos", jogo->selecao_reliquia == 1);
    InterfaceDesenharTexto("X/Backspace: voltar", 205, 331, 14, DARKGRAY);
}

static void DesenharOpcoes(const EstadoJogo *jogo)
{
    static const char *difficulty_names[] = {"CALMO", "NORMAL", "DIFICIL"};
    InterfaceDesenharPainel((Rectangle){62, 30, 516, 420});
    InterfaceDesenharTexto("OPCOES", (640 - InterfaceMedirTexto("OPCOES", 28)) / 2, 55, 28, BLACK);
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
    InterfaceDesenharTexto("DICA", 72, 179, 14, (Color){203, 84, 104, 255});
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
    InterfaceDesenharTexto("COMO JOGAR", (640 - InterfaceMedirTexto("COMO JOGAR", 26)) / 2, 57, 26, BLACK);
    for (int indice = 0; indice < 6; indice++) {
        int y = 98 + indice * 49;
        InterfaceDesenharTexto(titulos[indice], 72, y, 12, (Color){203, 84, 104, 255});
        InterfaceDesenharTextoQuebrado(textos[indice],
                    (Rectangle){72, (float)y + 15, 496, 31}, 14, 2, BLACK);
    }
    InterfaceDesenharAtalho("X  voltar", 478, 410);
}

static void DesenharDepuracaoEfeitos(const EstadoJogo *jogo)
{
    const IdSprite enemy_id = SpriteIdInimigo((IdInimigo)jogo->inimigo_depuracao_padrao, false);

    InterfaceDesenharPainel((Rectangle){12, 12, 616, 82});
    InterfaceDesenharPainel((Rectangle){12, 428, 616, 46});
    InterfaceDesenharTexto("TESTE DE PADROES", (640 - InterfaceMedirTexto("TESTE DE PADROES", 25)) / 2, 28, 25, BLACK);
    InterfaceDesenharTexto(TextFormat("Padrao %d | inimigo %d", jogo->id_depuracao_padrao, jogo->inimigo_depuracao_padrao),
             70, 66, 18, DARKGRAY);
    InterfaceDesenharTexto(TextFormat("HP %d/%d | dano: %s", jogo->jogador.hp, jogo->jogador.vida_maxima,
                        jogo->batalha.defesa.depurar_invencivel ? "ignorado" : "ativo"),
             70, 90, 14, DARKGRAY);
    InterfaceDesenharTexto("WASD/setas: mover | SHIFT: dash | ESPACO: pular | R: reiniciar | G: Gas Sombrio", 20, 438, 11, DARKGRAY);
    InterfaceDesenharTexto("F9/F10: padrao | PgUp/PgDn: inimigo | I: alternar dano | X: voltar", 35, 454, 11, DARKGRAY);
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

static void DesenharResumoBonusItem(const BonusAtributos *bonus, int x, int y, Color cor)
{
    InterfaceDesenharTexto(TextFormat("ATQ %+d   DEF %+d   VIDA %+d", bonus->ataque, bonus->defesa, bonus->vida),
             x, y, 11, cor);
    InterfaceDesenharTexto(TextFormat("MANA %+d   REC %+d/t   VEL %+d%%", bonus->mana_maxima,
                        bonus->mana_por_turno, bonus->velocidade), x, y + 14, 11, cor);
}

static Vector2 PosicaoEquipamentoNoBoneco(EspacoEquipamento espaco)
{
    static const Vector2 posicoes[ESPACO_EQUIPADO_TOTAL] = {
        {470, 173}, {444, 184}, {506, 181}, {461, 224}, {486, 249},
        {457, 143}, {470, 139}, {483, 143}, {470, 190}
    };
    return espaco >= 0 && espaco < ESPACO_EQUIPADO_TOTAL ? posicoes[espaco] : (Vector2){470, 190};
}

static void DesenharComparacaoEquipamento(const EstadoJogo *jogo, const DefinicaoItem *item, int x, int y)
{
    if (item == NULL || item->categoria != CATEGORIA_EQUIPAMENTO) return;
    EspacoEquipamento espaco = item->espaco;
    if (espaco == ESPACO_AMULETO_1 && jogo->inventario_foco_equipamento)
        espaco = (EspacoEquipamento)jogo->selecao_equipamento;
    const IdItem equipado = espaco >= 0 && espaco < ESPACO_EQUIPADO_TOTAL ?
        jogo->jogador.equipamentos[espaco] : ITEM_NENHUM;
    const DefinicaoItem *atual = equipado != ITEM_NENHUM ? ItemObterDefinicao(equipado) : NULL;
    InterfaceDesenharTexto("COMPARACAO", x, y, 10, DARKGRAY);
    const char *nome_atual = atual != NULL ? atual->nome : "Espaco vazio";
    InterfaceDesenharTextoAjustado(TextFormat("USANDO: %s", nome_atual), (Rectangle){x, y + 12, 249, 12}, 10, DARKGRAY);
    const BonusAtributos vazio = {0};
    const BonusAtributos *base = atual != NULL ? &atual->bonus : &vazio;
    const BonusAtributos *novo = &item->bonus;
    InterfaceDesenharTexto(TextFormat("DELTA  ATQ %+d  DEF %+d  VIDA %+d", novo->ataque - base->ataque,
                        novo->defesa - base->defesa, novo->vida - base->vida), x, y + 27, 10,
             (novo->ataque + novo->defesa + novo->vida) >= (base->ataque + base->defesa + base->vida) ?
                 (Color){37, 126, 74, 255} : MAROON);
    InterfaceDesenharTexto(TextFormat("MANA %+d  REC %+d/t  VEL %+d%%", novo->mana_maxima - base->mana_maxima,
                        novo->mana_por_turno - base->mana_por_turno,
                        novo->velocidade - base->velocidade), x, y + 42, 10, DARKBLUE);
}

static void DesenharInventario(const EstadoJogo *jogo)
{
    InterfaceDesenharPainel((Rectangle){20, 28, 600, 424});
    InterfaceDesenharTexto("INVENTARIO", 38, 48, 25, BLACK);
    InterfaceDesenharTexto("24 ESPACOS", 190, 55, 13, DARKGRAY);
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
            else InterfaceDesenharTexto("+", (int)area.x + 14, (int)area.y + 8, 20, BLACK);
            InterfaceDesenharTexto(TextFormat("%d", pilha->quantidade), (int)area.x + 27, (int)area.y + 25, 10, BLACK);
        }
    }

    InterfaceDesenharTexto("EQUIPADO", 413, 48, 20, BLACK);
    DrawCircle(470, 137, 17, (Color){91, 93, 105, 255});
    DrawRectangle(453, 154, 34, 67, (Color){91, 93, 105, 255});
    DrawLineEx((Vector2){453, 165}, (Vector2){430, 200}, 8, (Color){91, 93, 105, 255});
    DrawLineEx((Vector2){487, 165}, (Vector2){510, 200}, 8, (Color){91, 93, 105, 255});
    DrawLineEx((Vector2){461, 218}, (Vector2){448, 255}, 9, (Color){91, 93, 105, 255});
    DrawLineEx((Vector2){479, 218}, (Vector2){492, 255}, 9, (Color){91, 93, 105, 255});
    InterfaceDesenharTexto("SEU EQUIPAMENTO", 407, 76, 12, DARKGRAY);
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
            /* O mesmo item aparece no boneco: a silhueta agora comunica o que esta vestido. */
            const Vector2 posicao_boneco = PosicaoEquipamentoNoBoneco((EspacoEquipamento)espaco);
            if (sprite_obter(&jogo->sprites, SpriteIdItem(id)) != NULL)
                SpriteDesenharCentralizadoEscalado(&jogo->sprites, SpriteIdItem(id), posicao_boneco,
                    WHITE, 0.0f, 0.62f, jogo->sprites.depurar_limites);
            else DesenharIconeEquipamento((EspacoEquipamento)espaco, posicao_boneco,
                                          CorRaridade(item->raridade));
        } else InterfaceDesenharTexto("+", (int)area.x + 14, (int)area.y + 7, 22, GRAY);
        InterfaceDesenharTexto(ItemNomeEspaco((EspacoEquipamento)espaco), (int)area.x - 3,
                 (int)area.y + (int)area.height + 1, 8, DARKGRAY);
    }

    const DefinicaoItem *selecionado = NULL;
    if (jogo->inventario_foco_equipamento) {
        const IdItem id = jogo->jogador.equipamentos[jogo->selecao_equipamento];
        if (id != ITEM_NENHUM) selecionado = ItemObterDefinicao(id);
    } else {
        const ItemInventario *pilha = &jogo->jogador.inventario[jogo->selecao_inventario];
        if (pilha->quantidade > 0) selecionado = ItemObterDefinicao(pilha->id);
    }
    InterfaceDesenharPainel((Rectangle){38, 294, 564, 122});
    if (selecionado != NULL) {
        const Color cor = selecionado->categoria == CATEGORIA_EQUIPAMENTO ? CorRaridade(selecionado->raridade) : DARKGRAY;
        InterfaceDesenharTextoAjustado(selecionado->nome, (Rectangle){52, 305, 532, 20}, 17, cor);
        InterfaceDesenharTextoQuebrado(selecionado->descricao, (Rectangle){52, 327, 260, 24}, 10, 1, DARKGRAY);
        if (selecionado->categoria == CATEGORIA_EQUIPAMENTO) {
            InterfaceDesenharTexto(TextFormat("%s | %s", ItemNomeRaridade(selecionado->raridade),
                                ItemNomeEspaco(selecionado->espaco)), 52, 354, 10, cor);
            InterfaceDesenharTexto("ESTE ITEM", 52, 374, 10, DARKGRAY);
            DesenharResumoBonusItem(&selecionado->bonus, 52, 386, cor);
            if (selecionado->passiva != PASSIVA_NENHUMA)
                InterfaceDesenharTexto(TextFormat("PASSIVA: %s", ItemNomePassiva(selecionado->passiva)), 52, 402, 10, MAROON);
            DesenharComparacaoEquipamento(jogo, selecionado, 335, 354);
        } else {
            InterfaceDesenharTexto("Consumivel: use durante a batalha.", 52, 366, 11, DARKGRAY);
        }
    }
    InterfaceDesenharTexto(TextFormat("ATQ %d  DEF %d  VEL %+d%%  MANA %d (+%d)",
        jogo->jogador.bonus_equipamento.ataque, jogo->jogador.bonus_equipamento.defesa,
        jogo->jogador.bonus_equipamento.velocidade, jogo->jogador.mana_maxima,
        jogo->jogador.mana_por_turno), 52, 421, 11, BLACK);
    InterfaceDesenharAtalho("Q alternar | ESPACO equipar/remover | X remover | DEL descartar | TAB fechar", 48, 438);
}

static void DesenharPausa(const EstadoJogo *jogo)
{
    const char *opcoes[] = {"CONTINUAR", "INVENTARIO", "OPCOES", "COMO JOGAR", "DESISTIR DA RUN", "TITULO"};
    InterfaceDesenharPainel((Rectangle){136, 92, 368, 292});
    InterfaceDesenharTexto("PAUSADO", (640 - InterfaceMedirTexto("PAUSADO", 27)) / 2, 119, 27, BLACK);
    if (jogo->pausa_confirmar_saida) {
        const Rectangle botao_desistir = {164, 218, 146, 38};
        const Rectangle botao_cancelar = {330, 218, 146, 38};
        const bool sobre_desistir = CheckCollisionPointRec(jogo->posicao_mouse, botao_desistir);
        const bool sobre_cancelar = CheckCollisionPointRec(jogo->posicao_mouse, botao_cancelar);
        InterfaceDesenharTexto("Desistir desta run?", 210, 178, 18, MAROON);
        InterfaceDesenharBotao(&jogo->sprites, botao_desistir, "DESISTIR", sobre_desistir, true,
                               (Color){197, 69, 78, 255});
        InterfaceDesenharBotao(&jogo->sprites, botao_cancelar, "CANCELAR", sobre_cancelar || !sobre_desistir, true,
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
    InterfaceDesenharTexto("Apagar todo o save?", (640 - InterfaceMedirTexto("Apagar todo o save?", 23)) / 2, 190, 23, BLACK);
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

static Tipo TipoDoBroche(int indice)
{
    static const Tipo tipos[] = {TIPO_FOGO, TIPO_PLANTA, TIPO_AGUA};
    if (indice < 0 || indice >= (int)(sizeof(tipos) / sizeof(tipos[0]))) return TIPO_PLANTA;
    return tipos[indice];
}

static void AbrirEscolhaBroche(EstadoJogo *jogo)
{
    jogo->selecao_broche = 1;
    jogo->tela = ESCOLHER_BROCHE;
}

static void IniciarPartida(EstadoJogo *jogo, Tipo tipo_broche)
{
    jogo->contador_partidas++;
    jogo->partida_iniciada_em = (float)GetTime();
    JogadorInicializarComBroche(&jogo->jogador, tipo_broche);
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

static void DesenharEscolhaBroche(const EstadoJogo *jogo)
{
    static const char *const nomes[] = {"BROCHE DE BRASA", "BROCHE DE FOLHA", "BROCHE DE GOTA"};
    static const char *const descricoes[] = {
        "CHAMA  |  espada simples de fogo",
        "PLANTA |  espada simples de folha",
        "AGUA   |  espada simples de agua"
    };
    static const IdSprite sprites_broche[] = {
        SPRITE_BROCHE_INICIAL_CHAMA,
        SPRITE_BROCHE_INICIAL_PLANTA,
        SPRITE_BROCHE_INICIAL_AGUA
    };
    InterfaceDesenharPainel((Rectangle){44, 88, 552, 304});
    InterfaceDesenharTexto("ESCOLHA SEU BROCHE", (640 - InterfaceMedirTexto("ESCOLHA SEU BROCHE", 26)) / 2, 112, 26, BLACK);
    InterfaceDesenharTexto("Ele define sua defesa e sua primeira espada.", 138, 145, 14, DARKGRAY);
    for (int indice = 0; indice < 3; indice++) {
        const Rectangle cartao = {68.0f + indice * 174.0f, 184.0f, 156.0f, 128.0f};
        const Tipo tipo = TipoDoBroche(indice);
        const bool selecionado = jogo->selecao_broche == indice;
        const Color cor = cor_tipo(tipo);
        DrawRectangleRec(cartao, selecionado ? Fade(cor, 0.30f) : Fade(RAYWHITE, 0.75f));
        DrawRectangleLinesEx(cartao, selecionado ? 4.0f : 2.0f, selecionado ? cor : DARKGRAY);
        const SpriteJogo *sprite_broche = sprite_obter(&jogo->sprites, sprites_broche[indice]);
        if (sprite_broche != NULL) {
            const float largura_maxima = 58.0f;
            const float altura_maxima = 58.0f;
            const float escala = fminf(largura_maxima / (float)sprite_broche->textura.width,
                                       altura_maxima / (float)sprite_broche->textura.height);
            const float largura = sprite_broche->textura.width * escala;
            const float altura = sprite_broche->textura.height * escala;
            SpriteDesenharAjustado(&jogo->sprites, sprites_broche[indice],
                                   (Rectangle){cartao.x + (cartao.width - largura) * 0.5f,
                                               186.0f + (altura_maxima - altura) * 0.5f,
                                               largura, altura}, WHITE);
        } else {
            DrawCircle((int)(cartao.x + cartao.width / 2.0f), 216, 20, cor);
            DrawCircleLines((int)(cartao.x + cartao.width / 2.0f), 216, 20, BLACK);
        }
        InterfaceDesenharTextoAjustado(nomes[indice], (Rectangle){cartao.x + 11, 249, cartao.width - 22, 17}, 13, BLACK);
        InterfaceDesenharTextoQuebrado(descricoes[indice], (Rectangle){cartao.x + 10, 274, cartao.width - 20, 30}, 12, 2, DARKGRAY);
        if (selecionado) InterfaceDesenharTexto(">", (int)cartao.x - 15, 237, 22, cor);
    }
    InterfaceDesenharAtalho("A/D ou setas escolher  |  ESPACO confirmar  |  X voltar", 104, 346);
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
        }
    }
    for (int espaco = 0; espaco < ESPACO_EQUIPADO_TOTAL; espaco++) {
        if (CheckCollisionPointRec(jogo->posicao_mouse, AreaEquipamento(espaco))) {
            jogo->selecao_equipamento = espaco;
            jogo->inventario_foco_equipamento = true;
        }
    }
    if (jogo->inventario_foco_equipamento) {
        if (entrada_cima_pressionada() || entrada_esquerda_pressionada())
            jogo->selecao_equipamento = (jogo->selecao_equipamento + ESPACO_EQUIPADO_TOTAL - 1) % ESPACO_EQUIPADO_TOTAL;
        if (entrada_baixo_pressionada() || entrada_direita_pressionada())
            jogo->selecao_equipamento = (jogo->selecao_equipamento + 1) % ESPACO_EQUIPADO_TOTAL;
        if (entrada_confirmar_pressionada() || entrada_cancelar_pressionada())
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
    if ((!jogo->inventario_foco_equipamento && entrada_cancelar_pressionada()) || IsKeyPressed(KEY_ESCAPE))
        jogo->tela = jogo->tela_antes_inventario;
}


static void ReiniciarPiloto(EstadoJogo *jogo)
{
    bool ignorar=jogo->batalha.defesa.depurar_invencivel;
    JogadorInicializarComBroche(&jogo->jogador,jogo->piloto_tipo);
    jogo->jogador.tipo_defesa=jogo->piloto_tipo;
    jogo->jogador.vida_base=jogo->piloto_vida?80:40;
    jogo->jogador.multiplicador_dificuldade=BalanceamentoMultiplicadorDanoRecebido((DificuldadeJogo)jogo->piloto_dificuldade);
    JogadorRecalcularAtributos(&jogo->jogador);
    jogo->jogador.hp=jogo->jogador.vida_maxima;
    jogo->inimigo=InimigoCriar(PilotoInimigoId(jogo->piloto_inimigo));
    jogo->id_depuracao_padrao=PilotoBase(jogo->piloto_inimigo)+jogo->piloto_ataque;
    DefesaIniciar(&jogo->batalha.defesa,(IdPadraoProjetil)jogo->id_depuracao_padrao,
        (Rectangle){42,228,556,94},50,0,1,1);
    jogo->batalha.defesa.depurar_invencivel=ignorar;
    jogo->piloto_pausado=false;
}
void JogoAbrirPiloto(EstadoJogo *jogo,int inimigo)
{
    jogo->piloto_ativo=true;jogo->piloto_inimigo=(inimigo%PilotoTotalInimigos()+PilotoTotalInimigos())%PilotoTotalInimigos();jogo->piloto_ataque=0;
    jogo->piloto_tipo=TIPO_NEUTRO;jogo->piloto_dificuldade=DIFICULDADE_NORMAL;
    jogo->tela=DEPURAR_EFEITOS;
    ReiniciarPiloto(jogo);
}
static void AtualizarPiloto(EstadoJogo *jogo)
{
    bool reiniciar=false;
    if(IsKeyPressed(KEY_ONE)){jogo->piloto_ataque=0;reiniciar=true;}
    if(IsKeyPressed(KEY_TWO)){jogo->piloto_ataque=1;reiniciar=true;}
    if(IsKeyPressed(KEY_THREE)){jogo->piloto_ataque=2;reiniciar=true;}
    if(IsKeyPressed(KEY_TAB)){jogo->piloto_inimigo=(jogo->piloto_inimigo+(IsKeyDown(KEY_LEFT_SHIFT)?PilotoTotalInimigos()-1:1))%PilotoTotalInimigos();reiniciar=true;}
    if(IsKeyPressed(KEY_C)){jogo->piloto_tipo=(Tipo)((jogo->piloto_tipo+1)%TIPO_TOTAL);reiniciar=true;}
    if(IsKeyPressed(KEY_F7)){jogo->piloto_dificuldade=(jogo->piloto_dificuldade+1)%3;reiniciar=true;}
    if(IsKeyPressed(KEY_V)){jogo->piloto_vida=!jogo->piloto_vida;reiniciar=true;}
    if(IsKeyPressed(KEY_I))jogo->batalha.defesa.depurar_invencivel=!jogo->batalha.defesa.depurar_invencivel;
    if(IsKeyPressed(KEY_B))jogo->sprites.depurar_limites=!jogo->sprites.depurar_limites;
    if(IsKeyPressed(KEY_F6))SpritesRecarregar(&jogo->sprites);
    if(IsKeyPressed(KEY_ESCAPE)||IsKeyPressed(KEY_P))jogo->piloto_pausado=!jogo->piloto_pausado;
    if(IsKeyPressed(KEY_X))jogo->saida_solicitada=true;
    if(reiniciar||IsKeyPressed(KEY_R))ReiniciarPiloto(jogo);
    if(!jogo->piloto_pausado){
        DefesaAtualizar(&jogo->batalha.defesa,&jogo->jogador,GetFrameTime());
        if(jogo->batalha.defesa.fase==DEFESA_FINALIZADA)jogo->piloto_pausado=true;
    }
}
static void DesenharPiloto(const EstadoJogo *jogo)
{
    const EstadoDefesa *d=&jogo->batalha.defesa;
    ClearBackground((Color){30,33,43,255});
    InterfaceDesenharPainel((Rectangle){20,16,600,89});
    InterfaceDesenharTextoAjustado(jogo->inimigo.nome,(Rectangle){36,29,430,24},22,BLACK);
    InterfaceDesenharTexto(TextFormat("TESTE  %d/%d   |   ataque %d/3   |   dificuldade %d",
        jogo->piloto_inimigo+1,PilotoTotalInimigos(),jogo->piloto_ataque+1,jogo->piloto_dificuldade+1),36,62,14,DARKGRAY);
    InterfaceDesenharTextoAjustado(TextFormat("Defesa: %s  |  imagens temporarias: manifesto de assets",
        nome_tipo(jogo->jogador.tipo_defesa)),(Rectangle){36,82,480,16},12,DARKGRAY);
    IdSprite inimigo=SpriteIdInimigo(jogo->inimigo.id,false);
    const SpriteJogo *s=sprite_obter(&jogo->sprites,inimigo);
    if(s)SpriteDesenharCentralizadoEscalado(&jogo->sprites,inimigo,(Vector2){566,60},WHITE,
        jogo->tempo_decorrido,68.0f/fmaxf(s->textura.width,s->textura.height),false);
    DefesaDesenhar(d,true,&jogo->sprites);
    InterfaceDesenharPainel((Rectangle){20,343,600,122});
    InterfaceDesenharTexto(TextFormat("HP %d/%d   |   dano da rodada %d/%d   |   i-frames %.2f s",
        jogo->jogador.hp,jogo->jogador.vida_maxima,d->orcamento.recebido,d->orcamento.limite_rodada,
        d->temporizador_invencibilidade),36,355,15,BLACK);
    InterfaceDesenharTexto("1/2/3 ataque | TAB inimigo | R reiniciar | P/ESC pausar | F8 voltar",36,382,13,DARKGRAY);
    InterfaceDesenharTexto("WASD/setas mover | SHIFT dash | I invencivel | B hitbox",36,402,13,DARKGRAY);
    InterfaceDesenharTexto("C elemento | F7 dificuldade | V vida 40/80 | F6 recarregar imagens",36,422,13,DARKGRAY);
    if(jogo->piloto_pausado)InterfaceDesenharTexto("PAUSADO / R para repetir",36,443,12,MAROON);
    else if(d->depurar_invencivel)InterfaceDesenharTexto("INVENCIBILIDADE DE TESTE ATIVA",36,443,12,MAROON);
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
    static EstadoJogo estado_antes_piloto;
    static bool retorno_piloto_disponivel = false;
    if (IsKeyPressed(KEY_F8) || (jogo->piloto_ativo && IsKeyPressed(KEY_X))) {
        if (jogo->piloto_ativo && retorno_piloto_disponivel) {
            SistemaSprites sprites_atuais=jogo->sprites;
            *jogo = estado_antes_piloto;
            jogo->sprites=sprites_atuais;
            retorno_piloto_disponivel = false;
        } else if (!jogo->piloto_ativo) {
            estado_antes_piloto = *jogo;
            retorno_piloto_disponivel = true;
            JogoAbrirPiloto(jogo, 8);
            jogo->batalha.defesa.depurar_invencivel = false;
        }
        return;
    }
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
    if (jogo->piloto_ativo) {
        jogo->tempo_decorrido += GetFrameTime();
        AtualizarPiloto(jogo);
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
            if (jogo->selecao_titulo == 0) AbrirEscolhaBroche(jogo);
            else if (jogo->selecao_titulo == 1) jogo->tela = RELIQUIAS;
            else if (jogo->selecao_titulo == 2) jogo->tela = COMO_JOGAR;
            else if (jogo->selecao_titulo == 3) jogo->tela = OPCOES;
            else jogo->saida_solicitada = true;
        }
    } else if (jogo->tela == ESCOLHER_BROCHE) {
        bool confirmou_mouse = false;
        for (int indice = 0; indice < 3; indice++) {
            const Rectangle cartao = {68.0f + indice * 174.0f, 184.0f, 156.0f, 128.0f};
            if (CheckCollisionPointRec(jogo->posicao_mouse, cartao)) {
                jogo->selecao_broche = indice;
                confirmou_mouse = jogo->mouse_pressionado;
            }
        }
        if (entrada_esquerda_pressionada() || entrada_cima_pressionada())
            jogo->selecao_broche = (jogo->selecao_broche + 2) % 3;
        if (entrada_direita_pressionada() || entrada_baixo_pressionada())
            jogo->selecao_broche = (jogo->selecao_broche + 1) % 3;
        if (entrada_confirmar_pressionada() || confirmou_mouse)
            IniciarPartida(jogo, TipoDoBroche(jogo->selecao_broche));
        if (entrada_cancelar_pressionada()) jogo->tela = TITULO;
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
            if (jogo->selecao_final == 0 && jogo->tela == FIM_JOGO) AbrirEscolhaBroche(jogo);
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
    if (jogo->piloto_ativo) { DesenharPiloto(jogo); return; }
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
    } else if (jogo->tela == ESCOLHER_BROCHE) {
        DesenharEscolhaBroche(jogo);
    } else if (jogo->tela == BATALHA) {
        BatalhaDesenhar(&jogo->batalha, &jogo->jogador, &jogo->inimigo, &jogo->dialogo, &jogo->qte,
                   jogo->renderizacao.tremor_linha && !jogo->salvamento.efeitos_reduzidos, jogo->salvamento.efeitos_reduzidos,
                   &jogo->sprites);
        if (jogo->temporizador_tutorial > 0.0f) DesenharTutorial(jogo);
        if (jogo->temporizador_transicao > 0.0f) {
            InterfaceDesenharPainel((Rectangle){198, 193, 244, 82});
            InterfaceDesenharTexto("SUBINDO...", (640 - InterfaceMedirTexto("SUBINDO...", 25)) / 2, 215, 25, BLACK);
            InterfaceDesenharTexto(TextFormat("ANDAR %d", jogo->torre.andar), 276, 246, 16, DARKGRAY);
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
        InterfaceDesenharTexto("FIM DA RUN", (640 - InterfaceMedirTexto("FIM DA RUN", 30)) / 2, 152, 30, BLACK);
        InterfaceDesenharTexto(TextFormat("Andar %d  |  Moedas %d", jogo->torre.andar, jogo->torre.moedas),
                 190, 206, 18, DARKGRAY);
        DesenharEscolha(270, "TENTAR DE NOVO", jogo->selecao_final == 0);
        DesenharEscolha(310, "TITULO", jogo->selecao_final == 1);
    } else if (jogo->tela == VITORIA) {
        InterfaceDesenharPainel((Rectangle){62, 104, 516, 276});
        InterfaceDesenharTexto("TORRE CONCLUIDA", (640 - InterfaceMedirTexto("TORRE CONCLUIDA", 29)) / 2, 136, 29, BLACK);
        InterfaceDesenharTexto(TextFormat("%d andares | %d moedas", TOTAL_ANDARES_TORRE, jogo->torre.moedas), 205, 190, 18, DARKGRAY);
        InterfaceDesenharTexto(TextFormat("Poupar: %d  |  Dano: %d", jogo->torre.vitorias_misericordia,
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
        InterfaceDesenharTexto("TECLADO PAUSADO", (640 - InterfaceMedirTexto("TECLADO PAUSADO", 20)) / 2, 208, 20, BLACK);
        InterfaceDesenharTexto("Clique na janela para continuar", 207, 241, 14, DARKGRAY);
    }
}
