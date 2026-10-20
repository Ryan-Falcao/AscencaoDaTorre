# Elenco elemental — novos sprites

Os arquivos abaixo foram criados para a expansao da torre ate o andar 30. Os inimigos comuns usam telas de 96 x 96 pixels. Os conceitos de chefe e elite usam 160 x 160 pixels.

## Conceitos de chefe e elite

| Personagem | Tipo sugerido | Arquivo |
|---|---|---|
| Cultista das Raizes | Planta | `assets/sprites/boss_cultista_raizes.png` |
| Afogado de Coral | Agua | `assets/sprites/boss_afogado_coral.png` |
| Mago das Cinzas | Fogo | `assets/sprites/boss_mago_cinzas.png` |
| Oni das Brasas | Fogo | `assets/sprites/boss_oni_brasas.png` |
| Goblin da Cripta | Sombra | `assets/sprites/boss_goblin_cripta.png` |
| Inquisidor do Sol Partido | Luz | `assets/sprites/boss_inquisidor_sol_partido.png` |

O Mago e o Oni podem formar duas fases do mesmo chefe de Fogo. O Inquisidor pode servir como elite da regiao de Luz ou como conceito alternativo ao Sineiro. A Ultima Torre continua reservada para o chefe final de todos os tipos.

## Inimigos comuns

| Personagem | Tipo | Arquivo |
|---|---|---|
| Soldado das Sarcas | Planta | `assets/sprites/enemy_soldado_sarcas.png` |
| Herbalista da Peste | Planta | `assets/sprites/enemy_herbalista_peste.png` |
| Legionario Afogado | Agua | `assets/sprites/enemy_legionario_afogado.png` |
| Bruxa do Lodo | Agua | `assets/sprites/enemy_bruxa_lodo.png` |
| Revenante de Gelo | Agua | `assets/sprites/enemy_revenante_gelo.png` |
| Esqueleto Carbonizado | Fogo | `assets/sprites/enemy_esqueleto_carbonizado.png` |
| Acendedor da Forja | Fogo | `assets/sprites/enemy_acendedor_forja.png` |
| Penitente sem Rosto | Sombra | `assets/sprites/enemy_penitente_sem_rosto.png` |
| Ladrao da Mortalha | Sombra | `assets/sprites/enemy_ladrao_mortalha.png` |
| Acolito do Vitral | Luz | `assets/sprites/enemy_acolito_vitral.png` |
| Goblin Lanceiro | Neutro | `assets/sprites/enemy_goblin_lanceiro.png` |
| Automato de Pedra | Neutro | `assets/sprites/enemy_automato_pedra.png` |
| Mercenario da Ferrugem | Neutro | `assets/sprites/enemy_mercenario_ferrugem.png` |
| Carcereiro das Correntes | Neutro | `assets/sprites/enemy_carcereiro_correntes.png` |

## Direcao visual usada

- Pixel art compacta com contorno escuro e silhueta legivel.
- Inimigos comuns com roupas e armas simples, poucos acessorios e nenhum efeito grandioso.
- Chefes com escala maior, detalhes mais ricos e iluminacao elemental forte.
- Nenhum inimigo comum usa aparencia de mascote, animal ou criatura fofa.
- Fundo transparente, sem texto, moldura ou cenario dentro do sprite.

A prancha de revisao esta em `assets/sprites/_preview_novos_20.png`.

Os ataques do jogador agora possuem icones proprios, e os perigos inimigos usam projeteis visuais
por elemento inclusive nos padroes caracteristicos e de movimento. A prancha correspondente esta em
`assets/sprites/_preview_ataques.png`.

## Golpes de assinatura

Cada personagem novo abre seu repertorio com uma mecanica exclusiva:

| Personagem | Golpe | Regra principal |
|---|---|---|
| Soldado das Sarcas | Portoes das Sarcas | atravessar tampas pela abertura movel |
| Herbalista da Peste | Frascos da Peste | abandonar a coluna marcada antes da queda |
| Legionario Afogado | Falange Afogada | acompanhar a fileira sem escudos |
| Bruxa do Lodo | Caldeirao Vivo | trocar entre cantos enquanto as bolhas crescem |
| Revenante de Gelo | Prisao de Gelo | escapar pela unica parede descongelada |
| Esqueleto Carbonizado | Pavio Ossudo | passar pelo elo apagado da fileira explosiva |
| Acendedor da Forja | Martelo da Forja | sair da bigorna e cruzar os rebotes |
| Penitente sem Rosto | Incensario | fugir do peso que cai sobre a posicao anterior |
| Ladrao da Mortalha | Reflexo Roubado | escapar do golpe espelhado |
| Acolito do Vitral | Vitral Apagado | acompanhar o painel sem luz |
| Goblin Lanceiro | Sequencia de Estocadas | ler alcances diferentes e trocar de faixa |
| Automato de Pedra | Engrenagens | girar junto dos dentes em orbitas diferentes |
| Mercenario Ferrugem | Ricochete | antecipar a bala e seu rastro nas paredes |
| Carcereiro Correntes | Cela de Correntes | sair da cruz fechada sobre a posicao anterior |
| Cultista das Raizes | Labirinto de Raizes | atravessar portoes horizontal e vertical no compasso |
| Afogado de Coral | Mare de Coral | acompanhar a faixa entre a mare e o teto |
| Mago das Cinzas | Espiral de Cinzas | ocupar os dois bracos ausentes da espiral |
| Goblin da Cripta | Exercito da Cripta | alternar fileiras entre duas marchas opostas |
| Inquisidor Sol | Veredito Solar | correr ao unico quadrante sem luz |
