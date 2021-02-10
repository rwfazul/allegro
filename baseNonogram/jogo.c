#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

// definicoes janela
#define LARGURA_JANELA 800
#define ALTURA_JANELA 800
#define TITULO_JANELA "Nonogram"

// definicoes tabuleiro
#define LINHAS 8
#define COLUNAS 8
#define ALTURA_CELULA 50
#define LARGURA_CELULA 50
#define LARGURA_TABULEIRO LARGURA_CELULA * COLUNAS
#define ALTURA_TABULEIRO ALTURA_CELULA * LINHAS
#define MARGIN_HORIZONTAL (int) ((LARGURA_JANELA / 2) - (LARGURA_TABULEIRO / 2))
#define MARGIN_VERTICAL  (int) ((ALTURA_JANELA / 2) - (ALTURA_TABULEIRO / 2))

// prototipos das funcoes
void inicia_tabuleiro(Celula tabuleiro[][COLUNAS]);
void desenha_tabuleiro(Tela *t, Celula tabuleiro[][COLUNAS]);
void verifica_clique(Tela *t, Celula tabuleiro[][COLUNAS]);
void escreve_numeros(Tela *t, int *numeros, int qtd, Ponto inicio, bool horizontal);
void cicla_estado_celula(Celula *c, bool botao_primario);

int main(int argc, char **argv) {
    int tecla_pressionada;
    Tela t;

    Celula tabuleiro_nonogram[LINHAS][COLUNAS];

    inicia_tabuleiro(tabuleiro_nonogram);

    inicia_tela(&t, LARGURA_JANELA, ALTURA_JANELA, TITULO_JANELA);

    while ((tecla_pressionada = codigo_tecla(&t)) != ALLEGRO_EVENT_DISPLAY_CLOSE) {
        limpa_tela(&t);
        
        desenha_tabuleiro(&t, tabuleiro_nonogram);
        
        verifica_clique(&t, tabuleiro_nonogram);

        mostra_tela(&t);
        
        // espera 30 ms antes de atualizar a tela
        espera(30);
    }
    
    finaliza_tela(&t);

    return 0;
}

// inicia tabuleiro do jogo, definindo as posicoes de cada celula
void inicia_tabuleiro(Celula tabuleiro[][COLUNAS]) {
    for (int i = 0; i < LINHAS; i++) {
        for (int j = 0; j < COLUNAS; j++) {
            // coordenadas de cada celula (retangulo)
            int x = MARGIN_HORIZONTAL + (LARGURA_CELULA * i);
            int y = MARGIN_VERTICAL + (ALTURA_CELULA * j);
            // inicializacao da celula com coordenadas (ponto), tamanho e o estado (por padrao = vazio)
            Celula c = { { x, y }, { ALTURA_CELULA, LARGURA_CELULA }, VAZIO };
            tabuleiro[i][j] = c;
        }
    }
}

