#ifndef DESENHO_TABULEIRO_H
#define DESENHO_TABULEIRO_H
#define BRANCO 1
#define PRETO 2
#define SEM_JOGADA (-1)
void limpaTela();
void desenhaTabuleiro();
void posicaoDesenho(int jogada,int *posicaoX,int *posicaoY);
void desenhaJogada(int jogadaAtual,int corAtual,int jogadaAnterior);
void desenhaDesfazer(int ultimaJogada,int penultimaJogada,int antepenultimaJogada);
void desenhaMeioDesfazer(int ultimaJogada,int penultimaJogada);
void poeFoco(int posicao);
void tiraFoco(int posicao);
#endif
