#include "qte.h"

#include "input.h"

#include <math.h>
#include <string.h>

#define QTE_SEGUNDOS_GUARDA 0.38f
#define QTE_SEGUNDOS_RESULTADO 0.65f
#define QTE_IMPACTO_ALVO 0.72f
#define QTE_IMPACTO_DURACAO_BASE 1.35f
#define QTE_RITMO_DURACAO_BASE 1.05f
#define QTE_CARGA_DURACAO_BASE 1.35f

static const float ALVOS_RITMO[] = {0.32f, 0.68f, 0.48f};

static float LimitarDificuldade(int dificuldade, float base, float passo, float minimo)
{
    const float valor = base - (float)(dificuldade - 1) * passo;
    return valor < minimo ? minimo : valor;
}

static float Limitar01(float valor)
{
    if (valor < 0.0f) return 0.0f;
    if (valor > 1.0f) return 1.0f;
    return valor;
}

static void FinalizarQte(Qte *qte, ResultadoQte resultado)
{
    qte->resultado = resultado;
    qte->resultado_pronto = true;
    qte->temporizador_resultado = 0.0f;
}

static void GanchoAudioBatida(void) { }
static void GanchoAudioErro(void) { }
static void GanchoAudioPerfeito(void) { }

ResultadoQte qte_resultado_por_distancia_tempo(float distancia, int dificuldade)
{
    const float janela_perfeita = LimitarDificuldade(dificuldade, 0.060f, 0.006f, 0.028f);
    const float janela_boa = LimitarDificuldade(dificuldade, 0.145f, 0.010f, 0.080f);
    const float janela_ruim = LimitarDificuldade(dificuldade, 0.260f, 0.014f, 0.145f);

    if (distancia <= janela_perfeita) return QTE_PERFEITO;
    if (distancia <= janela_boa) return QTE_BOM;
    if (distancia <= janela_ruim) return QTE_RUIM;
    return QTE_ERRO;
}

ResultadoQte qte_resultado_por_ritmo(int pontos, int total_batidas)
{
    const int maximo = total_batidas * 3;

    if (total_batidas <= 0 || pontos <= 0) return QTE_ERRO;
    if (pontos == maximo) return QTE_PERFEITO;
    if (pontos >= total_batidas * 2) return QTE_BOM;
    if (pontos >= total_batidas) return QTE_RUIM;
    return QTE_ERRO;
}

void qte_iniciar(Qte *qte, TipoQte categoria, int dificuldade)
{
    memset(qte, 0, sizeof(*qte));
    qte->ativo = true;
    qte->categoria = categoria;
    qte->dificuldade = dificuldade < 1 ? 1 : dificuldade;
    qte->tempo_alvo = QTE_IMPACTO_ALVO;
    qte->temporizador_guarda = QTE_SEGUNDOS_GUARDA;
    qte->segurar_pronto = categoria != QTE_CARGA;
    qte->carga_solta = -1.0f;

    if (categoria == QTE_IMPACTO) {
        qte->duracao = LimitarDificuldade(qte->dificuldade, QTE_IMPACTO_DURACAO_BASE, 0.07f, 0.95f);
    } else if (categoria == QTE_RITMO) {
        qte->total_batidas = 3;
        qte->tempo_alvo = ALVOS_RITMO[0];
        qte->duracao = LimitarDificuldade(qte->dificuldade, QTE_RITMO_DURACAO_BASE, 0.045f, 0.78f);
    } else {
        qte->duracao = LimitarDificuldade(qte->dificuldade, QTE_CARGA_DURACAO_BASE, 0.07f, 0.95f);
    }
}

static ResultadoQte ResultadoCarga(const Qte *qte)
{
    const float centro = 0.68f;
    const float meia_janela_perfeita = LimitarDificuldade(qte->dificuldade, 0.090f, 0.008f, 0.050f);
    const float distancia = fabsf(qte->carga_segurada - centro);

    if (distancia <= meia_janela_perfeita) return QTE_PERFEITO;
    if (distancia <= meia_janela_perfeita + 0.11f) return QTE_BOM;
    if (distancia <= meia_janela_perfeita + 0.23f) return QTE_RUIM;
    return QTE_ERRO;
}

