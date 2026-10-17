#include "enemy.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#define AGRESSIVIDADE_ALTA 67

static int LimitarValorInimigo(int valor)
{
    if (valor < 0) return 0;
    if (valor > 100) return 100;
    return valor;
}

static AcaoInimigo CriarAcao(const char *nome, const char *resposta, EfeitoAcaoInimigo efeito,
                              int variacao_misericordia, int variacao_agressividade, int maximo_usos,
                              int turnos_recarga, float penalidade_repeticao,
                              int agressividade_maxima_exigida)
{
    return (AcaoInimigo){nome, resposta, efeito, variacao_misericordia, variacao_agressividade, maximo_usos,
                         turnos_recarga, penalidade_repeticao, agressividade_maxima_exigida,
                         false, TIPO_NEUTRO, 1.0f, 50, 1.0f, 1.0f};
}

void InimigoAtualizarPoupavel(Inimigo *inimigo)
{
    inimigo->esgotado = inimigo->hp * 4 < inimigo->vida_maxima;
    inimigo->poupavel = inimigo->misericordia >= 100 || inimigo->esgotado;
}

bool InimigoPodePoupar(const Inimigo *inimigo)
{
    return inimigo->poupavel && inimigo->agressividade < AGRESSIVIDADE_ALTA;
}

void InimigoAplicarStatus(Inimigo *inimigo, EfeitoStatus status, int turnos)
{
    if (status == STATUS_NENHUM || turnos <= 0) return;
    inimigo->status = status;
    inimigo->turnos_status = turnos;
}

bool InimigoAvancarStatus(Inimigo *inimigo, int *dano)
{
    *dano = 0;
    if (inimigo->turnos_status <= 0) return false;
    if (inimigo->status == STATUS_QUEIMAR) {
        *dano = 3;
        InimigoReceberDano(inimigo, *dano, 1.0f);
    }
    inimigo->turnos_status--;
    if (inimigo->turnos_status == 0) inimigo->status = STATUS_NENHUM;
    return *dano > 0;
}

