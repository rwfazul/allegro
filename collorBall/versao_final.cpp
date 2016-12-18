// Allegro
#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
//Aux
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

//CONSTANTES
const int LARGURA_TELA = 800;
const int ALTURA_TELA = 600;
const int NUM_INIMIGOS = 50;
const int FPS = 60;

//------ VARIÁVEIS -------
int x = LARGURA_TELA / 2;
int y = ALTURA_TELA / 2;
int troca = 800;
int pontuacao;
int cor;
int i;
int contador = 12;
int cor_jog;
int vidas;
int nivel;
int cont = 0;
int tempo;
int pos = 0;
int v_pos[5];
bool menu = true;
bool sair = false;
bool jogar = false;
bool rank = false;
bool manual = false;
bool game_over = false;
bool congela = false;
bool toca_som = false;
bool concluido = false;
bool nome_ok = false;
bool som_ativo = true;
char nome[20];
FILE *fp;

//DECLARAÇÕES ALLEGRO
ALLEGRO_EVENT_QUEUE *fila_eventos = NULL;
ALLEGRO_TIMER *timer = NULL;
ALLEGRO_BITMAP *img_ranking = NULL, *bolinha_vermelha = NULL, *bolinha_azul = NULL, *bolinha_azul_menu = NULL, *bolinha_amarela = NULL, *bolinha_verde = NULL, *bolinha_verde_menu = NULL, *bolinha_certa = NULL, *bolinha_errada = NULL, *som_lig = NULL, *som_desl = NULL;
ALLEGRO_DISPLAY *janela = NULL;
ALLEGRO_FONT *fonte = NULL, *fonte_titulo = NULL, *fonte_info = NULL, *fonte_game_over = NULL, *fonte_temporizador = NULL;
ALLEGRO_SAMPLE *som_dragger = NULL, *som_inviso = NULL, *som_hyper = NULL, *som_expander = NULL, *som_certa = NULL, *som_errada = NULL, *som_click = NULL, *som_level = NULL, *som_menu = NULL, *som_jogo = NULL, *som_game_over = NULL;
ALLEGRO_SAMPLE_INSTANCE *inst_menu = NULL, *inst_jogo = NULL, *inst_game_over = NULL;

//STRUCTS
struct Inimigos
{
    int x;
    int y;
    int cor;
    int velocidade;
    int borda_x;
    int borda_y;
    bool ativo;
	bool errada;
	bool certa;
};
struct DadosJogador
{
    char nome_arq[20];
	int pontuacao_arq;
};

//PROTOTYPES FUNÇÕES
bool Inicializa();
bool CarregaArquivos();
void Finaliza();
void Reset(int *vidas, int *pontuacao, int *nivel, int *troca, int *cor, int *cor_jog, bool *toca_som);
void DesenhaMenu(bool som_ativo);
void DesenhaRank(DadosJogador inserir, DadosJogador ler[]);
void DesenhaManual();
void DesenhaAreaJogo(int vidas, int pontuacao, int nivel);
void EscolheCor(int cor, int x, int y, int cor_jog, bool toca_som, bool som_ativo);
void DesenhaGameOver(int pontuacao, bool concluido);
void EntradaTeclado(ALLEGRO_EVENT ev);
void InitInimigos(Inimigos inimigos[], int tamanho);
void LiberaInimigos(Inimigos inimigos[], int tamanho, bool congela, int nivel, int *v_pos, int *pos);
void AtualizaInimigos(Inimigos inimigos[], int tamanho, int cor, bool congela, int nivel);
void DesenhaInimigos(Inimigos inimigos[], int tamanho, int nivel, int *vidas, int *pontuacao);
void Colide(Inimigos inimigos[], int tamanho, int cor, int x, int y);

