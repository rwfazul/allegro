#include "Tela.hpp"
#include "geom.hpp"
#include "vetor.hpp"
#include <iostream>
#include <list>

using namespace tela;
using namespace geom;

struct laser_t {
    float v;       /* velocidade */
    Retangulo ret; /* figura */
};

struct tiro_t {
    float v;      
    Retangulo ret;
};

struct Inimigos{
    float v;
    Retangulo ret;
};

struct botao_t {
    Retangulo ret;
    char const *texto;
};


botao_t b1;

enum { ganhou, perdeu} estado_j; //estado do jogo para msg
laser_t laser; /* configuracao inicial do canhao de laser */
tad::Vetor<Inimigos*> vetor; //vetor de ponteiros para inimigos
std::list<tiro_t*> lista_tiro;


//figuras tela
void desenha_figuras(Tela &t, int tecla);
void move_figuras(Tela &t);
//laser
void laser_inicia(Tela &t);
void laser_desenha(Tela &t, laser_t *l);
void laser_move(Tela &t, laser_t *l);
void laser_altera_velocidade(laser_t *l, int tecla);
void laser_atira(laser_t *l, int tecla);
//tiro
tiro_t *tiro_fogo(laser_t *l);
void tiro_desenha(Tela &t);
void tiro_movimenta(void);
//inimigos
void inimigos_cria(void);
void inimigos_inicia(int i);
void inimigos_desenha(Tela &t);
void inimigos_movimenta(Tela &t);
void inimigo_colidiu(Inimigos *p, Tela &t, int i);
//jogar novamente
void botao_desenha(botao_t *b, Tela &t);
void campo_texto(Tela &t);
void termina_jogo(void);

int main(int argc, char **argv) {
    int tecla;
    Tela t;
    
    t.inicia(600, 400, "Space Invaders");
    laser_inicia(t);
    inimigos_cria();


    //incia botao
    b1.ret.pos.x = 250;
    b1.ret.pos.y = 190;
    b1.ret.tam.alt = 50;
    b1.ret.tam.larg = 155;
    b1.texto = "Jogar novamente";

    while ((tecla = t.tecla()) != ALLEGRO_KEY_Q) {
        t.limpa();
        desenha_figuras(t, tecla);
        move_figuras(t);
        t.mostra();
        t.espera(30);   // espera 30 ms antes de atualizar a tela
    }

    termina_jogo();
    t.finaliza();

    return 0;
}

void desenha_figuras(Tela &t, int tecla) {
    laser_altera_velocidade(&laser, tecla);
    laser_atira(&laser, tecla);
    laser_desenha(t, &laser);
    tiro_desenha(t);
    inimigos_desenha(t);

    if(vetor.vazio()){//nao tem mais inimigos
        botao_desenha(&b1, t);
        campo_texto(t);
    }
}

void move_figuras(Tela &t) {
    laser_move(t, &laser);
    tiro_movimenta();
    inimigos_movimenta(t);

}

void laser_inicia(Tela &t) {
    Tamanho tam = t.tamanho();
    laser.v = 0.0;
    laser.ret.pos.x = tam.larg / 2;
    laser.ret.pos.y = tam.alt - 20;
    laser.ret.tam.larg = 10;
    laser.ret.tam.alt = 20;
}

void laser_desenha(Tela &t, laser_t *l) {
    Cor vermelho = {1.0, 0.0, 0.0};
    t.cor(vermelho);
    t.retangulo(l->ret);
}

void laser_move(Tela &t, laser_t *l) {
    Tamanho tam = t.tamanho();
    if (((l->ret.pos.x + l->v) < 0.0) || ((l->ret.pos.x + l->v) > (tam.larg - 10)))
        l->v = 0.0;
    l->ret.pos.x += l->v;
}

void laser_altera_velocidade(laser_t *l, int tecla) {
    if (tecla == ALLEGRO_KEY_A)  /* altera velocidade mais a esquerda */
        l->v -= 1;
    else if (tecla == ALLEGRO_KEY_D)  /* altera velocidade mais a direita */
        l->v += 1; 
}


void laser_atira(laser_t *l, int tecla) {
    if (tecla == ALLEGRO_KEY_F) {
        lista_tiro.push_back(tiro_fogo(l));
    }
}

tiro_t *tiro_fogo(laser_t *l) { /* lanca tiro do laser */
    tiro_t *t = new tiro_t;
    t->v = 5	;  //padrao: 1.8
    t->ret.pos.x = l->ret.pos.x;
    t->ret.pos.y = l->ret.pos.y - 10;
    t->ret.tam.larg = 10;
    t->ret.tam.alt = 10;

    return t;
}

void tiro_desenha(Tela &t) {
	for(auto& tiro: lista_tiro){
    	if (tiro != NULL){
        	Cor preta = {0.0, 0.0, 0.0};
       		t.cor(preta);
      		t.retangulo(tiro->ret);
   		 }
	}
}

void tiro_movimenta(void) {
	for(auto& tiro: lista_tiro){
	    if (tiro != NULL) { 
	        tiro->ret.pos.y -= tiro->v;
	        
	        if (tiro->ret.pos.y < 0.0) { /* saiu da tela */
	            delete tiro;
	            tiro = NULL;
	        }
	    }
	}
}

void inimigos_cria(void){
    int tam = 21;  //qtd de inimigos. padrao: 21
    vetor.cria(tam);

    for(auto i = 0; i < tam; i++)
        inimigos_inicia(i);
}

