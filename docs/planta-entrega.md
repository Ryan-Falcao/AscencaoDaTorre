# Lote 1 por elemento — Planta

Os cinco inimigos da primeira fase da campanha agora têm três ataques próprios cada. Não foram alterados os padrões aprovados de Mariposa, Ladrão e Afogado. Sineiro continua sendo o boss de sombra. Este lote termina nos cinco de planta; os demais elementos aguardam teste e aprovação.

## Acesso pelo executável padrão

F8 abre o teste diretamente no Soldado das Sarcas. TAB avança: Soldado → Herbalista → Limo → Lebre → Cultista → Mariposa → Ladrão → Afogado. SHIFT+TAB volta. 1/2/3 escolhe o ataque. R reinicia. F8 ou X restaura o jogo anterior. WASD/setas move, SHIFT dá dash, P/ESC pausa. F7 muda a dificuldade global, C muda o tipo de defesa, V alterna 40/80 HP. I alterna invencibilidade; manter desligada para avaliar desafio. B mostra colisões. F6 recarrega o manifesto de imagens. O teste pausa no fim da rodada; R repete.

Os oito inimigos disponíveis são definidos em um registro único para evitar limites fixos de três no F8. A visualização automática percorre o mesmo registro. Na campanha, os repertórios novos substituem os antigos, com progressão por HP, alternância e recarga do final. Os três padrões do Cultista são difíceis; o final entra abaixo de 35% de vida e tem ao menos duas outras rodadas entre usos.

## Ataques e leitura

| Inimigo | Ataque | O que observar e como desviar |
|---|---|---|
| Soldado | Estilhaço de Lança | Leia a preparação da ponta na lateral; saia da estocada e mude de eixo antes das farpas do recolhimento. |
| Soldado | Escudo Enraizado | Disparo nasce do escudo fincado; contorne a raiz e reposicione entre as duas rajadas. |
| Soldado | Tampas em Dobradiça | Leia os dois eixos; contorne o arco físico das tampas enquanto fecham em tempos diferentes. |
| Herbalista | Frasco Agitado | Frasco faz parábola, quica e quebra. O pouso é seguido de gotas, exigindo outro desvio. |
| Herbalista | Receita Reversa | Roxo causa dano; azul é antídoto inofensivo. As gotas roxas se neutralizam ao tocar reagentes azuis, abrindo janelas móveis. |
| Herbalista | Almofariz Transbordante | Expulsão espiralada vira sucção. Inverta o sentido do desvio quando as gotas voltam. |
| Limo | Gotas Gêmeas | Duas massas convergem, fundem e separam em quatro fragmentos diagonais. Evite o encontro e a dispersão. |
| Limo | Arrasto de Menta | Núcleos giram com fio inicialmente frouxo; atravesse antes da tensão e compense a atração. |
| Limo | Divisão Celular | A massa avança, alonga e divide. O pescoço some durante a separação e permite cruzar por dentro. |
| Lebre | Sacudida Espinhosa | Espinhos sobem, ficam presos no alto e caem. Desvie em ambos os sentidos. |
| Lebre | Tocas em Cadeia | Montículo anuncia a emergência; siga atrás da lebre que avança até a posição antiga da alma, seguida de farpas. |
| Lebre | Disparada da Roseira | Cruze sob o ápice do salto e reposicione para o leque apontado pelo pouso. |
| Cultista | Ceifa Germinante | Contorne o cabo e a lâmina físicos; sementes saem tangentes à ceifa. O centro ocupado pelo altar também causa dano. |
| Cultista | Raízes de Procura | Raízes de três origens crescem até a posição anterior da alma, depois bifurcam. Atrair e passar por trás da ponta. |
| Cultista | Ritual da Colheita | Sementes se expandem e depois são colhidas enquanto três raízes giram no sentido oposto. Troque a distância e o ângulo. |

Receita Reversa implementa neutralização entre fluxos móveis, em vez de uma poça estática da proposta. Tocas em Cadeia usa uma emergência seguida de corrida subterrânea contínua até a posição anterior; o aviso é local. Esses ajustes evitam áreas de espera e trajetórias antecipadas desenhadas. Não há linhas que revelem o percurso futuro. Filamentos e raízes visíveis são corpos físicos atuais.

## Dano antes/depois

Os valores “antes” são danos base dos slots substituídos, sem os multiplicadores removidos dos primeiros andares. O repertório antigo do Cultista tinha cinco ataques; os dois últimos compartilhados foram retirados. A comparação usa seus três primeiros slots, sem afirmar equivalência mecânica.

