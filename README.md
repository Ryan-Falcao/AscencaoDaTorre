# Ascensao da Torre

Roguelike 2D de batalhas por turnos, feito em C11 e Raylib. A jornada possui 10 andares e nao tem exploracao: cada decisao acontece em menus, recompensas e combates.

## Estado atual

**Fase 8A concluida:** save entre runs, desbloqueios, Relíquias, Opções e visual pastel original.
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

## Proximas fases

1. Audio, polimento e balanceamento.

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
