#include "Tela.hpp"
#include "geom.hpp"
#include <iostream>
#include <sstream>
#include "Calculadora.hpp"

using namespace tela;
using namespace geom;

struct botao_t {
    Retangulo ret;
    enum { normal, apertado } estado;
    enum { numero, zero, operador, parenteses, virgula, clear, igual } tipo;
    char const *texto;
};

std::string str;
botao_t* inicia_botoes(void);
void desenha_figuras(Tela &t, botao_t* vetor_botoes);
void desenha_campo_texto(Tela &t, botao_t* vetor_botoes);
void botao_desenha(Tela &t, botao_t *b);
void detecta_clique(Tela &t, botao_t* vetor_botoes);
std::string resolve(std::string str);

int main(int argc, char **argv) {
    int tecla;
    Tela t;

    botao_t* vetor_botoes = inicia_botoes();

    t.inicia(260, 250, "Calculadora");
    while ((tecla = t.tecla()) != ALLEGRO_KEY_Q) {
        t.limpa();
        desenha_figuras(t, vetor_botoes);
        t.mostra();
        // espera 10 ms antes de atualizar a tela
        t.espera(10);
    }

    free(vetor_botoes);
    t.finaliza();
    
    return 0;
}

botao_t* inicia_botoes(void){
    botao_t* vetor_botoes = new botao_t[20];
    float x = 0, y = 1;

    for(auto i = 0; i < 20; i++){  
        vetor_botoes[i].estado = botao_t::normal;
        vetor_botoes[i].ret.tam.alt = 40;
        vetor_botoes[i].ret.tam.larg = 40;
        vetor_botoes[i].ret.pos.x = 10 + (x * 50);
        vetor_botoes[i].ret.pos.y = (y * 50);
        x++;
        if(x > 4){
            x = 0;
            y++;
        }
    }

    vetor_botoes[0].texto = "1";
    vetor_botoes[0].tipo = botao_t::numero;
    vetor_botoes[1].texto = "2";
    vetor_botoes[1].tipo = botao_t::numero;
    vetor_botoes[2].texto = "3";
    vetor_botoes[2].tipo = botao_t::numero;

    vetor_botoes[3].texto = "+";
    vetor_botoes[3].tipo = botao_t::operador;
    vetor_botoes[4].texto = "(";
    vetor_botoes[4].tipo = botao_t::parenteses;

    vetor_botoes[5].texto = "4";
    vetor_botoes[5].tipo = botao_t::numero;
    vetor_botoes[6].texto = "5";
    vetor_botoes[6].tipo = botao_t::numero;
    vetor_botoes[7].texto = "6";
    vetor_botoes[7].tipo = botao_t::numero;

    vetor_botoes[8].texto = "-";
    vetor_botoes[8].tipo = botao_t::operador;
    vetor_botoes[9].texto = ")";
    vetor_botoes[9].tipo = botao_t::parenteses;

    vetor_botoes[10].texto = "7";
    vetor_botoes[10].tipo = botao_t::numero;
    vetor_botoes[11].texto = "8";
    vetor_botoes[11].tipo = botao_t::numero;
    vetor_botoes[12].texto = "9";
    vetor_botoes[12].tipo = botao_t::numero;

    vetor_botoes[13].texto = "*";
    vetor_botoes[13].tipo = botao_t::operador;
    vetor_botoes[14].texto = "^";
    vetor_botoes[14].tipo = botao_t::operador;

    vetor_botoes[15].texto = ",";
    vetor_botoes[15].tipo = botao_t::virgula;

    vetor_botoes[16].texto = "0";
    vetor_botoes[16].tipo = botao_t::zero;

    vetor_botoes[17].texto = "=";
    vetor_botoes[17].tipo = botao_t::igual;

    vetor_botoes[18].texto = "/";
    vetor_botoes[18].tipo = botao_t::operador;

    vetor_botoes[19].texto = "Clear";
    vetor_botoes[19].tipo = botao_t::clear;

    str = "0";
    return vetor_botoes;
}

void desenha_figuras(Tela &t, botao_t* vetor_botoes){
    desenha_campo_texto(t, vetor_botoes);

    for(auto i = 0; i < 20; i++){
        botao_desenha(t, &vetor_botoes[i]);
        vetor_botoes[i].estado = botao_t::normal;
    }
}

void desenha_campo_texto(Tela &t, botao_t* vetor_botoes){
    Retangulo ret = {{10, 10}, {240, 20}};
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

    detecta_clique(t, vetor_botoes);

    if(str[0] == 'i')  // divisao por 0 resulta em inf
        str = "ERRO: Divisao por 0!";

    char const *texto = static_cast<char const*>(str.c_str()); // converte string para char const*

    t.cor(Corexto);
    tt = t.tamanho_texto(texto);
    pt.x = ret.pos.x + ret.tam.larg - tt.larg - 4;
    pt.y = ret.pos.y + ret.tam.alt / 2 - tt.alt / 2;
    t.texto(pt, texto);
}

