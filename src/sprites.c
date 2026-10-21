#include "sprites.h"

#include <stdio.h>
#include <string.h>

typedef struct { IdSprite id; const char *caminho; float escala; Vector2 deslocamento; } EspecificacaoSprite;

static const EspecificacaoSprite ESPECIFICACOES[] = {
    {SPRITE_INIMIGO_POEIRA_MARIPOSA, "assets/sprites/enemy_mariposa_poeira.png", 1, {0, 0}},
    {SPRITE_INIMIGO_BRILHO_SAPO, "assets/sprites/enemy_sapo_luz.png", 1, {0, 0}},
    {SPRITE_INIMIGO_BRASA_PASSARO, "assets/sprites/enemy_passaro_brasa.png", 1, {0, 0}},
    {SPRITE_INIMIGO_MENTA_LIMO, "assets/sprites/enemy_slime_menta.png", 1, {0, 0}},
    {SPRITE_INIMIGO_LANTERNA_FAGULHA, "assets/sprites/enemy_fogo_fatuo.png", 1, {0, 0}},
    {SPRITE_INIMIGO_ESPINHO_LEBRE, "assets/sprites/enemy_lebre_espinho.png", 1, {0, 0}},
    {SPRITE_INIMIGO_SOMBRA_GATO, "assets/sprites/enemy_gato_sombra.png", 1, {0, 0}},
    {SPRITE_INIMIGO_PETALA_CAVALEIRO, "assets/sprites/enemy_cavaleiro_petala.png", 1, {0, 0}},
    {SPRITE_CHEFE_SINEIRO_P1, "assets/sprites/boss_sineiro_p1.png", 1, {0, 0}},
    {SPRITE_CHEFE_SINEIRO_P2, "assets/sprites/boss_sineiro_p2.png", 1, {0, 0}},
    {SPRITE_CHEFE_ULTIMA_TORRE_P1, "assets/sprites/boss_torre_final_p1.png", 1, {0, 0}},
    {SPRITE_CHEFE_ULTIMA_TORRE_P2, "assets/sprites/boss_torre_final_p2.png", 1, {0, 0}},
    {SPRITE_INIMIGO_SOLDADO_SARCAS,"assets/sprites/enemy_soldado_sarcas.png",1,{0,0}},
    {SPRITE_INIMIGO_HERBALISTA_PESTE,"assets/sprites/enemy_herbalista_peste.png",1,{0,0}},
    {SPRITE_INIMIGO_LEGIONARIO_AFOGADO,"assets/sprites/enemy_legionario_afogado.png",1,{0,0}},
    {SPRITE_INIMIGO_BRUXA_LODO,"assets/sprites/enemy_bruxa_lodo.png",1,{0,0}},
    {SPRITE_INIMIGO_REVENANTE_GELO,"assets/sprites/enemy_revenante_gelo.png",1,{0,0}},
    {SPRITE_INIMIGO_ESQUELETO_CARBONIZADO,"assets/sprites/enemy_esqueleto_carbonizado.png",1,{0,0}},
    {SPRITE_INIMIGO_ACENDEDOR_FORJA,"assets/sprites/enemy_acendedor_forja.png",1,{0,0}},
    {SPRITE_INIMIGO_PENITENTE_SEM_ROSTO,"assets/sprites/enemy_penitente_sem_rosto.png",1,{0,0}},
    {SPRITE_INIMIGO_LADRAO_MORTALHA,"assets/sprites/enemy_ladrao_mortalha.png",1,{0,0}},
    {SPRITE_INIMIGO_ACOLITO_VITRAL,"assets/sprites/enemy_acolito_vitral.png",1,{0,0}},
    {SPRITE_INIMIGO_GOBLIN_LANCEIRO,"assets/sprites/enemy_goblin_lanceiro.png",1,{0,0}},
    {SPRITE_INIMIGO_AUTOMATO_PEDRA,"assets/sprites/enemy_automato_pedra.png",1,{0,0}},
    {SPRITE_INIMIGO_MERCENARIO_FERRUGEM,"assets/sprites/enemy_mercenario_ferrugem.png",1,{0,0}},
    {SPRITE_INIMIGO_CARCEREIRO_CORRENTES,"assets/sprites/enemy_carcereiro_correntes.png",1,{0,0}},
    {SPRITE_CHEFE_CULTISTA_RAIZES,"assets/sprites/boss_cultista_raizes.png",1,{0,0}},
    {SPRITE_CHEFE_AFOGADO_CORAL,"assets/sprites/boss_afogado_coral.png",1,{0,0}},
    {SPRITE_CHEFE_MAGO_CINZAS,"assets/sprites/boss_mago_cinzas.png",1,{0,0}},
    {SPRITE_CHEFE_ONI_BRASAS,"assets/sprites/boss_oni_brasas.png",1,{0,0}},
    {SPRITE_CHEFE_GOBLIN_CRIPTA,"assets/sprites/boss_goblin_cripta.png",1,{0,0}},
    {SPRITE_CHEFE_INQUISIDOR_SOL,"assets/sprites/boss_inquisidor_sol_partido.png",1,{0,0}},
    {SPRITE_ALMA, "assets/sprites/soul.png", 1, {0, 0}},
    {SPRITE_TITULO_TORRE, "assets/sprites/title_tower.png", 1, {0, 0}},
    {SPRITE_FUNDO_TITULO, "assets/sprites/bg_title.png", 1, {0, 0}},
    {SPRITE_BOTAO_TITULO, "assets/sprites/ui_button_title.png", 1, {0, 0}},
    {SPRITE_BOTAO_TITULO_SELECIONADO, "assets/sprites/ui_button_title_selected.png", 1, {0, 0}},
    {SPRITE_LOGO_ASCENSAO_TORRE, "assets/sprites/logo_ascensao_torre.png", 1, {0, 0}},
    /* As artes dos broches sao 1024px; esta escala as traz ao tamanho de icone da interface. */
    {SPRITE_BROCHE_INICIAL_CHAMA, "assets/sprites/broche_chama_inicial.png", 0.025f, {0, 0}},
    {SPRITE_BROCHE_INICIAL_PLANTA, "assets/sprites/broche_planta_inicial.png", 0.025f, {0, 0}},
    {SPRITE_BROCHE_INICIAL_AGUA, "assets/sprites/broche_agua_inicial.png", 0.025f, {0, 0}},
    {SPRITE_ACAO_ATACAR, "assets/sprites/ui_action_attack.png", 1, {0, 0}},
    {SPRITE_ACAO_AGIR, "assets/sprites/ui_action_act.png", 1, {0, 0}},
    {SPRITE_ACAO_ITEM, "assets/sprites/ui_action_item.png", 1, {0, 0}},
    {SPRITE_ACAO_POUPAR, "assets/sprites/ui_action_spare.png", 1, {0, 0}},
    {SPRITE_TIPO_FOGO, "assets/sprites/type_chama.png", 1, {0, 0}},
    {SPRITE_TIPO_AGUA, "assets/sprites/type_agua.png", 1, {0, 0}},
    {SPRITE_TIPO_PLANTA, "assets/sprites/type_planta.png", 1, {0, 0}},
    {SPRITE_TIPO_LUZ, "assets/sprites/type_luz.png", 1, {0, 0}},
    {SPRITE_TIPO_SOMBRA, "assets/sprites/type_sombra.png", 1, {0, 0}},
    {SPRITE_TIPO_NEUTRO, "assets/sprites/type_neutro.png", 1, {0, 0}},
    {SPRITE_PROJETIL_FOGO, "assets/sprites/bullet_chama.png", 1, {0, 0}},
    {SPRITE_PROJETIL_AGUA, "assets/sprites/bullet_agua.png", 1, {0, 0}},
    {SPRITE_PROJETIL_PLANTA, "assets/sprites/bullet_planta.png", 1, {0, 0}},
    {SPRITE_PROJETIL_LUZ, "assets/sprites/bullet_luz.png", 1, {0, 0}},
    {SPRITE_PROJETIL_SOMBRA, "assets/sprites/bullet_sombra.png", 1, {0, 0}},
    {SPRITE_PROJETIL_NEUTRO, "assets/sprites/bullet_neutro.png", 1, {0, 0}}
    ,{SPRITE_ITEM_CHA, "assets/sprites/item_cha.png", 1, {0, 0}}
    ,{SPRITE_ITEM_LUA_ERVA, "assets/sprites/item_erva_lunar.png", 1, {0, 0}}
    ,{SPRITE_ITEM_VERMELHA_FITA, "assets/sprites/item_fita_vermelha.png", 1, {0, 0}}
    ,{SPRITE_ITEM_TORRE_SINO, "assets/sprites/item_sino_torre.png", 1, {0, 0}}
    ,{SPRITE_ITEM_MOLHADO_MANTO, "assets/sprites/item_manto_umido.png", 1, {0, 0}}
    ,{SPRITE_ITEM_SOLAR_BROCHE, "assets/sprites/item_broche_solar.png", 1, {0, 0}}
    ,{SPRITE_ITEM_ESTALINHO, "assets/sprites/item_fogos.png", 1, {0, 0}}
    ,{SPRITE_ITEM_ESPINHO_BOMBA, "assets/sprites/item_bomba_espinho.png", 1, {0, 0}}
    ,{SPRITE_ITEM_ARMADURA_CASCA, "assets/sprites/item_armadura_casca.png", 1, {0, 0}}
    ,{SPRITE_ITEM_ESCUDO_MARE, "assets/sprites/item_escudo_mare.png", 1, {0, 0}}
    ,{SPRITE_ITEM_ESPADA_NATUREZA, "assets/sprites/item_espada_natureza.png", 1, {0, 0}}
    ,{SPRITE_ITEM_ESPADA_LUZ, "assets/sprites/item_espada_luz.png", 1, {0, 0}}
    ,{SPRITE_ITEM_ESPADA_FOGO, "assets/sprites/item_espada_fogo.png", 1, {0, 0}}
    ,{SPRITE_ITEM_CAJADO_GELO, "assets/sprites/item_cajado_gelo.png", 1, {0, 0}}
    ,{SPRITE_ITEM_CALCA_FERRUGEM, "assets/sprites/item_calca_ferrugem.png", 1, {0, 0}}
    ,{SPRITE_ITEM_BOTAS_CINZA, "assets/sprites/item_botas_cinza.png", 1, {0, 0}}
    ,{SPRITE_ITEM_AMULETO_ESPELHO, "assets/sprites/item_amuleto_espelho.png", 1, {0, 0}}
    ,{SPRITE_ITEM_AMULETO_DENTE, "assets/sprites/item_amuleto_dente.png", 1, {0, 0}}
    ,{SPRITE_ITEM_AMULETO_OLHO, "assets/sprites/item_amuleto_olho.png", 1, {0, 0}}
    ,{SPRITE_ITEM_ARMADURA_ESPINHEIRO, "assets/sprites/item_armadura_espinheiro.png", 1, {0, 0}}
    ,{SPRITE_ITEM_BOTAS_CARNICEIRO, "assets/sprites/item_botas_carniceiro.png", 1, {0, 0}}
    ,{SPRITE_ITEM_ESCUDO_INQUISIDOR, "assets/sprites/item_escudo_inquisidor.png", 1, {0, 0}}
    ,{SPRITE_ITEM_CALCA_AFOGADO, "assets/sprites/item_calca_afogado.png", 1, {0, 0}}
    ,{SPRITE_ITEM_AMULETO_AMPULHETA, "assets/sprites/item_amuleto_ampulheta.png", 1, {0, 0}}
    ,{SPRITE_FUNDO_PASTEL, "assets/sprites/bg_pastel.png", 1, {0, 0}}
    ,{SPRITE_FUNDO_CHEFE, "assets/sprites/bg_boss.png", 1, {0, 0}}
    ,{SPRITE_FUNDO_PLANTA, "assets/sprites/bg_planta.png", 1, {0, 0}}
    ,{SPRITE_FUNDO_AGUA, "assets/sprites/bg_agua.png", 1, {0, 0}}
    ,{SPRITE_FUNDO_FOGO, "assets/sprites/bg_fogo.png", 1, {0, 0}}
    ,{SPRITE_FUNDO_SOMBRA, "assets/sprites/bg_sombra.png", 1, {0, 0}}
    ,{SPRITE_FUNDO_LUZ, "assets/sprites/bg_luz.png", 1, {0, 0}}
    ,{SPRITE_FUNDO_NEUTRO, "assets/sprites/bg_neutro.png", 1, {0, 0}}
    ,{SPRITE_ATAQUE_BRASA, "assets/sprites/attack_brasa.png", 1, {0, 0}}
    ,{SPRITE_ATAQUE_GOTA_CLARA, "assets/sprites/attack_gota_clara.png", 1, {0, 0}}
    ,{SPRITE_ATAQUE_RAIZ_RAPIDA, "assets/sprites/attack_raiz_rapida.png", 1, {0, 0}}
    ,{SPRITE_ATAQUE_RAIO_MANSO, "assets/sprites/attack_raio_manso.png", 1, {0, 0}}
    ,{SPRITE_ATAQUE_ECO_SOMBRIO, "assets/sprites/attack_eco_sombrio.png", 1, {0, 0}}
    ,{SPRITE_ATAQUE_GOLPE_NEUTRO, "assets/sprites/attack_golpe_neutro.png", 1, {0, 0}}
    ,{SPRITE_ATAQUE_CHAMA_ALTA, "assets/sprites/attack_chama_alta.png", 1, {0, 0}}
    ,{SPRITE_ATAQUE_MARE_CURTA, "assets/sprites/attack_mare_curta.png", 1, {0, 0}}
    ,{SPRITE_ATAQUE_VINHA_FINA, "assets/sprites/attack_vinha_fina.png", 1, {0, 0}}
    ,{SPRITE_ATAQUE_LUZ_GUIA, "assets/sprites/attack_luz_guia.png", 1, {0, 0}}
    ,{SPRITE_ATAQUE_SOMBRA_FRIA, "assets/sprites/attack_sombra_fria.png", 1, {0, 0}}
    ,{SPRITE_ATAQUE_PULSO_CALMO, "assets/sprites/attack_pulso_calmo.png", 1, {0, 0}}
    ,{SPRITE_ATAQUE_FAGULHA, "assets/sprites/attack_fagulha.png", 1, {0, 0}}
    ,{SPRITE_ATAQUE_ORVALHO, "assets/sprites/attack_orvalho.png", 1, {0, 0}}
    ,{SPRITE_ATAQUE_ESPINHO_LUNAR, "assets/sprites/attack_espinho_lunar.png", 1, {0, 0}}
    ,{SPRITE_ATAQUE_SINO_VAZIO, "assets/sprites/attack_sino_vazio.png", 1, {0, 0}}
    ,{SPRITE_EFEITO_TIPO_FOGO, "assets/sprites/fx_type_chama.png", 1, {0, 0}}
    ,{SPRITE_EFEITO_TIPO_AGUA, "assets/sprites/fx_type_agua.png", 1, {0, 0}}
    ,{SPRITE_EFEITO_TIPO_PLANTA, "assets/sprites/fx_type_planta.png", 1, {0, 0}}
    ,{SPRITE_EFEITO_TIPO_LUZ, "assets/sprites/fx_type_luz.png", 1, {0, 0}}
    ,{SPRITE_EFEITO_TIPO_SOMBRA, "assets/sprites/fx_type_sombra.png", 1, {0, 0}}
    ,{SPRITE_EFEITO_TIPO_NEUTRO, "assets/sprites/fx_type_neutro.png", 1, {0, 0}}
    ,{SPRITE_EFEITO_ATAQUE_BRASA, "assets/sprites/fx_brasa.png", 1, {0, 0}}
    ,{SPRITE_EFEITO_ATAQUE_GOTA_CLARA, "assets/sprites/fx_gota_clara.png", 1, {0, 0}}
    ,{SPRITE_EFEITO_ATAQUE_RAIZ_RAPIDA, "assets/sprites/fx_raiz_rapida.png", 1, {0, 0}}
    ,{SPRITE_EFEITO_ATAQUE_RAIO_MANSO, "assets/sprites/fx_raio_manso.png", 1, {0, 0}}
    ,{SPRITE_EFEITO_ATAQUE_ECO_SOMBRIO, "assets/sprites/fx_eco_sombrio.png", 1, {0, 0}}
    ,{SPRITE_EFEITO_ATAQUE_GOLPE_NEUTRO, "assets/sprites/fx_golpe_neutro.png", 1, {0, 0}}
    ,{SPRITE_EFEITO_ATAQUE_CHAMA_ALTA, "assets/sprites/fx_chama_alta.png", 1, {0, 0}}
    ,{SPRITE_EFEITO_ATAQUE_MARE_CURTA, "assets/sprites/fx_mare_curta.png", 1, {0, 0}}
    ,{SPRITE_EFEITO_ATAQUE_VINHA_FINA, "assets/sprites/fx_vinha_fina.png", 1, {0, 0}}
    ,{SPRITE_EFEITO_ATAQUE_LUZ_GUIA, "assets/sprites/fx_luz_guia.png", 1, {0, 0}}
    ,{SPRITE_EFEITO_ATAQUE_SOMBRA_FRIA, "assets/sprites/fx_sombra_fria.png", 1, {0, 0}}
    ,{SPRITE_EFEITO_ATAQUE_PULSO_CALMO, "assets/sprites/fx_pulso_calmo.png", 1, {0, 0}}
    ,{SPRITE_EFEITO_ATAQUE_FAGULHA, "assets/sprites/fx_fagulha.png", 1, {0, 0}}
    ,{SPRITE_EFEITO_ATAQUE_ORVALHO, "assets/sprites/fx_orvalho.png", 1, {0, 0}}
    ,{SPRITE_EFEITO_ATAQUE_ESPINHO_LUNAR, "assets/sprites/fx_espinho_lunar.png", 1, {0, 0}}
    ,{SPRITE_EFEITO_ATAQUE_SINO_VAZIO, "assets/sprites/fx_sino_vazio.png", 1, {0, 0}}
    ,{SPRITE_PROJETIL_PADRAO_CHUVA, "assets/sprites/bullet_rain.png", 1, {0, 0}}
    ,{SPRITE_PROJETIL_PADRAO_LATERAL_ONDA, "assets/sprites/bullet_side_wave.png", 1, {0, 0}}
    ,{SPRITE_PROJETIL_PADRAO_ESPIRAL, "assets/sprites/bullet_spiral.png", 1, {0, 0}}
    ,{SPRITE_PROJETIL_PADRAO_RAPIDA_CHUVA, "assets/sprites/bullet_fast_rain.png", 1, {0, 0}}
    ,{SPRITE_PROJETIL_PADRAO_LARGA_ONDA, "assets/sprites/bullet_wide_wave.png", 1, {0, 0}}
    ,{SPRITE_PROJETIL_PADRAO_FECHADA_ESPIRAL, "assets/sprites/bullet_tight_spiral.png", 1, {0, 0}}
    ,{SPRITE_PROJETIL_PADRAO_PETALA_ORBITA, "assets/sprites/bullet_petal_orbit.png", 1, {0, 0}}
    ,{SPRITE_PROJETIL_PADRAO_REBOTE_ORBES, "assets/sprites/bullet_bounce_orbs.png", 1, {0, 0}}
    ,{SPRITE_PROJETIL_PADRAO_MOVEL_PAREDE, "assets/sprites/bullet_moving_wall.png", 1, {0, 0}}
    ,{SPRITE_PROJETIL_PADRAO_DIVISAO_DISPAROS, "assets/sprites/bullet_split_shots.png", 1, {0, 0}}
    ,{SPRITE_PROJETIL_PADRAO_CRUZADOS_LASERS, "assets/sprites/bullet_cross_lasers.png", 1, {0, 0}}
    ,{SPRITE_PROJETIL_PADRAO_PAUSA_DARDOS, "assets/sprites/bullet_pause_darts.png", 1, {0, 0}}
    ,{SPRITE_PROJETIL_PADRAO_ANEL_EXPLOSAO, "assets/sprites/bullet_ring_burst.png", 1, {0, 0}}
    ,{SPRITE_PROJETIL_PADRAO_DIAGONAL_CHUVA, "assets/sprites/bullet_diagonal_rain.png", 1, {0, 0}}
    ,{SPRITE_PROJETIL_PADRAO_PERSEGUICAO_ONDA, "assets/sprites/bullet_chase_wave.png", 1, {0, 0}}
    ,{SPRITE_PROJETIL_PADRAO_CATA_VENTO, "assets/sprites/bullet_pinwheel.png", 1, {0, 0}}
    ,{SPRITE_PROJETIL_PADRAO_GRAVIDADE_INVERSAO, "assets/sprites/bullet_gravity_flip.png", 1, {0, 0}}
    ,{SPRITE_PROJETIL_PADRAO_SOMBRA_FLORACAO, "assets/sprites/bullet_shadow_bloom.png", 1, {0, 0}}
    ,{SPRITE_DEFESA_MARIPOSA_ASAS, "assets/sprites/defense_mariposa_asas.png", 1, {0, 0}}
    ,{SPRITE_DEFESA_SAPO_SALTOS, "assets/sprites/defense_sapo_saltos.png", 1, {0, 0}}
    ,{SPRITE_DEFESA_PASSARO_MERGULHO, "assets/sprites/defense_passaro_mergulho.png", 1, {0, 0}}
    ,{SPRITE_DEFESA_LIMO_CRESCIMENTO, "assets/sprites/defense_limo_crescimento.png", 1, {0, 0}}
    ,{SPRITE_DEFESA_FAGULHA_LUZ, "assets/sprites/defense_fagulha_luz.png", 1, {0, 0}}
    ,{SPRITE_DEFESA_LEBRE_PULOS, "assets/sprites/defense_lebre_pulos.png", 1, {0, 0}}
    ,{SPRITE_DEFESA_GATO_EMBOSCADA, "assets/sprites/defense_gato_emboscada.png", 1, {0, 0}}
    ,{SPRITE_DEFESA_CAVALEIRO_DUELO, "assets/sprites/defense_cavaleiro_duelo.png", 1, {0, 0}}
    ,{SPRITE_DEFESA_SINO_ECO, "assets/sprites/defense_sino_eco.png", 1, {0, 0}}
    ,{SPRITE_DEFESA_TORRE_ESMAGAMENTO, "assets/sprites/defense_torre_esmagamento.png", 1, {0, 0}}
    ,{SPRITE_DEFESA_MARIPOSA_TRAMA, "assets/sprites/defense_mariposa_trama.png", 1, {0, 0}}
    ,{SPRITE_DEFESA_MARIPOSA_CORRENTE, "assets/sprites/defense_mariposa_corrente.png", 1, {0, 0}}
    ,{SPRITE_DEFESA_SAPO_BARREIRAS, "assets/sprites/defense_sapo_barreiras.png", 1, {0, 0}}
    ,{SPRITE_DEFESA_SAPO_VITORIAS_REGIAS, "assets/sprites/defense_sapo_vitorias_regias.png", 1, {0, 0}}
    ,{SPRITE_DEFESA_PASSARO_FOGO_CRUZADO, "assets/sprites/defense_passaro_fogo_cruzado.png", 1, {0, 0}}
    ,{SPRITE_DEFESA_PASSARO_TUNEL, "assets/sprites/defense_passaro_tunel.png", 1, {0, 0}}
    ,{SPRITE_DEFESA_CHUVA_FOGO, "assets/sprites/defense_chuva_fogo.png", 1, {0, 0}}
    ,{SPRITE_DEFESA_LIMO_MARE, "assets/sprites/defense_limo_mare.png", 1, {0, 0}}
    ,{SPRITE_DEFESA_LIMO_BALSAS, "assets/sprites/defense_limo_balsas.png", 1, {0, 0}}
    ,{SPRITE_DEFESA_FAGULHA_CANHOES, "assets/sprites/defense_fagulha_canhoes.png", 1, {0, 0}}
    ,{SPRITE_DEFESA_FAGULHA_ORBITA, "assets/sprites/defense_fagulha_orbita.png", 1, {0, 0}}
    ,{SPRITE_DEFESA_LEBRE_BARREIRAS, "assets/sprites/defense_lebre_barreiras.png", 1, {0, 0}}
    ,{SPRITE_DEFESA_LEBRE_PARKOUR, "assets/sprites/defense_lebre_parkour.png", 1, {0, 0}}
    ,{SPRITE_DEFESA_GATO_IMPACTO, "assets/sprites/defense_gato_impacto.png", 1, {0, 0}}
    ,{SPRITE_DEFESA_GATO_PERSEGUICAO, "assets/sprites/defense_gato_perseguicao.png", 1, {0, 0}}
    ,{SPRITE_DEFESA_CAVALEIRO_LANCAS, "assets/sprites/defense_cavaleiro_lancas.png", 1, {0, 0}}
    ,{SPRITE_DEFESA_CAVALEIRO_DESAFIO, "assets/sprites/defense_cavaleiro_desafio.png", 1, {0, 0}}
    ,{SPRITE_DEFESA_SINO_CANHOES, "assets/sprites/defense_sino_canhoes.png", 1, {0, 0}}
    ,{SPRITE_DEFESA_SINO_IMPACTO, "assets/sprites/defense_sino_impacto.png", 1, {0, 0}}
    ,{SPRITE_DEFESA_SINO_DESAFIO, "assets/sprites/defense_sino_desafio.png", 1, {0, 0}}
    ,{SPRITE_DEFESA_SINO_GAS_SOMBRA, "assets/sprites/defense_sino_gas_sombra.png", 1, {0, 0}}
    ,{SPRITE_DEFESA_TORRE_PARKOUR, "assets/sprites/defense_torre_parkour.png", 1, {0, 0}}
    ,{SPRITE_DEFESA_TORRE_CANHOES, "assets/sprites/defense_torre_canhoes.png", 1, {0, 0}}
    ,{SPRITE_DEFESA_TORRE_DESAFIO, "assets/sprites/defense_torre_desafio.png", 1, {0, 0}}
    ,{SPRITE_DEFESA_SARCAS_PORTOES, "assets/sprites/defense_sarcas_portoes.png", 1, {0, 0}}
    ,{SPRITE_DEFESA_HERBALISTA_FRASCOS, "assets/sprites/defense_herbalista_frascos.png", 1, {0, 0}}
    ,{SPRITE_DEFESA_LEGIONARIO_ESCUDOS, "assets/sprites/defense_legionario_escudos.png", 1, {0, 0}}
    ,{SPRITE_DEFESA_BRUXA_CALDEIRAO, "assets/sprites/defense_bruxa_caldeirao.png", 1, {0, 0}}
    ,{SPRITE_DEFESA_REVENANTE_PRISAO, "assets/sprites/defense_revenante_prisao.png", 1, {0, 0}}
    ,{SPRITE_DEFESA_ESQUELETO_ESTOUPIDO, "assets/sprites/defense_esqueleto_estoupido.png", 1, {0, 0}}
    ,{SPRITE_DEFESA_ACENDEDOR_MARTELO, "assets/sprites/defense_acendedor_martelo.png", 1, {0, 0}}
    ,{SPRITE_DEFESA_PENITENTE_INCENSARIO, "assets/sprites/defense_penitente_incensario.png", 1, {0, 0}}
    ,{SPRITE_DEFESA_LADRAO_REFLEXO, "assets/sprites/defense_ladrao_reflexo.png", 1, {0, 0}}
    ,{SPRITE_DEFESA_ACOLITO_VITRAL, "assets/sprites/defense_acolito_vitral.png", 1, {0, 0}}
    ,{SPRITE_DEFESA_GOBLIN_ESTOCADAS, "assets/sprites/defense_goblin_estocadas.png", 1, {0, 0}}
    ,{SPRITE_DEFESA_AUTOMATO_ENGRENAGENS, "assets/sprites/defense_automato_engrenagens.png", 1, {0, 0}}
    ,{SPRITE_DEFESA_MERCENARIO_RICOCHETE, "assets/sprites/defense_mercenario_ricochete.png", 1, {0, 0}}
    ,{SPRITE_DEFESA_CARCEREIRO_GRADE, "assets/sprites/defense_carcereiro_grade.png", 1, {0, 0}}
    ,{SPRITE_DEFESA_CULTISTA_LABIRINTO, "assets/sprites/defense_cultista_labirinto.png", 1, {0, 0}}
    ,{SPRITE_DEFESA_AFOGADO_MARE_CORAL, "assets/sprites/defense_afogado_mare_coral.png", 1, {0, 0}}
    ,{SPRITE_DEFESA_MAGO_ESPIRAL_CINZAS, "assets/sprites/defense_mago_espiral_cinzas.png", 1, {0, 0}}
    ,{SPRITE_DEFESA_GOBLIN_EXERCITO, "assets/sprites/defense_goblin_exercito.png", 1, {0, 0}}
    ,{SPRITE_DEFESA_INQUISIDOR_VEREDITO, "assets/sprites/defense_inquisidor_veredito.png", 1, {0, 0}}
    ,{SPRITE_ARENA_BORDA, "assets/sprites/arena_border.png", 1, {0, 0}}
    ,{SPRITE_ARENA_FUNDO, "assets/sprites/arena_bg_pastel.png", 1, {0, 0}}
    ,{SPRITE_AVISO_PERIGO, "assets/sprites/warn_danger.png", 1, {0, 0}}
    ,{SPRITE_EFEITO_ACERTO_ALMA, "assets/sprites/fx_hit_soul.png", 1, {0, 0}}
    ,{SPRITE_ESPINHO_PAREDE, "assets/sprites/thorn_wall.png", 1, {0, 0}}
    ,{SPRITE_ESPINHO_PAREDE_AVISO, "assets/sprites/thorn_wall_warn.png", 1, {0, 0}}
    ,{SPRITE_SOMBRA_ORBE, "assets/sprites/shadow_orb.png", 1, {0, 0}}
    ,{SPRITE_SOMBRA_FLORACAO_AVISO, "assets/sprites/shadow_bloom_warn.png", 1, {0, 0}}
    ,{SPRITE_SOMBRA_EXPLOSAO, "assets/sprites/shadow_burst.png", 1, {0, 0}}
    ,{SPRITE_PILOTO_ADAGA, "assets/sprites/defense_ladrao_reflexo.png", 1, {0, 0}}
    ,{SPRITE_PILOTO_ESPELHO, "assets/sprites/shadow_orb.png", 1, {0, 0}}
    ,{SPRITE_PILOTO_FIO, "assets/sprites/bullet_sombra.png", 1, {0, 0}}
    ,{SPRITE_PILOTO_ANCORA, "assets/sprites/defense_afogado_mare_coral.png", 1, {0, 0}}
    ,{SPRITE_PILOTO_CABO, "assets/sprites/defense_carcereiro_grade.png", 1, {0, 0}}
    ,{SPRITE_PILOTO_CORAL, "assets/sprites/defense_afogado_mare_coral.png", 1, {0, 0}}
    ,{SPRITE_PILOTO_ASA, "assets/sprites/defense_mariposa_asas.png", 1, {0, 0}}
    ,{SPRITE_PILOTO_POEIRA, "assets/sprites/defense_mariposa_corrente.png", 1, {0, 0}}
    ,{SPRITE_PILOTO_ESTILHACO, "assets/sprites/bullet_agua.png", 1, {0, 0}}
    ,{SPRITE_PILOTO_MAO, "assets/sprites/enemy_ladrao_mortalha.png", 1, {0, 0}}
    ,{SPRITE_PLANTA_GOTA, "assets/sprites/bullet_planta.png", 1, {0, 0}}
    ,{SPRITE_PLANTA_VENENO, "assets/sprites/bullet_planta.png", 1, {0, 0}}
    ,{SPRITE_PLANTA_ANTIDOTO, "assets/sprites/bullet_planta.png", 1, {0, 0}}
    ,{SPRITE_PLANTA_FRASCO, "assets/sprites/bullet_planta.png", 1, {0, 0}}
    ,{SPRITE_PLANTA_FARPA, "assets/sprites/bullet_planta.png", 1, {0, 0}}
    ,{SPRITE_PLANTA_LANCA, "assets/sprites/bullet_planta.png", 1, {0, 0}}
    ,{SPRITE_PLANTA_ESCUDO, "assets/sprites/bullet_planta.png", 1, {0, 0}}
    ,{SPRITE_PLANTA_TABUA, "assets/sprites/bullet_planta.png", 1, {0, 0}}
    ,{SPRITE_PLANTA_RAIZ, "assets/sprites/bullet_planta.png", 1, {0, 0}}
    ,{SPRITE_PLANTA_FOICE, "assets/sprites/bullet_planta.png", 1, {0, 0}}
    ,{SPRITE_PLANTA_SEMENTE, "assets/sprites/bullet_planta.png", 1, {0, 0}}
    ,{SPRITE_PLANTA_LEBRE, "assets/sprites/enemy_lebre_espinho.png", 1, {0, 0}}
    ,{SPRITE_PLANTA_TOCA, "assets/sprites/bullet_planta.png", 1, {0, 0}}
    ,{SPRITE_PLANTA_ALMOFARIZ, "assets/sprites/bullet_planta.png", 1, {0, 0}}
    ,{SPRITE_PLANTA_ALTAR, "assets/sprites/bullet_planta.png", 1, {0, 0}}
    ,{SPRITE_PLANTA_FILAMENTO, "assets/sprites/bullet_planta.png", 1, {0, 0}}
    ,{SPRITE_AGUA_ONDA, "assets/sprites/bullet_agua.png", 1, {0, 0}}
    ,{SPRITE_AGUA_ESCUDO, "assets/sprites/bullet_agua.png", 1, {0, 0}}
    ,{SPRITE_AGUA_JATO, "assets/sprites/bullet_agua.png", 1, {0, 0}}
    ,{SPRITE_AGUA_DIQUE, "assets/sprites/bullet_agua.png", 1, {0, 0}}
    ,{SPRITE_AGUA_LAMA, "assets/sprites/bullet_agua.png", 1, {0, 0}}
    ,{SPRITE_AGUA_BOLHA, "assets/sprites/bullet_agua.png", 1, {0, 0}}
    ,{SPRITE_AGUA_NEVE, "assets/sprites/bullet_agua.png", 1, {0, 0}}
    ,{SPRITE_AGUA_ESTACA, "assets/sprites/bullet_agua.png", 1, {0, 0}}
    ,{SPRITE_AGUA_FISSURA, "assets/sprites/bullet_agua.png", 1, {0, 0}}
    ,{SPRITE_AGUA_PRISMA, "assets/sprites/bullet_agua.png", 1, {0, 0}}
    ,{SPRITE_AGUA_MEMBRANA, "assets/sprites/bullet_agua.png", 1, {0, 0}}
};

