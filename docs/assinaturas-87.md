# Assinaturas de desvio — 87 ataques

Documento entregue antes das alterações de código. Só os nove ataques de Mariposa, Ladrão e Afogado serão implementados nesta etapa.

## Duplicatas encontradas e redesenho

- Ricochete balístico pertence ao Ladrão. Revenante usa gelo em queda e inércia; Legionário usa abrigo direcional, cone e convés móvel; Mercenário usa resvalo da espada presa à mão; Acólito decompõe luz, sem ricochete.
- Acólito decompõe luz em projéteis. Sapo refrata um jato contínuo na superfície da água, controla duas almas e atravessa membranas com dash; Inquisidor usa memória de selos ordenados.
- Lança com pivô pertence ao Goblin. Soldado finca escudos como coberturas e lança linearmente; Legionário usa abrigo direcional do quebra-mar, flanqueamento do jato e transporte no convés.
- “Passe pela brecha” não basta: cada linha especifica objeto, previsão, movimento e instante. Os 78 ataques futuros são especificações, sujeitos a revisão prática antes de cada lote.

| # | Inimigo | Ataque | O que observar | O que prever | Como mover | Quando atravessar |
|---|---|---|---|---|---|---|
| 1 | Mariposa de Poeira | Poeira de Asa | Bater de asas e grãos | Inversão das senoides | Serpentear entre grãos | No retorno da asa |
| 2 | Mariposa de Poeira | Pó em Contratempo | Cor e pulso de aviso | Mudança azul/laranja | Parar ou manter deslocamento | Após tolerância de troca |
| 3 | Mariposa de Poeira | Revoada Cega | Duas asas e caudas | Cruzamento e atraso do rastro | Entrar por dentro da curva | Depois da asa e antes da cauda |
| 4 | Sapo de Vidro | Jato Refratado | Emissor e superfície da água | Mudança de direção e largura ao atravessar a superfície | Corrigir a posição conforme a metade da arena | Cruzar a linha d'água longe da junção do jato |
| 5 | Sapo de Vidro | Reflexo do Lago | Alma verdadeira e reflexo azul | Duas áreas ameaçam posições diferentes do mesmo controle | Procurar posição segura para ambas simultaneamente | Só quando alma e reflexo ficam fora das bolhas |
| 6 | Sapo de Vidro | Membrana de Impulso | Cortina com borda dourada e aviso de dash | Momento de contato e recarga do impulso | Aproximar e atravessar com dash ativo | Durante o dash; andar através causa dano |
| 7 | Pássaro de Cinza | Penas Incandescentes | Peso e giro das penas | Ápice de cada parábola | Cruzar sob penas ascendentes | Antes da descida |
| 8 | Pássaro de Cinza | Mergulho de Cinza | Finta e rota travada | Percurso e esteira quente | Cortar por trás da ave | Após passagem do corpo |
| 9 | Pássaro de Cinza | Ascendente de Brasas | Térmica e leques | Velocidades verticais relativas | Compensar subida lateralmente | Entre expulsões de penas |
| 10 | Limo de Menta | Gotas Gêmeas | Gotas e volume | Fusão e direção dos fragmentos | Separar-se dos pares convergentes | Antes da fusão ou após fragmentar |
| 11 | Limo de Menta | Arrasto de Menta | Fios frouxos e núcleos | Arrasto que fecha a distância | Cruzar perpendicular ao fio frouxo | Quando os núcleos se afastam |
| 12 | Limo de Menta | Divisão Celular | Pescoço da massa | Divisão conservando volume | Entrar no pescoço e sair lateralmente | Durante compressão, antes de separar |
| 13 | Fagulha de Lanterna | Facho Oscilante | Balanço da lanterna | Giro do cone e base cega | Acompanhar o ângulo pela base | Ao inverter o balanço |
| 14 | Fagulha de Lanterna | Satélites de Luz | Dois anéis e pulso | Troca de órbita dos satélites | Mudar a distância do centro | Durante a troca radial |
| 15 | Fagulha de Lanterna | Procissão de Lanternas | Sequência de lanternas | Propagação do acendimento | Seguir a curva de sombra | Antes de acender a lanterna seguinte |
| 16 | Lebre de Espinhos | Sacudida Espinhosa | Leque e espinhos presos | Queda após uma fixação | Desviar da ida e da queda | Quando a parede libera o espinho |
| 17 | Lebre de Espinhos | Tocas em Cadeia | Montinhos de terra | Ordem da rota subterrânea | Seguir atrás da cabeça da trilha | Depois da emergência |
| 18 | Lebre de Espinhos | Disparada da Roseira | Ápice e corpo da lebre | Pouso, rebote e dispersão | Alternar passar sob e contornar | Sob o ápice ou após pouso |
| 19 | Gato de Sombra | Bote de Veludo | Agachamento e sombra | Curva do bote fixado | Ir ao interior da curva | Após arrancada |
| 20 | Gato de Sombra | Rastro de Caça | Rastro próprio atrasado | Cortes sobre posições antigas | Desenhar caminho sem auto-interseção | Após a cópia passar |
| 21 | Gato de Sombra | Nove Vultos | Vulto com sombra e olhos | Qual corpo dá o bote | Trocar lado em relação ao atacante real | No salto do vulto verdadeiro |
| 22 | Cavaleiro de Pétalas | Saudação da Lâmina | Punho e arco da espada | Limite finito de alcance | Contornar ponta ou aproximar do punho | Durante recuperação |
| 23 | Cavaleiro de Pétalas | Guarda de Pétalas | Guarda e pétalas | Curvas de retorno à empunhadura | Passar pelo punho sem cortar a cauda | Antes do retorno |
| 24 | Cavaleiro de Pétalas | Dança das Três Guardas | Três posturas | Ordem de arcos e estocada | Memorizar e alternar giro/distância | Nas recuperações da sequência |
| 25 | Soldado das Sarcas | Estilhaço de Lança | Comprimento da lança | Recolhimento com lascas | Passar atrás da ponta | Antes de espalhar farpas |
| 26 | Soldado das Sarcas | Escudo Enraizado | Escudo fincado e raízes | Nova posição fixa de tiro | Atrair disparo e mudar de cobertura | Enquanto o soldado troca de escudo |
| 27 | Soldado das Sarcas | Tampas em Dobradiça | Dobradiças e tábuas | Arcos sólidos das portas | Contornar eixo de portas sucessivas | Quando uma abre e a outra recupera |
| 28 | Herbalista da Peste | Frasco Agitado | Sombra e reagente do frasco | Quica e fragmenta | Sair do pouso sem entrar nas gotas | Após quique e antes de quebra |
| 29 | Herbalista da Peste | Receita Reversa | Gotas reagentes | Neutralização temporária do veneno | Acompanhar a frente da reação | Enquanto a poça está neutralizada |
| 30 | Herbalista da Peste | Almofariz Transbordante | Boca do almofariz | Expulsão que vira aspiração | Inverter o sentido do desvio | No fim da expulsão |
| 31 | Legionário Afogado | Tsunami da Falange | Quebra-mar e sentido da onda | Qual face do escudo ficará protegida | Entrar na sombra direcional do escudo | Trocar de face entre ondas |
| 32 | Legionário Afogado | Jato de Quadrante | Bocal central e abertura do jato | Rotação do cone em torno do bocal | Contornar a origem e flanquear o cone | Passar por trás do bocal; seu corpo causa dano durante o disparo |
| 33 | Legionário Afogado | Convés à Deriva | Convés móvel cercado de água e espuma do gêiser | Deslocamento do convés e jato na posição anterior | Acompanhar a plataforma e desviar dentro dela | Reposicionar após a espuma, antes da erupção |
| 34 | Bruxa do Lodo | Pegadas do Brejo | Marcas deixadas pelo próprio movimento | Ativação atrasada das próprias pegadas | Percorrer espaço novo e evitar voltar cedo | Retornar apenas depois de a marca desaparecer |
| 35 | Bruxa do Lodo | Bancos de Viscosidade | Faixas de lama e origem da enxurrada oblíqua | Frenagem na lama e alcance lateral da enxurrada | Escolher piso livre e escapar pela extremidade do fluxo | Cruzar a lama antes de o fluxo chegar |
| 36 | Bruxa do Lodo | Bolha Inversa | Bolha azul e mudança para violeta | Abrigo que se transforma em perigo | Entrar na bolha, depois sair do seu centro | Na transição de cor, antes do colapso |
| 37 | Revenante de Gelo | Avalanche Oblíqua | Acúmulos de neve no teto | Duas massas largas caindo com atraso e deriva lateral | Fugir lateralmente do volume inteiro | Passar atrás da cauda da primeira massa, sem entrar na segunda |
| 38 | Revenante de Gelo | Estacas de Geada | Cinco pontos gelados no teto | Queda acelerada em sequência e permanência no chão | Deslocar lateralmente; evitar voltar sobre as estacas | Depois de a estaca desaparecer |
| 39 | Revenante de Gelo | Prisão Deslizante | Placas de gelo vindas de lados alternados | Inércia da alma, oscilação das placas e espaço de frenagem | Fazer curvas antecipadas e frear antes da troca de lado | Entre as placas; dash continua vulnerável ao gelo |
| 40 | Esqueleto Carbonizado | Costelas em Arco | Arco sólido da costela | Orientação do vazio interno | Atravessar dentro da costela | Quando a concavidade aponta para a alma |
| 41 | Esqueleto Carbonizado | Pavio de Vértebras | Pavio e números | Próxima vértebra a explodir | Seguir os elos já apagados | Após explosão de um elo |
| 42 | Esqueleto Carbonizado | Desmonte Explosivo | Membro ausente | Dispersão e encaixe dos ossos | Sair do tronco e entrar pelo membro ausente | Durante remontagem |
| 43 | Acendedor da Forja | Martelo na Bigorna | Martelo e sombra | Impacto e quique de fagulhas | Contornar cabeça e cruzar fagulhas | Após pancada |
| 44 | Acendedor da Forja | Metal Temperado | Peça incandescente | Achatamento lateral | Passar atrás do metal que desliza | Quando martelo comprime a peça |
| 45 | Acendedor da Forja | Prensa de Forja | Martelos e barra forjada | Alternância e giro da ponta | Sincronizar entrada com retirada | Após subir o martelo do seu lado |
| 46 | Penitente sem Rosto | Pêndulo de Incenso | Pivô e corrente | Arco do peso | Passar perto do pivô | Quando o incensário se afasta |
| 47 | Penitente sem Rosto | Fumaça de Oração | Traço incompleto de fumaça | Símbolo se desenrola | Atravessar vazios internos móveis | Antes de completar o traço |
| 48 | Penitente sem Rosto | Litania Inversa | Oração e comprimento | Arco novo mais eco anterior | Alternar raio e lado do pêndulo | Na mudança de comprimento |
| 49 | Ladrão da Mortalha | Adaga de Retorno | Mão, adaga e marca na parede | Único rebote e volta à mão | Sair do triângulo mão-parede-lâmina | Depois da ida, antes da volta |
| 50 | Ladrão da Mortalha | Reflexo Roubado | Espelho e eixo sinalizado | Origem invertida, não trajetória refletida | Sair lateralmente da linha projetada | Depois de travar o reflexo |
| 51 | Ladrão da Mortalha | Roubo de Passagem | Âncoras e fios de adaga | Recolhimento fecha polígono | Atravessar lado sem fio e contornar a ponta | Antes da tensão e depois da passagem |
| 52 | Acólito do Vitral | Cacos em Leque | Formas dos cacos | Velocidade e orientação das pontas | Serpentear entre peças de ritmos distintos | Quando as pontas se afastam |
| 53 | Acólito do Vitral | Prisma Peregrino | Prisma móvel e raio incidente | Duas saídas simultâneas | Escolher lado pelo ângulo de incidência | Após mudar a incidência |
| 54 | Acólito do Vitral | Rosácea Partida | Mosaico iluminado | Quais peças ejetam e retornam | Cruzar lacunas do desenho desmontado | Entre ejeção e retorno |
| 55 | Goblin Lanceiro | Curta, Longa, Curta | Recuo do braço | Três alcances finitos | Entrar no curto e recuar do longo | Na retirada da estocada |
| 56 | Goblin Lanceiro | Ponta Fincada | Ponta fincada | Círculo do corpo em torno da ponta | Cruzar perto da ponta imóvel | Quando o corpo passa pelo lado oposto |
| 57 | Goblin Lanceiro | Salto de Vara | Inclinação da vara | Pouso do corpo e queda da haste | Desviar para o lado contrário ao pouso | Sob o salto sem entrar na haste |
| 58 | Autômato de Pedra | Punhos Desencaixados | Trilhos dos punhos | Encaixes de retorno diferentes | Cruzar trilhos durante o recuo | Depois de passar um punho |
| 59 | Autômato de Pedra | Engrenagem Excêntrica | Eixo excêntrico | Avanço do centro e dos dentes | Regular distância do centro móvel | Quando a folga acompanha o avanço |
| 60 | Autômato de Pedra | Reconfiguração de Granito | Ordem de encaixe | Peça seguinte e espaço que ocupa | Atravessar a montagem incompleta | Antes do travamento da peça |
| 61 | Mercenário Ferrugem | Lâmina Arrastada | Espada raspando | Limalha acelera à frente | Passar pela cauda da nuvem | Depois da lâmina arrastada |
| 62 | Mercenário Ferrugem | Aço Desviado | Escudo de sucata e ressalto | Espada resvala sem projétil livre | Contornar o golpe encurtado perto do punho | Durante o recuo após impacto |
| 63 | Mercenário Ferrugem | Troca de Alcance | Mãos e extensão encaixada | Mudança de alcance | Aproximar-se no longo ou recuar no curto | Antes da ponta completar o arco |
| 64 | Carcereiro Correntes | Gancho e Recolha | Gancho e cabo frouxo | Linha se tensiona | Passar atrás do gancho | Antes de esticar o cabo |
| 65 | Carcereiro Correntes | Cela Rebocada | Aberturas da gaiola | Translação da cela | Entrar por uma lateral e sair pela outra | Enquanto ambas permanecem conectadas |
| 66 | Carcereiro Correntes | Nó de Correntes | Âncoras de dois cabos | Laço fecha e reabre | Contornar âncora e atravessar laço aberto | Quando recolhe uma âncora |
| 67 | Cultista das Raízes | Ceifa Germinante | Fio da foice | Sementes tangentes na volta | Contornar cabo e desviar sementes | Entre ceifa e replantio |
| 68 | Cultista das Raízes | Raízes de Procura | Pontas de raízes | Bifurcação segue posições antigas | Atrair e passar atrás da ponta | Após fixar a bifurcação |
| 69 | Cultista das Raízes | Ritual da Colheita | Altar e sementes | Colheita converge à foice | Completar volta entre raízes | Durante sucção, antes de replantar |
| 70 | Afogado de Coral | Âncora de Coral | Arma, sombra e cabo | Arco pesado e arrasto do cabo | Contornar o peso sem cruzar cabo tenso | Atrás da passagem da arma |
| 71 | Afogado de Coral | Ressaca Calcificada | Setas da corrente e ramos | Movimento relativo água/coral | Compensar fluxo e passar pelas pontas | Quando os ramos se afastam |
| 72 | Afogado de Coral | Recife em Ruptura | Coral numerado e arma | Quebra, queda e fluxo pelo buraco | Mudar de lado pela seção recém-quebrada | Depois do impacto, antes dos fragmentos |
| 73 | Mago das Cinzas / Oni em Brasas | Caligrafia de Cinza | Ponta do cajado | Extremidade de espiral incompleta | Cruzar pela extremidade do traço | Antes de a tinta desprender |
| 74 | Mago das Cinzas / Oni em Brasas | Cinza que Retorna | Ordem de escrita | Apagamento em ordem inversa | Acompanhar o desfazer do desenho | Na retirada da ponta |
| 75 | Mago das Cinzas / Oni em Brasas | Oni: Fornalha Aberta | Arma do Oni e brasas | Arco físico comprime e expele fogo | Alternar distância da arma e desvio do fogo | Após pancada, antes da expulsão |
| 76 | Goblin da Cripta | Convocação dos Sepultos | Mãos e túmulos | Ordem das diagonais convocadas | Cruzar entre soldados nascendo | Entre convocações |
| 77 | Goblin da Cripta | Ordem de Retirada | Estandarte e ordem | Marchar ou retornar ao túmulo | Preparar rota pelo destino do pelotão | Depois de interpretar a ordem |
| 78 | Goblin da Cripta | Cerco da Cripta | Retaguarda e reforços | Formação roda em torno do estandarte | Circular pelo flanco aberto | Antes de chegar reforço à retaguarda |
| 79 | Inquisidor Sol | Balança de Luz | Pesos da arma | Quais pares vão se conectar | Cruzar entre conexões ainda apagadas | Na troca de equilíbrio |
| 80 | Inquisidor Sol | Sentença em Sequência | Selos ordenados e ponteiro solar | Sequência de direções sem prisma | Memorizar setores e girar no sentido indicado | Entre duas sentenças |
| 81 | Inquisidor Sol | Eclipse do Veredito | Disco de eclipse | Sombra gira durante rajada radial | Acompanhar a sombra por arco | Entre as bordas iluminadas do eclipse |
| 82 | Sineiro Vazio | Badalo Suspenso | Badalo e dois pulsos | Fendas diferentes em cada eco | Contornar metal e atravessar ondas em sequência | Depois do badalo, em cada falha |
| 83 | Sineiro Vazio | Peso do Silêncio | Toque e seta de gravidade | Quarto de volta da atração | Compensar mudança e desviar dos pesos | Após anunciar o novo eixo |
| 84 | Sineiro Vazio | Réquiem do Vazio | Sino calado e reflexões | Qual eco tem abertura | Ligar falha ao sino de origem | Na passagem do eco do sino silencioso |
| 85 | A Última Torre | Alvenaria em Queda | Peças e contrapesos | Queda, encaixe e retirada | Cruzar entre peças móveis | Quando retira a peça encaixada |
| 86 | A Última Torre | Seteiras Vivas | Origem real das seteiras | Miras travadas e translação da caixa | Mover relativo à arena e aos disparos | Após disparo sair da seteira |
| 87 | A Última Torre | Última Ascensão | Etapas de compressão | Qual conexão permanece aberta | Escolher caminho entre degraus e disparos | Durante reabertura por contrapeso |
