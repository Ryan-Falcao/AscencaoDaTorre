#include "player.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

#define ATAQUE(ID,NOME,TIPO,QTE,DANO,EFEITO,GOLPES,STATUS,DURACAO,RECARGA,CRIT,MANA,SANG) \
    {ID,NOME,TIPO,QTE,DANO,EFEITO,GOLPES,STATUS,DURACAO,RECARGA,CRIT,MANA,SANG}

static const Ataque ATAQUES[JOGADOR_ATAQUE_TOTAL] = {
    ATAQUE(ATAQUE_BRASA,"Brasa",TIPO_FOGO,QTE_IMPACTO,8,EFEITO_IMPACTO,1,STATUS_QUEIMAR,3,0,true,0,0),
    ATAQUE(ATAQUE_GOTA_CLARA,"Gota Clara",TIPO_AGUA,QTE_RITMO,7,EFEITO_PROJETIL,1,STATUS_MOLHADO,2,0,true,0,0),
    ATAQUE(ATAQUE_RAIZ_RAPIDA,"Corte de Espinhos",TIPO_PLANTA,QTE_CARGA,9,EFEITO_CORTE,1,STATUS_ENRAIZAR,1,0,false,0,2),
    ATAQUE(ATAQUE_RAIO_MANSO,"Corte Solar",TIPO_LUZ,QTE_IMPACTO,9,EFEITO_FEIXE,1,STATUS_CEGAR,1,0,true,0,0),
    ATAQUE(ATAQUE_ECO_SOMBRIO,"Eco Sombrio",TIPO_SOMBRA,QTE_RITMO,10,EFEITO_IMPACTO,2,STATUS_NENHUM,0,0,false,0,2),
    ATAQUE(ATAQUE_GOLPE_NEUTRO,"Golpe Improvisado",TIPO_NEUTRO,QTE_CARGA,7,EFEITO_CORTE,1,STATUS_NENHUM,0,0,false,0,0),
    ATAQUE(ATAQUE_CHAMA_ALTA,"Inferno Vertical",TIPO_FOGO,QTE_CARGA,15,EFEITO_CHUVA,2,STATUS_QUEIMAR,3,1,true,7,3),
    ATAQUE(ATAQUE_MARE_CURTA,"Mare Congelante",TIPO_AGUA,QTE_IMPACTO,12,EFEITO_PROJETIL,1,STATUS_MOLHADO,2,0,false,3,0),
    ATAQUE(ATAQUE_VINHA_FINA,"Raiz Serrilhada",TIPO_PLANTA,QTE_RITMO,12,EFEITO_CORTE,2,STATUS_ENRAIZAR,1,0,true,3,4),
    ATAQUE(ATAQUE_LUZ_GUIA,"Arco de Luz",TIPO_LUZ,QTE_CARGA,13,EFEITO_FEIXE,1,STATUS_CEGAR,1,0,false,3,0),
    ATAQUE(ATAQUE_SOMBRA_FRIA,"Sombra Fria",TIPO_SOMBRA,QTE_IMPACTO,14,EFEITO_IMPACTO,1,STATUS_ATORDOAR,1,1,true,5,3),
    ATAQUE(ATAQUE_PULSO_CALMO,"Pulso Calmo",TIPO_NEUTRO,QTE_RITMO,13,EFEITO_CHUVA,3,STATUS_NENHUM,0,0,false,4,0),
    ATAQUE(ATAQUE_FAGULHA,"Lamina Incandescente",TIPO_FOGO,QTE_IMPACTO,11,EFEITO_CORTE,1,STATUS_QUEIMAR,2,0,false,3,1),
    ATAQUE(ATAQUE_ORVALHO,"Zero Absoluto",TIPO_AGUA,QTE_CARGA,16,EFEITO_FEIXE,1,STATUS_MOLHADO,3,1,true,7,0),
    ATAQUE(ATAQUE_ESPINHO_LUNAR,"Jardim do Carrasco",TIPO_PLANTA,QTE_RITMO,16,EFEITO_CHUVA,3,STATUS_ENRAIZAR,1,1,true,7,6),
    ATAQUE(ATAQUE_SINO_VAZIO,"Julgamento Solar",TIPO_LUZ,QTE_IMPACTO,17,EFEITO_IMPACTO,1,STATUS_ATORDOAR,1,1,true,7,0)
};