bool SpriteInterpretarMetadadosTexto(const char *texto, MetadadosSprite *metadados)
{
    MetadadosSprite parsed = {1, 1, 0, 0};
    int read = sscanf(texto, "frames=%d fps=%d frame_w=%d frame_h=%d", &parsed.quadros, &parsed.fps,
                      &parsed.largura_quadro, &parsed.altura_quadro);
    if (read != 4 || parsed.quadros < 1 || parsed.fps < 1 || parsed.largura_quadro < 1 || parsed.altura_quadro < 1) return false;
    *metadados = parsed;
    return true;
}

/* Localiza assets sem alterar a pasta de trabalho nem o caminho do save. */
static bool ResolverCaminhoAsset(const char *relativo,char *saida,size_t capacidade)
{
    const char *app=GetApplicationDirectory();
    for(int base=0;base<3;base++){
        int n=base==0?snprintf(saida,capacidade,"%s",relativo):
            base==1?snprintf(saida,capacidade,"%s%s",app,relativo):
                    snprintf(saida,capacidade,"%s../%s",app,relativo);
        if(n<0||(size_t)n>=capacidade)continue;
        if(FileExists(saida))return true;
        char folha[1024];snprintf(folha,sizeof(folha),"%s",saida);
        char *ponto=strrchr(folha,'.');
        if(ponto&&strcmp(ponto,".png")==0&&(size_t)(ponto-folha)+11<sizeof(folha)){
            strcpy(ponto,"_sheet.png");
            if(FileExists(folha))return true;
        }
    }
    return false;
}
static void CarregarUm(SistemaSprites *sprites, const EspecificacaoSprite *especificacao)
{
    SpriteJogo *sprite = &sprites->sprites[especificacao->id];
    char caminho_base[1024];
    char caminho_metadados[1024];
    char caminho_folha[1024];
    const char *caminho_textura = caminho_base;
    FILE *arquivo;

    *sprite = (SpriteJogo){0};
    sprite->peca_temporaria = especificacao->id>=SPRITE_PLANTA_GOTA && especificacao->id<=SPRITE_AGUA_MEMBRANA && especificacao->id!=SPRITE_PLANTA_LEBRE;
    sprite->escala = especificacao->escala;
    sprite->deslocamento = especificacao->deslocamento;
    sprite->metadados = (MetadadosSprite){1, 1, 0, 0};
    if(!ResolverCaminhoAsset(especificacao->caminho,caminho_base,sizeof(caminho_base)))return;
    snprintf(caminho_folha, sizeof(caminho_folha), "%s", caminho_base);
    char *ponto = strrchr(caminho_folha, '.');
    if (ponto != NULL) strcpy(ponto, "_sheet.png");
    if (FileExists(caminho_folha)) caminho_textura = caminho_folha;
    if (!FileExists(caminho_textura)) {
#ifndef NDEBUG
        TraceLog(LOG_DEBUG, "Sprite opcional ausente: %s", especificacao->caminho);
#endif
        return;
    }
    sprite->textura = LoadTexture(caminho_textura);
    sprite->carregado = sprite->textura.id != 0;
    if (!sprite->carregado) return;
    SetTextureFilter(sprite->textura, TEXTURE_FILTER_POINT);
    /* Pecas pequenas giram e encolhem: mipmaps evitam pixels soltos e cintilacao. */
    if(especificacao->id>=SPRITE_PILOTO_ADAGA && especificacao->id<=SPRITE_AGUA_MEMBRANA) {
        GenTextureMipmaps(&sprite->textura);
        SetTextureFilter(sprite->textura,TEXTURE_FILTER_TRILINEAR);
    }
    if(especificacao->id>=SPRITE_AGUA_ONDA&&especificacao->id<=SPRITE_AGUA_MEMBRANA)SetTextureWrap(sprite->textura,TEXTURE_WRAP_REPEAT);
    snprintf(caminho_metadados, sizeof(caminho_metadados), "%s", caminho_base);
    ponto = strrchr(caminho_metadados, '.');
    if (ponto != NULL) strcpy(ponto, ".txt");
    arquivo = fopen(caminho_metadados, "r");
    if (arquivo != NULL) {
        char texto[128] = {0};
        if (fgets(texto, sizeof(texto), arquivo)) SpriteInterpretarMetadadosTexto(texto, &sprite->metadados);
        fclose(arquivo);
    }
}

