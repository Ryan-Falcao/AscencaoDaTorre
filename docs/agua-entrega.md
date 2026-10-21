# Lote Água — entrega

Quatro inimigos comuns, três ataques próprios cada: Legionário Afogado, Bruxa do Lodo, Revenante de Gelo e Sapo de Vidro. Os padrões aprovados de Planta, Mariposa, Ladrão e Afogado de Coral foram preservados. Coral permanece como chefe de Água; Sineiro permanece como chefe de Sombra no andar 20.

## Como testar

Abrir o executável padrão `build/ascensao_torre.exe` e pressionar F8. O teste começa no Legionário Afogado. TAB avança para Bruxa, Revenante e Sapo; SHIFT+TAB volta. Os inimigos anteriores continuam disponíveis no mesmo teste. 1/2/3 escolhe o ataque, R reinicia, WASD/setas move, SHIFT dá dash, P/ESC pausa, F8/X retorna à partida anterior. D somente move para a direita. F7 muda dificuldade, C muda defesa elemental, V alterna 40/80 HP, I alterna invencibilidade, B mostra colisões e F6 recarrega imagens. Avaliar desafio com invencibilidade desligada.

Os três ataques de cada inimigo entram também na campanha, usando alternância e progressão por HP. Este lote não adiciona chefe novo nem modifica o repertório aprovado do Coral.

## Regras

- Congelamento em acerto de gelo: 0,28 s. SHIFT com uma direção rompe o efeito. A proteção global de 0,85 s começa junto do acerto e dura mais que o congelamento, evitando dano em cadeia durante a imobilização.
- Membrana dourada permite atravessar apenas durante o dash ativo. Dash não protege de gelo, jatos ou outros ataques.
- Lama reduz a velocidade para 56%; as faixas de viscosidade não causam dano por si. Pegadas ativadas causam dano.
- Não há linhas de trajetória futura. Avisos são espuma, bolha, emissor ou concentração de neve na origem.
- Dano e ritmo ficam em `include/balance.h`. Sangramento explícito igual a zero nos 12 ataques. Teto final por impacto aplicado depois de tipo/dificuldade. Teto comum por rodada: 35% da vida máxima, ou 14 com 40 HP e 28 com 80 HP. Os quatro inimigos são comuns, inclusive seus ataques de nível 3; não recebem o teto de 45% reservado a finais de chefe.

## Balanceamento antes / depois

Comparação por posição no repertório substituído. Valores antes são danos base sem multiplicadores antigos de andar; não representam equivalência entre mecânicas.

| Ataque | Inimigo | Antes | Depois | Teto por hit | Duração | Motivo |
|---|---|---:|---:|---:|---:|---|
| Tsunami da Falange | Legionário | 4 | 5 | 6 | 6 s | Grande área com abrigo direcional legível |
| Jato de Quadrante | Legionário | 4 | 5 | 6 | 6 s | Flanqueamento contínuo do cone |
| Convés à Deriva | Legionário | 4 | 6 | 7 | 7 s | Transporte e desvio interno simultâneos |
| Pegadas do Brejo | Bruxa | 4 | 4 | 6 | 6 s | Introdução à memória do próprio percurso |
| Bancos de Viscosidade | Bruxa | 3 | 5 | 6 | 6,5 s | Planejamento de rota com redução de velocidade |
| Bolha Inversa | Bruxa | 3 | 5 | 6 | 7 s | Duas regras opostas, aviso de transição explícito |
| Avalanche Oblíqua | Revenante | 5 | 5 | 6 | 6,5 s | Duas massas grandes com atraso entre quedas |
| Estacas de Geada | Revenante | 4 | 6 | 7 | 7 s | Aceleração, chão temporariamente ocupado e gelo |
| Prisão Deslizante | Revenante | 5 | 6 | 7 | 9 s | Frenagem antecipada e obstáculos alternados |
| Jato Refratado | Sapo | 4 | 4 | 6 | 6 s | Introdução à mudança do jato na superfície |
| Reflexo do Lago | Sapo | 4 | 5 | 6 | 6,5 s | Proteger duas posições com o mesmo controle |
| Membrana de Impulso | Sapo | 4 | 6 | 7 | 7 s | Travessia exige sincronizar o dash |

## Checklist manual específico