Inimigo InimigoCriar(IdInimigo id)
{
    Inimigo inimigo;

    memset(&inimigo, 0, sizeof(inimigo));
    inimigo.id = id;
    inimigo.multiplicador_dano_projetil = 1.0f;
    inimigo.indice_ultima_acao = -1;
    for (int indice = 0; indice < MAXIMO_ACOES_INIMIGO; indice++) inimigo.ultimo_turno_acoes[indice] = -1;
    switch (id) {
        case INIMIGO_POEIRA_MARIPOSA:
            snprintf(inimigo.nome, sizeof(inimigo.nome), "Mariposa de Poeira");
            inimigo.vida_maxima = 36;
            inimigo.hp = inimigo.vida_maxima;
            inimigo.tipo = TIPO_SOMBRA;
            inimigo.agressividade = 40;
            inimigo.ids_padroes[0] = PADRAO_MARIPOSA_TRAMA;
            inimigo.ids_padroes[1] = PADRAO_MARIPOSA_CORRENTE;
            inimigo.ids_padroes[2] = PADRAO_MARIPOSA_ASAS; inimigo.total_padroes = 3;
            snprintf(inimigo.descricao, sizeof(inimigo.descricao),
                     "Ela ronda lampadas apagadas e parece esperar um elogio.");
            inimigo.acoes[0] = CriarAcao("Analisar", "", INIMIGO_ACAO_ANALISAR, 0, 0, 0, 0, 1.0f, 100);
            inimigo.acoes[1] = CriarAcao("Elogiar", "As asas vibram, felizes.", INIMIGO_ACAO_SOCIAL, 30, -5, 2, 1, 0.50f, 100);
            inimigo.acoes[2] = CriarAcao("Provocar", "As asas batem com raiva.", INIMIGO_ACAO_SOCIAL, -10, 25, 1, 2, 1.0f, 100);
            inimigo.acoes[3] = CriarAcao("Acalmar", "A poeira se acomoda devagar.", INIMIGO_ACAO_SOCIAL, 15, -20, 3, 1, 0.50f, 100);
            inimigo.acoes[3].possui_bonus_tipo = true;
            inimigo.acoes[3].tipo_bonus = TIPO_SOMBRA;
            inimigo.acoes[3].multiplicador_tipo = 1.30f;
            inimigo.acoes[3].limite_agressividade = 55;
            inimigo.acoes[3].multiplicador_calmo = 1.15f;
            inimigo.acoes[3].multiplicador_furioso = 0.45f;
            inimigo.total_acoes = 4;
            break;
        case INIMIGO_BRASA_PASSARO:
            snprintf(inimigo.nome, sizeof(inimigo.nome), "Passaro de Cinza");
            inimigo.vida_maxima = 38; inimigo.hp = inimigo.vida_maxima; inimigo.tipo = TIPO_FOGO; inimigo.agressividade = 48;
            inimigo.ids_padroes[0] = PADRAO_PASSARO_FOGO_CRUZADO;
            inimigo.ids_padroes[1] = PADRAO_PASSARO_TUNEL;
            inimigo.ids_padroes[2] = PADRAO_PASSARO_MERGULHO;
            inimigo.ids_padroes[3] = PADRAO_CHUVA_FOGO; inimigo.total_padroes = 4;
            snprintf(inimigo.descricao, sizeof(inimigo.descricao), "Uma ave quente demais para pousar.");
            inimigo.acoes[0] = CriarAcao("Analisar", "", INIMIGO_ACAO_ANALISAR, 0, 0, 0, 0, 1.0f, 100);
            inimigo.acoes[1] = CriarAcao("Assobiar", "O canto perde a pressa.", INIMIGO_ACAO_SOCIAL, 25, -15, 2, 1, .5f, 100);
            inimigo.acoes[2] = CriarAcao("Ameacar", "As penas viram brasas.", INIMIGO_ACAO_SOCIAL, -15, 30, 1, 2, 1.0f, 100);
            inimigo.acoes[3] = CriarAcao("Acalmar", "A brasa diminui.", INIMIGO_ACAO_SOCIAL, 15, -20, 3, 1, .5f, 100);
            inimigo.total_acoes = 4; break;
        case INIMIGO_MENTA_LIMO:
            snprintf(inimigo.nome, sizeof(inimigo.nome), "Limo de Menta");
            inimigo.vida_maxima = 34; inimigo.hp = inimigo.vida_maxima; inimigo.tipo = TIPO_PLANTA; inimigo.agressividade = 28;
            inimigo.ids_padroes[0] = PADRAO_LIMO_MARE;
            inimigo.ids_padroes[1] = PADRAO_LIMO_BALSAS;
            inimigo.ids_padroes[2] = PADRAO_LIMO_CRESCIMENTO; inimigo.total_padroes = 3;
            snprintf(inimigo.descricao, sizeof(inimigo.descricao), "Ele cresce onde ninguem olha.");
            inimigo.acoes[0] = CriarAcao("Analisar", "", INIMIGO_ACAO_ANALISAR, 0, 0, 0, 0, 1.0f, 100);
            inimigo.acoes[1] = CriarAcao("Regar", "O limo balanca contente.", INIMIGO_ACAO_SOCIAL, 28, -8, 2, 1, .5f, 100);
            inimigo.acoes[2] = CriarAcao("Pisar", "O limo espirra de raiva.", INIMIGO_ACAO_SOCIAL, -15, 28, 1, 2, 1.0f, 100);
            inimigo.acoes[3] = CriarAcao("Esperar", "Ele para de se esticar.", INIMIGO_ACAO_SOCIAL, 16, -18, 3, 1, .5f, 100);
            inimigo.total_acoes = 4; break;
        case INIMIGO_LANTERNA_FAGULHA:
            snprintf(inimigo.nome, sizeof(inimigo.nome), "Fagulha de Lanterna");
            inimigo.vida_maxima = 40; inimigo.hp = inimigo.vida_maxima; inimigo.tipo = TIPO_LUZ; inimigo.agressividade = 50;
            inimigo.ids_padroes[0] = PADRAO_FAGULHA_CANHOES;
            inimigo.ids_padroes[1] = PADRAO_FAGULHA_ORBITA;
            inimigo.ids_padroes[2] = PADRAO_FAGULHA_LUZ; inimigo.total_padroes = 3;
            snprintf(inimigo.descricao, sizeof(inimigo.descricao), "Uma luz que quer ser seguida.");
            inimigo.acoes[0] = CriarAcao("Analisar", "", INIMIGO_ACAO_ANALISAR, 0, 0, 0, 0, 1.0f, 100);
            inimigo.acoes[1] = CriarAcao("Ouvir", "A luz fica mais baixa.", INIMIGO_ACAO_SOCIAL, 26, -12, 2, 1, .5f, 100);
            inimigo.acoes[2] = CriarAcao("Apagar", "Ela brilha num salto.", INIMIGO_ACAO_SOCIAL, -18, 32, 1, 2, 1.0f, 100);
            inimigo.acoes[3] = CriarAcao("Guiar", "A fagulha encontra descanso.", INIMIGO_ACAO_SOCIAL, 16, -18, 3, 1, .5f, 100);
            inimigo.total_acoes = 4; break;
        case INIMIGO_ESPINHO_LEBRE:
            snprintf(inimigo.nome, sizeof(inimigo.nome), "Lebre de Espinhos");
            inimigo.vida_maxima = 44; inimigo.hp = inimigo.vida_maxima; inimigo.tipo = TIPO_PLANTA; inimigo.agressividade = 58;
            inimigo.ids_padroes[0] = PADRAO_LEBRE_BARREIRAS;
            inimigo.ids_padroes[1] = PADRAO_LEBRE_PARKOUR;
            inimigo.ids_padroes[2] = PADRAO_LEBRE_PULOS; inimigo.total_padroes = 3;
            snprintf(inimigo.descricao, sizeof(inimigo.descricao), "Ela se esconde atras dos espinhos.");
            inimigo.acoes[0] = CriarAcao("Analisar", "", INIMIGO_ACAO_ANALISAR, 0, 0, 0, 0, 1.0f, 100);
            inimigo.acoes[1] = CriarAcao("Oferecer folha", "Ela mastiga devagar.", INIMIGO_ACAO_SOCIAL, 25, -15, 2, 1, .5f, 100);
            inimigo.acoes[2] = CriarAcao("Correr atras", "Ela mostra os espinhos.", INIMIGO_ACAO_SOCIAL, -12, 30, 1, 2, 1.0f, 100);
            inimigo.acoes[3] = CriarAcao("Afastar", "Ela percebe que tem espaco.", INIMIGO_ACAO_SOCIAL, 18, -18, 3, 1, .5f, 100);
            inimigo.total_acoes = 4; break;
        case INIMIGO_SOMBRA_GATO:
            snprintf(inimigo.nome, sizeof(inimigo.nome), "Gato de Sombra");
            inimigo.vida_maxima = 46; inimigo.hp = inimigo.vida_maxima; inimigo.tipo = TIPO_SOMBRA; inimigo.agressividade = 62;
            inimigo.ids_padroes[0] = PADRAO_GATO_IMPACTO;
            inimigo.ids_padroes[1] = PADRAO_GATO_PERSEGUICAO;
            inimigo.ids_padroes[2] = PADRAO_GATO_EMBOSCADA; inimigo.total_padroes = 3;
            snprintf(inimigo.descricao, sizeof(inimigo.descricao), "Ele rosna para o proprio reflexo.");
            inimigo.acoes[0] = CriarAcao("Analisar", "", INIMIGO_ACAO_ANALISAR, 0, 0, 0, 0, 1.0f, 100);
            inimigo.acoes[1] = CriarAcao("Fazer carinho", "O rabo para de balancar.", INIMIGO_ACAO_SOCIAL, 27, -16, 2, 1, .5f, 100);
            inimigo.acoes[2] = CriarAcao("Encurralar", "O pelo se arrepia.", INIMIGO_ACAO_SOCIAL, -18, 33, 1, 2, 1.0f, 100);
            inimigo.acoes[3] = CriarAcao("Sentar", "Ele observa em silencio.", INIMIGO_ACAO_SOCIAL, 17, -18, 3, 1, .5f, 100);
            inimigo.total_acoes = 4; break;
        case INIMIGO_PETALA_CAVALEIRO:
            snprintf(inimigo.nome, sizeof(inimigo.nome), "Cavaleiro de Petalas");
            inimigo.vida_maxima = 50; inimigo.hp = inimigo.vida_maxima; inimigo.tipo = TIPO_LUZ; inimigo.agressividade = 60;
            inimigo.ids_padroes[0] = PADRAO_CAVALEIRO_LANCAS;
            inimigo.ids_padroes[1] = PADRAO_CAVALEIRO_DESAFIO;
            inimigo.ids_padroes[2] = PADRAO_CAVALEIRO_DUELO; inimigo.total_padroes = 3;
            snprintf(inimigo.descricao, sizeof(inimigo.descricao), "Sua armadura cheira a jardim depois da chuva.");
            inimigo.acoes[0] = CriarAcao("Analisar", "", INIMIGO_ACAO_ANALISAR, 0, 0, 0, 0, 1.0f, 100);
            inimigo.acoes[1] = CriarAcao("Saudar", "Ele baixa a espada.", INIMIGO_ACAO_SOCIAL, 26, -15, 2, 1, .5f, 100);
            inimigo.acoes[2] = CriarAcao("Desafiar", "Ele aceita com rigor.", INIMIGO_ACAO_SOCIAL, -15, 30, 1, 2, 1.0f, 100);
            inimigo.acoes[3] = CriarAcao("Conversar", "Uma petala cai devagar.", INIMIGO_ACAO_SOCIAL, 18, -20, 3, 1, .5f, 100);
            inimigo.total_acoes = 4; break;
        case INIMIGO_CHEFE_SINEIRO:
            snprintf(inimigo.nome, sizeof(inimigo.nome), "Sineiro Vazio");
            inimigo.vida_maxima = 100; inimigo.hp = inimigo.vida_maxima; inimigo.tipo = TIPO_SOMBRA; inimigo.tipo2 = TIPO_LUZ;
            inimigo.possui_tipo2 = true; inimigo.chefe = true; inimigo.agressividade = 55;
            inimigo.ids_padroes[0] = PADRAO_SINO_CANHOES;
            inimigo.ids_padroes[1] = PADRAO_SINO_IMPACTO;
            inimigo.ids_padroes[2] = PADRAO_SINO_DESAFIO;
            inimigo.ids_padroes[3] = PADRAO_SINO_GAS_SOMBRA;
            inimigo.ids_padroes[4] = PADRAO_SINO_ECO;
            inimigo.total_padroes = 5;
            snprintf(inimigo.descricao, sizeof(inimigo.descricao), "Um sino sem som guarda o quinto andar.");
            inimigo.acoes[0] = CriarAcao("Analisar", "", INIMIGO_ACAO_ANALISAR, 0, 0, 0, 0, 1.0f, 100);
            inimigo.acoes[1] = CriarAcao("Escutar", "O metal para de tremer.", INIMIGO_ACAO_SOCIAL, 28, -16, 3, 1, .5f, 100);
            inimigo.acoes[2] = CriarAcao("Bater no sino", "O eco o enfurece.", INIMIGO_ACAO_SOCIAL, -15, 30, 1, 2, 1.0f, 100);
            inimigo.acoes[3] = CriarAcao("Respirar", "O vazio parece menos pesado.", INIMIGO_ACAO_SOCIAL, 18, -20, 3, 1, .5f, 100);
            inimigo.total_acoes = 4; break;
        case INIMIGO_CHEFE_ULTIMA_TORRE:
            snprintf(inimigo.nome, sizeof(inimigo.nome), "A Ultima Torre");
            inimigo.vida_maxima = 145; inimigo.hp = inimigo.vida_maxima; inimigo.tipo = TIPO_NEUTRO; inimigo.chefe = true; inimigo.agressividade = 62;
            inimigo.ids_padroes[0] = PADRAO_TORRE_PARKOUR;
            inimigo.ids_padroes[1] = PADRAO_TORRE_CANHOES;
            inimigo.ids_padroes[2] = PADRAO_TORRE_DESAFIO;
            inimigo.ids_padroes[3] = PADRAO_TORRE_ESMAGAMENTO; inimigo.total_padroes = 4;
            snprintf(inimigo.descricao, sizeof(inimigo.descricao), "A torre responde com a sua propria voz.");
            inimigo.acoes[0] = CriarAcao("Analisar", "", INIMIGO_ACAO_ANALISAR, 0, 0, 0, 0, 1.0f, 100);
            inimigo.acoes[1] = CriarAcao("Lembrar", "As paredes ficam mais leves.", INIMIGO_ACAO_SOCIAL, 26, -16, 3, 1, .5f, 100);
            inimigo.acoes[2] = CriarAcao("Gritar", "A escada se fecha em volta.", INIMIGO_ACAO_SOCIAL, -20, 35, 1, 2, 1.0f, 100);
            inimigo.acoes[3] = CriarAcao("Perdoar", "Uma janela se abre no alto.", INIMIGO_ACAO_SOCIAL, 20, -22, 3, 1, .5f, 100);
            inimigo.total_acoes = 4; break;
        case INIMIGO_BRILHO_SAPO:
        default:
            snprintf(inimigo.nome, sizeof(inimigo.nome), "Sapo de Vidro");
            inimigo.vida_maxima = 42;
            inimigo.hp = inimigo.vida_maxima;
            inimigo.tipo = TIPO_AGUA;
            inimigo.tipo2 = TIPO_LUZ;
            inimigo.possui_tipo2 = true;
            inimigo.agressividade = 55;
            inimigo.ids_padroes[0] = PADRAO_SAPO_BARREIRAS;
            inimigo.ids_padroes[1] = PADRAO_SAPO_VITORIAS_REGIAS;
            inimigo.ids_padroes[2] = PADRAO_SAPO_SALTOS; inimigo.total_padroes = 3;
            snprintf(inimigo.descricao, sizeof(inimigo.descricao),
                     "Seu brilho gentil esconde uma pressa estranha.");
            inimigo.acoes[0] = CriarAcao("Analisar", "", INIMIGO_ACAO_ANALISAR, 0, 0, 0, 0, 1.0f, 100);
            inimigo.acoes[1] = CriarAcao("Cantar", "O brilho pulsa no ritmo.", INIMIGO_ACAO_SOCIAL, 28, -10, 2, 1, 0.50f, 100);
            inimigo.acoes[1].limite_agressividade = 45;
            inimigo.acoes[1].multiplicador_calmo = 1.25f;
            inimigo.acoes[1].multiplicador_furioso = 0.40f;
            inimigo.acoes[2] = CriarAcao("Ameacar", "O sapo recua, ofendido.", INIMIGO_ACAO_SOCIAL, -20, 35, 1, 2, 1.0f, 100);
            inimigo.acoes[3] = CriarAcao("Acalmar", "A agua volta a ficar lisa.", INIMIGO_ACAO_SOCIAL, 18, -20, 3, 1, 0.50f, 100);
            inimigo.acoes[3].possui_bonus_tipo = true;
            inimigo.acoes[3].tipo_bonus = TIPO_AGUA;
            inimigo.acoes[3].multiplicador_tipo = 1.20f;
            inimigo.total_acoes = 4;
            break;
    }
    return inimigo;
}