EntradaQte qte_entrada_para_teste(bool confirmar_pressionado, bool confirmar_mantido,
                                  bool confirmar_solto)
{
    return (EntradaQte){confirmar_pressionado, confirmar_mantido, confirmar_solto};
}

static EntradaQte LerEntrada(void)
{
    return qte_entrada_para_teste(entrada_confirmar_pressionada(), entrada_confirmar_mantida(),
                                  entrada_confirmar_solta());
}

bool qte_esta_em_guarda(const Qte *qte)
{
    return qte->temporizador_guarda > 0.0f;
}

void qte_retomar_com_guarda(Qte *qte)
{
    if (qte->ativo && !qte->resultado_pronto) qte->temporizador_guarda = QTE_SEGUNDOS_GUARDA;
}

static int PontosResultado(ResultadoQte resultado)
{
    if (resultado == QTE_PERFEITO) return 3;
    if (resultado == QTE_BOM) return 2;
    if (resultado == QTE_RUIM) return 1;
    return 0;
}

static void AvancarBatida(Qte *qte, ResultadoQte resultado)
{
    qte->pontos_ritmo += PontosResultado(resultado);
    qte->indice_batida++;
    qte->temporizador_retorno = 0.18f;
    qte->retorno_sucesso = resultado != QTE_ERRO;

    if (qte->indice_batida >= qte->total_batidas) {
        const ResultadoQte final = qte_resultado_por_ritmo(qte->pontos_ritmo, qte->total_batidas);
        if (final == QTE_PERFEITO) GanchoAudioPerfeito();
        else if (final == QTE_ERRO) GanchoAudioErro();
        else GanchoAudioBatida();
        FinalizarQte(qte, final);
        return;
    }

    qte->decorrido = 0.0f;
    qte->tempo_alvo = ALVOS_RITMO[qte->indice_batida];
}

void qte_atualizar_com_entrada(Qte *qte, float tempo_delta, EntradaQte entrada)
{
    if (!qte->ativo) return;
    if (qte->temporizador_retorno > 0.0f) qte->temporizador_retorno -= tempo_delta;
    if (qte->resultado_pronto) {
        qte->temporizador_resultado += tempo_delta;
        return;
    }

    if (qte->temporizador_guarda > 0.0f) {
        qte->temporizador_guarda -= tempo_delta;
        if (qte->temporizador_guarda < 0.0f) qte->temporizador_guarda = 0.0f;
        return;
    }
    if (qte->categoria == QTE_CARGA && !qte->segurar_pronto) {
        if (!entrada.confirmar_mantido) qte->segurar_pronto = true;
        return;
    }

    qte->decorrido += tempo_delta;
    if (qte->categoria == QTE_IMPACTO) {
        if (entrada.confirmar_pressionado) {
            const float progresso = qte->decorrido / qte->duracao;
            const ResultadoQte resultado = qte_resultado_por_distancia_tempo(
                fabsf(progresso - qte->tempo_alvo), qte->dificuldade);
            qte->temporizador_retorno = 0.20f;
            qte->retorno_sucesso = resultado != QTE_ERRO;
            if (resultado == QTE_PERFEITO) GanchoAudioPerfeito();
            else if (resultado == QTE_ERRO) GanchoAudioErro();
            else GanchoAudioBatida();
            FinalizarQte(qte, resultado);
        } else if (qte->decorrido >= qte->duracao) {
            qte->temporizador_retorno = 0.20f;
            qte->retorno_sucesso = false;
            GanchoAudioErro();
            FinalizarQte(qte, QTE_ERRO);
        }
    } else if (qte->categoria == QTE_RITMO) {
        if (entrada.confirmar_pressionado) {
            const float progresso = Limitar01(qte->decorrido / qte->duracao);
            AvancarBatida(qte, qte_resultado_por_distancia_tempo(
                fabsf(progresso - qte->tempo_alvo), qte->dificuldade));
        } else if (qte->decorrido >= qte->duracao) {
            AvancarBatida(qte, QTE_ERRO);
        }
    } else {
        if (entrada.confirmar_pressionado && !qte->segurar_iniciado) qte->segurar_iniciado = true;
        if (qte->segurar_iniciado && entrada.confirmar_mantido) {
            qte->carga_segurada += tempo_delta / qte->duracao;
        } else if (qte->segurar_iniciado && entrada.confirmar_solto) {
            const ResultadoQte resultado = ResultadoCarga(qte);
            qte->carga_solta = qte->carga_segurada;
            qte->temporizador_retorno = 0.20f;
            qte->retorno_sucesso = resultado != QTE_ERRO;
            if (resultado == QTE_PERFEITO) GanchoAudioPerfeito();
            else if (resultado == QTE_ERRO) GanchoAudioErro();
            else GanchoAudioBatida();
            FinalizarQte(qte, resultado);
        }

        if (qte->carga_segurada >= 1.0f || qte->decorrido >= qte->duracao * 1.55f) {
            qte->carga_solta = qte->carga_segurada;
            qte->temporizador_retorno = 0.20f;
            qte->retorno_sucesso = false;
            GanchoAudioErro();
            FinalizarQte(qte, QTE_ERRO);
        }
    }
}

