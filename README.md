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

No Windows deste computador, a Raylib esta disponivel pelo MSYS2 UCRT64.

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

- Setas ou WASD: navegar no titulo e nos menus
- Enter, Espaco ou Z: confirmar
- Em batalha, esquerda/direita: escolher ATACAR, AGIR, ITEM ou POUPAR
- Em submenus, cima/baixo: escolher uma opcao
- Enter, Espaco ou Z: confirmar ou acelerar/avancar o texto
- X ou Backspace: voltar de um submenu de batalha
- Durante a defesa, setas ou WASD: mover a alma
- QTE de timing: aperte Espaco, Enter ou Z quando os aneis coincidirem
- QTE de sequencia: repita W, A, S e D mostrados na ordem (setas tambem funcionam)
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

Para adicionar um tipo novo, inclua-o em `Type` e `TYPE_COUNT` em `include/types.h`, adicione
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
defesa e QTE. ESPACO, ENTER, Z, X ou Backspace pulam a dica atual. **Opcoes > Rever tutorial**
redefine essas dicas. O save agora esta na versao 3; saves da versao 2 sao migrados mantendo a
progressao e exibindo o tutorial, enquanto arquivos invalidos continuam seguros e recriam o padrao.

## Como expandir o conteudo

Os modulos `player`, `enemy`, `battle`, `bullets`, `types`, `qte`, `items`, `tower` e `ui` ja
sustentam uma run completa. `save` e `render` cuidam da progressao e apresentacao entre runs.

### QTEs e dificuldade

Cada `Attack` em `include/player.h` escolhe seu QTE por `qte_kind`. Os tres tipos atuais sao
`QTE_TIMING`, `QTE_SEQUENCE` e `QTE_HOLD`. A dificuldade usa o andar atual; os tempos, a guarda
de 0,30 s e janelas de
acerto estao em `src/qte.c`. Todo QTE mostra `PREPARAR...` nessa guarda e so comeca a contar depois
dela. Para criar outro QTE, adicione um valor em `QteKind`, inicialize-o em `qte_start`,
atualize-o em `qte_update` e desenhe-o em `qte_draw`.

Os testes podem ser executados com `cmake --build build --target test_qte` ou `make test-qte`.

### Criar uma acao de AGIR ou um inimigo

Em `src/enemy.c`, cada caso de `EnemyCreate` preenche `enemy.actions` com ate quatro
`EnemyAction`. `Analisar` usa `ENEMY_ACTION_ANALYZE` e fica sempre disponivel; as demais usam
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

Adicione uma entrada em `ITEM_DEFINITIONS`, em `src/items.c`, e o respectivo id em `ItemId` no
arquivo `include/items.h`. Os efeitos disponiveis sao `ITEM_EFFECT_HEAL`,
`ITEM_EFFECT_DAMAGE_BUFF`, `ITEM_EFFECT_DEFENSE_TYPE` e `ITEM_EFFECT_SURPRISE_DAMAGE`.
Para colocar o item inicial na mochila, acrescente o id e a quantidade em `PlayerInit`.

O teste da Fase 6 verifica misericordia, exaustao e buffs: execute
`cmake --build build --target test_phase6` ou `make test-phase6`.

### Torre, chefes e recompensas

`Tower`, em `include/tower.h`, guarda a seed, o andar, as moedas e a lista de inimigos sorteados.
`TOWER_FLOORS` controla a quantidade de andares. Os andares 5 e 10 sao chefes; os demais sorteiam
inimigos comuns sem repetir o mesmo inimigo consecutivamente. Depois do andar 3 ocorre descanso,
depois do 6 ocorre loja e depois do 9 ocorre descanso. A escala de HP, dano de projeteis e moedas
fica em `TowerScaleEnemy`, em `src/tower.c`.

As recompensas sao sorteadas em `TowerRollRewards`: ataque, item ou atributo. Ataques ficam na
tabela `REWARD_ATTACKS`, em `src/player.c`; para adicionar um, inclua-o nessa tabela com tipo,
dano e `qte_kind`. A loja usa `TowerRollShop` e o preco base de cada item pode ser ajustado em
`ItemShopPrice`, em `src/items.c`. Chefes sao definidos em `EnemyCreate` e usam seis padroes;
ao chegar a 50% de HP entram na segunda fase.

O teste da torre verifica chefes, ausencia de repeticao consecutiva, recompensa sem ataque duplicado
e limite do inventario: `cmake --build build --target test_tower` ou `make test-tower`.

### Save, desbloqueios e visual

O save fica em `save.dat`, na pasta em que o executavel e iniciado. Ele tem versao e checksum; se
nao existir, estiver corrompido ou for de outra versao, o jogo inicia um save padrao sem travar.
O teste `test_save` valida salvar/carregar, corrupcao, versao antiga e desbloqueios:
`cmake --build build --target test_save` ou `make test-save`.

Os desbloqueios ficam na tabela `UNLOCKS`, em `src/save.c`. Cada regra pode pedir andar alcancado,
vitorias ou poupadas e libera um item ou ataque. A tela Relíquias permite gastar Fragmentos por um
item ou ataque inicial; apenas uma escolha fica equipada para a proxima run.

As paletas vivem em `include/palettes.h`. Para criar outra, adicione um `FloorPalette` com fundo,
cor flutuante e cor do papel. `render.c` faz a transicao suave. O shader e propositalmente opcional:
`ENABLE_OPTIONAL_SHADER` em `src/render.c` fica desligado por padrao e o jogo possui fallback sem
shader. O tremor de linha pode ser desligado em Opções; F11 alterna tela cheia.

### Adicionar um padrao de projeteis

Em `src/bullets.c`, adicione um novo item na tabela `PATTERNS`, com duracao, tamanho da caixa,
intervalo, quantidade, velocidade, raio, tipo e dano. Em seguida, acrescente o id em
`BulletPatternId` e na lista `pattern_ids` do inimigo em `src/enemy.c`. Os tres modos atuais de
spawn sao chuva, onda lateral com brecha e espiral. Novos modos podem ser acrescentados no unico
`switch` de spawn, sem duplicar o loop de movimento, colisao ou dano.

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
usos e cooldown. Os cinco estilos de efeito sao `IMPACT`, `PROJECTILE`, `SLASH`, `RAIN` e `BEAM`.
Todo efeito procura primeiro `fx_<ataque>_sheet.png`, depois `fx_type_<tipo>_sheet.png`; se nenhum
PNG estiver instalado, um efeito geometricamente desenhado por codigo ocupa o lugar.

`F8`, em build de depuracao, abre o teste de efeitos: A/D escolhe o ataque, W/S escolhe o resultado
do QTE e Espaco toca o efeito. `F6` recarrega sprites. A lista completa de nomes de imagem e
dimensoes fica em `assets/sprites/README.md`.

Status: Queimar causa dano no inicio do turno inimigo; Molhar aumenta em 25% ataques de Planta e
Luz; Enraizar reduz a velocidade da defesa; Cegar reduz o dano; Atordoar faz o inimigo perder um
turno. Usos e cooldowns impedem repetir ataques pesados sem custo. Rode
`cmake --build build --target test_phase10` ou `make test-phase10` para validar essa logica.