static void CarregarPecasPiloto(SistemaSprites *sprites)
{
    char caminho_manifesto[1024];
    if(!ResolverCaminhoAsset("assets/pilot-assets.txt",caminho_manifesto,sizeof(caminho_manifesto)))return;
    FILE *manifesto = fopen(caminho_manifesto, "r");
    if (manifesto == NULL) return;
    const char *chaves[] = {"adaga","espelho","fio","ancora","cabo","coral","asa","poeira","estilhaco","mao","planta_gota","planta_veneno","planta_antidoto","planta_frasco","planta_farpa","planta_lanca","planta_escudo","planta_tabua","planta_raiz","planta_foice","planta_semente","planta_lebre","planta_toca","planta_almofariz","planta_altar","planta_filamento","agua_onda","agua_escudo","agua_jato","agua_dique","agua_lama","agua_bolha","agua_neve","agua_estaca","agua_fissura","agua_prisma","agua_membrana"};
    char linha[512], chave[40], caminho[400];
    while (fgets(linha, sizeof(linha), manifesto)) {
        if (linha[0] == '#' || sscanf(linha, "%39[^=]=%399[^\r\n]", chave, caminho) != 2) continue;
        for (int i=0; i<(int)(sizeof(chaves)/sizeof(chaves[0])); i++) {
            char caminho_validado[1024];
            if (strcmp(chave,chaves[i]) || !ResolverCaminhoAsset(caminho,caminho_validado,sizeof(caminho_validado))) continue;
            IdSprite id=(IdSprite)(SPRITE_PILOTO_ADAGA+i);
            if(sprites->sprites[id].carregado) UnloadTexture(sprites->sprites[id].textura);
            EspecificacaoSprite spec={id,caminho,1,{0,0}};
            CarregarUm(sprites,&spec);
              if((id>=SPRITE_PLANTA_GOTA && id<=SPRITE_PLANTA_FILAMENTO && strcmp(caminho,"assets/sprites/bullet_planta.png")!=0)||(id>=SPRITE_AGUA_ONDA && strcmp(caminho,"assets/sprites/bullet_agua.png")!=0)) sprites->sprites[id].peca_temporaria=false;
        }
    }
    fclose(manifesto);
}