void InimigoReceberDano(Inimigo *inimigo, int dano, float efetividade)
{
    inimigo->hp -= dano;
    if (inimigo->hp < 0) inimigo->hp = 0;
    inimigo->forca_flash_acerto = efetividade;
    inimigo->temporizador_flash_acerto = efetividade >= 2.0f ? 0.34f : 0.18f;
    if (inimigo->chefe && !inimigo->segunda_fase && inimigo->hp * 2 <= inimigo->vida_maxima) {
        inimigo->segunda_fase = true;
        inimigo->agressividade = LimitarValorInimigo(inimigo->agressividade + 20);
        inimigo->indice_proximo_padrao += 2;
    }
    InimigoAtualizarPoupavel(inimigo);
}

static float MultiplicadorAcao(const Inimigo *inimigo, const AcaoInimigo *acao)
{
    float multiplicador = 1.0f;
    int indice_acao = (int)(acao - inimigo->acoes);

    if (acao->possui_bonus_tipo && (inimigo->tipo == acao->tipo_bonus ||
                                   (inimigo->possui_tipo2 && inimigo->tipo2 == acao->tipo_bonus))) {
        multiplicador *= acao->multiplicador_tipo;
    }
    multiplicador *= inimigo->agressividade <= acao->limite_agressividade ?
                  acao->multiplicador_calmo : acao->multiplicador_furioso;
    if (inimigo->indice_ultima_acao == indice_acao) {
        for (int repeat = 0; repeat < inimigo->usos_acao_repetida; repeat++) {
            multiplicador *= acao->penalidade_repeticao;
        }
    }
    return multiplicador;
}

