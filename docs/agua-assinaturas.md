# Água — assinaturas de desvio

Este lote substitui os repertórios de quatro inimigos comuns. O Afogado de Coral continua com os três ataques de chefe aprovados. Não foram adicionadas trajetórias futuras desenhadas: os avisos ficam na origem ou no próprio objeto.

| Inimigo / ataque | Observar | Prever | Como mover | Quando atravessar | Dificuldade |
|---|---|---|---|---|---|
| Legionário — Tsunami da Falange | Quebra-mar e sentido da onda | Qual face do escudo ficará protegida | Entrar na sombra direcional do escudo | Trocar de face entre ondas | Fácil |
| Legionário — Jato de Quadrante | Bocal central e abertura do jato | Rotação do cone em torno do bocal | Contornar a origem e flanquear o cone | Passar por trás do bocal; seu corpo causa dano durante o disparo | Média |
| Legionário — Convés à Deriva | Convés móvel cercado de água e espuma do gêiser | Deslocamento do convés e jato na posição anterior | Acompanhar a plataforma e desviar dentro dela | Reposicionar após a espuma, antes da erupção | Difícil |
| Bruxa — Pegadas do Brejo | Marcas deixadas pelo próprio movimento | Ativação atrasada das próprias pegadas | Percorrer espaço novo e evitar voltar cedo | Retornar apenas depois de a marca desaparecer | Fácil |
| Bruxa — Bancos de Viscosidade | Faixas de lama e origem da enxurrada oblíqua | Frenagem na lama e alcance lateral da enxurrada | Escolher piso livre e escapar pela extremidade do fluxo | Cruzar a lama antes de o fluxo chegar | Média |
| Bruxa — Bolha Inversa | Bolha azul e mudança para violeta | Abrigo que se transforma em perigo | Entrar na bolha, depois sair do seu centro | Na transição de cor, antes do colapso | Difícil |
| Revenante — Avalanche Oblíqua | Acúmulos de neve no teto | Duas massas largas caindo com atraso e deriva lateral | Fugir lateralmente do volume inteiro | Passar atrás da cauda da primeira massa, sem entrar na segunda | Fácil |
| Revenante — Estacas de Geada | Cinco pontos gelados no teto | Queda acelerada em sequência e permanência no chão | Deslocar lateralmente; evitar voltar sobre as estacas | Depois de a estaca desaparecer | Média |
| Revenante — Prisão Deslizante | Placas de gelo vindas de lados alternados | Inércia da alma, oscilação das placas e espaço de frenagem | Fazer curvas antecipadas e frear antes da troca de lado | Entre as placas; dash continua vulnerável ao gelo | Difícil |
| Sapo de Vidro — Jato Refratado | Emissor e superfície da água | Mudança de direção e largura ao atravessar a superfície | Corrigir a posição conforme a metade da arena | Cruzar a linha d'água longe da junção do jato | Fácil |
| Sapo — Reflexo do Lago | Alma verdadeira e reflexo azul | Duas áreas ameaçam posições diferentes do mesmo controle | Procurar posição segura para ambas simultaneamente | Só quando alma e reflexo ficam fora das bolhas | Média |
| Sapo — Membrana de Impulso | Cortina com borda dourada e aviso de dash | Momento de contato e recarga do impulso | Aproximar e atravessar com dash ativo | Durante o dash; andar através causa dano | Difícil |

## Auditoria de repetição

- O tsunami usa abrigo direcional físico; não é uma parede com abertura móvel.
- O convés transporta a alma e exige desvio interno. Não reutiliza a correnteza ou a brecha da Ressaca Calcificada do Coral.
- A lama usa o histórico real do jogador; não repete projéteis de veneno ou áreas fixas da Herbalista.
- Avalanche é uma massa preenchida com deriva; estacas são quedas aceleradas sequenciais que permanecem no piso. Nenhuma sobe e volta como os espinhos da Lebre.
- O gelo exige controle de aceleração e frenagem, não ricochete de projéteis.
- A refração é um jato contínuo mudando de direção na superfície, sem leque de projéteis de prisma.
- Reflexo do Lago duplica a hitbox controlada; não repete a adaga que mira uma posição espelhada antiga do Ladrão.
- Membrana tem regra exclusiva de permeabilidade durante o dash. Não concede invulnerabilidade contra outros ataques.

Os ataques usam funções separadas, peças de imagem independentes e parâmetros de ritmo/dano em `include/balance.h`. A progressão usa os três níveis e alternância por HP já existentes na campanha.