static void SomarBonus(BonusAtributos *a, const BonusAtributos *b) {
    a->ataque += b->ataque; a->defesa += b->defesa; a->vida += b->vida; a->velocidade += b->velocidade;
    a->mana_maxima += b->mana_maxima; a->mana_por_turno += b->mana_por_turno;
    a->chance_critica += b->chance_critica; a->dano_critico += b->dano_critico;
    a->sangramento += b->sangramento; a->resistencia_sangramento += b->resistencia_sangramento;
}

static const IdAtaque *AtaquesDoTipo(Tipo tipo, int *total) {
    static const IdAtaque natureza[] = {ATAQUE_RAIZ_RAPIDA,ATAQUE_VINHA_FINA,ATAQUE_ESPINHO_LUNAR};
    static const IdAtaque luz[] = {ATAQUE_RAIO_MANSO,ATAQUE_LUZ_GUIA,ATAQUE_SINO_VAZIO};
    static const IdAtaque fogo[] = {ATAQUE_BRASA,ATAQUE_FAGULHA,ATAQUE_CHAMA_ALTA};
    static const IdAtaque gelo[] = {ATAQUE_GOTA_CLARA,ATAQUE_MARE_CURTA,ATAQUE_ORVALHO};
    static const IdAtaque sombra[] = {ATAQUE_ECO_SOMBRIO,ATAQUE_SOMBRA_FRIA};
    static const IdAtaque neutro[] = {ATAQUE_GOLPE_NEUTRO,ATAQUE_PULSO_CALMO};
    *total = 3;
    if (tipo == TIPO_PLANTA) return natureza;
    if (tipo == TIPO_LUZ) return luz;
    if (tipo == TIPO_FOGO) return fogo;
    if (tipo == TIPO_AGUA) return gelo;
    *total = 2;
    return tipo == TIPO_SOMBRA ? sombra : neutro;
}

static void AdicionarAtaquesDoTipo(AtributosJogador *j, Tipo tipo) {
    int total_tipo = 0;
    const IdAtaque *ids = AtaquesDoTipo(tipo, &total_tipo);
    for (int i = 0; i < total_tipo && j->total_ataques < MAXIMO_EQUIPADOS_ATAQUES; i++) {
        bool repetido = false;
        for (int equipado = 0; equipado < j->total_ataques; equipado++)
            if (j->ataques[equipado].id == ids[i]) repetido = true;
        if (!repetido) j->ataques[j->total_ataques++] = ATAQUES[ids[i]];
    }
}

static void DefinirAtaquesEquipados(AtributosJogador *j) {
    const IdItem arma = j->equipamentos[ESPACO_ARMAMENTO];
    Tipo tipo_arma = TIPO_NEUTRO;
    if (arma == ITEM_ESPADA_NATUREZA) tipo_arma = TIPO_PLANTA;
    else if (arma == ITEM_ESPADA_LUZ) tipo_arma = TIPO_LUZ;
    else if (arma == ITEM_ESPADA_FOGO) tipo_arma = TIPO_FOGO;
    else if (arma == ITEM_CAJADO_GELO) tipo_arma = TIPO_AGUA;

    j->total_ataques = 0;
    AdicionarAtaquesDoTipo(j, tipo_arma);
    const IdItem amuleto = j->equipamentos[ESPACO_AMULETO_TIPO];
    if (ItemEhEquipamento(amuleto)) {
        const Tipo tipo_amuleto = ItemObterDefinicao(amuleto)->tipo_elemental;
        if (tipo_amuleto != TIPO_NEUTRO) AdicionarAtaquesDoTipo(j, tipo_amuleto);
    }
}

