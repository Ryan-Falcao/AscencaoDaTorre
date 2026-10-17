# Sprites opcionais

Todos os PNGs desta pasta sao **opcionais**. Se um arquivo nao existir, o jogo usa o desenho por codigo, sem erro.
Use PNG com transparencia, sem antialiasing e pixel art em escala inteira (1x, 2x ou 3x).

| Categoria | Nome | Tamanho recomendado |
|---|---|---|
| Inimigo | `enemy_<id>.png` | 96x96 ou 128x128 |
| Chefe | `boss_<id>_p1.png`, `boss_<id>_p2.png` | 160x160 ou 192x192 |
| Alma | `soul.png` | 16x16 |
| Tipo | `type_<nome>.png` | 24x24 |
| Item | `item_<id>.png` | 24x24 |
| Projetil | `bullet_<tipo>.png` | 16x16 ou 24x24 |
| Titulo | `title_tower.png` | 200x300 |
| Fundo | `bg_pastel.png`, `bg_boss.png` | 640x480 |

## IDs aceitos

Inimigos: `mariposa_poeira`, `sapo_luz`, `passaro_brasa`, `slime_menta`, `fogo_fatuo`, `lebre_espinho`, `gato_sombra`, `cavaleiro_petala`.

Chefes: `sineiro` e `torre_final` (exemplo: `boss_sineiro_p1.png`).

Tipos: `chama`, `agua`, `planta`, `luz`, `sombra`, `neutro`.

Itens: `cha`, `erva_lunar`, `fita_vermelha`, `sino_torre`, `manto_umido`, `broche_solar`, `fogos`, `bomba_espinho`.

## Animacao por spritesheet

Para um inimigo animado, prefira `enemy_<id>_sheet.png` a imagem estatica e crie
`enemy_<id>.txt` ao lado dele, com uma linha como:

```text
frames=4 fps=6 frame_w=96 frame_h=96
```

Os quadros devem ficar em uma unica linha horizontal.

Em uma build de depuracao, F6 recarrega os PNGs e F7 mostra limites e ancoras.
Use `ascensao_torre --export-placeholders` para criar gabaritos em `assets/sprites_template/`.

## Ataques, efeitos e projeteis

Cada ataque pode ter `attack_<id>.png` (24x24) e um efeito opcional
`fx_<id>_sheet.png`. O efeito deve ter quadros de 96x96 em uma linha horizontal; use o arquivo
`fx_<id>.txt` com `frames=6 fps=12 frame_w=96 frame_h=96`. Se ele nao existir, o jogo procura
`fx_type_<tipo>_sheet.png` e finalmente desenha o efeito por codigo.

IDs de ataque: `brasa`, `gota_clara`, `raiz_rapida`, `raio_manso`, `eco_sombrio`,
`golpe_neutro`, `chama_alta`, `mare_curta`, `vinha_fina`, `luz_guia`, `sombra_fria`,
`pulso_calmo`, `fagulha`, `orvalho`, `espinho_lunar`, `sino_vazio`.

Projeteis aceitam `bullet_<tipo>.png` ou `bullet_<tipo>_sheet.png` (12x12 ou 16x16 por quadro),
com o mesmo metadado `.txt`. Os nomes de tipo sao os mesmos da secao anterior. O aviso opcional
para golpes perigosos e `warn_danger.png` (32x32); sem ele, aparece um circulo com `!` desenhado
pelo jogo.

Um padrao especifico pode sobrescrever o sprite do tipo com `bullet_<padrao>.png`. Os IDs sao:
`rain`, `side_wave`, `spiral`, `fast_rain`, `wide_wave`, `tight_spiral`, `petal_orbit`,
`bounce_orbs`, `moving_wall`, `split_shots`, `cross_lasers`, `pause_darts`, `ring_burst`,
`diagonal_rain`, `chase_wave`, `pinwheel`, `gravity_flip` e `shadow_bloom`.

### Ataques especiais do Sineiro

`gravity_flip` e `shadow_bloom` usam zonas de perigo, e portanto nao dependem do pool normal de
balas. Todos os arquivos abaixo sao opcionais:

| Arquivo | Tamanho / metadado |
|---|---|
| `fx_hit_soul_sheet.png` | 4 frames de 32x32; `fx_hit_soul.txt` |
| `thorn_wall_sheet.png` | 4 frames de 16x48; `thorn_wall.txt` |
| `thorn_wall_warn.png` | 16x48 |
| `shadow_orb_sheet.png` | 4 frames de 32x32; `shadow_orb.txt` |
| `shadow_bloom_warn.png` | 64x64 |
| `shadow_burst_sheet.png` | 6 frames de 96x96; `shadow_burst.txt` |

Exemplo de metadado: `frames=4 fps=12 frame_w=32 frame_h=32`. Espinhos devem apontar para cima;
a versao por codigo usa triangulos claros sobre fundo escuro quando o PNG nao estiver presente.

Os sprites de projeteis especificos tambem podem ser animados: por exemplo,
`bullet_pause_darts_sheet.png` usa `bullet_pause_darts.txt` com
`frames=4 fps=10 frame_w=16 frame_h=16`. A ordem de busca e sempre
`bullet_<padrao>` -> `bullet_<tipo>` -> forma desenhada pelo jogo. A colisao continua usando o
circulo logico, nao o tamanho do PNG.

Para ataques grandes, use `warn_danger.png` (32x32) como marcador opcional. Use
`fx_hit_soul.png` (32x32, ou sheet) para o impacto na alma. `arena_bg_pastel.png` e um fundo
opcional da caixa (320x180 recomendado); `arena_border.png` e uma textura de borda simples de
16x16. Ambos sao esticados para a arena atual e possuem fallback para fundo preto e borda branca.