void SpritesInicializar(SistemaSprites *sprites)
{
    memset(sprites, 0, sizeof(*sprites));
    for (size_t indice = 0; indice < sizeof(ESPECIFICACOES) / sizeof(ESPECIFICACOES[0]); indice++) CarregarUm(sprites, &ESPECIFICACOES[indice]);
    CarregarPecasPiloto(sprites);
}

void SpritesDescarregar(SistemaSprites *sprites)
{
    for (int indice = 0; indice < SPRITE_TOTAL; indice++) {
        if (sprites->sprites[indice].carregado) UnloadTexture(sprites->sprites[indice].textura);
    }
    memset(sprites, 0, sizeof(*sprites));
}

void SpritesRecarregar(SistemaSprites *sprites) { SpritesDescarregar(sprites); SpritesInicializar(sprites); }
const SpriteJogo *sprite_obter(const SistemaSprites *sprites, IdSprite id)
{
    return id >= 0 && id < SPRITE_TOTAL && sprites->sprites[id].carregado ? &sprites->sprites[id] : NULL;
}
IdSprite SpriteIdInimigo(IdInimigo inimigo, bool segunda_fase)
{
    if (inimigo == INIMIGO_CHEFE_SINEIRO) return segunda_fase ? SPRITE_CHEFE_SINEIRO_P2 : SPRITE_CHEFE_SINEIRO_P1;
    if (inimigo == INIMIGO_CHEFE_ULTIMA_TORRE) return segunda_fase ? SPRITE_CHEFE_ULTIMA_TORRE_P2 : SPRITE_CHEFE_ULTIMA_TORRE_P1;
    if (inimigo >= INIMIGO_SOLDADO_SARCAS && inimigo <= INIMIGO_CARCEREIRO_CORRENTES)
        return (IdSprite)(SPRITE_INIMIGO_SOLDADO_SARCAS + inimigo - INIMIGO_SOLDADO_SARCAS);
    if (inimigo == INIMIGO_CHEFE_CULTISTA_RAIZES) return SPRITE_CHEFE_CULTISTA_RAIZES;
    if (inimigo == INIMIGO_CHEFE_AFOGADO_CORAL) return SPRITE_CHEFE_AFOGADO_CORAL;
    if (inimigo == INIMIGO_CHEFE_MAGO_CINZAS) return segunda_fase ? SPRITE_CHEFE_ONI_BRASAS : SPRITE_CHEFE_MAGO_CINZAS;
    if (inimigo == INIMIGO_CHEFE_GOBLIN_CRIPTA) return SPRITE_CHEFE_GOBLIN_CRIPTA;
    if (inimigo == INIMIGO_CHEFE_INQUISIDOR_SOL) return SPRITE_CHEFE_INQUISIDOR_SOL;
    return (IdSprite)(SPRITE_INIMIGO_POEIRA_MARIPOSA + inimigo);
}
IdSprite SpriteIdTipo(Tipo tipo) { return (IdSprite)(SPRITE_TIPO_FOGO + tipo); }
IdSprite SpriteIdProjetil(Tipo tipo) { return (IdSprite)(SPRITE_PROJETIL_FOGO + tipo); }
IdSprite SpriteIdFundoTipo(Tipo tipo)
{
    static const IdSprite fundos[] = {SPRITE_FUNDO_FOGO, SPRITE_FUNDO_AGUA,
        SPRITE_FUNDO_PLANTA, SPRITE_FUNDO_LUZ, SPRITE_FUNDO_SOMBRA, SPRITE_FUNDO_NEUTRO};
    return tipo >= TIPO_FOGO && tipo < TIPO_TOTAL ? fundos[tipo] : SPRITE_FUNDO_NEUTRO;
}
IdSprite SpriteIdItem(IdItem item)
{
    if (item == ITEM_BROCHE_INICIAL_CHAMA) return SPRITE_BROCHE_INICIAL_CHAMA;
    if (item == ITEM_BROCHE_INICIAL_PLANTA) return SPRITE_BROCHE_INICIAL_PLANTA;
    if (item == ITEM_BROCHE_INICIAL_AGUA) return SPRITE_BROCHE_INICIAL_AGUA;
    /* Itens sem arte propria caem no icone desenhado por codigo. */
    return item >= ITEM_CHA && item <= ITEM_AMULETO_AMPULHETA ?
        (IdSprite)(SPRITE_ITEM_CHA + item) : SPRITE_TOTAL;
}
IdSprite SpriteIdPadraoProjetil(IdPadraoProjetil padrao)
{
    if(padrao>=PADRAO_AGUA_TSUNAMI&&padrao<=PADRAO_AGUA_MEMBRANA)return SPRITE_AGUA_ONDA;
    if(padrao>=PADRAO_PLANTA_ESTILHACO_LANCA && padrao<=PADRAO_PLANTA_RITUAL)return SPRITE_PLANTA_SEMENTE;
    if (padrao >= PADRAO_PILOTO_POEIRA_ASA && padrao <= PADRAO_PILOTO_RECIFE_RUPTURA) {
        static const IdSprite mapa[] = {SPRITE_PILOTO_POEIRA,SPRITE_PILOTO_POEIRA,SPRITE_PILOTO_ASA,
            SPRITE_PILOTO_ADAGA,SPRITE_PILOTO_ESPELHO,SPRITE_PILOTO_ADAGA,
            SPRITE_PILOTO_ANCORA,SPRITE_PILOTO_CORAL,SPRITE_PILOTO_CORAL};
        return mapa[padrao-PADRAO_PILOTO_POEIRA_ASA];
    }
    if (padrao >= PADRAO_CHUVA && padrao <= PADRAO_SOMBRA_FLORACAO)
        return (IdSprite)(SPRITE_PROJETIL_PADRAO_CHUVA + padrao);
    if (padrao >= PADRAO_MARIPOSA_ASAS && padrao < PADRAO_TOTAL)
        return (IdSprite)(SPRITE_DEFESA_MARIPOSA_ASAS + padrao - PADRAO_MARIPOSA_ASAS);
    return SPRITE_TOTAL;
}
IdSprite SpriteIdAtaque(IdAtaque ataque) { return (IdSprite)(SPRITE_ATAQUE_BRASA + ataque); }
IdSprite SpriteIdEfeitoAtaque(IdAtaque ataque) { return (IdSprite)(SPRITE_EFEITO_ATAQUE_BRASA + ataque); }
IdSprite SpriteIdEfeitoTipo(Tipo tipo) { return (IdSprite)(SPRITE_EFEITO_TIPO_FOGO + tipo); }

