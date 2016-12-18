#include "Tela.hpp"
#include "geom.hpp"
#include "arv.hpp"
#include <iostream>
#include <sstream>
#include <list>

using namespace tela;
using namespace geom;
using namespace arvore;

struct laser_t {
    float v;       /* velocidade */
    Retangulo ret; /* figura */
    Circulo circ;
};

struct tiro_t {
    float v;      
    Circulo circ;
};

struct Inimigos {
    float v;
    bool desenha; 
    Ponto destino;
    Circulo circ;
};

struct botao_t {
    Retangulo ret;
    char const *texto;
    enum { normal, ganhou, perdeu } estado_j; /* estado do jogo para msg */
};

botao_t b1;
laser_t laser; /* configuracao inicial do canhao de laser */
std::list<tiro_t*> lista_tiro;


//figuras tela
void desenha_figuras(Tela &t, Arv<Inimigos> *a, int tecla, int *libera_tiro, int *pontuacao);
void move_figuras(Tela &t, Arv<Inimigos> *a, int *move_lado, int *pontuacao);
//laser
void laser_inicia(Tela &t);
void laser_desenha(Tela &t, laser_t *l);
void laser_movimenta(Tela &t, laser_t *l);
void laser_altera_velocidade(laser_t *l, int tecla);
void laser_atira(laser_t *l, int tecla, int *libera_tiro);
//tiro
tiro_t *tiro_fogo(laser_t *l);
void tiro_desenha(Tela &t);
void tiro_movimenta(void);
//inimigos
Arv<Inimigos>* inimigos_inicia(void);
void inimigos_desenha(Tela &t, Arv<Inimigos> *a);
void inimigos_movimenta(Tela &t, Arv<Inimigos> *a, int *move_lado, int *pontuacao);
void inimigos_movimenta_diagonal(Arv<Inimigos> *a, int *pontuacao);
Arv<Inimigos>* inimigo_colidiu(Tela &t, Arv<Inimigos> *a, Arv<Inimigos> *no_colidido);
Arv<Inimigos>* testa_posicao(Tela &t, Arv<Inimigos> *a, Arv<Inimigos> *no_colidido, int tecla, int *libera_tiro, int *pontuacao);
//jogar novamente
void botao_desenha(Tela &t, Arv<Inimigos> *a, botao_t *b);
void campotexto_desenha(Tela &t, int *pontuacao);
bool jogar_novamente(Tela &t, int *pontuacao, int *libera_tiro, int *move_lado);
void finaliza_jogo(Tela &t, Arv<Inimigos> *a);


int main(int argc, char **argv) {   
    Tela t;
    Arv<Inimigos> *a = inimigos_inicia();
    Arv<Inimigos> *no_colidido = nullptr;
    int tecla, pontuacao = 0, libera_tiro = 0, move_lado = 0;

    t.inicia(600, 400, "Space Invaders");
    laser_inicia(t);
    
    //inicia botao
    b1.ret.pos.x = 230;
    b1.ret.pos.y = 170;
    b1.ret.tam.alt = 50;
    b1.ret.tam.larg = 155;
    b1.estado_j = b1.normal;
    b1.texto = "Jogar novamente";

    while ((tecla = t.tecla()) != ALLEGRO_KEY_Q) {
        t.limpa();
        desenha_figuras(t, a, tecla, &libera_tiro, &pontuacao);
        move_figuras(t, a, &move_lado, &pontuacao);
        libera_tiro = libera_tiro - 1; // so libera tiro novamente dps de um tempo 


        no_colidido = inimigo_colidiu(t, a, no_colidido);

        if (no_colidido != nullptr) {
            a = testa_posicao(t, a, no_colidido, tecla, &libera_tiro, &pontuacao);

            if (b1.estado_j != b1.normal)
                a = a->libera(a); 

            no_colidido = nullptr;
        }

        if (a->vazia(a)) // arvore vazia
            if (jogar_novamente(t, &pontuacao, &libera_tiro, &move_lado))   // deseja jogar novamente
                a = inimigos_inicia();

        t.mostra();
        t.espera(20);   // espera 20 ms antes de atualizar a tela
    }

    finaliza_jogo(t, a);

    return 0;
}

