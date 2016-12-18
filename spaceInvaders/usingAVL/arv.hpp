// inclui este arquivo apenas uma vez
#pragma once 
#include <iostream>

namespace arvore {
	
template<typename T>	

struct Arv{
	Arv* esq;
	Arv* dir;
	T info;
	int chave;
	int alt; 

	Arv* criar(T invader, int indice) {
	   Arv* a = new Arv;
	   a->esq = NULL;
	   a->dir = NULL;  
	   a->chave = indice;
	   a->alt = 0;
	   a->info = invader;

	   return a;
	}

	// insere um novo no na arvore
	Arv* insere(Arv* a, T invader, int indice) {
		if (a == nullptr) {
			a = criar(invader, indice);
		}
		else if (indice < a->chave)
			a->esq = insere(a->esq, invader, indice);
		else 
			a->dir = insere(a->dir, invader, indice);

		a = balancear(a);

		return a;
	}

	// remove no da arvore
	Arv* retira (Arv* a, Arv* r, int indice) {
		if (vazia(r))
			return nullptr;
		if (indice < r->chave)
			r->esq = retira(a, r->esq, indice);
		else if (indice > r->chave)
			r->dir = retira(a, r->dir, indice);
		else { /* achou o nó a remover */
			if (r->esq == nullptr && r->dir == nullptr) { // no folha
				delete r;
				r = nullptr;
			}
			else if (r->esq == nullptr) { // tem filho dir
				Arv* t = r;
				r = r->dir;
				delete t;
			}
			else if (r->dir == nullptr) { // tem filho esq
				Arv* t = r; 
				r = r->esq;
				delete t;
			}
			else { // tem os dois filhos
				
				// faz a troca dos ponteiros e nao do conteudo

				Arv* raiz = nullptr;
				raiz = 	busca_raiz(a, raiz, r->chave); // verifica se o no a ser removido tinha raiz

				Arv* ant = r;
				Arv* f = r->esq;  // um esq
				while (f->dir != nullptr) { // max que der direita
					ant = f; 
					f = f->dir;
				}

				// arruma raiz do elemento que ira tomar o lugar do removido
				if (ant->esq == f) 
					ant->esq = f->esq;
				else if (ant->dir == f)
					ant->dir = f->esq;

				// arruma sae e sad do elemento que tomou o lugar do no que ira se removido
				f->esq = r->esq; 
				f->dir = r->dir;

				if (raiz != nullptr) {  // se o no a ser removido tinha raiz
					// faz a raiz apontar para o novo filho
					if (raiz->esq == r) 
						raiz->esq = f;
					else if (raiz->dir == r)
						raiz->dir = f;
				}

				delete r; // remove o no

				r = f; // ponteiro de retorno
			}
		}

		r = balancear(r);

		return r;
	}

	Arv* balancear(Arv* a){ // Balanceamento da arvore
		ajustarAltura(a);  
	    if (caso1(a))  
	        a = rotacaoDireita(a);
	    else if (caso2(a))
	        a = rotacaoEsquerdaDireita(a);
	    else if (caso3(a))   
	        a = rotacaoDireitaEsquerda(a);
	    else if (caso4(a))
	    	a = rotacaoEsquerda(a);

		return a;
	}

	bool caso1(Arv* a){ // Desbalanceada a Esquerda / Esquerda
   		return (fator(a) > 1) && (fator(a->esq) > 0);
	}

	bool caso2(Arv* a){ // Desbalanceada a Esquerda / Direita
	    return (fator(a) > 1) && !(fator(a->esq) > 0);
	}

	bool caso3(Arv* a){ // Desbalanceada a Esquerda / Direita
	    return (fator(a) < -1) && !(fator(a->dir) < 0);
	}

	bool caso4(Arv* a){ // Desbalanceada a Esquerda / Direita
	    return (fator(a) < -1) && (fator(a->dir) < 0);
	}

	Arv* rotacaoDireita(Arv* raiz){  
		Arv* q = raiz->esq;
	    Arv* hold = q->dir;
	    q->dir = raiz;
	    raiz->esq = hold;

	    ajustarAltura(raiz->esq);
	    ajustarAltura(raiz);

	    return q;
	}

	Arv* rotacaoEsquerdaDireita(Arv* raiz){
	   raiz->esq = rotacaoEsquerda(raiz->esq);
	   raiz = rotacaoDireita(raiz);

	   return raiz;
	}

	Arv* rotacaoDireitaEsquerda(Arv* raiz){
	   raiz->dir = rotacaoDireita(raiz->dir);
	   raiz = rotacaoEsquerda(raiz);

	   return raiz;
	}

	Arv* rotacaoEsquerda(Arv* raiz){
		Arv* q = raiz->dir;
	    Arv* hold = q->esq;
	    q->esq = raiz;
	    raiz->dir = hold;

	    ajustarAltura(raiz->dir);
	    ajustarAltura(raiz);

	    return q;
	}

	int altura(Arv* a){
	    if (vazia(a))     
	        return -1;
	    else 
	    	return a->alt;
	}

	void ajustarAltura(Arv* a){
    	if (!(vazia(a)))
        	a->alt = 1 + max(altura(a->esq), altura(a->dir));                     
	}

	int max(int a, int b){
  		if (a > b)        
    		return a;

  		return b;
	}

	int fator(Arv* a){
   		if (vazia(a))
      		return 0;

   		return (altura(a->esq) - altura(a->dir));
	}

	// retorna verdadeiro se a arvore estiver vazia
	bool vazia(Arv* a) {
		return (a == nullptr);
	}

	// retorna a raiz do no buscado
	Arv* busca_raiz(Arv* a, Arv* raiz, int chave) {
		if (!vazia(a)) {
			if (a->esq != nullptr && a->esq->chave == chave) { 
					raiz = a;
					return raiz;
			}
			if (a->dir != nullptr && a->dir->chave == chave) {
					raiz = a;
					return raiz;
			}
			
			raiz = busca_raiz(a->esq, raiz, chave);
			raiz = busca_raiz(a->dir, raiz, chave);
			
		}
		return raiz;
	}

	// desaloca a arvore
	Arv* libera(Arv* a) {
		if (!vazia(a)) {
			libera(a->esq);
			libera(a->dir);
			delete a;
		}

		return nullptr;
	}

};

}; // namespace arvore