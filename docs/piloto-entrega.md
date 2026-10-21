# Entrega — Fases 0 e 1

Implementação limitada à infraestrutura compartilhada e aos nove ataques de Mariposa, Ladrão e Afogado. Os outros 78 ataques da proposta não foram implementados. Fase 2 aguarda aprovação após teste manual.

## Como testar

Abra o executável padrão e pressione **F8**. F8 ou X retorna ao estado anterior do jogo.

- **TAB:** Mariposa → Ladrão → Afogado.
- **1 / 2 / 3:** escolhe o padrão. No Afogado, os três são difíceis; o terceiro é o final.
- **WASD/setas:** mover; **SHIFT:** dash com trajeto vulnerável; **P/ESC:** pausar; **R:** reiniciar; **X:** sair.
- **C:** tipo defensivo; **F7:** Calmo/Normal/Difícil; **V:** 40/80 HP. Essas mudanças reiniciam o ataque.
- **I:** invencibilidade de teste; **B:** hitboxes; **F6:** recarregar imagens do manifesto.
- A rodada termina pausada para permitir ler os contadores; R repete.

## Infraestrutura

- Todos os modos de movimento passam pelo desenho de sprites; a arte é ajustada à geometria, sem substituir hitboxes por caixas da textura.
- Dash percorre 32 px durante 80 ms, com passos curtos e colisão varrida ao longo do percurso. Não atravessa perigos sem registrar contato. Não concede invulnerabilidade.
- Um único orçamento por rodada cobre projéteis, corpo, perigos e sangramento. I-frames globais de 0,85 s; o sangramento também respeita a proteção.
- Dano máximo total: floor(vida máxima na entrada × 35%) nos comuns; floor(vida máxima na entrada × 45%) nos finais. Vale em todas as dificuldades e tipos.
- O final do Afogado tem teto de 8 HP por impacto, abaixo dos 12 anteriormente propostos; com 40 HP, são no máximo 18 HP na rodada, sem um único impacto consumir 30% da vida inicial.
- Cada um dos nove ataques declara sangramento 0. Sangramento herdado de turnos anteriores usa o mesmo teto e proteção; o sistema de sangramento causado pelo jogador permanece intacto.
- Removidos os multiplicadores artificiais dos primeiros andares. Cegar continua reduzindo o dano e enraizar continua reduzindo o movimento dos ataques do piloto.
- Configuração de dano, duração, avisos, limites e perfis em include/balance.h; funções próprias dos nove ataques em src/pilot_patterns.c; avisos reutilizáveis em attack_warning.
- Troca de imagem pelo manifesto assets/pilot-assets.txt; nenhuma lógica de movimento depende do nome ou resolução do PNG. Todas as peças fixas têm pulsação e as móveis têm orientação/rotação.
- Sineiro Vazio ocupa o andar 20, chefe da região de sombra. Goblin da Cripta continua cadastrado para trabalho futuro, fora dessa vaga.

## Antes/depois dos três repertórios

“Antes” é dano BASE da entrada antiga, sem andar, agressividade, elemento, dificuldade ou sangramento. A associação indica substituição da posição no repertório, não equivalência mecânica. Os tetos novos são sobre o dano final, depois dos modificadores.

| Inimigo | Ataque novo | Entrada anterior | Base antes | Base depois | Teto/impacto | Teto/rodada | Duração | Motivo |
|---|---|---|---:|---:|---:|---:|---:|---|
| Mariposa | Poeira de Asa | Trama | 3 | 3 | 5 | 35% | 5 s | Projéteis ondulados; remover inflação inicial |
| Mariposa | Pó em Contratempo | Corrente | 3 | 4 | 6 | 35% | 6 s | Regra de estado com tolerância e movimento real |
| Mariposa | Revoada Cega | Asas | 3 | 5 | 6 | 35% | 7 s | Duas asas em curvas defasadas e caudas atrasadas |
| Ladrão | Adaga de Retorno | Reflexo antigo | 5 | 4 | 6 | 35% | 5 s | Ensinar ida, rebote e retorno com menor punição |
| Ladrão | Reflexo Roubado | Gato: Perseguição | 4 | 5 | 7 | 35% | 6 s | Origem invertida pelo espelho; retirar ataque emprestado |
| Ladrão | Roubo de Passagem | Gato: Emboscada | 4 | 6 | 7 | 35% | 7 s | Ler lado aberto e recolhimento dos fios; sem sangramento automático |
| Afogado | Âncora de Coral | Maré de Coral | 6 | 5 | 7 | 35% | 6 s | Peso e cabo físicos substituem faixa estática |
| Afogado | Ressaca Calcificada | Sapo: Vitórias-régias | 4 | 6 | 8 | 35% | 8 s | Correnteza compensável e ramos móveis; retirar empréstimo |
| Afogado | Recife em Ruptura | Limo: Balsas | 3 | 7 | 8 | 45% | 10 s | Final com aviso 1,2 s; teto por impacto reduzido de 12 propostos para 8 |
| Afogado | Removido do repertório | Pássaro: Fogo Cruzado | 4 | — | — | — | — | Ataque emprestado; permanece disponível aos seus outros usuários |
| Afogado | Removido do repertório | Sineiro: Eco | 5 | — | — | — | — | Ataque emprestado; permanece com o Sineiro |

