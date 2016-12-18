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
    std::string num;
    Circulo circ;
    Ponto destino;
    bool desenha;
};

struct botao_t {
    Retangulo ret;
    char const *texto;
    enum { normal, ganhou, perdeu } estado_j; /* estado do jogo para msg */
};

botao_t b1;
laser_t laser; /* configuracao inicial do canhao de laser */
std::list<tiro_t*> lista_tiro;
#define QtMaximaVertical 5
#define QtMaximaHorizontal 18
float horizontal[QtMaximaHorizontal];
float vertical[QtMaximaVertical];
//figuras tela
void desenha_figuras(Tela &t, Arv<Inimigos> *a, int tecla, int *libera_tiro, int *pontuacao);
void move_figuras(Tela &t, Arv<Inimigos> *a, int *pontuacao);
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
void inimigos_movimenta(Tela &t, Arv<Inimigos> *a, int *pontuacao);
void inimigos_movimenta_diagonal(Arv<Inimigos> *a, int *pontuacao);
Arv<Inimigos>* inimigo_colidiu(Tela &t, Arv<Inimigos> *a, Arv<Inimigos> *no_colidido);
Arv<Inimigos>* arruma_pos(Arv<Inimigos> *a);
//jogar novamente
void botao_desenha(Tela &t, Arv<Inimigos> *a, botao_t *b);
void campotexto_desenha(Tela &t, int *pontuacao);
bool jogar_novamente(Tela &t, int *pontuacao, int *libera_tiro);
void finaliza_jogo(Tela &t, Arv<Inimigos> *a);


int main(int argc, char **argv) {   
    Tela t;
    Arv<Inimigos> *a = inimigos_inicia();
    Arv<Inimigos> *no_colidido = nullptr;
    int tecla, pontuacao = 0, libera_tiro = 0;

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
        move_figuras(t, a, &pontuacao);
        libera_tiro = libera_tiro - 1; // so libera tiro novamente dps de um tempo 

        no_colidido = inimigo_colidiu(t, a, no_colidido);

        if (no_colidido != nullptr) {
            a = a->retira(a, a, no_colidido->chave);

            if (!a->vazia(a))
                a = arruma_pos(a);

            if (b1.estado_j != b1.normal)
                a = a->libera(a); 

            pontuacao += 200;
            no_colidido = nullptr;
        }

        if (a->vazia(a)) 
            if (jogar_novamente(t, &pontuacao, &libera_tiro))   // deseja jogar novamente
                a = inimigos_inicia();

        t.mostra();
        t.espera(20);   // espera 20 ms antes de atualizar a tela
    }

    finaliza_jogo(t, a);

    return 0;
}

