// inclui este arquivo apenas uma vez
#pragma once 

namespace arvore {
	
template<typename T>	

struct Arv{
	T info;
	int v; //indice
	Arv* dir;
	Arv* esq;

	// insere um novo no na arvore
	Arv* insere(Arv* a, T invader, int indice) {
		if (a == nullptr){
			a = new Arv;
			a->info = invader;
			a->v = indice;
			a->esq = a->dir = nullptr;
		}
		else if (indice < a->v)
			a->esq = insere(a->esq, invader, indice);
		else 
			a->dir = insere(a->dir, invader, indice);

		return a;
	}

	
	// problema ao usar a remocao de arvore de busca convencional: ao tentar remover um no que possui
	// apenas um filho e esse filho nao eh um no folha normalmente faria a raiz apontar para o filho 
	// (que nem sempre eh uma folha), porem no caso especifico do jogo, ele deve apontar obrigatoriamente
	// para um no folha.

	/*Arv* busca(Arv* r, int valor) {
		if (r == NULL) return NULL;
		else if (r->v > valor) return busca (r->v, valor);
		else if (r->v < valor) return busca (r->v, valor);
		else return r;
	}*/

	// retorna verdadeiro se a arvore estiver vazia
	bool vazia(Arv* a) {
		return (a == nullptr);
	}
 
	// remove no da arvore
	Arv* retira (Arv* r, int indice) {
		if (r == nullptr)
			return nullptr;
		else if (r->v > indice)
			r->esq = retira(r->esq, indice);
		else if (r->v < indice)
			r->dir = retira(r->dir, indice);
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
				Arv* f = r->esq;  // um esq
				while (f->dir != nullptr)  // max que der direita
					f = f->dir;

				// troca info
				r->v = f->v;
				f->v = indice;

				r->esq = retira(r->esq, indice);
			}
		}
		return r;
	}

	// retorna uma folha encontrada apartir do no usado como parametro
	Arv* busca_folha(Arv* a, Arv* folha) {
		if (!vazia(a)) {
			if (a->esq == nullptr && a->dir == nullptr) { 
				folha = a;
				return folha;
			}
			
			folha = busca_folha(a->esq, folha);
			folha = busca_folha(a->dir, folha);
			
		}
		return folha;
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