Com 40 HP máximos: comuns 14 HP/rodada; final 18 HP/rodada. Com 80 HP: 28/36 HP. Em 41 HP, comuns continuam em 14 HP: o arredondamento é sempre para baixo. O teto não promete dano automático: esquivar corretamente continua permitindo zero dano.

## Validação executada

- 240 rotas encontradas e reproduzidas sem perda de HP nos controles reais, sem dash ou invulnerabilidade de teste: nove ataques × centro/quatro cantos × 30/60/120/144 FPS, incluindo arena menor dos três ataques do Afogado na segunda fase.
- Busca finita com margem espacial de 0,4 px e margem temporal nas trocas de cor. Usa previsão geométrica; não é prova matemática de todas as posições possíveis nem uma medição de reação humana. O teste manual continua necessário.
- 45 estratégias de ficar parado (centro e quatro cantos, em nove ataques) recebem dano. Empurrar contra a parede sem deslocamento não satisfaz a regra laranja.
- 162 combinações de ataque, HP máximo (40/41/80) e tipo defensivo, sob modificador 1,25, submetidas a 100 tentativas fortes cada; tetos por impacto e rodada preservados.
- Teste de dash com parede de 2 px e projétil simultâneo: o trajeto registra dano, e as fontes compartilham os i-frames.
- Progressão por HP, primeiro ataque fácil mesmo ao provocar segunda fase cedo, ausência de repetição consecutiva e intervalo de pelo menos duas outras rodadas entre finais do chefe.
- Regressão de padrões legados: 1.728 cenários de esquiva e 234 posições paradas aprovados. Os avisos de segunda fase dos chefes legados receberam mínimo de 0,90 s.
- Compilação com -Wall/-Wextra sem avisos. Testes de batalha, projéteis, dano/perigos, equipamentos, torre, tipos, QTE, sprites, salvamento e tipografia aprovados.
- Limitação já existente: test_motion ainda falha no padrão legado 60 (Acólito do Vitral), fase 0 a 60 FPS. A mesma falha existia antes; o Acólito não foi redesenhado nesta fase.

Arquivos de evidência: docs/pilot-routes.csv, docs/pilot-idle.log e docs/testes-piloto.log.

## Checklist manual específico

### Mariposa de Poeira
- [ ] Poeira de Asa: ler a inversão do bater de asas e atravessar entre grãos; experimentar centro e cantos.
- [ ] Pó em Contratempo: azul/parado e laranja/movendo devem ser seguros; confirmar dano ao fazer o inverso.
- [ ] Segurar contra uma parede sem deslocar não deve contar como movimento laranja.
- [ ] Conferir o pulso e a tolerância da troca de cor antes de surgir dano.
- [ ] Revoada Cega: passar depois da asa e antes da cauda, sem depender de um canto permanentemente seguro.

### Ladrão da Mortalha
- [ ] Adaga de Retorno: identificar mão, ponto de rebote e percurso de volta; testar cruzar a linha após a ida.
- [ ] Reflexo Roubado: distinguir eixo horizontal/vertical e origem invertida; mira não pode perseguir durante o disparo.
- [ ] Roubo de Passagem: identificar o lado sem fio, atravessar antes da contração e testar os quatro cantos.
- [ ] Fazer dash através de uma lâmina/fio: deve registrar contato durante o trajeto; múltiplas fontes não podem causar dano instantâneo em cadeia.

### Afogado de Coral
- [ ] Âncora de Coral: contornar o peso e conferir que o cabo acompanha a curva com aviso prévio.
- [ ] Ressaca Calcificada: compensar a correnteza e ler pontas dos ramos; força deve permitir controle em ambas as direções.
- [ ] Recife em Ruptura: ler número/posição de quebra, escapar da arma e atravessar a seção removida entre os estilhaços.
- [ ] Verificar 8 HP máximos em um impacto do final, inclusive com fraqueza elemental e modo Difícil.
- [ ] Com 40 HP, dano total deve parar em 14 nos ataques 1/2 e em 18 no final; repetir com V para 80 HP (28/36).
- [ ] Na campanha, conferir a arena menor após 50% de HP e disponibilidade do final abaixo de 35%, com intervalo entre usos.

### Integração
- [ ] Pausar/retomar em aviso, disparo e retorno; nada deve avançar durante a pausa.
- [ ] Conferir 30/60/120/144 FPS e escalas de janela/tela cheia.
- [ ] Conferir Sineiro no andar 20.
- [ ] Trocar uma imagem no manifesto e usar F6; lógica e hitbox devem permanecer iguais.
- [ ] Conferir que encerrar o treino não modifica save.dat.

Aguardar aprovação do piloto antes da Fase 2.