void SpriteDesenharCentralizado(const SistemaSprites *sprites, IdSprite id, Vector2 centro, Color tingimento,
                        float tempo, bool depurar_limites)
{
    const SpriteJogo *sprite = sprite_obter(sprites, id);
    if (sprite == NULL) return;
    const int quadro = sprite->metadados.quadros > 1 ? ((int)(tempo * sprite->metadados.fps) % sprite->metadados.quadros) : 0;
    const int largura_quadro = sprite->metadados.largura_quadro > 0 ? sprite->metadados.largura_quadro : sprite->textura.width;
    const int altura_quadro = sprite->metadados.altura_quadro > 0 ? sprite->metadados.altura_quadro : sprite->textura.height;
    const Rectangle origem_textura = {(float)(quadro * largura_quadro), 0, (float)largura_quadro, (float)altura_quadro};
    const Rectangle destino = {centro.x - largura_quadro * sprite->escala * 0.5f + sprite->deslocamento.x,
                                   centro.y - altura_quadro * sprite->escala * 0.5f + sprite->deslocamento.y,
                                   largura_quadro * sprite->escala, altura_quadro * sprite->escala};
    DrawTexturePro(sprite->textura, origem_textura, destino, (Vector2){0, 0}, 0, tingimento);
    if (depurar_limites) { DrawRectangleLinesEx(destino, 1, MAGENTA); DrawCircleV(centro, 2, MAGENTA); }
}