bool InimigoAcaoDisponivel(const Inimigo *inimigo, int indice_acao, char *motivo, int tamanho_motivo)
{
    const AcaoInimigo *acao;

    if (indice_acao < 0 || indice_acao >= inimigo->total_acoes) return false;
    acao = &inimigo->acoes[indice_acao];
    if (acao->efeito == INIMIGO_ACAO_ANALISAR) {
        if (inimigo->analisado) {
            snprintf(motivo, (size_t)tamanho_motivo, "Ja analisado");
            return false;
        }
        return true;
    }
    if (!inimigo->analisado) {
        snprintf(motivo, (size_t)tamanho_motivo, "Analise primeiro");
        return false;
    }
    if (acao->maximo_usos > 0 && inimigo->usos_acoes[indice_acao] >= acao->maximo_usos) {
        snprintf(motivo, (size_t)tamanho_motivo, "Esgotada");
        return false;
    }
    if (inimigo->recargas_acoes[indice_acao] > 0) {
        snprintf(motivo, (size_t)tamanho_motivo, "Espere %d turno(s)", inimigo->recargas_acoes[indice_acao]);
        return false;
    }
    if (MultiplicadorAcao(inimigo, acao) <= 0.01f) {
        snprintf(motivo, (size_t)tamanho_motivo, "Ja ouviu demais");
        return false;
    }
    if (inimigo->agressividade > acao->agressividade_maxima_exigida) {
        snprintf(motivo, (size_t)tamanho_motivo, "Inquieto demais");
        return false;
    }
    return true;
}

