# Identidade e dificuldade dos golpes

## Regra estrutural

O repertorio ativo tem 32 ataques: tres para cada inimigo comum e quatro para
cada chefe. Os dez ataques em `src/patterns.c` possuem sequencias proprias. Os
22 ataques em `src/motion_patterns.c` tambem possuem uma identidade publica
exclusiva em `ModoMovimento`; `test_motion` falha se duas dessas identidades forem
reutilizadas.

Fisica, colisao, dano e desenho podem compartilhar funcoes internas. A regra que
o jogador aprende, a leitura do aviso e a resposta exigida nao podem compartilhar
um molde apenas com outra cor.

## Repertorio atual

| Personagem | Golpe | Regra principal |
| --- | --- | --- |
| Mariposa | Venezianas | Serpentear por aberturas discretas das asas |
| Mariposa | Poeira azul/laranja | Parar no azul e continuar em movimento no laranja |
| Mariposa | Asas e nuvem | Cruzar a asa e abandonar a nuvem marcada |
| Sapo | Marolas | Saltar ondas de alturas alternadas |
| Sapo | Vitorias-regias | Pular entre folhas que afundam |
| Sapo | Salto e lingua | Sair da sombra, cruzar a onda e mudar de altura |
| Passaro | Revoada | Trocar uma faixa por vez na lacuna da formacao |
| Passaro | Mergulho incendiario | Sair da linha travada e evitar as brasas deixadas nela |
| Passaro | Mergulho em cinco marcas | Abandonar a rota antes da passagem da ave |
| Limo | Mare pegajosa | Ajustar o tempo do salto a cristas largas e estreitas |
| Limo | Ilhas divididas | Recalcular a aterrissagem quando uma jangada se destaca |
| Limo | Broto | Fugir da poca antes que ela se divida em duas filhas |
| Fagulha | Farol cardinal | Rodear feixes que giram em angulos retos |
| Fagulha | Orbita pulsante | Passar pela estrela ausente e alternar nucleo/borda |
| Fagulha | Luz | Alcancar o pequeno quadrado iluminado durante o aviso |
| Lebre | Corrida | Encadear saltos curtos em cercas rapidas |
| Lebre | Tocas | Alcancar saidas elevadas em um percurso irregular |
| Lebre | Pegadas | Escapar de tres aterrissagens em sequencia |
| Gato | Bote | Segurar a direcao contraria ao empurrao indicado |
| Gato | Tres garras | Ler corte vertical, horizontal e bote concentrado |
| Gato | Olhar | Parar sob olhos azuis e correr quando surgem as garras |
| Cavaleiro | Formacao de lancas | Ocupar a unica faixa vazia da formacao |
| Cavaleiro | Justa | Saltar a lamina baixa e permanecer baixo na lanca alta |
| Cavaleiro | Duelo | Responder ao corte vertical e depois ao horizontal |
| Sineiro | Cordas do carrilhao | Escapar de dois badalos travados na posicao anterior |
| Sineiro | Peso do sino | Resistir a puxoes verticais alternados |
| Sineiro | Requiem | Mudar uma casa no tabuleiro a cada compasso |
| Sineiro | Eco | Atravessar aneis de origens alternadas |
| Torre | Escadaria | Subir degraus de alturas crescentes sobre estacas |
| Torre | Bastiao | Ler o lado do disparo e atravessar uma seteira fixa |
| Torre | Colapso | Fugir da coluna marcada sem recuar para o escombro |
| Torre | Prensa | Alternar o eixo de fuga antes das paredes fecharem |

## Justica

- Toda mira dependente do jogador e travada no inicio do aviso. O impacto nao
  persegue a alma.
- O antigo corredor que obrigava o jogador a acompanhar uma caixa foi removido
  do repertorio. As seteiras do Bastiao ficam paradas durante cada disparo.
- `VELOCIDADE_MOVIMENTO_DEFESA` vale 135 px/s. O repertorio atual nao possui areas
  seguras moveis; qualquer mecanica futura desse tipo deve respeitar essa
  velocidade para continuar alcancavel.
- Mudancas de faixa usam destinos adjacentes. Avisos nunca pedem mais distancia
  que a alma percorre antes do impacto.
- Plataformas aceitam sete pixels de margem lateral ao aterrissar. Saltos altos
  ficam limitados a altura alcancavel pela fisica real.
- Golpes baixos e altos da Justa nunca ocupam o mesmo ponto ao mesmo tempo.
- A fase 2 estreita margens e acelera objetos, mas nao acelera areas seguras.
- Ficar parado e dar apenas um toque lateral precisam causar dano em todos os 22
  ataques de movimento.

## Validacao

`test_motion` executa 132 defesas completas: 22 ataques, duas configuracoes de
arena/fase e 30, 60 e 120 FPS. O controlador usa a mesma entrada, velocidade,
gravidade, salto e colisao do jogo. Ele reage apenas ao quadro atual e aos avisos
visiveis. Cada simulacao deve terminar sem dano e com movimento significativo;
golpes de gravidade exigem ao menos quatro saltos.

O mesmo teste executa 44 politicas passivas, ficar parado e dar um unico toque,
que devem falhar. Ele tambem verifica a exclusividade das 22 identidades.

`test_patterns` cobre 648 cenarios dos dez golpes de sequencia, incluindo arenas
de chefe, taxas de quadros diferentes, atraso de reacao e 90 posicoes passivas.
`test_battle` percorre duas rotacoes completas de cada repertorio.

Os testes provam que existe uma rota com os controles reais e rejeitam vitorias
passivas. Dificuldade percebida, clareza visual e diversao ainda devem ser
confirmadas no laboratorio F8 e em runs completas.