void qte_atualizar(Qte *qte, float tempo_delta)
{
    qte_atualizar_com_entrada(qte, tempo_delta, LerEntrada());
}

bool qte_terminou(const Qte *qte)
{
    return qte->resultado_pronto && qte->temporizador_resultado >= QTE_SEGUNDOS_RESULTADO;
}

float qte_multiplicador(ResultadoQte resultado)
{
    static const float multiplicadores[] = {1.5f, 1.0f, 0.5f, 0.25f};
    return multiplicadores[resultado];
}

const char *qte_nome_resultado(ResultadoQte resultado)
{
    static const char *nomes[] = {"PERFEITO!", "BOM", "RUIM", "ERRO"};
    return nomes[resultado];
}

static Color CorResultado(ResultadoQte resultado)
{
    static const Color cores[] = {
        {54, 161, 91, 255}, {55, 132, 205, 255}, {220, 145, 40, 255}, {207, 65, 62, 255}
    };
    return cores[resultado];
}

static void DesenharTeclaEspaco(int x, int y)
{
    const Rectangle tecla = {(float)x, (float)y, 78.0f, 20.0f};
    DrawRectangleRec((Rectangle){tecla.x + 2, tecla.y + 2, tecla.width, tecla.height},
                     (Color){174, 171, 164, 255});
    DrawRectangleRec(tecla, RAYWHITE);
    DrawRectangleLinesEx(tecla, 1.5f, BLACK);
    DrawText("ESPACO", x + 12, y + 4, 12, BLACK);
}

static void DesenharPassos(const Qte *qte, int x, int y)
{
    const int total = qte->categoria == QTE_RITMO ? qte->total_batidas : 1;
    const int atual = qte->categoria == QTE_RITMO ? qte->indice_batida : 0;

    for (int indice = 0; indice < total; indice++) {
        const Color cor = indice < atual ? (Color){70, 170, 102, 255} :
                          (indice == atual ? (Color){225, 95, 122, 255} : LIGHTGRAY);
        DrawCircle(x + indice * 18, y, 5.0f, cor);
        DrawCircleLines(x + indice * 18, y, 5.0f, BLACK);
    }
}

static void DesenharGuarda(const Qte *qte)
{
    const float progresso = 1.0f - qte->temporizador_guarda / QTE_SEGUNDOS_GUARDA;
    const int contagem = 3 - (int)(progresso * 3.0f);

    DrawText("PREPARE-SE", 70, 244, 14, DARKGRAY);
    DrawText(TextFormat("%d", contagem < 1 ? 1 : contagem), 302, 255, 34,
             (Color){225, 95, 122, 255});
    DrawRectangle(170, 299, 300, 7, LIGHTGRAY);
    DrawRectangle(170, 299, (int)(300.0f * progresso), 7, (Color){225, 95, 122, 255});
    DrawRectangleLines(170, 299, 300, 7, BLACK);
}

