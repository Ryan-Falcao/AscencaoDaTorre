# Revisão do ritmo e da leitura visual — 21/10/2026

Somente Ladrão da Mortalha e Afogado de Coral foram ajustados nesta revisão. Dano, tetos por rodada e invulnerabilidade de 0,85 s foram mantidos.

| Ataque | Ciclo antes | Ciclo agora | Movimento do golpe antes/agora |
|---|---:|---:|---|
| Adaga de Retorno | 2,40 s | 1,60 s | 1,35 / 0,78 s |
| Reflexo Roubado | 2,40 s | 1,45 s | 1,15 / 0,62 s |
| Roubo de Passagem | 2,40 s | 1,65 s | recolhimento 1,10 / 0,85 s |
| Âncora de Coral | 2,80 s | 1,95 s | 1,35 / 0,95 s |
| Ressaca Calcificada | contínuo | contínuo | deslocamento 64 / 92 px/s |
| Recife em Ruptura | 3,20 s | 2,35 s | fragmentos 1,15 / 0,95 s |

Os ataques cíclicos deixaram de somar o aviso inicial duas vezes. Mantidos os avisos de 0,70 s do Ladrão, 0,90 s da âncora e 1,20 s do final. A ressaca conserva seu aviso inicial e usa corredores mais largos para compensar a velocidade maior.

Peças temporárias separadas foram criadas em assets/sprites/pilot: ancora.png (26 px na tela), cabo.png, coral.png e estilhaco.png. Substituem imagens inteiras de efeitos reaproveitadas em miniatura. O manifesto continua permitindo trocar cada arquivo sem mudar a lógica. São peças geométricas temporárias, não a arte final. Cabos acompanham seu segmento; corais acompanham seu crescimento, sem repetir a imagem do ataque inteiro. Escala dos sprites de ameaça corrigida para descontar a escala-base do carregador.

Teste manual: abra o executável padrão, F8, TAB para o Ladrão/Afogado, 1/2/3, R para repetir. Conferir se há tempo de reconhecer o aviso, se a volta da adaga exige reposicionar, se os fios dão tempo de escapar pelo lado aberto e se âncora/cabo/coral são distinguíveis. O treino pausa no fim da rodada por projeto; R inicia outra tentativa.
Validação: compilação sem avisos; 180 rotas dos seis ataques alterados reproduzidas sem dano em 30/60/120/144 FPS, cinco posições iniciais e ambas as arenas do chefe. Os 45 testes de jogador parado continuam recebendo dano. Testes de tetos, sangramento, i-frames, dash e campanha passaram. Relatório: pilot-routes-revisao-ritmo.csv.