void desenha_figuras(Tela &t, Arv<Inimigos> *a, int tecla, int *libera_tiro, int *pontuacao) {
    if (a->vazia(a)){ //nao tem mais inimigos
        if(b1.estado_j == b1.normal){
            *pontuacao += 300;
            b1.estado_j = b1.ganhou;
        }
        botao_desenha(t, a, &b1);
    }  
        
    else {
        inimigos_desenha(t, a);
        laser_atira(&laser, tecla, libera_tiro);
        tiro_desenha(t);
    }

    // msm na tela de resultado permite mexer no laser 
    campotexto_desenha(t, pontuacao);
    laser_altera_velocidade(&laser, tecla);
    laser_desenha(t, &laser);
}

void move_figuras(Tela &t, Arv<Inimigos> *a, int *move_lado, int *pontuacao) {
    laser_movimenta(t, &laser);
    tiro_movimenta();
    inimigos_movimenta(t, a, move_lado, pontuacao);
    *move_lado += 1;
}

void laser_inicia(Tela &t) {
    Tamanho tam = t.tamanho();
    laser.v = 0.0;
    laser.ret.pos.x = tam.larg / 2;
    laser.ret.pos.y = tam.alt - 20;
    laser.ret.tam.larg = 10;
    laser.ret.tam.alt = 15;
    laser.circ.centro.x = tam.larg / 2 + 5;
    laser.circ.centro.y =  tam.alt;
    laser.circ.raio = 12;
}

void laser_desenha(Tela &t, laser_t *l) {
    Cor vermelho = {1.0, 0.0, 0.0};
    Cor branco = {1.0, 1.0, 1.0};
    t.cor(vermelho);
    t.retangulo(l->ret);
    t.cor(branco);
    t.circulo(l->circ);
}

void laser_movimenta(Tela &t, laser_t *l) {
    Tamanho tam = t.tamanho();
    if (((l->ret.pos.x + l->v) < 0.0) || ((l->ret.pos.x + l->v) > (tam.larg - 10)))
        l->v = 0.0;
    l->ret.pos.x += l->v;
    l->circ.centro.x += l->v;
}

void laser_altera_velocidade(laser_t *l, int tecla) {
    if (tecla == ALLEGRO_KEY_A)  /* altera velocidade mais a esquerda */
        l->v -= 1;
    else if (tecla == ALLEGRO_KEY_D)  /* altera velocidade mais a direita */
        l->v += 1; 
}


void laser_atira(laser_t *l, int tecla, int *libera_tiro) {
    if (tecla == ALLEGRO_KEY_F) {
        if (*libera_tiro < 0) {
            lista_tiro.push_back(tiro_fogo(l));
            *libera_tiro = 10;
        }
    }
}

tiro_t *tiro_fogo(laser_t *l) { /* lanca tiro do laser */
    tiro_t *t = new tiro_t;
    t->v = 5.5;  //padrao: 1.8
    t->circ.centro.x = l->ret.pos.x + 4;
    t->circ.centro.y = l->ret.pos.y - 10;
    t->circ.raio = 2;

    return t;
}

void tiro_desenha(Tela &t) {
	for (auto& tiro: lista_tiro) {
    	if (tiro != nullptr) {
        	Cor branca = {1.0, 1.0, 1.0};
       		t.cor(branca);
      		t.circulo(tiro->circ);
   		 }
	}
}

void tiro_movimenta(void) {
	for(auto& tiro: lista_tiro) {
	    if (tiro != nullptr) { 
	        tiro->circ.centro.y -= tiro->v;
	        
	        if (tiro->circ.centro.y < 0.0) { /* saiu da tela */
	            delete tiro;
	            tiro = nullptr;
	        }
	    }
	}
}

Arv<Inimigos>* inimigos_inicia(void) {
    Arv<Inimigos> *a = nullptr;
    int indices[] = {8, 4, 12, 2, 6, 10, 14, 1, 3, 5, 7, 9, 11, 13, 15};
    float pos_x[] = {280, 170, 390, 115, 225, 335, 445, 90, 140, 200, 250, 310, 360, 420, 470}; 
    float pos_y[] = {-85, -60, -60, -35, -35, -35, -35, 0, 0, 0, 0, 0, 0, 0, 0};

    Inimigos p;
    p.v = 0.3;   //padrao: 0.3  
    p.circ.raio = 15;   
    p.desenha = true;


    for(auto i = 0; i < 15; i ++){
        p.circ.centro.x = pos_x[i];
        p.circ.centro.y = pos_y[i];
        a = a->insere(a, p, indices[i]);
    }

    return a;
}