void SpriteDesenharCentralizadoEscalado(const SistemaSprites *sprites, IdSprite id, Vector2 centro, Color tingimento,
                              float tempo, float escala, bool depurar_limites)
{
    const SpriteJogo *sprite = sprite_obter(sprites, id);
    if (sprite == NULL) return;
    const int quadro = sprite->metadados.quadros > 1 ? ((int)(tempo * sprite->metadados.fps) % sprite->metadados.quadros) : 0;
    const int largura_quadro = sprite->metadados.largura_quadro > 0 ? sprite->metadados.largura_quadro : sprite->textura.width;
    const int altura_quadro = sprite->metadados.altura_quadro > 0 ? sprite->metadados.altura_quadro : sprite->textura.height;
    const float escala_total = sprite->escala * escala;
    const Rectangle origem_textura = {(float)(quadro * largura_quadro), 0, (float)largura_quadro, (float)altura_quadro};
    const Rectangle destino = {centro.x - largura_quadro * escala_total * 0.5f + sprite->deslocamento.x,
                                   centro.y - altura_quadro * escala_total * 0.5f + sprite->deslocamento.y,
                                   largura_quadro * escala_total, altura_quadro * escala_total};
    DrawTexturePro(sprite->textura, origem_textura, destino, (Vector2){0, 0}, 0, tingimento);
    if (depurar_limites) DrawRectangleLinesEx(destino, 1, MAGENTA);
}