void botao_desenha(Tela &t, botao_t *b){
    Ponto be, bd, ce, cd; // baixo, cima, esquerda, direita
    Cor cor_normal = {.5, .5, .5};
    Cor cor_linha1 = {.3, .3, .3};
    Cor cor_linha2 = {.7, .7, .7};
    Cor preto = {0, 0, 0};

    be.x = ce.x = b->ret.pos.x;
    bd.x = cd.x = b->ret.pos.x + b->ret.tam.larg;
    ce.y = cd.y = b->ret.pos.y;
    be.y = bd.y = b->ret.pos.y + b->ret.tam.alt;
    // desenha retangulo

    t.cor(cor_normal);
    
    t.retangulo(b->ret);
    // desenha linhas a esquerda e acima
    if (b->estado == botao_t::apertado) {
        t.cor(cor_linha1);
    } else {
        t.cor(cor_linha2);
    }
    t.linha(be, ce);
    t.linha(ce, cd);
    // desenha linhas a direita e abaixo
    if (b->estado == botao_t::apertado) {
        t.cor(cor_linha2);
    } else {
        t.cor(cor_linha1);
    }
    t.linha(be, bd);
    t.linha(bd, cd);

    // desenha texto no meio do botao
    Ponto pt;
    Tamanho tt;
    t.cor(preto);
    tt = t.tamanho_texto(b->texto);
    pt.x = b->ret.pos.x + b->ret.tam.larg / 2 - tt.larg / 2;
    pt.y = b->ret.pos.y + b->ret.tam.alt / 2 - tt.alt / 2;
    if (b->estado == botao_t::apertado) {
        pt.x++;
        pt.y++;
    }
    t.texto(pt, b->texto);
}

void detecta_clique(Tela& t, botao_t* vetor_botoes){
    if (t.botao()){
        Ponto rato;
        rato = t.rato();
        int tam_str = str.size();

        if(tam_str > 1 && str[0] == 'E'){
            str = "0";
            tam_str = str.size();
        }

        for(auto i = 0; i < 20; i++){
            if(ptemret(rato, vetor_botoes[i].ret)){
                // numeros (tratamento: caso digite 1 e campo texto exibe 0, deve aparecer apenas 1 (e nao 01))
                if(vetor_botoes[i].tipo == botao_t::numero)
                    str = (str[0] == '0' && tam_str == 1) ? vetor_botoes[i].texto : str + vetor_botoes[i].texto;   
                // operadores (tratamento: so insere novo operador se antecessor nao for operador)
                else if(vetor_botoes[i].tipo == botao_t::operador){
                    //char* aux = const_cast<char*>(vetor_botoes[i].texto); // converte char const* para char*
                    //char op = reinterpret_cast<char>(*aux);   // converte char* para char
                    if((tam_str <= 1) || (tam_str > 1 && str[tam_str-1] != *vetor_botoes[i].texto))
                            str += vetor_botoes[i].texto;
                }
                // numero 0 (tratamento: so insere 0 se ja tiver alguma coisa no campo texto)
                else if(vetor_botoes[i].tipo == botao_t::zero && tam_str > 1)    
                    str += "0";
                // virgula (so insere se antecessor nao for virgula)
                else if(vetor_botoes[i].tipo == botao_t::virgula && ((tam_str <= 1) || (tam_str > 1 && str[tam_str-1] != '.')))
                    str += ".";
                // parenteses (tratamento: se campo texto for 0 apaga e coloca o paratenses)
                else if(vetor_botoes[i].tipo == botao_t::parenteses)
                    str = (str[0] == '0' && tam_str == 1) ? vetor_botoes[i].texto : str + vetor_botoes[i].texto;
                // clear                  
                else if(vetor_botoes[i].tipo == botao_t::clear)
                    str = "0";
                // igual
                else if(vetor_botoes[i].tipo == botao_t::igual) 
                    str = resolve(str);

                vetor_botoes[i].estado = botao_t::apertado;
                break;
            }
        }
    }
    t._botao = false;
}

std::string resolve(std::string str){
    Calculadora<float> calc;
    std::string aux;
    aux[0]= '\0'; 
    float num = 0;
    for(auto i = 0; str[i] != '\0'; i++){
        if(str[i] != '+' && str[i] != '-' && str[i] != '*' && str[i] != '/' && str[i] != '^' && str[i] != '(' && str[i] != ')'){
            if(aux[0] != '\0')
                aux += str[i];
            else
                aux = str[i];


            if(str[i+1] == '\0'){
                if(aux[0] != '\0'){
                    num = atof(aux.c_str());
                    calc.operando(num);
                }
                aux[0] = '\0';
                num = 0;
            }
        }
        else{
            if(aux[0] != '\0'){
                num = atof(aux.c_str());
                calc.operando(num);
            }
            aux[0] = '\0';
            num = 0;

            calc.operador(str[i]);
        }

    }
    if(!calc.fim())
        str = "ERRO: formula errada!";
    else{
        float x = calc.resultado();
        std::ostringstream buffer;
        buffer << x;
        str = buffer.str();

    }
    calc.destroi();

    return str;
}