void qte_desenhar(const Qte *qte)
{
    const Color destaque = (Color){225, 95, 122, 255};

    if (qte_esta_em_guarda(qte)) {
        DesenharGuarda(qte);
        return;
    }
    if (qte->resultado_pronto) {
        const char *nome = qte_nome_resultado(qte->resultado);
        DrawText(nome, 320 - MeasureText(nome, 30) / 2, 252, 30, CorResultado(qte->resultado));
        DrawText(qte->resultado == QTE_ERRO ? "O golpe ainda causa dano reduzido." : "Golpe confirmado!",
                 320 - MeasureText(qte->resultado == QTE_ERRO ? "O golpe ainda causa dano reduzido." :
                                   "Golpe confirmado!", 12) / 2, 289, 12, DARKGRAY);
        return;
    }

    if (qte->categoria == QTE_IMPACTO) {
        const float progresso = Limitar01(qte->decorrido / qte->duracao);
        const float raio = 25.0f - progresso * 19.0f;

        DrawText("IMPACTO", 70, 240, 14, destaque);
        DrawText("Aperte quando os circulos se encontrarem", 70, 258, 12, BLACK);
        DesenharTeclaEspaco(472, 242);
        DrawCircleLines(320, 288, 9.0f, BLACK);
        DrawCircleLines(320, 288, raio, progresso > qte->tempo_alvo ? RED : destaque);
        DrawCircle(320, 288, 3.0f, BLACK);
    } else if (qte->categoria == QTE_RITMO) {
        const int trilho_x = 170;
        const int trilho_y = 286;
        const int largura = 300;
        const float progresso = Limitar01(qte->decorrido / qte->duracao);
        const int alvo_x = trilho_x + (int)(largura * qte->tempo_alvo);
        const float janela = LimitarDificuldade(qte->dificuldade, 0.145f, 0.010f, 0.080f);
        const int largura_alvo = (int)(largura * janela * 2.0f);

        DrawText("RITMO", 70, 240, 14, destaque);
        DrawText("Acerte as 3 zonas usando apenas ESPACO", 70, 258, 12, BLACK);
        DesenharTeclaEspaco(472, 242);
        DesenharPassos(qte, 516, 298);
        DrawRectangle(trilho_x, trilho_y - 5, largura, 10, (Color){218, 216, 210, 255});
        DrawRectangle(alvo_x - largura_alvo / 2, trilho_y - 8, largura_alvo, 16,
                      (Color){114, 198, 128, 255});
        DrawRectangleLines(trilho_x, trilho_y - 5, largura, 10, BLACK);
        DrawLineEx((Vector2){trilho_x + largura * progresso, trilho_y - 13},
                   (Vector2){trilho_x + largura * progresso, trilho_y + 13}, 3.0f, destaque);
    } else {
        const int barra_x = 165;
        const int barra_y = 280;
        const int largura = 310;
        const float largura_zona = LimitarDificuldade(qte->dificuldade, 0.20f, 0.015f, 0.11f);
        const int zona_x = barra_x + (int)(largura * (0.68f - largura_zona * 0.5f));
        const int pixels_zona = (int)(largura * largura_zona);

        DrawText("CARGA", 70, 240, 14, destaque);
        DrawText(qte->segurar_pronto ? "Segure e solte dentro da zona verde" :
                                      "Solte ESPACO para preparar", 70, 258, 12, BLACK);
        DesenharTeclaEspaco(472, 242);
        DrawRectangle(barra_x, barra_y, largura, 18, (Color){218, 216, 210, 255});
        DrawRectangle(zona_x, barra_y, pixels_zona, 18, (Color){114, 198, 128, 255});
        DrawRectangle(barra_x, barra_y, (int)(largura * Limitar01(qte->carga_segurada)), 18,
                      (Color){79, 139, 215, 210});
        DrawRectangleLines(barra_x, barra_y, largura, 18, BLACK);
        DrawLine(barra_x + (int)(largura * 0.68f), barra_y - 3,
                 barra_x + (int)(largura * 0.68f), barra_y + 21, BLACK);
    }

    if (qte->temporizador_retorno > 0.0f) {
        DrawText(qte->retorno_sucesso ? "ACERTO" : "ERRO", 492, 278, 12,
                 qte->retorno_sucesso ? GREEN : RED);
    }
}
