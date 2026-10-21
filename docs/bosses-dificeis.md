# Regra de bosses — revisão de 21/10/2026

Todos os ataques de bosses devem ser difíceis. O primeiro ataque de um boss apresenta a identidade, mas não é uma versão fácil. A dificuldade deve exigir leitura e desvios sucessivos, sem depender de aumentar dano ou esconder hitboxes. Final ainda mais intenso e bem sinalizado. Nenhuma linha deve mostrar o trajeto futuro. Os limites de dano por rodada, por impacto e i-frames permanecem.

Esta regra vale para o design de todos os bosses. Nesta entrega, somente os três ataques do Afogado foram refeitos, respeitando a aprovação por fases. Os outros bosses ainda aguardam Fase 2.

## Afogado

- Âncora de Coral: duas armas em curvas opostas, separadas por 0,38 s, cabos perigosos e leque radial de sete estilhaços depois do impacto. Armas viajam em 0,75 s; fragmentos a 175 px/s. Ciclo de 2,15 s; duração de 7 s.
- Ressaca Calcificada: corais a 122 px/s, ramos alternados a cada 84 px, correnteza de até 42 px/s, corredor oscilante e fragmentos cadenciados caindo. Duração de 8 s.
- Recife em Ruptura: recife físico, golpe de âncora em 0,25 s e duas erupções radiais de nove estilhaços, separadas por 0,34 s, com correnteza de 44 px/s. Ciclo de 2,45 s; duração de 10 s.

Corais desenhados como ramos contínuos com peças nas pontas, cabos como linhas segmentadas, sem mosaico de texturas esticadas. Preparação visível nas armas e no recife, sem previsão desenhada do movimento. Peças ainda temporárias.

Dano permanece 5 / 6 / 7; teto por impacto 7 / 8 / 8; teto de rodada 35% / 35% / 45%; invulnerabilidade de 0,85 s.

Teste manual: F8, TAB até Afogado, 1/2/3, invencibilidade desligada. Verificar se o segundo golpe obriga a reposicionar, se correnteza + chuva exige movimento em dois eixos e se o final exige duas respostas. A busca automática verifica possibilidade de rota, não a dificuldade percebida ou a qualidade estética.
Validação: compilação sem avisos; 120 replays sem dano em 30/60/120/144 FPS, cinco posições iniciais e duas arenas. Testes de dano, invulnerabilidade, dash, campanha e alternância de padrões aprovados. Relatório: pilot-routes-boss-dificil.csv.