| Inimigo | Ataque | Antes | Depois | Teto por hit | Rodada |
|---|---|---:|---:|---:|---:|
| Soldado das Sarcas | Estilhaco de Lanca | 4 | 4 | 6 | 35% |
| Soldado das Sarcas | Escudo Enraizado | 4 | 5 | 7 | 35% |
| Soldado das Sarcas | Tampas em Dobradica | 4 | 5 | 7 | 35% |
| Herbalista da Peste | Frasco Agitado | 4 | 4 | 6 | 35% |
| Herbalista da Peste | Receita Reversa | 3 | 4 | 6 | 35% |
| Herbalista da Peste | Almofariz Transbordante | 3 | 5 | 7 | 35% |
| Limo de Menta | Gotas Gemeas | 3 | 3 | 5 | 35% |
| Limo de Menta | Arrasto de Menta | 3 | 4 | 6 | 35% |
| Limo de Menta | Divisao Celular | 3 | 5 | 6 | 35% |
| Lebre de Espinhos | Sacudida Espinhosa | 4 | 4 | 6 | 35% |
| Lebre de Espinhos | Tocas em Cadeia | 4 | 4 | 6 | 35% |
| Lebre de Espinhos | Disparada da Roseira | 4 | 5 | 7 | 35% |
| Cultista das Raízes | Ceifa Germinante | 6 | 6 | 7 | 35% |
| Cultista das Raízes | Raizes de Procura | 4 | 6 | 7 | 35% |
| Cultista das Raízes | Ritual da Colheita | 3 | 7 | 8 | 45% |

Invulnerabilidade global: 0,85 s. Sangramento explícito: zero nos 15 ataques novos. O teto é aplicado ao dano final, já considerando tipo e dificuldade. Com 40 HP, teto total de 14 nos comuns e 18 no final; com 80 HP, 28 e 36. O dano não aumenta simplesmente porque há mais objetos na tela.

## Checklist manual

- Soldado: desviar da ponta e das farpas, contornar os dois escudos possíveis e escapar do arco das dobradiças também começando nos cantos.
- Herbalista: reconhecer quique e quebra; conferir que azul não causa dano e apaga o roxo no encontro; sentir a mudança de expulsão para sucção.
- Limo: perceber a fusão antes da fragmentação; atravessar o fio frouxo e ser atingido apenas pelo tensionado; cruzar o pescoço depois da divisão.
- Lebre: desviar da subida e da queda dos espinhos; reconhecer o montículo e passar atrás da cabeça; usar o espaço sob o salto sem ficar na mira do pouso.
- Cultista: testar os três sem invencibilidade; confirmar que não há centro seguro no altar, que as raízes miram a posição antiga e que o ritual exige trocar de distância.
- Todos: B para comparar desenho e colisão, SHIFT atravessando projéteis para conferir dash vulnerável, C/F7/V para variar elemento/dificuldade/vida. Conferir os tetos 14/18 com 40 HP. F8 deve retornar ao estado anterior sem alterar a campanha.

## Arte temporária

Ver assets-planta.md. As silhuetas de armas e recipientes são desenhadas pelo jogo; folhas/projéteis e a Lebre usam imagens já existentes. Não foram inventados caminhos para arquivos inexistentes. O manifesto tem entradas independentes para trocar cada peça sem alterar ataques. Não são artes finais.

## Verificação da entrega

- Executável padrão compilado sem avisos. Todos os alvos do projeto compilaram.
- 360 rotas dos 15 ataques novos, sem dano: 30/60/120/144 FPS, cinco posições iniciais, duas arenas para o Cultista. CSV: rotas-planta.csv; log: rotas-planta.log.
- 240 rotas dos nove ataques aprovados anteriormente, sem dano, na mesma matriz. CSV: rotas-regressao-piloto.csv. As funções de comportamento e os nove registros de balanceamento originais foram comparados com o backup e permanecem idênticos.
- 120 cenários parados (24 ataques × cinco posições): todos recebem dano. Destes, 75 são do lote novo. Log: planta-idle-regressao.log.
- Tetos em 432 combinações de ataque/vida/tipo, proteção entre fontes de dano, sangramento, dash contínuo e permanência do Sineiro no andar 20 verificados.
- As 14 regressões executadas passaram: battle, types, qte, ui, save, sprites, phase6, phase10, tower, balance, bullets, patterns, hazards e equipment. A falha legada já conhecida do Acólito em test_motion fica fora deste lote; esse teste não foi repetido nesta entrega.
- Prévia renderizada com início na raiz do projeto e com início na pasta build. O carregador agora encontra imagens e manifesto a partir do executável também, sem mudar o diretório de trabalho ou o save. Testes de sprites aprovados.
- A busca de rotas usa controles reais contínuos, sem teleporte, dash ou invencibilidade. Sua busca tem antecipação e heurísticas, portanto confirma possibilidade de rota; a dificuldade percebida continua dependendo do teste humano.

Prévia visual: previa-lote-planta.png. Algumas peças continuam temporárias, conforme assets-planta.md.

## Correção da Receita Reversa

O teste reproduziu uma gota roxa reaparecendo em 2,467 s: a neutralização era calculada só pela proximidade atual do antídoto. Agora o encontro fica definitivo por gota, calculado pelo histórico determinístico das trajetórias; a gota neutralizada não volta a ser desenhada nem causar dano. Os reagentes alternam a origem a cada três levas para a neutralização permanente não criar uma metade sempre segura. Dano, velocidades, tamanhos e duração mantidos.

Teste específico verifica ausência de reaparecimento em 30/60/120/144 FPS nas duas larguras de arena; antídoto permanece inofensivo. As 20 rotas do ataque passaram sem dano, e as cinco posições paradas continuam recebendo dano. Relatório atualizado: rotas-receita-corrigida.csv. Esse arquivo substitui as linhas da Receita em rotas-planta.csv para esta revisão.