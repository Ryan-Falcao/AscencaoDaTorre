# Assets isolados do piloto
As peças abaixo ainda não foram fornecidas. O jogo está usando somente imagens existentes como placeholders.
Edite assets/pilot-assets.txt para apontar cada chave ao arquivo fornecido. F6 recarrega as imagens no treino. A troca não altera movimento, dano ou hitbox.

PNG com fundo transparente, sem sombra recortada no contorno sólido. Centro do canvas é o pivô de rotação; +X aponta à direita, +Y para baixo. Mantenha a parte sólida centralizada. Os tamanhos são sugestões para a arte-fonte, não tamanhos de colisão em jogo.

| Chave do manifesto | Peça isolada faltante | Dimensão sugerida | Orientação na imagem | Placeholder atual existente |
|---|---|---|---|---|
| adaga | Adaga do Ladrão, sem mão ou cenário | 48×16 px | Ponta à direita (+X), cabo à esquerda | assets/sprites/defense_ladrao_reflexo.png |
| espelho | Espelho da Mortalha, sem reflexo do personagem embutido | 32×48 px | Vertical, topo para cima (-Y) | assets/sprites/shadow_orb.png |
| fio | Fio de recolhimento da adaga, repetível | 16×4 px | Horizontal (+X), sem pontas desenhadas | assets/sprites/bullet_sombra.png |
| ancora | Âncora/arma pesada do Afogado, isolada do chefe e da água | 48×48 px | Cabeça de impacto à direita (+X) | assets/sprites/defense_afogado_mare_coral.png |
| cabo | Cabo da arma, repetível e separado do peso | 16×4 px | Horizontal (+X) | assets/sprites/defense_carcereiro_grade.png |
| coral | Ramo de coral sólido, sem personagem ou água | 32×64 px | Raiz embaixo (+Y), ponta para cima (-Y) | assets/sprites/defense_afogado_mare_coral.png |
| asa | Asa da Mariposa, separada do corpo e dos grãos | 32×48 px | Base à esquerda, extremidade à direita (+X) | assets/sprites/defense_mariposa_asas.png |
| poeira | Um único grão de poeira, sem asa ou frasco | 12×12 px | Sem direção preferencial | assets/sprites/defense_mariposa_corrente.png |
| estilhaco | Um fragmento de coral, sem espuma | 16×16 px | Ponta principal à direita (+X) | assets/sprites/bullet_agua.png |
| mao | Mão/luva de lançamento do Ladrão | 32×32 px | Dedos/lançamento à direita (+X) | assets/sprites/enemy_ladrao_mortalha.png |

As setas, eixos do espelho, círculos de previsão, sombra de impacto e números de quebra são avisos procedurais; não exigem imagens adicionais.
As folhas de animação opcionais seguem o carregador existente: arquivo _sheet.png e metadados .txt no mesmo nome-base. Não é obrigatório produzir folhas para testar o piloto.
As hitboxes são menores que a arte e podem ser vistas com B. A pele é dimensionada pelo tamanho de jogo, independentemente da resolução do PNG.