void inimigos_inicia(int i){
    Inimigos* p = new Inimigos;

    p->v = 0.3;   //padrao: 0.3  
    p->ret.tam.larg = 45;
    p->ret.tam.alt = 30;

    if(i < 7){ //primeira leva de inimigos
        p->ret.pos.x = 30 + (i*80);
        p->ret.pos.y = 0;
        vetor.insere(i, p);
    }
    else if(i < 14){ 
        p->ret.pos.x = 30 + ((i-7)*80);
        p->ret.pos.y = -70;
        vetor.insere(i, p); 
    }

    else{
        p->ret.pos.x = 30 + ((i-14)*80);
        p->ret.pos.y = -140;
        vetor.insere(i, p); 
    }
}

void inimigos_desenha(Tela &t){
    for(auto i = 0; i < vetor.tamanho(); i++){
        Inimigos* p = vetor.retorna_ini(i);
        Cor azul = {0.2, 0.3, 0.8};
        t.cor(azul);
        t.retangulo(p->ret);
    }
}

void inimigos_movimenta(Tela &t) {
    for(auto i = 0; i < vetor.tamanho(); i++){
        Inimigos* p = vetor.retorna_ini(i);
        p->ret.pos.y += p->v; //move
        inimigo_colidiu(p, t, i);
    }
}

void inimigo_colidiu(Inimigos *p, Tela &t, int i){
     //se tiro colidiu com inimigo
	for(auto& tiro: lista_tiro){
	    if(tiro != NULL){ 
	        bool colidiu = interrr(p->ret, tiro->ret);
	        if(colidiu){ 
	        	tiro = NULL;
	        	delete(tiro);

	            Cor vermelho = {1.0, 0.0, 0.0}; //desenha um retangulo em vermelho
	            t.cor(vermelho);
	            t.retangulo(p->ret);

	            Inimigos* p = vetor.remove(i);
	            delete p;
	        }
	        if(vetor.vazio())
	            estado_j = ganhou;
	    }

	     //se inimigo colidiu com o laser ou saiu da tela
	    if(p->ret.pos.y > 375 || interrr(p->ret, laser.ret)){
	        Cor vermelho = {1.0, 0.0, 0.0}; //desenha um retangulo em vermelho
	        t.cor(vermelho);
	        t.retangulo(p->ret);
	        termina_jogo();
	        estado_j = perdeu; //fim de jogo por inimigos terem vencido;
	        break;
	    }
           
	}
}

void  botao_desenha(botao_t *b, Tela &t) {
    Ponto be, bd, ce, cd; // baixo, cima, esquerda, direita
    Cor cor_b = {.5, .5, .5};
    Cor cor_linha1 = {.3, .3, .3};
    Cor cor_linha2 = {.7, .7, .7};
    Cor preto = {0, 0, 0};

    be.x = ce.x = b->ret.pos.x;
    bd.x = cd.x = b->ret.pos.x + b->ret.tam.larg;
    ce.y = cd.y = b->ret.pos.y;
    be.y = bd.y = b->ret.pos.y + b->ret.tam.alt;


     // desenha retangulo
    t.cor(cor_b);
    t.retangulo(b->ret);
    // desenha linhas a esquerda e acima
    t.cor(cor_linha1);
    t.linha(be, ce);
    t.linha(ce, cd);
    // desenha linhas a direita e abaixo
    t.cor(cor_linha2);
    t.linha(be, bd);
    t.linha(bd, cd);

    // desenha texto no meio do botao
    Ponto pt;
    Tamanho tt;
    t.cor(preto);
    tt = t.tamanho_texto(b->texto);
    pt.x = b->ret.pos.x + b->ret.tam.larg / 2 - tt.larg / 2;
    pt.y = b->ret.pos.y + b->ret.tam.alt / 2 - tt.alt / 2;
    t.texto(pt, b->texto);

    if (ptemret(t.rato(), b1.ret) && t.botao()) { //jogar novamente
        laser_inicia(t);
        inimigos_cria();
    }
}

void campo_texto(Tela &t){
    Retangulo ret = {{190, 100}, {260, 60}}; 
    Ponto be, bd, ce, cd; // baixo, cima, esquerda, direita
    Cor cor_linha1 = {.3, .3, .3};
    Cor cor_linha2 = {.7, .7, .7};
    Cor Corexto = {.7, 0, .2};
    Cor cor_ret = {1, 1, 1};

    be.x = ce.x = ret.pos.x;
    bd.x = cd.x = ret.pos.x + ret.tam.larg;
    ce.y = cd.y = ret.pos.y;
    be.y = bd.y = ret.pos.y + ret.tam.alt;

    // desenha retangulo
    t.cor(cor_ret);
    t.retangulo(ret);
    // desenha linhas a esquerda e acima
    t.cor(cor_linha1);
    t.linha(be, ce);
    t.linha(ce, cd);
    // desenha linhas a direita e abaixo
    t.cor(cor_linha2);
    t.linha(be, bd);
    t.linha(bd, cd);

    // desenha texto a direita do espaco
    Ponto pt;
    Tamanho tt;
    char const *texto;
    if (estado_j == ganhou)
        texto = "***PARABENS! Voce ganhou***";
    else
        texto = "***Fim de jogo. Voce perdeu***";

    t.cor(Corexto);
    tt = t.tamanho_texto(texto);
    pt.x = ret.pos.x + ret.tam.larg - tt.larg - 10;  
    pt.y = ret.pos.y + ret.tam.alt / 2 - tt.alt / 2;
    t.texto(pt, texto);
}

void termina_jogo(void){
    while(!vetor.vazio()){ //deleta todo vetor
        Inimigos* p = vetor.remove(0);
        delete p;
    }
    vetor.destroi();
}