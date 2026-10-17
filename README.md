# Ascensao da Torre

Roguelike 2D de batalhas por turnos, feito em C11 e Raylib. A jornada possui 10 andares e nao tem exploracao: cada decisao acontece em menus, recompensas e combates.

## Estado atual

**Fase 8C concluida:** save migrável, tutorial contextual, áudio procedural, polimento,
acessibilidade e revisão de balanceamento. As Fases 1 a 7 tambem estao prontas.
As Fases 1 a 7 tambem estao prontas.

## Dependencias

- Compilador C com suporte a C11
- Raylib instalada
- CMake 3.20+ ou Make

No Windows, instale as dependencias pelo [MSYS2 UCRT64](https://www.msys2.org/).
Instale o MSYS2 em `C:\msys64`, abra o terminal **MSYS2 UCRT64** e execute:

```sh
pacman -Syu
```

Se o terminal solicitar que seja fechado, reabra o **MSYS2 UCRT64** e execute
`pacman -Syu` novamente. Depois instale as ferramentas e a Raylib:

```sh
pacman -S --needed mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-ninja mingw-w64-ucrt-x86_64-raylib
```

A tarefa do VS Code (`Ctrl+Shift+B`) configura e compila o projeto, incluindo
`C:\msys64\ucrt64\bin` no PATH do processo. Se instalar o MSYS2 em outro local,
ajuste esse caminho em `.vscode/tasks.json` e nos comandos abaixo.
O erro de comando CMake nao reconhecido indica que a ferramenta ainda nao foi
instalada ou que sua pasta `bin` nao esta no PATH.

## Compilar e executar com CMake

```powershell
$env:Path = 'C:\msys64\ucrt64\bin;' + $env:Path
cmake -S . -B build -G Ninja
cmake --build build
.\build\ascensao_torre.exe
```

No Linux e macOS, use os mesmos comandos no terminal, desde que CMake encontre a Raylib instalada.

## Compilar com Make

```sh
make
make run
```

O Makefile usa `pkg-config` para localizar a Raylib.

## Controles atuais

Os inimigos agora usam [golpes exclusivos e novas mecanicas de esquiva](docs/combat-patterns.md),
com avisos antes do impacto: saltos, mergulhos, crescimento de pocas, luz protetora,
olhar que exige ficar parado, duelos de espada, ecos e prensas.

- Setas ou WASD: navegar no titulo e nos menus
- Enter, Espaco ou Z: confirmar
- Em batalha, esquerda/direita: escolher ATACAR, AGIR, ITEM ou POUPAR
- Em submenus, cima/baixo: escolher uma opcao
- Enter, Espaco ou Z: confirmar ou acelerar/avancar o texto
- X ou Backspace: voltar de um submenu de batalha
- Durante a defesa, setas ou WASD: mover a alma; Shift: executar um dash na direcao pressionada
- Nas defesas com chao/plataformas, Espaco, W ou Cima: pular (A/D ou esquerda/direita movem)
- QTE de impacto: aperte Espaco, Enter ou Z quando os aneis coincidirem
- QTE de ritmo: acerte tres zonas sucessivas usando a mesma tecla de confirmacao
- QTE de carga: segure Espaco, Enter ou Z e solte na faixa verde
- Esc: pausar

AGIR, ITEM e POUPAR gastam o turno: depois da mensagem, o inimigo atacara. POUPAR da mais
moedas e XP, mas so encerra a batalha quando a misericordia chegar a 100% ou o inimigo estiver
exausto (menos de 25% de HP), desde que a agressividade esteja abaixo de 67.

## Tipos

| Atacante \ Defensor | Chama | Agua | Planta | Luz | Sombra | Neutro |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| Chama | 1 | 0.5 | 2 | 1 | 1 | 1 |
| Agua | 2 | 1 | 0.5 | 1 | 1 | 1 |
| Planta | 0.5 | 2 | 1 | 1 | 1 | 1 |
| Luz | 1 | 1 | 1 | 1 | 2 | 1 |
| Sombra | 1 | 1 | 1 | 2 | 1 | 1 |
| Neutro | 1 | 1 | 1 | 1 | 1 | 1 |

O primeiro tipo da linha ataca o tipo da coluna. Inimigos com dois tipos multiplicam os dois
resultados. A matriz editavel fica em `src/types.c`.

Para adicionar um tipo novo, inclua-o em `Tipo` e `TIPO_TOTAL` em `include/types.h`, adicione
a linha e coluna correspondentes em `EFFECTIVENESS`, seu nome e cor em `src/types.c`, e atualize
`tests/test_types.c` com os valores esperados.

## Fase 8B: audio, acessibilidade e balanceamento

O jogo gera efeitos e tres trilhas curtas diretamente em memoria: andares comuns, chefes e
descanso/loja. Se o dispositivo de audio falhar, a run continua silenciosamente. Para substituir
um efeito gerado, crie `assets/audio/<nome>.wav`; os nomes aceitos estao em `EFFECT_FILES`, em
`src/audio.c` (por exemplo, `confirm.wav`, `qte_perfect.wav` e `victory.wav`).

Em **Opcoes** ficam os volumes, tela cheia, tremor de linha, **Efeitos reduzidos** e dificuldade.
Efeitos reduzidos remove tremores visuais; Calmo recebe 25% menos dano, Normal usa a curva padrao
e Dificil recebe 25% mais dano e sobe um nivel na dificuldade dos QTEs. Todas as constantes de
curva ficam centralizadas em `include/balance.h`.

A tela **Como jogar** fica no titulo e no menu de pausa. A pausa agora oferece continuar, opcoes,
como jogar, desistir com confirmacao ou voltar ao titulo. Ela congela defesa e QTE normalmente.

### Simulacao de balanceamento

Execute `cmake --build build --target test_balance` ou `make test-balance`. O teste nao abre
janela: imprime uma simulacao deterministica de lutas representativas por andar para tres estilos
de bot (aleatorio, focado em tipo e poupador), incluindo taxa de vitoria e HP medio. Ele serve para
comparar alteracoes em `balance.h` antes de testar jogando.

O simulador roda 1.000 batalhas por andar, estilo de bot e dificuldade. Uma batalha termina por
HP do jogador zerado, inimigo derrotado ou inimigo ainda vivo depois de 18 turnos. Isto torna a
dificuldade capaz de afetar a taxa de vitoria, e nao apenas o HP exibido.

### Telemetria de playtest

Pressione **F3** durante o jogo, ou execute `ascensao_torre.exe --telemetry`, para gravar
`playtest_log.csv`. O arquivo registra cada batalha (andar, inimigo, vitoria, HP perdido, turnos,
duracao e resultados de QTE) e o resumo da run. Ele nao e criado quando a telemetria esta desligada.

### Release e distribuicao no Windows

Com Make: `make release`. Com CMake: `cmake -S . -B build-release -G Ninja -DCMAKE_BUILD_TYPE=Release`
e depois `cmake --build build-release`. A build de release usa `-O2` e `-DNDEBUG`.

Tambem existe o preset: `cmake --preset release` e `cmake --build --preset release`.

Para montar `dist/`, copie `ascensao_torre.exe`, `assets/` caso tenha colocado sons personalizados,
e a DLL da Raylib se a sua instalacao gerar executavel dinamico. A build atual pelo MSYS2 UCRT64
normalmente tambem precisa das DLLs correspondentes ao lado do executavel.

### Tutorial e save

Na primeira run, o jogo mostra cinco dicas curtas: menu, tipos/Analisar, mercy/agressividade,
defesa e QTE. Cada dica pausa os comandos do jogo ate apertar ESPACO para fechar.
**Opcoes > Dicas** ativa ou desativa as dicas e salva a escolha. **Opcoes > Rever tutorial**
redefine essas dicas. O save agora esta na versao 3; saves da versao 2 sao migrados mantendo a
progressao e exibindo o tutorial, enquanto arquivos invalidos continuam seguros e recriam o padrao.

## Como expandir o conteudo

Os modulos `player`, `enemy`, `battle`, `bullets`, `types`, `qte`, `items`, `tower` e `ui` ja
sustentam uma run completa. `save` e `render` cuidam da progressao e apresentacao entre runs.

### QTEs e dificuldade

Cada `Ataque` em `include/player.h` escolhe seu QTE por `tipo_qte`. Os tres tipos atuais sao
`QTE_IMPACTO`, `QTE_RITMO` e `QTE_CARGA`. A dificuldade usa o andar atual; os tempos, a guarda
e as janelas de acerto estao em `src/qte.c`. Todo QTE mostra `PREPARE-SE` nessa guarda e so comeca
a contar depois dela. Para criar outro QTE, adicione um valor em `TipoQte`, inicialize-o em
`qte_iniciar`, atualize-o em `qte_atualizar_com_entrada` e desenhe-o em `qte_desenhar`.

Os testes podem ser executados com `cmake --build build --target test_qte` ou `make test-qte`.

### Criar uma acao de AGIR ou um inimigo

Em `src/enemy.c`, cada caso de `InimigoCriar` preenche `inimigo.acoes` com ate quatro
`AcaoInimigo`. `Analisar` usa `INIMIGO_ACAO_ANALISAR` e fica sempre disponivel; as demais usam
`ENEMY_ACTION_SOCIAL`. Os campos importantes sao:

- `mercy_delta` e `aggression_delta`: mudancas, positivas ou negativas, limitadas entre 0 e 100.
- `max_uses`: limite por batalha (`0` significa ilimitado).
- `cooldown_turns`: quantos turnos precisam passar ate poder repetir a acao.
- `repeat_penalty`: multiplicador aplicado quando a mesma acao e repetida consecutivamente.
- `max_aggression_required`: agressividade maxima para a acao funcionar.

Exemplo: `Elogiar` da Mariposa usa `mercy_delta = 30`, `aggression_delta = -5`, pode ser usada
duas vezes, tem um turno de cooldown e `repeat_penalty = 0.50f`; repetida, ela rende so metade.
`Acalmar` tambem demonstra um efeito condicional: ela funciona melhor contra o tipo Sombra e piora
quando o inimigo esta muito agressivo. A interface mostra acoes indisponiveis em cinza, com o motivo.

Depois de Analisar, aparecem barras animadas de misericordia (amarela) e agressividade (vermelha).
A agressividade baixa, media e alta escolhe respectivamente padroes de dificuldade 1, 2 e 3. Na faixa
alta, projeteis recebem +15% de velocidade e +25% de dano. Mesmo com Mercy cheia, POUPAR falha se a
agressividade estiver em 67 ou mais: primeiro e preciso acalmar o inimigo.

### Criar um item

Adicione uma entrada em `DEFINICOES_ITENS`, em `src/items.c`, e o respectivo id em `IdItem` no
arquivo `include/items.h`. Os efeitos disponiveis sao `ITEM_EFFECT_HEAL`,
`ITEM_EFEITO_BUFF_DANO`, `ITEM_EFEITO_TIPO_DEFESA` e `ITEM_EFEITO_DANO_SURPRESA`.
Para colocar o item inicial na mochila, acrescente o id e a quantidade em `JogadorInicializar`.

O teste da Fase 6 verifica misericordia, exaustao e buffs: execute
`cmake --build build --target test_phase6` ou `make test-phase6`.

### Torre, chefes e recompensas

`Torre`, em `include/tower.h`, guarda a semente, o andar, as moedas e a lista de inimigos sorteados.
`TOWER_FLOORS` controla a quantidade de andares. Os andares 5 e 10 sao chefes; os demais sorteiam
inimigos comuns sem repetir o mesmo inimigo consecutivamente. Depois do andar 3 ocorre descanso,
depois do 6 ocorre loja e depois do 9 ocorre descanso. A escala de HP, dano de projeteis e moedas
fica em `TorreEscalarInimigo`, em `src/tower.c`.

As recompensas sao sorteadas em `TorreSortearRecompensas`: ataque, item ou atributo. Ataques ficam na
tabela `ATAQUES_RECOMPENSA`, em `src/player.c`; para adicionar um, inclua-o nessa tabela com tipo,
dano e `tipo_qte`. A loja usa `TorreSortearLoja` e o preco base pode ser ajustado em
`BALANCEAMENTO_LOJA_BASE_PRECO`, em `include/balance.h`. Chefes sao definidos em `InimigoCriar`;
ao chegar a 50% de HP entram na segunda fase.

O teste da torre verifica chefes, ausencia de repeticao consecutiva, recompensa sem ataque duplicado
e limite do inventario: `cmake --build build --target test_tower` ou `make test-tower`.

### Salvamento, desbloqueios e visual

O save fica em `save.dat`, na pasta em que o executavel e iniciado. Ele tem versao e checksum; se
nao existir, estiver corrompido ou for de outra versao, o jogo inicia um save padrao sem travar.
O teste `test_save` valida salvar/carregar, corrupcao, versao antiga e desbloqueios:
`cmake --build build --target test_save` ou `make test-save`.

Os desbloqueios ficam na tabela `UNLOCKS`, em `src/save.c`. Cada regra pode pedir andar alcancado,
vitorias ou poupadas e libera um item ou ataque. A tela Relíquias permite gastar Fragmentos por um
item ou ataque inicial; apenas uma escolha fica equipada para a proxima run.

As paletas vivem em `include/palettes.h`. Para criar outra, adicione um `FloorPalette` com fundo,
cor flutuante e cor do papel. `render.c` faz a transicao suave diretamente com formas do raylib.
O tremor de linha pode ser desligado em Opções; F11 alterna tela cheia.

### Adicionar um padrao de projeteis

Em `src/bullets.c`, adicione um novo item na tabela `PATTERNS`, com duracao, tamanho da caixa,
intervalo, quantidade, velocidade, raio, tipo e dano. Em seguida, acrescente o id em
`IdPadraoProjetil` e na lista `ids_padroes` do inimigo em `src/enemy.c`. Golpes de sequencia vivem
em `src/patterns.c`; golpes com movimento ou fisica propria vivem em `src/motion_patterns.c`.
Cada golpe novo desse segundo grupo precisa receber um `ModoMovimento` exclusivo. Compartilhe a
infraestrutura de colisao e gravidade, mas nao reutilize a mesma regra de jogo com outra cor.
O repertorio e as regras de justica estao descritos em `docs/combat-patterns.md`.

### Sprites PNG opcionais

Coloque artes PNG em `assets/sprites/`. Nenhum arquivo e obrigatorio: quando um sprite nao existe,
o jogo usa automaticamente o desenho por codigo. Isso permite trocar uma arte sem tocar em nenhuma
regra de combate. Os nomes, IDs e dimensoes recomendadas estao em
`assets/sprites/README.md`.

Para experimentar, crie somente `assets/sprites/soul.png` (16x16) ou
`assets/sprites/enemy_mariposa_poeira.png` (96x96) e execute normalmente. F6 recarrega os
arquivos em uma build de depuracao, enquanto F7 exibe limites e ancoras para ajuste visual.

Inimigos podem ser animados com `enemy_<id>_sheet.png` e um metadado
`enemy_<id>.txt`, por exemplo `frames=4 fps=6 frame_w=96 frame_h=96`. Os quadros ficam em uma
linha horizontal. Tipos, itens e projeteis tambem aceitam PNGs opcionais; o raio de colisao sempre
continua sendo o da logica do jogo, nunca o tamanho da imagem.

Para gerar gabaritos de tamanhos, rode `ascensao_torre --export-placeholders`. Os arquivos serao
criados em `assets/sprites_template/`, prontos para serem redesenhados e copiados para
`assets/sprites/`.

O teste do parser e dos IDs de fallback pode ser rodado com
`cmake --build build --target test_sprites` ou `make test-sprites`.

### Ataques e efeitos visuais

Os ataques usam dados em `src/player.c`: tipo, dano, QTE, quantidade de golpes, efeito, status,
cooldown e usos ilimitados. Os cinco estilos de efeito sao `IMPACT`, `PROJECTILE`, `SLASH`, `RAIN` e `BEAM`.
Todo efeito procura primeiro `fx_<ataque>_sheet.png`, depois `fx_type_<tipo>_sheet.png`; se nenhum
PNG estiver instalado, um efeito geometricamente desenhado por codigo ocupa o lugar.

`F8`, em build de depuracao, abre o teste de padroes de defesa: A/D escolhe o padrao, W/S escolhe
o inimigo e Espaco reinicia a rodada sem dano. `F7` mostra hitboxes e `F6` recarrega sprites. A lista completa de nomes de imagem e
dimensoes fica em `assets/sprites/README.md`.

Status: Queimar causa dano no inicio do turno inimigo; Molhar aumenta em 25% ataques de Planta e
Luz; Enraizar reduz a velocidade da defesa; Cegar reduz o dano; Atordoar faz o inimigo perder um
turno. Os ataques do jogador possuem usos ilimitados; os cooldowns continuam limitando a frequencia dos golpes pesados. Rode
`cmake --build build --target test_phase10` ou `make test-phase10` para validar essa logica.

### Defesa: padroes e sprites

Os 16 padroes de defesa ficam em `src/bullets.c`. Cada um define a caixa, dano, velocidade,
tipo, hitbox e visual de forma data-driven. Os comportamentos especiais incluem orbes que quicam,
petalas orbitais, tiros que se dividem, dardos que param e perseguem, chuva diagonal e ondas
perseguidoras. `test_bullets` valida, sem abrir janela, a configuracao de todos os padroes.

Para artes opcionais, use `bullet_<padrao>.png` antes de `bullet_<tipo>.png`. PNGs podem ter
spritesheet horizontal acompanhado por um `.txt` de metadados. `F8` permite testar a fase de
defesa sem perder vida; `F7` revela as hitboxes, que permanecem circulares e independentes da arte.

O Sineiro tambem possui `gravity_flip`: a alma e puxada contra uma parede e precisa escapar da
faixa de espinhos antes que ela ative; e `shadow_bloom`: orbes de sombra mostram o raio final e
explodem uma vez ao terminar de crescer. Ambos usam o pool independente de zonas em `hazards.c`.
Os valores de fuga, espinhos, raio e dano ficam centralizados em `balance.h`.

Distribuicao atual: Mariposa (rain, side_wave, spiral); Sapo (side_wave, spiral, rain);
Passaro de Cinza (fast_rain, spiral, diagonal_rain); Limo de Menta (wide_wave, petal_orbit,
pinwheel); Fagulha de Lanterna (fast_rain, tight_spiral, spiral); Lebre de Espinho (wide_wave,
side_wave, fast_rain); Gato de Sombra (tight_spiral, spiral, side_wave); Cavaleiro de Petalas
(wide_wave, fast_rain, tight_spiral). O Sineiro e a Ultima Torre possuem oito padroes cada.