void desenha_figuras(Tela &t, Arv<Inimigos> *a, int tecla, int *libera_tiro, int *pontuacao) {
    if (a->vazia(a)) { //nao tem mais inimigos
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

void move_figuras(Tela &t, Arv<Inimigos> *a, int *pontuacao) {
    laser_movimenta(t, &laser);

    if (!a->vazia(a)) {
        tiro_movimenta();
        inimigos_movimenta(t, a, pontuacao);

        for (auto i = 0; i < QtMaximaVertical; i++)
           vertical[i] += a->info.v;
    }
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
    std::string str[] = {"8", "4", "12", "2", "6", "10", "14", "1", "3", "5", "7", "9", "11", "13", "15"};

    Inimigos p;
    p.v = 0.3;   //padrao: 0.3  
    p.circ.raio = 15;   

    for(auto i = 0; i < 15; i ++){
        p.circ.centro.x = pos_x[i];
        p.circ.centro.y = pos_y[i];
        p.desenha = true;
        p.num = str[i];
        a = a->insere(a, p, indices[i]);
    }

    float i_horizontal[] = {280, 170, 390, 115, 225, 335, 445, 90, 140, 
                            200, 250, 310, 360, 420, 470, 65, 165, 500};   
    float i_vertical[] = {-85, -60, -35, 0, 30};    

    for (auto i = 0; i < QtMaximaHorizontal; i++)   // preenche vetor horizontal p/ movimentos
        horizontal[i] = i_horizontal[i];

    for (auto i = 0; i < QtMaximaVertical; i++)     // preenche vetor vertical p/ movimentos
        vertical[i] = i_vertical[i];

    return a;
}

void inimigos_desenha(Tela &t, Arv<Inimigos> *a) {
    if (!a->vazia(a)) {
        Cor branco = {1.0, 1.0, 1.0};
        Cor azul = {0.2, 0.3, 0.8}; 
        t.cor(azul);

        t.circulo(a->info.circ);
        if (a->esq != nullptr) 
            t.linha(a->info.circ.centro, a->esq->info.circ.centro);
        if (a->dir != nullptr) 
            t.linha(a->info.circ.centro, a->dir->info.circ.centro);

        
        char const* texto = a->info.num.c_str(); // converte string para char const*
        t.cor(branco);
        t.texto(a->info.circ.centro, texto);

        inimigos_desenha(t, a->esq);
        inimigos_desenha(t, a->dir);
    }
}

void inimigos_movimenta(Tela &t, Arv<Inimigos> *a, int *pontuacao) {
    if (!a->vazia(a)) {
        a->info.circ.centro.y += a->info.v;

        if (!a->info.desenha) {
            a->info.destino.y += a->info.v;
            inimigos_movimenta_diagonal(a, pontuacao);
        }
        
        inimigos_movimenta(t, a->esq, pontuacao);
        inimigos_movimenta(t, a->dir, pontuacao);
    }
}

void inimigos_movimenta_diagonal(Arv<Inimigos> *a, int *pontuacao) {
    // verifica se tem de ir para cima ou pra baixo
    if (a->info.circ.centro.y > a->info.destino.y) {
        a->info.circ.centro.y -= 7.0;
        *pontuacao += 10; // quando subir recebe bonus pontos
        if (a->info.circ.centro.y < a->info.destino.y) 
            a->info.circ.centro.y = a->info.destino.y;
    }
    else {
        a->info.circ.centro.y += 7.0;
        if (a->info.circ.centro.y > a->info.destino.y) 
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
    if (a->info.circ.centro.y == a->info.destino.y && a->info.circ.centro.x == a->info.destino.x)
        a->info.desenha = true;
}


Arv<Inimigos>* inimigo_colidiu(Tela &t, Arv<Inimigos> *a, Arv<Inimigos> *no_colidido) {
    if (!a->vazia(a) && b1.estado_j == b1.normal){
        for (auto& tiro: lista_tiro) {   
            if (tiro != nullptr) { 
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

Arv<Inimigos>* arruma_pos(Arv<Inimigos> *a) {
    Arv<Inimigos> *b;
    Arv<Inimigos> *c;

    // linha 1
    a->info.destino.x = horizontal[0];
    a->info.destino.y = vertical[0]; 
    a->info.desenha = false;

    b = a->esq;
    if (!a->vazia(b)) {    
    // linha 2        
    b->info.destino.x = horizontal[1];
    b->info.destino.y = vertical[1];
    b->info.desenha = false;
        // linha 3
        if (!a->vazia(b->esq)) {  
            b->esq->info.destino.x = horizontal[3];
            b->esq->info.destino.y = vertical[2];
            b->esq->info.desenha = false;
            c = b->esq;
            // linha 4
            if (!a->vazia(c->esq)) {  
                c->esq->info.destino.x = horizontal[7];
                c->esq->info.destino.y = vertical[3];
                c->esq->info.desenha = false;

                if (!a->vazia(c->esq->esq)) {  // caso necessario cria linha 5
                    c->esq->esq->info.destino.x = horizontal[15];
                    c->esq->esq->info.destino.y = vertical[4];
                    c->esq->esq->info.desenha = false;
                }
            }
            if (!a->vazia(c->dir)) {
                c->dir->info.destino.x = horizontal[8];
                c->dir->info.destino.y = vertical[3];
                c->dir->info.desenha = false;

                if (!a->vazia(c->dir->dir)) {  // caso necessario cria linha 5
                    c->dir->dir->info.destino.x = horizontal[16];
                    c->dir->dir->info.destino.y = vertical[4];
                    c->dir->dir->info.desenha = false;

                }
            }
        }
        // linha 3
        if (!a->vazia(b->dir)) {
            b->dir->info.destino.x = horizontal[4];
            b->dir->info.destino.y = vertical[2];
            b->dir->info.desenha = false;

            c = b->dir;
            // linha 4
            if (!a->vazia(c->esq)) {
                c->esq->info.destino.x = horizontal[9];
                c->esq->info.destino.y = vertical[3];
                c->esq->info.desenha = false;

            }
            if (!a->vazia(c->dir)) {
                c->dir->info.destino.x = horizontal[10];
                c->dir->info.destino.y = vertical[3];
                c->dir->info.desenha = false;

            }
        }
    } // a->esq
    b = a->dir;
    // linha 2
    if (!a->vazia(b)) {
        b->info.destino.x = horizontal[2];
        b->info.destino.y = vertical[1];
        b->info.desenha = false;

        // linha 3
        if (!a->vazia(b->esq)) {
            b->esq->info.destino.x = horizontal[5];
            b->esq->info.destino.y = vertical[2];
            b->esq->info.desenha = false;

            c = b->esq;
            // linha 4
            if (!a->vazia(c->esq)) {
                c->esq->info.destino.x = horizontal[11];
                c->esq->info.destino.y = vertical[3];
                c->esq->info.desenha = false;
            }
            if (!a->vazia(c->dir)) {
                c->dir->info.destino.x = horizontal[12];
                c->dir->info.destino.y = vertical[3];
                c->dir->info.desenha = false;
            }
        }
        // linha 3
        if (!a->vazia(b->dir)) {
            b->dir->info.destino.x = horizontal[6];
            b->dir->info.destino.y = vertical[2];
            b->dir->info.desenha = false;
            c = b->dir;
            // linha 4
            if (!a->vazia(c->esq)) {
               c->esq->info.destino.x = horizontal[13];
               c->esq->info.destino.y = vertical[3];
               c->esq->info.desenha = false;
            }
            if (!a->vazia(c->dir)) {
                c->dir->info.destino.x = horizontal[14];
                c->dir->info.destino.y = vertical[3];
                c->dir->info.desenha = false;
                if(!a->vazia(c->dir->dir)) {  // caso necessario cria linha 5
                    c->dir->dir->info.destino.x = horizontal[17];
                    c->dir->dir->info.destino.y = vertical[4];
                    c->dir->dir->info.desenha = false;
                }   
            }
        }
    } // a->dir
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

bool jogar_novamente(Tela &t, int *pontuacao, int *libera_tiro) {
    if (ptemret(t.rato(), b1.ret) && t.botao()) { // clicou no botao jogar novamente
        b1.estado_j = b1.normal;
        *pontuacao = 0; //zera variaveis
        *libera_tiro = 0;

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