void JogadorRecalcularAtributos(AtributosJogador *j) {
    const int vida_antiga = j->vida_maxima;
    memset(&j->bonus_equipamento,0,sizeof(j->bonus_equipamento));
    for (int i=0;i<ESPACO_EQUIPADO_TOTAL;i++) {
        IdItem id=j->equipamentos[i];
        if (ItemEhEquipamento(id)) SomarBonus(&j->bonus_equipamento,&ItemObterDefinicao(id)->bonus);
    }
    j->vida_maxima = j->vida_base + j->bonus_equipamento.vida;
    if (vida_antiga > 0) j->hp += j->vida_maxima - vida_antiga;
    if (j->hp > j->vida_maxima) j->hp = j->vida_maxima;
    if (j->hp < 1) j->hp = 1;
    j->mana_maxima = 10 + j->bonus_equipamento.mana_maxima;
    j->mana_por_turno = 2 + j->bonus_equipamento.mana_por_turno +
                        JogadorValorPassiva(j, PASSIVA_MANA_EXTRA);
    if (j->mana > j->mana_maxima) j->mana = j->mana_maxima;
    j->multiplicador_dano_recebido = j->multiplicador_dificuldade * 60.0f / (60.0f + j->bonus_equipamento.defesa);
    DefinirAtaquesEquipados(j);
}

void JogadorInicializar(AtributosJogador *j) {
    memset(j,0,sizeof(*j)); snprintf(j->nome,sizeof(j->nome),"Ari");
    j->vida_base=40; j->nivel=1; j->tipo_defesa=TIPO_NEUTRO; j->multiplicador_dificuldade=1.0f;
    for(int i=0;i<ESPACO_EQUIPADO_TOTAL;i++) j->equipamentos[i]=ITEM_NENHUM;
    j->equipamentos[ESPACO_ARMAMENTO]=ITEM_ESPADA_NATUREZA;
    j->equipamentos[ESPACO_ARMADURA]=ITEM_ARMADURA_CASCA;
    j->equipamentos[ESPACO_CALCA]=ITEM_CALCA_FERRUGEM;
    JogadorRecalcularAtributos(j); j->hp=j->vida_maxima;
    JogadorAdicionarItem(j,ITEM_CHA,2); JogadorAdicionarItem(j,ITEM_VERMELHA_FITA,1);
    JogadorAdicionarItem(j,ITEM_MOLHADO_MANTO,1); JogadorAdicionarItem(j,ITEM_SOLAR_BROCHE,1);
    JogadorAdicionarItem(j,ITEM_ESCUDO_MARE,1); JogadorAdicionarItem(j,ITEM_BOTAS_CINZA,1);
    JogadorAdicionarItem(j,ITEM_AMULETO_ESPELHO,1); JogadorAdicionarItem(j,ITEM_CAJADO_GELO,1);
}

void JogadorIniciarBatalha(AtributosJogador *j) {
    if (j->mana > j->mana_maxima) j->mana = j->mana_maxima;
    memset(j->recargas_ataques,0,sizeof(j->recargas_ataques));
}
void JogadorIniciarTurno(AtributosJogador *j) { j->mana += j->mana_por_turno; if(j->mana>j->mana_maxima) j->mana=j->mana_maxima; }

bool JogadorAdicionarItem(AtributosJogador *j, IdItem id, int quantidade) {
    if(id<ITEM_CHA||id>=ITEM_TOTAL||quantidade<=0)return false;
    for(int i=0;i<MAXIMO_INVENTARIO_ITENS;i++) if(j->inventario[i].quantidade>0&&j->inventario[i].id==id){j->inventario[i].quantidade+=quantidade;return true;}
    for(int i=0;i<MAXIMO_INVENTARIO_ITENS;i++) if(j->inventario[i].quantidade==0){j->inventario[i]=(ItemInventario){id,quantidade};return true;}
    return false;
}