void SpriteDesenharCentralizadoRotacionado(const SistemaSprites *sprites, IdSprite id, Vector2 centro, Color tingimento,
                               float tempo, float rotacao, float escala, bool depurar_limites)
{
    const SpriteJogo *sprite = sprite_obter(sprites, id);
    if (sprite == NULL) return;
    const int quadro = sprite->metadados.quadros > 1 ? ((int)(tempo * sprite->metadados.fps) % sprite->metadados.quadros) : 0;
    const int largura_quadro = sprite->metadados.largura_quadro > 0 ? sprite->metadados.largura_quadro : sprite->textura.width;
    const int altura_quadro = sprite->metadados.altura_quadro > 0 ? sprite->metadados.altura_quadro : sprite->textura.height;
    const Rectangle origem_textura = {(float)(quadro * largura_quadro), 0, (float)largura_quadro, (float)altura_quadro};
    const float escala_total = sprite->escala * escala;
    const Rectangle destino = {centro.x + sprite->deslocamento.x, centro.y + sprite->deslocamento.y,
                                   largura_quadro * escala_total, altura_quadro * escala_total};
    DrawTexturePro(sprite->textura, origem_textura, destino,
                   (Vector2){destino.width * 0.5f, destino.height * 0.5f}, rotacao, tingimento);
    if (depurar_limites) DrawCircleLines((int)centro.x, (int)centro.y, largura_quadro * escala_total * 0.5f, MAGENTA);
}