- Legionário 1: começar nos cantos; reconhecer o lado protegido do quebra-mar e trocar de face quando a onda inverte. A face exposta deve causar dano.
- Legionário 2: contornar o bocal sem tocá-lo; perceber a rotação do cone. Conferir B no bocal e no jato.
- Legionário 3: entrar no convés antes da inundação, sentir o transporte automático e sair da espuma do gêiser sem abandonar a plataforma.
- Bruxa 1: desenhar uma volta com a alma e conferir que as marcas seguem o caminho real. Voltar cedo deve ferir; esperar desaparecer deve permitir cruzar.
- Bruxa 2: comparar movimento dentro e fora da lama; escapar pela extremidade da enxurrada sem tentar correr contra ela dentro da faixa lenta.
- Bruxa 3: entrar no abrigo azul e sair quando muda para violeta. Conferir que o aviso de colapso antecede o dano interno.
- Revenante 1: desviar das duas massas de neve, inclusive começando em um canto. Ao ser atingido, conferir congelamento curto e invulnerabilidade.
- Revenante 2: ler os cinco avisos no teto, desviar da sequência e evitar a estaca presa no piso. SHIFT deve romper o congelamento, sem tornar estacas atravessáveis.
- Revenante 3: soltar a direção e observar a frenagem; antecipar a curva entre placas. Conferir que gelo e velocidade residual zeram ao reiniciar ou trocar de ataque.
- Sapo 1: cruzar a superfície e observar o jato mudar de direção e largura. As regiões vazias fora do jato devem permanecer seguras.
- Sapo 2: mover para cima/baixo e reconhecer o movimento inverso do reflexo; testar uma bolha atingindo só o reflexo. Deve haver um único orçamento de dano e proteção para as duas almas.
- Sapo 3: comparar atravessar andando e com SHIFT. Somente contato durante o dash ativo é seguro. Testar a travessia perto do final do dash e com a recarga ainda ativa.
- Todos: I desligado; testar C/F7/V; conferir limite de 14 com 40 HP, B para colisão e F8 para retornar à campanha. F6 deve manter o jogo funcional ao sair do treino.

Assinaturas completas: `agua-assinaturas.md`. Peças de arte ainda temporárias e instruções de troca: `assets-agua.md`. A validação automática confirma possibilidade de desvio; legibilidade, ritmo e dificuldade percebida precisam desta avaliação manual.

## Verificação final

- Compilação de todos os alvos concluída sem avisos; executável padrão atualizado em build/ascensao_torre.exe. Log: agua-build.log.
- 240 replays reais de Água sem dano: 12 ataques × cinco posições × 30/60/120/144 FPS. Relatório consolidado em lotes: rotas-agua.csv. Membrana foi validada com dash real; Prisão Deslizante com duração final de 9 s. O validador usa a mesma movimentação, inércia e colisão do jogo, com margem extra de colisão.
- 600 replays dos 24 ataques anteriores sem dano, incluindo duas arenas dos chefes. Relatórios: rotas-regressao-agua-anteriores.csv e .log. Os nove comportamentos do piloto original e os 24 registros de balanceamento anteriores também foram comparados com o backup e estão idênticos.
- 60 cenários parados de Água: todos recebem dano, inclusive nos quatro cantos. Log: agua-parado.log.
- Tetos em 648 combinações de ataque/vida/tipo, dano residual, i-frames globais, dash contínuo e campanha verificados. Log: agua-infra.log.
- test_water passou: varredura de triângulos, abrigo circular, sombra direcional do tsunami, hitbox refletida, dash seletivo, congelamento após dano real, proteção durante o gelo e reinício sem estado residual.
- 14 regressões passaram: battle, types, qte, ui, save, sprites, phase6, phase10, tower, balance, bullets, patterns, hazards e equipment. Log: agua-regressoes.log. A falha legada conhecida do Acólito em test_motion fica fora deste lote; esse teste foi compilado, mas não foi executado novamente.
- Prévia dos 36 ataques renderizada; água/gelo, reflexo e membrana inspecionados. Os avisos locais não mostram o percurso futuro. A arte das peças de Água continua temporária, conforme assets-agua.md.

Os testes demonstram rotas possíveis com controles válidos; não medem a dificuldade percebida por uma pessoa. Este lote para aqui para o teste manual dos quatro inimigos de Água.