bool JogadorDescartarItem(AtributosJogador *j,int indice) {
    if(indice<0||indice>=MAXIMO_INVENTARIO_ITENS||j->inventario[indice].quantidade<=0)return false;
    j->inventario[indice].quantidade--;
    if(j->inventario[indice].quantidade==0)j->inventario[indice].id=ITEM_CHA;
    return true;
}

bool JogadorEquiparDoInventario(AtributosJogador *j,int indice,EspacoEquipamento espaco) {
    if(indice<0||indice>=MAXIMO_INVENTARIO_ITENS||j->inventario[indice].quantidade<=0)return false;
    IdItem novo=j->inventario[indice].id; if(!ItemPodeEquiparNoEspaco(novo,espaco))return false;
    IdItem antigo=j->equipamentos[espaco];
    if(antigo!=ITEM_NENHUM){
        bool cabe=false;
        for(int i=0;i<MAXIMO_INVENTARIO_ITENS;i++){
            if(j->inventario[i].quantidade==0||j->inventario[i].id==antigo){cabe=true;break;}
        }
        if(!cabe&&j->inventario[indice].quantidade>1)return false;
    }
    j->inventario[indice].quantidade--;
    if(j->inventario[indice].quantidade==0)j->inventario[indice].id=ITEM_CHA;
    if(antigo!=ITEM_NENHUM&&!JogadorAdicionarItem(j,antigo,1)){
        j->inventario[indice]=(ItemInventario){novo,j->inventario[indice].quantidade+1};return false;
    }
    j->equipamentos[espaco]=novo; JogadorRecalcularAtributos(j); return true;
}

bool JogadorDesequipar(AtributosJogador *j,EspacoEquipamento espaco) {
    if(espaco<0||espaco>=ESPACO_EQUIPADO_TOTAL||j->equipamentos[espaco]==ITEM_NENHUM)return false;
    if(!JogadorAdicionarItem(j,j->equipamentos[espaco],1))return false;
    j->equipamentos[espaco]=ITEM_NENHUM; JogadorRecalcularAtributos(j); return true;
}

int JogadorValorPassiva(const AtributosJogador *j,PassivaItem passiva) {
    int total=0; for(int i=0;i<ESPACO_EQUIPADO_TOTAL;i++){const DefinicaoItem *d=ItemObterDefinicao(j->equipamentos[i]);if(ItemEhEquipamento(j->equipamentos[i])&&d->passiva==passiva)total+=d->valor_passiva;} return total;
}

int JogadorAplicarSangramento(AtributosJogador *j,int quantidade) {
    int resistencia=j->bonus_equipamento.resistencia_sangramento+JogadorValorPassiva(j,PASSIVA_RESISTIR_SANGRAMENTO);
    int aplicado=(int)ceilf(quantidade*(100-resistencia)/100.0f); if(aplicado<0)aplicado=0; j->sangramento+=aplicado; return aplicado;
}
int JogadorProcessarSangramento(AtributosJogador *j){int d=j->sangramento;if(d>j->hp)d=j->hp;j->hp-=d;return d;}
void JogadorCurarSangramento(AtributosJogador *j){j->sangramento=0;}

Ataque JogadorObterAtaque(IdAtaque id){if(id<0||id>=JOGADOR_ATAQUE_TOTAL)id=ATAQUE_GOLPE_NEUTRO;return ATAQUES[id];}

bool JogadorGanharXp(AtributosJogador *j,int xp){int anterior=j->nivel;j->xp+=xp;while(j->xp>=j->nivel*20){j->xp-=j->nivel*20;j->nivel++;j->vida_base+=5;j->bonus_dano_permanente++;}JogadorRecalcularAtributos(j);return j->nivel>anterior;}

