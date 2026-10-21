# Peças isoladas do lote Planta

O manifesto `assets/pilot-assets.txt` já contém as chaves abaixo. Hoje, exceto a Lebre, cada chave aponta para o arquivo existente `assets/sprites/bullet_planta.png`: ele identifica o fallback, e o jogo monta uma silhueta temporária adequada à peça. Folhas e sementes usam também essa imagem. A Lebre usa `assets/sprites/enemy_lebre_espinho.png`.

Para usar uma arte isolada final: colocar seu PNG na pasta desejada, trocar apenas o caminho dessa chave e pressionar F6 no F8. O carregador detecta o novo caminho e passa a usar o sprite no lugar do fallback. Folhas com quadros `_sheet.png` e metadados `.txt` continuam suportadas. Fundo transparente; pivô central em todas as peças. Ponta do segmento para +X quando aplicável. Não incluir arena, sombra de trajetória nem personagem inteiro no arquivo de uma arma.

As peças abaixo ainda faltam como arte isolada final:

| Chave | Peça | Canvas sugerido | Orientação |
|---|---|---|---|
| planta_gota | Gota/massa de limo arredondada | 32×32 | Neutra, centro da massa no pivô |
| planta_veneno | Gota reagente roxa | 16×16 | Neutra |
| planta_antidoto | Gota azul de antídoto, bem distinta | 16×16 | Neutra |
| planta_frasco | Frasco de vidro com rolha e líquido | 24×32 | Rolha para cima (−Y) |
| planta_farpa | Espinho isolado | 24×8 | Ponta +X |
| planta_lanca | Trecho de lança de madeira com ponta | 48×10 | Ponta +X |
| planta_escudo | Escudo funerário isolado | 20×40 | Eixo longo vertical, topo −Y |
| planta_tabua | Tampa/tábua com dobradiça | 64×12 | Eixo longitudinal +X; pivô central |
| planta_raiz | Trecho de raiz sem cenário | 48×10 | Crescimento +X |
| planta_foice | Seção alongada de cabo/lâmina da foice | 48×10 | Ponta +X; usada nos segmentos físicos da ceifa |
| planta_semente | Semente com pequena folha | 16×12 | Ponta +X |
| planta_lebre | Corpo isolado da Lebre em salto | 48×40 | Frente +X; por enquanto usa o personagem existente |
| planta_toca | Montículo de terra com abertura | 32×20 | Chão embaixo (+Y), abertura para cima |
| planta_almofariz | Almofariz sem personagem | 32×28 | Boca −Y |
| planta_altar | Pequeno altar de raízes | 32×32 | Frente neutra, boca ritual −Y |
| planta_filamento | Trecho de fio de menta | 32×6 | Longitudinal +X |

As peças alongadas devem ocupar o eixo central sem margens grandes. A colisão continua definida pela lógica; manter largura visual coerente com os segmentos e verificar com B. O alcance e a dificuldade aprovados não dependem de trocar a imagem.