void SpriteDesenharAjustado(const SistemaSprites *sprites, IdSprite id, Rectangle destino, Color tingimento)
{
    const SpriteJogo *sprite = sprite_obter(sprites, id);
    if (sprite == NULL) return;
    DrawTexturePro(sprite->textura, (Rectangle){0, 0, (float)sprite->textura.width,
                   (float)sprite->textura.height}, destino, (Vector2){0, 0}, 0.0f, tingimento);
}

static void ExportarVazio(const char *caminho, int width, int height, Color cor)
{
    Image imagem = GenImageColor(width, height, cor);
    ExportImage(imagem, caminho);
    UnloadImage(imagem);
}

void SpritesExportarModelos(void)
{
    MakeDirectory("assets"); MakeDirectory("assets/sprites_template");
    static const char *const enemy_names[] = {"mariposa_poeira", "sapo_luz", "passaro_brasa",
        "slime_menta", "fogo_fatuo", "lebre_espinho", "gato_sombra", "cavaleiro_petala"};
    static const char *const type_names[] = {"chama", "agua", "planta", "luz", "sombra", "neutro"};
    for (int indice = 0; indice < 8; indice++) {
        char caminho[128]; snprintf(caminho, sizeof(caminho), "assets/sprites_template/enemy_%s.png", enemy_names[indice]);
        ExportarVazio(caminho, 96, 96, BLANK);
    }
    ExportarVazio("assets/sprites_template/boss_sineiro_p1.png", 160, 160, BLANK);
    ExportarVazio("assets/sprites_template/boss_torre_final_p1.png", 160, 160, BLANK);
    ExportarVazio("assets/sprites_template/soul.png", 16, 16, (Color){231, 87, 112, 255});
    for (int indice = 0; indice < TIPO_TOTAL; indice++) {
        char caminho[128]; snprintf(caminho, sizeof(caminho), "assets/sprites_template/type_%s.png", type_names[indice]);
        ExportarVazio(caminho, 24, 24, cor_tipo((Tipo)indice));
        snprintf(caminho, sizeof(caminho), "assets/sprites_template/fx_type_%s_sheet.png", type_names[indice]);
        ExportarVazio(caminho, 96, 96, BLANK);
        snprintf(caminho, sizeof(caminho), "assets/sprites_template/bullet_%s.png", type_names[indice]);
        ExportarVazio(caminho, 12, 12, cor_tipo((Tipo)indice));
    }
    static const char *const attack_names[] = {"brasa", "gota_clara", "raiz_rapida", "raio_manso",
        "eco_sombrio", "golpe_neutro", "chama_alta", "mare_curta", "vinha_fina", "luz_guia",
        "sombra_fria", "pulso_calmo", "fagulha", "orvalho", "espinho_lunar", "sino_vazio"};
    for (int indice = 0; indice < JOGADOR_ATAQUE_TOTAL; indice++) {
        char caminho[128];
        snprintf(caminho, sizeof(caminho), "assets/sprites_template/attack_%s.png", attack_names[indice]);
        ExportarVazio(caminho, 24, 24, BLANK);
        snprintf(caminho, sizeof(caminho), "assets/sprites_template/fx_%s_sheet.png", attack_names[indice]);
        ExportarVazio(caminho, 96, 96, BLANK);
    }
    static const char *const pattern_names[] = {"rain", "side_wave", "spiral", "fast_rain",
        "wide_wave", "tight_spiral", "petal_orbit", "bounce_orbs", "moving_wall",
        "split_shots", "cross_lasers", "pause_darts", "ring_burst", "diagonal_rain",
        "chase_wave", "pinwheel", "gravity_flip", "shadow_bloom"};
    for (int indice = PADRAO_CHUVA; indice <= PADRAO_SOMBRA_FLORACAO; indice++) {
        char caminho[128];
        snprintf(caminho, sizeof(caminho), "assets/sprites_template/bullet_%s.png", pattern_names[indice]);
        ExportarVazio(caminho, 16, 16, BLANK);
    }
    ExportarVazio("assets/sprites_template/warn_danger.png", 32, 32, (Color){245, 90, 82, 255});
    ExportarVazio("assets/sprites_template/arena_border.png", 16, 16, RAYWHITE);
    ExportarVazio("assets/sprites_template/arena_bg_pastel.png", 320, 180, BLACK);
    ExportarVazio("assets/sprites_template/fx_hit_soul.png", 32, 32, BLANK);
    ExportarVazio("assets/sprites_template/fx_hit_soul_sheet.png", 128, 32, BLANK);
    ExportarVazio("assets/sprites_template/thorn_wall_sheet.png", 64, 48, BLANK);
    ExportarVazio("assets/sprites_template/thorn_wall_warn.png", 16, 48, BLANK);
    ExportarVazio("assets/sprites_template/shadow_orb_sheet.png", 128, 32, BLANK);
    ExportarVazio("assets/sprites_template/shadow_bloom_warn.png", 64, 64, BLANK);
    ExportarVazio("assets/sprites_template/shadow_burst_sheet.png", 576, 96, BLANK);
}
