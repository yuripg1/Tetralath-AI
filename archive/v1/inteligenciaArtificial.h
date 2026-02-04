#ifndef INTELIGENCIA_ARTIFICIAL_H
#define INTELIGENCIA_ARTIFICIAL_H
#include <time.h>
#include <windows.h>
struct RESULTADO{
	int numeroNiveis;
	clock_t tempo;
	int melhorJogada;
};
struct MINIMAX{
	clock_t tempoLimite;
	int jogadasFeitas;
	int profundidade;
	int *tabuleiro;
	int corComputador;
	struct RESULTADO *resultado;
};
int jogadaComputador(int *tabuleiro,int profundidade,int jogadasFeitas,int corComputador);
DWORD WINAPI thread1(LPVOID lpParam);
DWORD WINAPI thread2(LPVOID lpParam);
int primeiroMax(clock_t tempoLimite,int jogadasFeitas,int profundidade,int *tabuleiro,int corComputador);
int nivelMin(clock_t tempoLimite,int *tabuleiro,int corAtual,int jogadasFeitas,int profundidade,int alfa,int beta);
int nivelMax(clock_t tempoLimite,int *tabuleiro,int corAtual,int jogadasFeitas,int profundidade,int alfa,int beta);
int primeiroVazioCentro(int *tabuleiro);
int formaSequencia(int *tabuleiro,int posicao,int adicaoIndice);
int resultadoJogo(int *tabuleiro,int corJogador,int jogadasFeitas);
#endif
