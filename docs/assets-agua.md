# Peças de arte — lote Água

As peças finais abaixo ainda não existem como arquivos isolados. Todas as onze entradas já estão em `assets/pilot-assets.txt`, apontando para o arquivo existente `assets/sprites/bullet_agua.png`. Esse fallback ativa arte temporária animada desenhada pelo jogo; não é apresentado como arte final.

Fornecer PNG, sem texto, moldura ou cenário. Superfícies devem preencher o quadro e repetir sem emendas; objetos isolados devem ter transparência. As medidas são sugestões, não requisitos do carregador.

| Chave do manifesto | Peça faltante | Tamanho sugerido | Orientação / tratamento |
|---|---|---|---|
| agua_onda | Superfície de água com espuma | 64 × 64 | Textura repetível nos dois eixos; animação de UV |
| agua_escudo | Quebra-mar / escudo da falange | 96 × 16 | Horizontal, centro no eixo; versão de bocal pode usar recorte circular |
| agua_jato | Superfície de jato | 64 × 64 | Repetível; fluxo principal para a direita |
| agua_dique | Tábuas do convés | 64 × 64 | Repetível; tábuas na vertical |
| agua_lama | Lama com pequenas bolhas | 64 × 64 | Repetível; usada em pegadas e faixas |
| agua_bolha | Bolha isolada | 96 × 96 | Circular, centro em 48/48, fundo transparente |
| agua_neve | Massa de neve granular | 64 × 64 | Repetível; aplicada dentro da massa da avalanche |
| agua_estaca | Estaca de gelo isolada | 48 × 12 | Ponta para a direita (+X); o jogo gira para baixo |
| agua_fissura | Superfície de gelo rachado | 64 × 64 | Repetível; aplicada às placas móveis |
| agua_prisma | Emissor de água do Sapo | 32 × 32 | Centro em 16/16, fundo transparente |
| agua_membrana | Superfície de película de água | 32 × 64 | Repetível; borda dourada de regra preservada pelo jogo |

Para substituir: colocar a imagem no projeto, alterar somente o valor da chave no manifesto e pressionar F6 no F8. A colisão e a mecânica permanecem no código, independentes da dimensão da imagem. A água permanece recortada nos polígonos reais, sem esticar um sprite quadrado sobre a arena. A bolha e o emissor recebem rotação/pulso; texturas de área têm deslocamento de superfície.

O contorno de colisão continua visível e o aviso não desenha a trajetória futura. Use B para conferir o encaixe da arte. O escudo usa corpo geométrico e imagem sobreposta; o bocal é circular. Não é necessário sprite de tsunami ocupando a tela inteira.
