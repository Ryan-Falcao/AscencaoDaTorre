/* Exercise wrapping with actual glyph metrics, including Portuguese UTF-8. */
#include "../src/ui.c"
#include <assert.h>

static void ConferirQuebra(const char *texto, int largura, int tamanho)
{
    LinhaTexto linhas[MAXIMO_LINHAS_TEXTO];
    int total = OrganizarLinhas(texto, (float)largura, tamanho, linhas);
    int ultimo = 0;
    for (int i = 0; i < total; i++) {
        assert(MedirTrecho(texto, linhas[i].inicio, linhas[i].fim, tamanho) <= largura);
        for (int k = ultimo; k < linhas[i].inicio; k++)
            assert(texto[k] == ' ' || texto[k] == '\n' || texto[k] == '\t');
        assert(((unsigned char)texto[linhas[i].inicio] & 0xC0) != 0x80);
        ultimo = linhas[i].fim;
    }
    for (int k = ultimo; texto[k]; k++) assert(texto[k] == ' ' || texto[k] == '\n');
}

int main(void)
{
    SetTraceLogLevel(LOG_WARNING);
    SetConfigFlags(FLAG_WINDOW_HIDDEN);
    InitWindow(640, 480, "Teste de tipografia");
    InterfaceCarregarFontes();
    assert(fonte_texto.texture.id != GetFontDefault().texture.id);
    assert(fonte_titulo.texture.id != GetFontDefault().texture.id);
    ConferirQuebra("Acentuação: ação, coração, bênção e água.", 90, 18);
    ConferirQuebra("Supercalifragilisticamentecomprida", 45, 18);
    ConferirQuebra("Uma linha\n\nOutra linha", 120, 18);
    ConferirQuebra("  teste  de   espaços ", 80, 18);
    ConferirQuebra("", 80, 18);
    TextoDigitado digitado;
    InterfaceDefinirTexto(&digitado, "ação");
    InterfaceAtualizarTexto(&digitado, 0.05f);
    assert(digitado.caracteres_visiveis == 3);
    while (!digitado.completo) {
        InterfaceAtualizarTexto(&digitado, 0.025f);
        assert(((unsigned char)digitado.texto[digitado.caracteres_visiveis] & 0xC0) != 0x80);
    }
    assert(digitado.caracteres_visiveis == (int)strlen(digitado.texto));
    InterfaceDescarregarFontes();
    CloseWindow();
    puts("Tipografia, palavras longas, quebras explicitas e UTF-8 verificados.");
    return 0;
}