void inimigos_desenha(Tela &t, Arv<Inimigos> *a) {
    if (!a->vazia(a)) {
        if (a->info.desenha) {
            Cor azul = {0.2, 0.3, 0.8}; 
            t.cor(azul);
        }
        else {
            Cor vermelho = {1.0, 0.0, 0.0};
            t.cor(vermelho);
        }

        t.circulo(a->info.circ);
        
        inimigos_desenha(t, a->esq);
        inimigos_desenha(t, a->dir);
    }
}

void inimigos_movimenta(Tela &t, Arv<Inimigos> *a, int *move_lado, int *pontuacao) {
    if (!a->vazia(a)) {
        if (*move_lado == 330)
            *move_lado = 0;
        else{
            if (a->info.desenha)
                a->info.circ.centro.x += (*move_lado < 150) ? -(a->info.v) : a->info.v; 
            // caso esteja movendo um inimigo colidido, atualiza o destino conforme o movimento da arvore
            else
                a->info.destino.x += (*move_lado < 150) ? -(a->info.v) : a->info.v;
        }

        if (a->info.desenha)
            a->info.circ.centro.y += a->info.v;
        else {
            a->info.destino.y += a->info.v;
            inimigos_movimenta_diagonal(a, pontuacao);
        }

        inimigos_movimenta(t, a->esq, move_lado, pontuacao);
        inimigos_movimenta(t, a->dir, move_lado, pontuacao);
    }
}

void inimigos_movimenta_diagonal(Arv<Inimigos> *a, int *pontuacao) {
    // verifica se tem de ir para cima
    if (a->info.circ.centro.y > a->info.destino.y) {
        a->info.circ.centro.y -= 7.0;
        *pontuacao += 10; // qto mais tiver que subir + pontuacao
        if (a->info.circ.centro.y < a->info.destino.y) 
            a->info.circ.centro.y = a->info.destino.y;
    }

    // verifica se tem de ir para direita ou para esquerda
    if (a->info.circ.centro.x > a->info.destino.x) { 
        a->info.circ.centro.x -= 7.0;
        if (a->info.circ.centro.x < a->info.destino.x) 
            a->info.circ.centro.x = a->info.destino.x;
    }
    else {
        a->info.circ.centro.x += 7.0;
        if (a->info.circ.centro.x > a->info.destino.x) 
            a->info.circ.centro.x = a->info.destino.x;
    }

    // verifica se ja chegou no local adequado
    if (a->info.circ.centro.y <= a->info.destino.y && a->info.circ.centro.x == a->info.destino.x)
        a->info.desenha = true;
}

Arv<Inimigos>* inimigo_colidiu(Tela &t, Arv<Inimigos> *a, Arv<Inimigos> *no_colidido) {
    if (!a->vazia(a) && b1.estado_j == b1.normal){
        for (auto& tiro: lista_tiro) {   
            // so testa se inimigo nao for no colidido em movimento para o novo local 
            if (tiro != nullptr && a->info.desenha) { 
                bool colidiu = intercc(a->info.circ, tiro->circ);

                //se tiro colidiu com inimigo
    	        if (colidiu) { 
    	        	tiro = nullptr;  
    	        	delete tiro;

    	            Cor vermelho = {1.0, 0.0, 0.0}; //desenha um retangulo em vermelho
    	            t.cor(vermelho);
    	            t.circulo(a->info.circ);

                    no_colidido = a;

                    return no_colidido;
    	        }
    	    }  
        }  
        // se tiro saiu da tela ou colidiu com o laser
    	if (a->info.circ.centro.y > 375 || intercc(a->info.circ, laser.circ)) { 
	        Cor vermelho = {1.0, 0.0, 0.0};  //desenha um retangulo em vermelho
            t.cor(vermelho);
            t.circulo(a->info.circ);
    	    b1.estado_j = b1.perdeu;  //fim de jogo por inimigos terem vencido;

            return a;
        }

        no_colidido = inimigo_colidiu(t, a->esq, no_colidido);
        no_colidido = inimigo_colidiu(t, a->dir, no_colidido);
    }

    return no_colidido;
}