bool InimigoAplicarAcao(Inimigo *inimigo, int indice_acao, char *mensagem, int tamanho_mensagem)
{
    AcaoInimigo *acao;
    char motivo[48] = "";
    float multiplicador;

    if (!InimigoAcaoDisponivel(inimigo, indice_acao, motivo, (int)sizeof(motivo))) {
        snprintf(mensagem, (size_t)tamanho_mensagem, "%s.", motivo[0] != '\0' ? motivo : "Acao indisponivel");
        return false;
    }
    acao = &inimigo->acoes[indice_acao];
    if (acao->efeito == INIMIGO_ACAO_ANALISAR) {
        inimigo->analisado = true;
        return true;
    }
    multiplicador = MultiplicadorAcao(inimigo, acao);
    inimigo->ultima_variacao_misericordia = (int)lroundf((float)acao->variacao_misericordia * multiplicador);
    inimigo->ultima_variacao_agressividade = (int)lroundf((float)acao->variacao_agressividade * multiplicador);
    inimigo->misericordia = LimitarValorInimigo(inimigo->misericordia + inimigo->ultima_variacao_misericordia);
    inimigo->agressividade = LimitarValorInimigo(inimigo->agressividade + inimigo->ultima_variacao_agressividade);
    inimigo->usos_acoes[indice_acao]++;
    inimigo->recargas_acoes[indice_acao] = acao->turnos_recarga + 1;
    inimigo->ultimo_turno_acoes[indice_acao] = inimigo->turno_jogador;
    if (inimigo->indice_ultima_acao == indice_acao) {
        inimigo->usos_acao_repetida++;
    } else {
        inimigo->indice_ultima_acao = indice_acao;
        inimigo->usos_acao_repetida = 1;
    }
    InimigoAtualizarPoupavel(inimigo);
    snprintf(mensagem, (size_t)tamanho_mensagem, "%s", acao->resposta);
    return true;
}

void InimigoAvancarTurno(Inimigo *inimigo)
{
    inimigo->turno_jogador++;
    for (int indice = 0; indice < inimigo->total_acoes; indice++) {
        if (inimigo->recargas_acoes[indice] > 0) inimigo->recargas_acoes[indice]--;
    }
}
