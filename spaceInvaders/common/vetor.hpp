
// inclui este arquivo apenas uma vez
#include <iostream>
#pragma once 

namespace tad {

template<typename T>

struct Vetor {
    T* dados;  // vetor de dados (ponteiros para tipo T)
    int tam;
    int capacidade;


    // inicializa o vetor com tamanho n
    void cria( const int n ) {
        dados = new T[n];
        tam = 0;
        capacidade = n;
    }

    // retorna o tamanho do vetor
    int tamanho() const {
        return tam;
    }

    // Insere na posiÃ§Ã£o i o valor  de tipo T.
    // Se a posiÃ§Ã£o i estiver ocupado, avanÃ§a uma posiÃ§Ã£o
    // ou seja, empurra todos 1 posiÃ§Ã£o.
    // Retorna true se inserido
    bool insere( const int i, T valor ){
        if(i >= tam && i < capacidade){ //inserir em posicao nao ocupada
            dados[i] = valor;
            tam++;
            return true;
        }
        else if(i < tam){ //posicao ocupada
            if(tam+1 >= capacidade) // nao cabe mais nenhum valor (vetor cheio)
                return false;
            else{
                int k = i;
                T aux;
                for(auto j = 0; k < tam-i; k++, j++){ //move todos uma casa
                    aux = dados[j+1];
                    dados[j+1] = dados[i];
                    dados[i] = aux;
                }
                dados[i] = valor;
                tam++;
                return true;
            }

        }

        return false;
    }

    T retorna_ini(const int i){
        T inimigo = dados[i];
        return inimigo;
    }

    // Remove da posiÃ§Ã£o i e retorna o valor.
    // Se tiver mais elementos depois, eles recuam
    // uma posiÃ§Ã£o.
    T remove( const int i ){
        T valor_removido = dados[i];
        if(i >= tam){ //nao tem  elementos dps dele
            tam--;
        } 
        else{ //move todos uma casa 
            for(auto k = i; k < tam-1; k++){
                dados[k] = dados[k+1];
            }
            tam--;            
        }

        return valor_removido;
    }

    // Retorna true se vetor vazio, senÃ£o false 
    bool vazio() {
        if(!tam)
            return true;
        else
            return false;
    }

    // Libera toda memÃ³ria alocada pelo vetor.
    // TambÃ©m zera contadores
    void destroi( void ){
       delete[] dados;
       capacidade = tam = 0;
    }

};

};