int main()
{
    struct Inimigos inimigos[NUM_INIMIGOS];
	struct DadosJogador inserir;
	struct DadosJogador ler[10];

	//INICIALIZA STRUCT DADOS JOGADOR
	for(i=0; i<10; i++)
    {
        strcpy(ler[i].nome_arq, " ");
        ler[i].pontuacao_arq = 0;

	}
    // ----- INICIALIZAÇÃO ------
	if (!Inicializa())
    {
        return false;
    }

	// -------CARREGAMENTOS-----
	if (!CarregaArquivos())
    {
        return false;
    }

    InitInimigos(inimigos, NUM_INIMIGOS);

    //----LOOP PRINCIPAL----
	strcpy(nome, "");
	srand(time(NULL));
	cor = 1 + rand() % 4;
	cor_jog = cor;
    while(!sair)
	{
        //------------EVENTOS E LÓGICA-------------
        ALLEGRO_EVENT ev;
        al_wait_for_event(fila_eventos, &ev);

		//FECHAR JANELA
        if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            sair = true;
        }

        else if(menu)
		{
            DesenhaMenu(som_ativo);
			if (som_ativo)
            {
                al_stop_sample_instance(inst_game_over);
                al_play_sample_instance(inst_menu);
            }

            else
            {
                al_stop_sample_instance(inst_game_over);
                al_stop_sample_instance(inst_menu);
            }

            //OPÇÕES DO MENU
            if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP)
            {
				//OPÇÃO JOGAR
                if (ev.mouse.button & 1 && ev.mouse.x >= 160 && ev.mouse.x <= 280 && ev.mouse.y >= 320 && ev.mouse.y <= 430)
                {
					if (som_ativo)
						al_play_sample(som_click, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                    jogar = true;
                    menu = false;
					vidas=10;
					pontuacao=0;
					nivel = 0;
                }
				//OPÇÃO RANKING
                else if (ev.mouse.button & 1 && ev.mouse.x >= 310 && ev.mouse.x <= 430 && ev.mouse.y >= 320 && ev.mouse.y <= 430)
                {
					if (som_ativo)
						al_play_sample(som_click, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                    rank = true;
                    menu = false;
                }
				//OPÇÃO MANUAL
                else if (ev.mouse.button & 1 && ev.mouse.x >= 480 && ev.mouse.x <= 600 && ev.mouse.y >= 320 && ev.mouse.y <= 430)
                {
					if (som_ativo)
						al_play_sample(som_click, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                    manual = true;
                    menu = false;
                }
				//BOTÃO SOM
				else if (ev.mouse.button & 1 && ev.mouse.x >= 50 && ev.mouse.x <= 140 && ev.mouse.y >= 500 && ev.mouse.y <= 574)
                {
					if (som_ativo)
						som_ativo = false;
					else
					{
						som_ativo = true;
						al_play_sample(som_click, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
					}
                }
            }
        }

        else if (rank)
        {
			DesenhaRank(inserir, ler);
            //VOLTAR
            if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP)
            {
                if (ev.mouse.button & 1 && ev.mouse.x >= 35 && ev.mouse.x <= 130 && ev.mouse.y >= 550 && ev.mouse.y <= 570)
                {
					if (som_ativo)
						al_play_sample(som_click, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                    rank = false;
                    menu = true;
                }
            }
        }

        else if (manual)
        {
			DesenhaManual();
			//VOLTAR
            if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP)
            {
                if (ev.mouse.button & 1 && ev.mouse.x >= 35 && ev.mouse.x <= 130 && ev.mouse.y >= 550 && ev.mouse.y <= 570)
                {
					if (som_ativo)
						al_play_sample(som_click, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                    manual = false;
                    menu = true;
                }
            }
        }
        else if(jogar)
        {
            if(som_ativo)
            {
                al_stop_sample_instance(inst_menu);
                al_stop_sample_instance(inst_game_over);
                al_play_sample_instance(inst_jogo);
            }

            al_hide_mouse_cursor(janela);
			//CONTROLA PARA QUE A BOLINHA DO JOGADOR NÃO SAIA DA TELA
            if(ev.type == ALLEGRO_EVENT_MOUSE_AXES || ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP && ev.mouse.button & 1)
            {
                x = ev.mouse.x;
                y = ev.mouse.y;
                if (cor_jog != 4)
                {
                    if(ev.mouse.x < 55)
                        x = 55;
                    else if(ev.mouse.x > 745)
                        x = 745;
                    if(ev.mouse.y < 207)
                        y = 207;
                    else if (ev.mouse.y > 555)
                        y = 555;
                }
                else
                {
                    if(ev.mouse.x < 85)
                        x = 85;
                    else if(ev.mouse.x > 715)
                        x = 715;
                    if(ev.mouse.y < 237)
                        y = 237;
                    else if (ev.mouse.y > 525)
                        y = 525;

                }
            }
			DesenhaAreaJogo(vidas, pontuacao, nivel);

			//CONTROLA TEMPO
            troca++; //variável contadora de frames
            if (troca == 25)
                toca_som = false;

            else if (troca == 800)
            {
                cor = 1 + rand() % 4;
				cont++;
				//nivel aumenta a cada 4 mudanças de cor
				if (cont==3)
				{
					nivel++;
					cont=0;
					if (som_ativo)
						al_play_sample(som_level, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
				}
            }
            else if (troca>800 && troca<1111)
			{
                congela = true;
                if (troca==1110)
                {
                    toca_som = true;
                    cor_jog = cor;
                    troca = 0;
                    congela = false;
                }
				else if (troca>800 && troca<900)
					tempo = 3;
				else if (troca>900 && troca<1000)
					tempo = 2;
				else if (troca>1000 && troca<1100)
					tempo = 1;

				al_draw_textf(fonte_temporizador, al_map_rgb(0, 0, 0), LARGURA_TELA/2, 250, ALLEGRO_ALIGN_CENTRE, "%d", tempo);
            }

            //EXECUÇÃO DO JOGO
            if(al_is_event_queue_empty(fila_eventos))
            {
				if (!game_over)
				{
	 				EscolheCor(cor, x, y, cor_jog, toca_som, som_ativo);
					Colide(inimigos, NUM_INIMIGOS, cor_jog, x, y);
		            LiberaInimigos(inimigos, NUM_INIMIGOS, congela, nivel, v_pos, &pos);
		            AtualizaInimigos(inimigos, NUM_INIMIGOS, cor, congela, nivel);
					DesenhaInimigos(inimigos, NUM_INIMIGOS, nivel, &vidas, &pontuacao);
		            al_flip_display();
		            al_clear_to_color(al_map_rgb (0, 0, 0));
					//indica o fim do jogo
					if (vidas<=0)
					{
						al_show_mouse_cursor(janela);
						game_over = true;
		                jogar = false;
						concluido = false;
						strcpy(nome, "");
					}
				}
            }
        }

		//QUANDO O JOGO TERMINAR
		if (game_over)
		{
		    if(som_ativo)
            {
                al_stop_sample_instance(inst_jogo);
                al_play_sample_instance(inst_game_over);
            }

			DesenhaGameOver(pontuacao, concluido);
			if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP)
			{
				//JOGAR NOVAMENTE
				if (ev.mouse.button & 1 && ev.mouse.x >= 150 && ev.mouse.x <= 450 && ev.mouse.y >= 390 && ev.mouse.y <= 440)
				{
					if (som_ativo)
						al_play_sample(som_click, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
					InitInimigos(inimigos, NUM_INIMIGOS);
					game_over = false;
					jogar = true;
					Reset(&vidas, &pontuacao, &nivel, &troca, &cor, &cor_jog, &toca_som);
				}
				//SAIR - volta para o menu
 				else if (ev.mouse.button & 1 && ev.mouse.x >= 520 && ev.mouse.x <= 600 && ev.mouse.y >= 390 && ev.mouse.y <= 440)
				{
					if (som_ativo)
						al_play_sample(som_click, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
				    //reseta para caso volte a jogar
                    InitInimigos(inimigos, NUM_INIMIGOS);
					game_over = false;
					menu = true;
					Reset(&vidas, &pontuacao, &nivel, &troca, &cor, &cor_jog, &toca_som);
				}
		    }
			//se escrita do nome ainda não está concluída
			if (!concluido)
            {
				EntradaTeclado(ev);
			    if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP)
				{
					if (som_ativo)
						al_play_sample(som_click, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
					//BOTÃO ENVIAR - conclui a escrita do nome
					if (ev.mouse.button & 1 && ev.mouse.x >= 600 && ev.mouse.x <= 700 && ev.mouse.y >= 290 && ev.mouse.y <= 340)
				    {
						concluido = true;
						inserir.pontuacao_arq = pontuacao;
						strcpy(inserir.nome_arq, nome);
						//INSERE DADOS NO ARQUIVO
						fp = fopen("ranking.txt", "a+");;
						fprintf(fp, "%s %d\n", inserir.nome_arq, inserir.pontuacao_arq);
						fclose(fp);
				    }
		        }
			}
			//exibe as letras na caixa de texto
			if (strlen(nome) > 0)
			{
				al_draw_text(fonte_game_over, al_map_rgb(0, 0, 0), 200, 300,  0, nome);
			}
		}
    }

    //------FINALIZAÇÃO DO PROGRAMA-----
	Finaliza();

    return 0;

}

bool Inicializa()
{
	if(!al_init())
    {
        al_show_native_message_box(NULL, "AVISO!", "ERRO!","ERRO AO INICIALIZAR A ALLEGRO!", NULL, ALLEGRO_MESSAGEBOX_ERROR);
        return false;
    }

    janela = al_create_display(LARGURA_TELA, ALTURA_TELA);
    if(!janela)
    {
        al_show_native_message_box(NULL, "AVISO!", "ERRO!","ERRO AO CRIAR O DISPLAY!", NULL, ALLEGRO_MESSAGEBOX_ERROR);
        return false;
    }
	if (!al_install_keyboard())
    {
		al_show_native_message_box(NULL, "AVISO!", "ERRO!","ERRO AO INICIALIZAR O TECLADO!", NULL, ALLEGRO_MESSAGEBOX_ERROR);
        return false;
    }
	if (!al_install_audio())
    {
        fprintf(stderr, "Falha ao inicializar áudio.\n");
        return false;
    }
	if (!al_init_acodec_addon())
    {
        fprintf(stderr, "Falha ao inicializar codecs de áudio.\n");
        return false;
    }
	if (!al_reserve_samples(1))
    {
        fprintf(stderr, "Falha ao alocar canais de áudio.\n");
        return false;
    }

    //---INSTALACAO DE ADDONS E DEMAIS DISPOSITIVOS------
	fila_eventos = al_create_event_queue();
	al_install_mouse();
	al_init_primitives_addon();
	al_init_font_addon();
	al_init_ttf_addon();
	al_init_image_addon();
	timer = al_create_timer(1.0 / FPS);
	al_start_timer(timer);


	//------REGISTRO DE SOURCES ------
	al_register_event_source(fila_eventos, al_get_display_event_source(janela));
	al_register_event_source(fila_eventos, al_get_mouse_event_source());
	al_register_event_source(fila_eventos, al_get_timer_event_source(timer));
	al_register_event_source(fila_eventos, al_get_keyboard_event_source());

	//TITULO DA JANELA
	al_set_window_title(janela, "Color Ball 2");

	return true;
}

bool CarregaArquivos()
{
	fonte = al_load_font("fontes/PoetsenOne.ttf", 30, 0);
	if (!fonte)
	{
		fprintf(stderr, "Falha ao carregar fonte.\n");
        return false;
	}

	fonte_titulo = al_load_font("fontes/PoetsenOne.ttf", 100, 0);
	if (!fonte_titulo)
	{
		fprintf(stderr, "Falha ao carregar fonte_titulo.\n");
        return false;
	}

	fonte_info = al_load_font("fontes/SERIF.ttf", 20, 0);
	if (!fonte_info)
	{
		fprintf(stderr, "Falha ao carregar fonte_info.\n");
        return false;
	}
	fonte_game_over = al_load_font("fontes/SERIF.ttf", 30, 0);
	if (!fonte_game_over)
	{
		fprintf(stderr, "Falha ao carregar fonte_game_over.\n");
        return false;
	}
	fonte_temporizador = al_load_font("fontes/PoetsenOne.ttf", 200, 0);
	if (!fonte)
	{
		fprintf(stderr, "Falha ao carregar fonte_tempo.\n");
        return false;
	}
	img_ranking = al_load_bitmap("imagens/im_rank.jpg");
	if (!img_ranking)
	{
		fprintf(stderr, "Falha ao carregar img_ranking.\n");
        return false;
	}
	bolinha_vermelha = al_load_bitmap("imagens/vermelho.png");
	if (!bolinha_vermelha)
	{
		fprintf(stderr, "Falha ao carregar img vermelho.\n");
        return false;
	}
	bolinha_azul = al_load_bitmap("imagens/azul.png");
	if (!bolinha_azul)
	{
		fprintf(stderr, "Falha ao carregar img azul.\n");
        return false;
	}
	bolinha_azul_menu = al_load_bitmap("imagens/azul_menu.png");
	if(!bolinha_azul_menu)
    {
        fprintf(stderr, "Falahar ao carregar img azul menu.\n");
        return false;
    }
	bolinha_amarela = al_load_bitmap("imagens/amarelo.png");
	if (!bolinha_amarela)
	{
		fprintf(stderr, "Falha ao carregar img amarelo.\n");
        return false;
	}
	bolinha_verde = al_load_bitmap("imagens/verde.png");
	if (!bolinha_verde)
	{
		fprintf(stderr, "Falha ao carregar img verde.\n");
        return false;
	}
	bolinha_verde_menu = al_load_bitmap("imagens/verde_menu.png");
	if (!bolinha_verde_menu)
    {
        fprintf(stderr, "Falha ao carregar img verde do menu.\n");
        return false;
    }
	bolinha_certa = al_load_bitmap("imagens/bolinha_certa.png");
	if (!bolinha_certa)
	{
		fprintf(stderr, "Falha ao carregar img bolinha certa.\n");
        return false;
	}
	bolinha_errada = al_load_bitmap("imagens/bolinha_errada.png");
	if (!bolinha_errada)
	{
		fprintf(stderr, "Falha ao carregar img bolinha errada.\n");
        return false;
	}
	som_lig = al_load_bitmap("imagens/som_lig.jpg");
	if (!som_lig)
	{
		fprintf(stderr, "Falha ao carregar img.\n");
        return false;
	}
	som_desl = al_load_bitmap("imagens/som_desl.jpg");
	if (!som_desl)
	{
		fprintf(stderr, "Falha ao carregar img.\n");
        return false;
	}

	som_dragger = al_load_sample("sons/dragger.wav");
	som_inviso = al_load_sample("sons/invisoball.wav");
	som_hyper = al_load_sample("sons/hyperball.wav");
	som_expander = al_load_sample("sons/expander.wav");
	som_certa = al_load_sample("sons/certa.wav");
	som_errada = al_load_sample("sons/errada.wav");
	if(!som_certa || !som_errada)
    {
        fprintf(stderr, "Falha ao carregar sample.\n");
        return false;
    }
    if (!som_dragger || !som_inviso || !som_hyper || !som_expander)
    {
        fprintf(stderr, "Falha ao carregar sample.\n");
        return false;
    }
	som_click = al_load_sample("sons/click.wav");
    if (!som_click)
    {
        fprintf(stderr, "Falha ao carregar sample.\n");
        return false;
    }
	som_level = al_load_sample("sons/levelup.wav");
    if (!som_level)
    {
        fprintf(stderr, "Falha ao carregar sample.\n");
        return false;
    }

    //som menu
    som_menu = al_load_sample("sons/menu.ogg");
    inst_menu= al_create_sample_instance(som_menu);
    al_attach_sample_instance_to_mixer(inst_menu, al_get_default_mixer());
    al_set_sample_instance_playmode(inst_menu, ALLEGRO_PLAYMODE_LOOP);
    al_set_sample_instance_gain(inst_menu, 0.8);
    //som jogo
    som_jogo = al_load_sample("sons/game.ogg");
    inst_jogo = al_create_sample_instance(som_jogo);
    al_attach_sample_instance_to_mixer(inst_jogo, al_get_default_mixer());
    al_set_sample_instance_playmode(inst_jogo, ALLEGRO_PLAYMODE_LOOP);
    al_set_sample_instance_gain(inst_jogo, 0.8);
    //som game over
    som_game_over = al_load_sample("sons/game_over.wav");
    inst_game_over = al_create_sample_instance(som_game_over);
    al_attach_sample_instance_to_mixer(inst_game_over, al_get_default_mixer());
    al_set_sample_instance_playmode(inst_game_over, ALLEGRO_PLAYMODE_LOOP);
    al_set_sample_instance_gain(inst_game_over, 0.8);



	return true;
}

void Reset(int *vidas, int *pontuacao, int *nivel, int *troca, int *cor, int *cor_jog, bool *toca_som)
{
	*vidas = 10;
	*pontuacao = 0;
	*nivel = 0;
	*troca = 800;
	*cor = 1 + rand() % 4;
	*cor_jog = *cor;
    *toca_som = false;
}

void DesenhaMenu(bool som_ativo)
{
	al_flip_display();
    al_clear_to_color(al_map_rgb (255, 255, 255));
	al_draw_text(fonte_titulo, al_map_rgb(255, 0, 0), 130, 60, 0, "Color Ball 2");
	al_draw_filled_circle(220, 370, 60, al_map_rgb(0, 255, 0));
	al_draw_text(fonte, al_map_rgb(0, 255, 0), 180, 430, 0, "Jogar");
	al_draw_filled_circle(370, 370, 60, al_map_rgb(255, 0, 0));
	al_draw_text(fonte, al_map_rgb(255, 0, 0), 330, 430, 0, "Rank");
	al_draw_filled_circle(540, 370, 60, al_map_rgb(0, 0, 255));
	al_draw_text(fonte, al_map_rgb(0, 0, 255), 480, 430, 0, "Manual");
	if (som_ativo)
		al_draw_bitmap(som_lig, 50, 500, 0);
	else
		al_draw_bitmap(som_desl, 50, 500, 0);
}

void DesenhaRank(DadosJogador inserir, DadosJogador ler[])
{
	int m, n, aux, r, g, b;
	char aux_nome[20];
	al_flip_display();
    al_clear_to_color(al_map_rgb (255, 255, 252));
    al_draw_text(fonte_titulo, al_map_rgb(255, 0, 0), 140, 40, 0, "Ranking");
    al_draw_bitmap(img_ranking, 0, 180, 0);
    al_draw_textf(fonte_info, al_map_rgb(0, 0, 0), 400, 200, 0, " NOME             PONTUACAO");

	//LÊ DADOS DO ARQUIVO
	m=0;
	fp = fopen("ranking.txt", "r+");
	while (fscanf(fp, "%s %d\n", inserir.nome_arq, &inserir.pontuacao_arq) != EOF)
	{
		strcpy(ler[m].nome_arq, inserir.nome_arq);
		ler[m].pontuacao_arq = inserir.pontuacao_arq;
		m++;
	}

	//ORDENA
	for (r=0; r<m-1; r++)
    {
		for (n=r+1; n<m; n++)
		{
			if (ler[r].pontuacao_arq < ler[n].pontuacao_arq)
			{
				//ordena pontuação
				aux = ler[r].pontuacao_arq;
				ler[r].pontuacao_arq = ler[n].pontuacao_arq;
				ler[n].pontuacao_arq = aux;
				//ordena nomes
				strcpy(aux_nome, ler[r].nome_arq);
				strcpy(ler[r].nome_arq, ler[n].nome_arq);
				strcpy(ler[n].nome_arq, aux_nome);
			}
		}
    }
	//IMPRESSÃO - imprime somente os 10 primeiros
	for (m=0; m<10; m++)
	{
		if (m%2==0 && m%3!=0)
		{
			r = 255;
			g = 0;
			b = 0;
		}
		else if (m%2!=0  && m%3!=0)
		{
			r = 255;
			g = 255;
			b = 0;
		}
		else if (m%3==0)
		{
			r = 0;
			g = 255;
			b = 0;
		}

		al_draw_textf(fonte_info, al_map_rgb(r, g, b), 340, 250+(m*30), 0, "%dst", m+1);
		al_draw_textf(fonte_info, al_map_rgb(r, g, b), 410, 250+(m*30), 0, "%s", ler[m].nome_arq);
		al_draw_textf(fonte_info, al_map_rgb(r, g, b), 600, 250+(m*30), 0, "%d", ler[m].pontuacao_arq);
	}
    al_draw_textf(fonte_info, al_map_rgb(0, 0, 0), 60, 550, 0, "VOLTAR");
    al_draw_filled_circle(50, 560, 10, al_map_rgb(255, 0, 0));
}

void DesenhaManual()
{
	al_flip_display();
    al_clear_to_color(al_map_rgb (255, 255, 252));
    al_draw_text(fonte_titulo, al_map_rgb(255, 0, 0), 140, 40, 0, "Color Ball 2");
	al_draw_text(fonte_info, al_map_rgb(0, 0, 0), 40, 200, 0, "  Color Ball eh um jogo muito simples. Basta pegar apenas as bolinhas");
    al_draw_text(fonte_info, al_map_rgb(0, 0, 0), 35, 220, 0, "que possuirem a mesma cor que a sua, nao deixando elas escaparem");
    al_draw_text(fonte_info, al_map_rgb(0, 0, 0), 35, 240, 0, "da tela.");

    al_draw_text(fonte_info, al_map_rgb(0, 0, 0), 40, 270, 0, "  Esteja preparado para uma mudanca de cor, pois a cada mudanca");
    al_draw_text(fonte_info, al_map_rgb(0, 0, 0), 35, 290, 0, "as bolinhas ganharao novos efeitos.  O menu no topo diz a cor de sua");
    al_draw_text(fonte_info, al_map_rgb(0, 0, 0), 35, 310, 0, "bolinha e depois aponta para sua nova cor.");

    al_draw_text(fonte_info, al_map_rgb(0, 0, 0), 35, 340, 0, "  Passe de nivel e cuidado para nao perder todas as suas vidas!");

    al_draw_text(fonte_info, al_map_rgb(255, 0, 0), 75, 430, 0, "Hyper Ball");
    al_draw_bitmap(bolinha_vermelha, 90, 370, 0);

    al_draw_text(fonte_info, al_map_rgb(255, 255, 0), 265, 430, 0, "Dragger");
    al_draw_bitmap(bolinha_amarela, 270, 370, 0);

    al_draw_text(fonte_info, al_map_rgb(0, 0, 255), 435, 430, 0, "Inviso Ball");
    al_draw_bitmap(bolinha_azul_menu, 450, 370, 0);

    al_draw_text(fonte_info, al_map_rgb(138, 193, 66), 615, 430, 0, "Expander");
    al_draw_bitmap(bolinha_verde_menu, 630, 370, 0);


    al_draw_text(fonte_info, al_map_rgb(0, 0, 0), 400, 480, ALLEGRO_ALIGN_CENTRE, "  Depois voce pode enviar sua pontuacao.  Boa sorte =) ");
    al_draw_textf(fonte_info, al_map_rgb(0, 0, 0), 60, 550, 0, "VOLTAR");
    al_draw_filled_circle(50, 560, 10, al_map_rgb(255, 0, 0));
}

void DesenhaAreaJogo(int vidas, int pontuacao, int nivel)
{
	//bolinhas apagadas
    al_draw_filled_circle(120, 70, 50, al_map_rgb(255, 190, 190));
    al_draw_filled_circle(300, 70, 50, al_map_rgb(255, 255, 200));
	al_draw_filled_circle(480, 70, 50, al_map_rgb(170, 170, 255));
    al_draw_filled_circle(660, 70, 50, al_map_rgb(200, 255, 200));
    //Retangulo de cima
    al_draw_filled_rounded_rectangle(29, 135, 771, 180, 15, 15, al_map_rgb(104, 34, 139));
    al_draw_textf(fonte_info, al_map_rgb(0, 0, 0), 50, 150, 0, "Vidas: %d", vidas);
    al_draw_textf(fonte_info, al_map_rgb(0, 0, 0), 250, 150, 0, "Pontuacao: %d", pontuacao);
    al_draw_textf(fonte_info, al_map_rgb(0, 0, 0), 500, 150, 0, "Nivel: %d", nivel);
    //Retangulo de baixo
	al_draw_rounded_rectangle(30, 180, 770, 580, 15, 15, al_map_rgb(104, 34, 139), 3);
    al_draw_filled_rounded_rectangle(32, 182, 768, 578, 15, 15, al_map_rgb(255, 255, 255));

}

void EscolheCor(int cor, int x, int y, int cor_jog, bool toca_som, bool som_ativo)
{
	if (cor==1) // acende vermelho
    {
    	al_draw_filled_triangle(30, 55, 30, 85, 50, 70, al_map_rgb (255, 255, 255));
        al_draw_filled_circle(120, 70, 50, al_map_rgb(255, 0, 0));
	}
    else if (cor==2) //acende amarelo
    {
        al_draw_filled_triangle(210, 55, 210, 85, 230 , 70, al_map_rgb (255, 255, 255));
        al_draw_filled_circle(300, 70, 50, al_map_rgb(255, 255, 0));
    }
    else if (cor==3) //acende azul
    {
        al_draw_filled_triangle(390, 55, 390, 85, 410 , 70, al_map_rgb (255, 255, 255));
        al_draw_filled_circle(480, 70, 50, al_map_rgb(0, 0, 255));

    }
    else if (cor==4) //acende verde
    {
        al_draw_filled_triangle(570, 55, 570, 85, 590 , 70, al_map_rgb (255, 255, 255));
        al_draw_filled_circle(660, 70, 50, al_map_rgb(138, 193, 66));
    }

    //CORRES JOGADOR
    if(cor_jog==1) //vermelho
    {
        al_draw_bitmap(bolinha_vermelha, x - 30, y - 30, 0);
        if(toca_som && som_ativo)
            al_play_sample(som_hyper, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
    }

    else if(cor_jog==2) //amarelo
    {
		al_draw_bitmap(bolinha_amarela, x - 30, y - 30, 0);
        if(toca_som && som_ativo)
            al_play_sample(som_dragger, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
    }

    else if(cor_jog==3) //azul
    {
		al_draw_bitmap(bolinha_azul, x - 30, y - 30, 0);
        if(toca_som && som_ativo)
            al_play_sample(som_inviso, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
    }

    else if(cor_jog==4) //verde
    {
		al_draw_bitmap(bolinha_verde, x - 60, y - 60, 0);
        if(toca_som && som_ativo)
            al_play_sample(som_expander, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
    }

}

void DesenhaGameOver(int pontuacao, bool concluido)
{
    al_show_mouse_cursor(janela);
	al_flip_display();
    al_clear_to_color(al_map_rgb (255, 255, 255));
	al_draw_filled_rounded_rectangle(50, 100, 750, 500, 15, 15, al_map_rgb(104, 34, 139));
	al_draw_textf(fonte_game_over, al_map_rgb(255, 255, 255), 280, 130, 0, "GAME OVER");
	al_draw_textf(fonte_game_over, al_map_rgb(255, 255, 255), 200, 200, 0, "Sua pontuacao foi: %d", pontuacao);
	al_draw_textf(fonte_game_over, al_map_rgb(255, 255, 255), 260, 250, 0, "Digite seu nome");
	al_draw_filled_rectangle(180, 290, 580, 340, al_map_rgb(255, 255, 255));
	if (!concluido)
	{
		al_draw_filled_rounded_rectangle(600, 290, 730, 340, 10, 10, al_map_rgb(0, 0, 0));
		al_draw_textf(fonte_game_over, al_map_rgb(255, 255, 255), 620, 300, 0, "Enviar");
	}
	al_draw_filled_rounded_rectangle(150, 390, 450, 440, 10, 10, al_map_rgb(0, 0, 0));
	al_draw_textf(fonte_game_over, al_map_rgb(255, 255, 255), 170, 400, 0, "Jogar novamente");
	al_draw_filled_rounded_rectangle(520, 390, 600, 440, 10, 10, al_map_rgb(0, 0, 0));
	al_draw_textf(fonte_game_over, al_map_rgb(255, 255, 255), 540, 400, 0, "Sair");

}

void EntradaTeclado(ALLEGRO_EVENT ev)
{
	if (ev.type == ALLEGRO_EVENT_KEY_CHAR)
	{
		if (strlen(nome) <= 20)
		{
			char temporaria[] = {ev.keyboard.unichar, '\0'};
			if (ev.keyboard.unichar == ' ')
			{
			    strcat(nome, temporaria);
			}
			else if (ev.keyboard.unichar >= 'A' && ev.keyboard.unichar <= 'Z')
			{
			    strcat(nome, temporaria);
			}
			else if (ev.keyboard.unichar >= 'a' && ev.keyboard.unichar <= 'z')
			{
			    strcat(nome, temporaria);
			}
			else if (ev.keyboard.unichar >= '0' && ev.keyboard.unichar <= '9')
			{
			    strcat(nome, temporaria);
			}
		}
		if (ev.keyboard.keycode == ALLEGRO_KEY_BACKSPACE && strlen(nome) != 0)
		{
			nome[strlen(nome) - 1] = '\0';
		}
	}

}

void Finaliza()
{
 	al_destroy_font(fonte);
 	al_destroy_font(fonte_info);
    al_destroy_font(fonte_titulo);
	al_destroy_font(fonte_temporizador);
	al_destroy_timer(timer);
	al_destroy_sample(som_dragger);
	al_destroy_sample(som_inviso);
	al_destroy_sample(som_hyper);
	al_destroy_sample(som_expander);
	al_destroy_sample(som_certa);
	al_destroy_sample(som_errada);
	al_destroy_sample(som_click);
	al_destroy_sample(som_menu);
	al_destroy_sample_instance(inst_menu);
	al_destroy_sample(som_jogo);
	al_destroy_sample_instance(inst_jogo);
	al_destroy_sample(som_game_over);
	al_destroy_sample_instance(inst_game_over);
    al_destroy_event_queue(fila_eventos);
    al_destroy_display(janela);
}

//INIMIGOS
void InitInimigos(Inimigos inimigos[], int tamanho)
{
    for(int i = 0; i < tamanho; i++)
    {
	    inimigos[i].x = 50;
	    inimigos[i].cor = 1 + rand() % 4;
	    inimigos[i].velocidade = 3;
	    inimigos[i].y = 100;
	    inimigos[i].ativo = false;
		inimigos[i].certa = false;
		inimigos[i].errada = false;
	}
}
void LiberaInimigos(Inimigos inimigos[], int tamanho, bool congela, int nivel, int *v_pos, int *pos)
{
    int p, k;

    if (*pos == 4)
        *pos = 0;

     k =  *pos;

    for(int i = 0; i < 7 + (nivel*2) && !congela; i++)
    {
        if(!inimigos[i].ativo)
        {
            if(rand() % 100 == 0)
            {
                inimigos[i].x = 50;
                inimigos[i].y = 210 + rand() % 330;

                //Verificacao para as bolinhas nao sairem sobrepostas
                v_pos[k] = inimigos[i].y;
                *pos = *pos + 1;
                for(p=0; p <= k; p++)
                {
                    while(inimigos[i].y > v_pos[p] - 35 && inimigos[i].y < v_pos[p] + 35)
                    {
                        inimigos[i].y = 210 + rand() % 330;

                    }
                }

                inimigos[i].ativo =  true;
                break;
            }
        }
    }
}
void AtualizaInimigos(Inimigos inimigos[], int tamanho, int cor, bool congela, int nivel)
{
    for(int i = 0; i < 7 + (nivel*2) && !congela; i++)
    {
		if(inimigos[i].ativo)
		{
        	inimigos[i].x += inimigos[i].velocidade + nivel; // quanto maior o nivel maior a velocidade
			if (cor==1)
				inimigos[i].x += inimigos[i].velocidade + nivel; //velocidade maior quando o jogador é vermelho
		}
    }
}
void DesenhaInimigos(Inimigos inimigos[], int tamanho, int nivel, int *vidas, int *pontuacao)
{
    for(int i = 0; i < 7 + (nivel*2); i++)
	{
        if (inimigos[i].ativo)
        {
			if(inimigos[i].cor == 1)
			{
				if (!inimigos[i].certa && !inimigos[i].errada)
			    	al_draw_filled_circle(inimigos[i].x, inimigos[i].y, 28 + (nivel*3), al_map_rgb(255, 0, 0)); // vermelho
				else if(inimigos[i].certa)
				{
					if(contador>=0)
					{
						al_draw_bitmap(bolinha_certa, inimigos[i].x-30, inimigos[i].y-30, 0);
						contador--;
						if (contador==0)
						{
							inimigos[i].certa = false;
							inimigos[i].ativo = false;
							contador = 12;
							*pontuacao=*pontuacao+100;
						}
						//efeito sonoro
						if (contador==12 && som_ativo)
						{
							al_play_sample(som_certa, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
						}
					}
				}
				else if(inimigos[i].errada)
				{
					if(contador>=0)
					{
						al_draw_bitmap(bolinha_errada, inimigos[i].x-30, inimigos[i].y-30, 0);
						contador--;
						if (contador==0)
						{
							inimigos[i].errada = false;
							inimigos[i].ativo = false;
							contador = 12;
							*vidas=*vidas-1;
						}
						//efeito sonoro
						if (contador==12 && som_ativo)
						{
							al_play_sample(som_errada, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
						}
					}
				}
			}
			else if(inimigos[i].cor == 2)
			{
				if (!inimigos[i].certa && !inimigos[i].errada)
			    	al_draw_filled_circle(inimigos[i].x, inimigos[i].y, 28 + (nivel*3), al_map_rgb(255, 255, 0)); //amarelo
				else if(inimigos[i].certa)
				{
					if(contador>=0)
					{
						al_draw_bitmap(bolinha_certa, inimigos[i].x-30, inimigos[i].y-30, 0);
						contador--;
						if (contador==0)
						{
							inimigos[i].certa = false;
							inimigos[i].ativo = false;
							contador = 12;
							*pontuacao=*pontuacao+100;
						}
						//efeito sonoro
						if (contador==12 && som_ativo)
						{
							al_play_sample(som_certa, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
						}
					}
				}
				else if(inimigos[i].errada)
				{
					if(contador>=0)
					{
						al_draw_bitmap(bolinha_errada, inimigos[i].x-30, inimigos[i].y-30, 0);
						contador--;
						if (contador==0)
						{
							inimigos[i].errada = false;
							inimigos[i].ativo = false;
							contador = 12;
							*vidas=*vidas-1;
						}
						//efeito sonoro
						if (contador==12 && som_ativo)
						{
							al_play_sample(som_errada, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
						}
					}
				}
			}
			else if(inimigos[i].cor == 3)
			{
				if (!inimigos[i].certa && !inimigos[i].errada)
			    	al_draw_filled_circle(inimigos[i].x, inimigos[i].y, 28 + (nivel*3), al_map_rgb(0, 0, 255)); //azul
				else if(inimigos[i].certa)
				{
					if(contador>=0)
					{
						al_draw_bitmap(bolinha_certa, inimigos[i].x-30, inimigos[i].y-30, 0);
						contador--;
						if (contador==0)
						{
							inimigos[i].certa = false;
							inimigos[i].ativo = false;
							contador = 12;
							*pontuacao=*pontuacao+100;
						}
						//efeito sonoro
						if (contador==12 && som_ativo)
						{
							al_play_sample(som_certa, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
						}
					}
				}
				else if(inimigos[i].errada)
				{
					if(contador>=0)
					{
						al_draw_bitmap(bolinha_errada, inimigos[i].x-30, inimigos[i].y-30, 0);
						contador--;
						if (contador==0)
						{
							inimigos[i].errada = false;
							inimigos[i].ativo = false;
							contador = 12;
							*vidas=*vidas-1;
						}
						//efeito sonoro
						if (contador==12 && som_ativo)
						{
							al_play_sample(som_errada, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
						}
					}
				}
			}
			else if(inimigos[i].cor == 4)
			{
				if (!inimigos[i].certa && !inimigos[i].errada)
			    	 al_draw_filled_circle(inimigos[i].x, inimigos[i].y, 28 + (nivel*3), al_map_rgb(138, 193, 66)); //verde
				else if(inimigos[i].certa)
				{
					if(contador>=0)
					{
						al_draw_bitmap(bolinha_certa, inimigos[i].x-30, inimigos[i].y-30, 0);
						contador--;
						if (contador==0)
						{
							inimigos[i].certa = false;
							inimigos[i].ativo = false;
							contador = 12;
							*pontuacao=*pontuacao+100;
						}
						//efeito sonoro
						if (contador==12 && som_ativo)
						{
							al_play_sample(som_certa, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
						}
					}
				}
				else if(inimigos[i].errada)
				{
					if(contador>=0)
					{
						al_draw_bitmap(bolinha_errada, inimigos[i].x-30, inimigos[i].y-30, 0);
						contador--;
						if (contador==0)
						{
							inimigos[i].errada = false;
							inimigos[i].ativo = false;
							contador = 12;
							*vidas=*vidas-1;
						}
						//efeito sonoro
						if (contador==12 && som_ativo)
						{
							al_play_sample(som_errada, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
						}
					}
				}
			}
        }
    }
}

void Colide(Inimigos inimigos[], int tamanho, int cor_jog, int x, int y)
{
    for(int i = 0; i < 7 + (nivel*2); i++)
	{
		if (inimigos[i].ativo)
		{
            if((inimigos[i].x > x-28 && inimigos[i].x < x+28 && inimigos[i].y > y-28 && inimigos[i].y < y+28 && cor_jog != 4)||(cor_jog==4 && inimigos[i].x > x-60 && inimigos[i].x < x+60 && inimigos[i].y > y-60 && inimigos[i].y < y+60)  )
            {
				if (cor_jog != inimigos[i].cor)
					inimigos[i].errada = true;
				else if (cor_jog == inimigos[i].cor)
					inimigos[i].certa = true;
            }

			else if(inimigos[i].x > 730)
			{
				if (cor_jog == inimigos[i].cor)
                {
                    inimigos[i].errada = true;
				}
				else
					inimigos[i].ativo = false;
			}
		}
    }
}
