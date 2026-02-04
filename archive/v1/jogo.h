#ifndef JOGO_H
#define JOGO_H
struct JOGO{
	int *tabuleiro;
	int ultimaJogada;
	int penultimaJogada;
	int antepenultimaJogada;
	int corAtual;
	int corJogador;
	int profundidadeMinimax;
	int jogadasFeitas;
	int podeDesfazer;
};
void jogo();
struct JOGO *inicializaJogo();
int escolheCorJogador(struct JOGO *jogo);
int profundidadeMinimax(struct JOGO *jogo);
void escreveComandos();
void jogadorRodada(int corAtual);
struct JOGO *finalizaJogo(struct JOGO *jogo);
void processaJogada(struct JOGO *jogo,int jogada);
void desfazJogadas(struct JOGO *jogo);
void desfazUmaJogada(struct JOGO *jogo);
int jogadaValida(int *tabuleiro,int jogada);
int primeiroVazio(int *tabuleiro);
#endif