Arv<Inimigos>* testa_posicao(Tela &t, Arv<Inimigos> *a, Arv<Inimigos> *no_colidido, int tecla, int *libera_tiro, int *pontuacao) {
    if (no_colidido->esq == nullptr && no_colidido->dir == nullptr) { // no folha
        *pontuacao += 100;
        a = a->retira(a, no_colidido->v); 
        return a;
    }

    Arv<Inimigos> *origem = no_colidido;
    Arv<Inimigos> *folha = nullptr;
    Arv<Inimigos> *aux = no_colidido;
    Ponto destino;
    destino.x = no_colidido->info.circ.centro.x;
    destino.y = no_colidido->info.circ.centro.y;
 
    if (origem->esq == nullptr) {
        origem = origem->dir; // se nao tiver filho da esquerda, vai para direita (procedimento de arvore de busca)
        if (origem->esq != nullptr || origem->dir != nullptr) {  // caso o no encontrado nao seja folha
            origem = folha->busca_folha(origem, folha);
            no_colidido = origem;
        }
    }
    else if (origem->dir == nullptr) {
        origem = origem->esq; // se nao tiver filho da direita, vai para esquerda
        if (origem->esq != nullptr || origem->dir != nullptr) { // caso o no encontrado nao seja folha
            origem = folha->busca_folha(origem, folha); 
            no_colidido = origem;
        }
    }
    else {
        origem = folha->busca_folha(origem, folha);
        no_colidido = origem;
    }

    origem->info.desenha = false;
    origem->info.destino = destino;

    if (origem == no_colidido) {
        aux->info.desenha = false;
        aux->info = origem->info;
        aux->info.destino = destino;
    }

    a = a->retira(a, no_colidido->v); 

    *pontuacao += 200;

    return a;
}

void botao_desenha(Tela &t, Arv<Inimigos> *a, botao_t *b) {
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

}

void campotexto_desenha(Tela &t, int *pontuacao) {
    Retangulo ret = {{520, 375}, {60, 20}}; 
    Ponto be, bd, ce, cd; // baixo, cima, esquerda, direita
    Cor cor_linha1 = {.3, .3, .3};
    Cor cor_linha2 = {.7, .7, .7};
    Cor Corexto = {.7, 0, .2};
    Cor cor_ret = {1, 1, 1};

    std::string str = std::to_string(*pontuacao);
    char const *texto;

    if (b1.estado_j == b1.normal) // exibe apenas numero de pontos 
        texto = str.c_str(); // converte string para char const*
    else {
        ret = {{190, 100}, {260, 60}}; 
        texto = (b1.estado_j == b1.ganhou) ? "***PARABENS! Voce ganhou***" : "***Fim de jogo. Voce perdeu***";
    }

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

    t.cor(Corexto);
    tt = t.tamanho_texto(texto);
    pt.x = ret.pos.x + ret.tam.larg - tt.larg - 10;  
    pt.y = ret.pos.y + ret.tam.alt / 2 - tt.alt / 2;
    t.texto(pt, texto);

    if (b1.estado_j != b1.normal) { // desenha pontuacao no meio da tela tambem
        texto = "PONTUACAO: ";
        tt = t.tamanho_texto(texto);
        pt.x += 40;
        pt.y += 150;
        t.texto(pt, texto);

        texto = str.c_str(); // converte string (numero de pontos) para char const*
        tt = t.tamanho_texto(texto);
        pt.x += 110;
        t.texto(pt, texto);
    }
}

bool jogar_novamente(Tela &t, int *pontuacao, int *libera_tiro, int *move_lado) {
    if (ptemret(t.rato(), b1.ret) && t.botao()) { // clicou no botao jogar novamente
        b1.estado_j = b1.normal;
        *pontuacao = 0; //zera variaveis
        *libera_tiro = 0;
        *move_lado = 0;

        for (auto& tiro: lista_tiro) { // para nao aparecer tiros que estavam ativos ao iniciar novo jogo
            if (tiro != nullptr) { 
                delete tiro;
                tiro = nullptr;
            }
        }

        laser_inicia(t);
        t._botao = false; 

        return true;
    }  

    t._botao = false;   

    return false;
}

void finaliza_jogo(Tela &t, Arv<Inimigos> *a) {  
    a->libera(a);
    t.finaliza();
}