bool JogadorAtaqueDisponivel(const AtributosJogador *j,const Ataque *a,char *motivo,int tamanho){
    if((int)a->id<0||(int)a->id>=JOGADOR_ATAQUE_TOTAL)return false;
    if(j->mana<a->custo_mana){snprintf(motivo,(size_t)tamanho,"Mana insuficiente: %d/%d",j->mana,a->custo_mana);return false;}
    if(j->recargas_ataques[a->id]>0){snprintf(motivo,(size_t)tamanho,"Espere %d",j->recargas_ataques[a->id]);return false;}return true;
}
void JogadorUsarAtaque(AtributosJogador *j,const Ataque *a){if((int)a->id<0||(int)a->id>=JOGADOR_ATAQUE_TOTAL)return;j->mana-=a->custo_mana;if(j->mana<0)j->mana=0;j->recargas_ataques[a->id]=a->turnos_recarga;}
void JogadorAvancarRecargasAtaques(AtributosJogador *j){for(int i=0;i<JOGADOR_ATAQUE_TOTAL;i++)if(j->recargas_ataques[i]>0)j->recargas_ataques[i]--;}
const char *JogadorNomeStatus(EfeitoStatus s){static const char *const n[]={"","QUEIMAR","MOLHAR","ENRAIZAR","CEGAR","ATORDOAR"};return s>=STATUS_NENHUM&&s<=STATUS_ATORDOAR?n[s]:"";}

bool JogadorUsarItem(AtributosJogador *j,int indice,const DefinicaoItem **usado,int *valor){
    if(indice<0||indice>=MAXIMO_INVENTARIO_ITENS||j->inventario[indice].quantidade<=0)return false;
    const DefinicaoItem *d=ItemObterDefinicao(j->inventario[indice].id);if(d->categoria!=CATEGORIA_CONSUMIVEL)return false;
    j->inventario[indice].quantidade--;*usado=d;*valor=d->valor;
    if(d->efeito==ITEM_EFEITO_CURAR)*valor=JogadorCurar(j,d->valor);
    else if(d->efeito==ITEM_EFEITO_CURAR_SANGRAMENTO){*valor=JogadorCurar(j,d->valor);JogadorCurarSangramento(j);}
    else if(d->efeito==ITEM_EFEITO_DANO_BUFF){j->percentual_buff_dano=d->valor;j->turnos_buff_dano=d->duracao;}
    else if(d->efeito==ITEM_EFEITO_DEFESA_TIPO)j->tipo_defesa=d->tipo_defesa;
    return true;
}

float JogadorMultiplicadorBuffAtaque(const AtributosJogador *j){return 1.0f+j->percentual_buff_dano/100.0f;}
void JogadorConsumirBuffAtaque(AtributosJogador *j){if(j->turnos_buff_dano<=0)return;if(--j->turnos_buff_dano==0)j->percentual_buff_dano=0;}
int JogadorCurar(AtributosJogador *j,int q){int antes=j->hp;j->hp+=q;if(j->hp>j->vida_maxima)j->hp=j->vida_maxima;return j->hp-antes;}
float JogadorVelocidadeDefesa(const AtributosJogador *j){float bonus=fminf(j->bonus_equipamento.velocidade,40)/100.0f;return 135.0f*(1.0f+bonus);}
float JogadorRecargaDash(const AtributosJogador *j){int reducao=JogadorValorPassiva(j,PASSIVA_DASH_RAPIDO);if(reducao>60)reducao=60;return .40f*(100-reducao)/100.0f;}
float JogadorEfetividadeDanoRecebido(const AtributosJogador *j,Tipo tipo_ataque){
    float efetividade=efetividade_tipo(tipo_ataque,j->tipo_defesa);
    int bloqueio=JogadorValorPassiva(j,PASSIVA_BLOQUEAR_CRITICO);
    if(efetividade>1.0f&&bloqueio>0)efetividade=1.0f+(efetividade-1.0f)*(100-bloqueio)/100.0f;
    return efetividade;
}

#undef ATAQUE
