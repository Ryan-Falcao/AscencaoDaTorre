#include "bullets.h"

#include <assert.h>

/* Este teste nao abre janela: valida os dados copiados para um turno de defesa. */
int main(void)
{
    const Rectangle caixa_menu = {160.0f, 285.0f, 320.0f, 90.0f};

    for (int id = PADRAO_CHUVA; id < PADRAO_TOTAL; id++) {
        EstadoDefesa defesa;
        DefesaInicializar(&defesa);
        DefesaIniciar(&defesa, (IdPadraoProjetil)id, caixa_menu, 50, 0, 1.0f, 1.0f);
        assert(defesa.id_padrao == (IdPadraoProjetil)id);
        assert(defesa.padrao.largura_caixa > 0.0f && defesa.padrao.largura_caixa <= 640.0f);
        assert(defesa.padrao.altura_caixa > 0.0f && defesa.padrao.altura_caixa <= 480.0f);
        assert(defesa.caixa_alvo.y + defesa.caixa_alvo.height <= 326.0f);
        assert(defesa.padrao.raio_colisao > 0.0f);
        assert(defesa.padrao.projeteis_por_criacao > 0);
        assert(DificuldadePadraoProjetil((IdPadraoProjetil)id) >= 1);
    }

    /* Uma arena pedida por chefe nunca pode cobrir o HUD, mesmo sendo maior
       que o corredor livre da tela. */
    EstadoDefesa arena_grande;
    DefesaInicializar(&arena_grande);
    DefesaIniciar(&arena_grande, PADRAO_SINO_ECO, caixa_menu, 50, 0, 1.0f, 1.0f);
    DefesaRedimensionarCaixa(&arena_grande, 520.0f, 400.0f);
    assert(arena_grande.caixa_alvo.width == 520.0f);
    assert(arena_grande.caixa_alvo.y + arena_grande.caixa_alvo.height <= 326.0f);
    /* A faixa de fuga direita permanece utilizavel por toda a espiral de fogo,
       inclusive com agressividade maxima e multiplicador extra de velocidade. */
    EstadoDefesa fire;
    AtributosJogador jogador;
    JogadorInicializar(&jogador);
    DefesaInicializar(&fire);
    DefesaIniciar(&fire, PADRAO_ESPIRAL, caixa_menu, 100, 0, 1.0f, 1.3f);
    fire.posicao_alma.x += 70.0f;
    const int vida_inicial = jogador.hp;
    for (int quadro = 0; quadro < 600; quadro++) {
        DefesaAtualizar(&fire, &jogador, 1.0f / 60.0f);
        assert(jogador.hp == vida_inicial);
    }
    assert(fire.total_criacoes > 5);

    /* Todos os padroes usam a mesma entrada amostrada. Antes, os padroes
       antigos ignoravam EntradaDefesa e consultavam o teclado novamente. */
    EstadoDefesa movimento;
    AtributosJogador jogador_movimento;
    JogadorInicializar(&jogador_movimento);
    DefesaInicializar(&movimento);
    DefesaIniciar(&movimento, PADRAO_CHUVA, caixa_menu, 50, 0, 1.0f, 1.0f);
    movimento.depurar_invencivel = true;
    DefesaAtualizarComEntrada(&movimento, &jogador_movimento, 0.26f, (EntradaDefesa){0});
    const Vector2 posicao_antes = movimento.posicao_alma;
    DefesaAtualizarComEntrada(&movimento, &jogador_movimento, 0.10f,
                              (EntradaDefesa){.movimento = {1.0f, -1.0f}});
    assert(movimento.posicao_alma.x > posicao_antes.x);
    assert(movimento.posicao_alma.y < posicao_antes.y);

    /* O dash pertence a alma, portanto funciona tanto em padroes de projeteis
       quanto nos padroes de movimento e respeita a mesma recarga. */
    const IdPadraoProjetil padroes_dash[] = {PADRAO_CHUVA, PADRAO_SINO_GAS_SOMBRA};
    for (int indice = 0; indice < 2; indice++) {
        EstadoDefesa dash;
        AtributosJogador jogador_dash;
        DefesaInicializar(&dash);
        JogadorInicializar(&jogador_dash);
        DefesaIniciar(&dash, padroes_dash[indice], caixa_menu, 50, 0, 1.0f, 1.0f);
        dash.depurar_invencivel = true;
        DefesaAtualizarComEntrada(&dash, &jogador_dash, 0.26f, (EntradaDefesa){0});
        assert(dash.fase == DEFESA_ATIVA);

        EntradaDefesa entrada_dash = {
            .movimento = {1.0f, 0.0f},
            .dash_pressionado = true
        };
        float x_antes = dash.posicao_alma.x;
        DefesaAtualizarComEntrada(&dash, &jogador_dash, 1.0f / 60.0f, entrada_dash);
        assert(dash.posicao_alma.x - x_antes >= DISTANCIA_DASH_DEFESA);
        assert(dash.temporizador_recarga_dash > 0.0f);

        x_antes = dash.posicao_alma.x;
        DefesaAtualizarComEntrada(&dash, &jogador_dash, 1.0f / 60.0f, entrada_dash);
        assert(dash.posicao_alma.x - x_antes < 5.0f);

        DefesaAtualizarComEntrada(&dash, &jogador_dash, RECARGA_DASH_DEFESA, (EntradaDefesa){0});
        x_antes = dash.posicao_alma.x;
        DefesaAtualizarComEntrada(&dash, &jogador_dash, 1.0f / 60.0f, entrada_dash);
        assert(dash.posicao_alma.x - x_antes >= DISTANCIA_DASH_DEFESA);
    }
    return 0;
}