// desenha tabuleiro (celulas) e os numeros nas linhas e colunas
void desenha_tabuleiro(Tela *t, Celula tabuleiro[][COLUNAS]) {
    Cor preto = {0, 0, 0};
    Cor vermelho = {1, 0, 0};
    define_cor(t, preto); // usado para as bordas (quando celula nao estiver colorida) e para o preenchimento (quando estiver colorida)

    for (int i = 0; i < LINHAS; i++)  {
        for (int j = 0; j < COLUNAS; j++) {
            // desenha a celula. se estado da celula for colorido, ira pintar o retangulo, senão desenha apenas as bordas
            desenha_retangulo(tabuleiro[i][j], t, tabuleiro[i][j].estado == COLORIDO);
            // se estado da celula for alerta, desenha um x vermelho dentro das bordas do retangulo
            if (tabuleiro[i][j].estado == ALERTA) {
                define_cor(t, vermelho);
                desenha_x_dentro_ret(tabuleiro[i][j], t);
                define_cor(t, preto); // restaura cor
            }
        }
    }

    // escrita dos numeros horizontalmente em cada linha
    for (int i = 0; i < LINHAS; i++) {
        int x = MARGIN_HORIZONTAL - 10;
        int y = MARGIN_VERTICAL + (i * ALTURA_CELULA) + 15;
        Ponto p = { x, y };

        /* apenas um exemplo... implemente a lógica para a definicao do array dos numero de acordo com o Nonogram */
        int numeros[] = {1, 1, 2};

        // escreve os numeros (vetor), a partir de uma coordenada. escrita horizontal = true
        escreve_numeros(t, numeros, 3, p, true);
    }

    // escrita dos numeros verticalmente em cada coluna
    for (int j = 0; j < COLUNAS; j++) {
        int x = MARGIN_HORIZONTAL + (j * LARGURA_CELULA) + 15;
        int y = MARGIN_VERTICAL - 10;
        Ponto p = { x, y };

        /* apenas um exemplo... implemente a lógica para a definicao do array dos numero de acordo com o Nonogram */
        int numeros[] = {2, 3, 1, 1};

        // escreve os numeros (vetor), a partir de uma coordenada. escrita horizontal = false
        escreve_numeros(t, numeros, 4, p, false);
    }
}

// escreve os numeros (vetor), a partir de uma coordenada, horizontalmente ou verticalmente
void escreve_numeros(Tela *t, int *numeros, int qtd, Ponto inicio, bool horizontal) {
    int x = inicio.x, y = inicio.y, offset = 20;
    for (int i = 0; i < qtd; i++) {
        char buffer[10];
        sprintf(buffer, "%d", numeros[i]);

        if (horizontal) {
            if (i > 0) strcat(buffer, ","); // se horizontal separa os numeros com , (menos no numero mais a direita)
            x -= offset;
        } else y -= offset;
        
        Ponto p = { x, y };
        // escreve o numero na posicao/coordenada definada
        escreve_texto(t, p, buffer);
    }
}

// verifica se houve um clique em alguma celula no tabuleiro
void verifica_clique(Tela *t, Celula tabuleiro[][COLUNAS]) {
    Ponto mouse = posicao_mouse(t);
    int botao = botao_clicado(t);
    if (botao) { // mouse foi clicado
        int x_clique_tabuleiro = (mouse.x - MARGIN_HORIZONTAL);
        int y_clique_tabuleiro = (mouse.y - MARGIN_VERTICAL);
        
        // verifica se o clique foi dentro do tabuleiro
        if (x_clique_tabuleiro >= 0 && y_clique_tabuleiro >= 0) {
            // descobre qual celula do tabuleiro foi clicada
            int i = (int) (x_clique_tabuleiro / ALTURA_CELULA);
            int j = (int) (y_clique_tabuleiro / LARGURA_CELULA);

            // verifica se eh uma celula valida
            if (i >= 0 && i < LINHAS && j >=0 && j < COLUNAS) {
                cicla_estado_celula(&tabuleiro[i][j], botao == 1); // botao == 1 eh o primario (esquerdo), == 2 eh o secundario (direito)
                /* dica: aqui eh um bom lugar para verificar se o usuario venceu :) */
            }
        }
        t->_botao = false; // clique ja foi tratado!
    }
}

// cicla o estado da celula
void cicla_estado_celula(Celula *c, bool botao_primario) {
    // cicla os estados da celula
    switch (c->estado) {
        // se celula esta vazia, fica colorida se click no primario ou alerta se click no secundario
        case VAZIO: c->estado = botao_primario ? COLORIDO : ALERTA;
            break;
        // se celula esta colorida, volta a ficar vazia independente do botao pressionado
        case COLORIDO: c->estado = VAZIO;
            break;
        // se celula esta alerta, volta a ficar vazia independente do botao pressionado
        case ALERTA: c->estado = VAZIO;
            break;
    }
}