#include <conio2.h>
#include "constantes.h"
#include "desenhoTabuleiro.h"
void limpaTela(){
	int y;
	clrscr();
	textbackground(DARKGRAY);
	textcolor(BLACK);
	for(y=1;y<26;y++){
		cputsxy(1,y,"                                                                                ");
	}
	gotoxy(1,1);
}
void desenhaTabuleiro(){
	textbackground(YELLOW);
	textcolor(BLACK);
	cputsxy(10,2,"---------------------");
	cputsxy(10,3,"|   |   |   |   |   |");
	cputsxy(8,4,"-------------------------");
	cputsxy(8,5,"|   |   |   |   |   |   |");
	cputsxy(6,6,"-----------------------------");
	cputsxy(6,7,"|   |   |   |   |   |   |   |");
	cputsxy(4,8,"---------------------------------");
	cputsxy(4,9,"|   |   |   |   |   |   |   |   |");
	cputsxy(2,10,"-------------------------------------");
	cputsxy(2,11,"|   |   |   |   |   |   |   |   |   |");
	cputsxy(2,12,"-------------------------------------");
	cputsxy(4,13,"|   |   |   |   |   |   |   |   |");
	cputsxy(4,14,"---------------------------------");
	cputsxy(6,15,"|   |   |   |   |   |   |   |");
	cputsxy(6,16,"-----------------------------");
	cputsxy(8,17,"|   |   |   |   |   |   |");
	cputsxy(8,18,"-------------------------");
	cputsxy(10,19,"|   |   |   |   |   |");
	cputsxy(10,20,"---------------------");
	gotoxy(1,1);
}
void posicaoDesenho(int jogada,int *posicaoX,int *posicaoY){
	if(jogada<5){
		(*posicaoX)=10+(4*jogada);
		(*posicaoY)=2;
	}
	if((jogada>4)&&(jogada<11)){
		(*posicaoX)=8+(4*(jogada-5));
		(*posicaoY)=4;
	}
	if((jogada>10)&&(jogada<18)){
		(*posicaoX)=6+(4*(jogada-11));
		(*posicaoY)=6;
	}
	if((jogada>17)&&(jogada<26)){
		(*posicaoX)=4+(4*(jogada-18));
		(*posicaoY)=8;
	}
	if((jogada>25)&&(jogada<35)){
		(*posicaoX)=2+(4*(jogada-26));
		(*posicaoY)=10;
	}
	if((jogada>34)&&(jogada<43)){
		(*posicaoX)=4+(4*(jogada-35));
		(*posicaoY)=12;
	}
	if((jogada>42)&&(jogada<50)){
		(*posicaoX)=6+(4*(jogada-43));
		(*posicaoY)=14;
	}
	if((jogada>49)&&(jogada<56)){
		(*posicaoX)=8+(4*(jogada-50));
		(*posicaoY)=16;
	}
	if(jogada>55){
		(*posicaoX)=10+(4*(jogada-56));
		(*posicaoY)=18;
	}
}
void desenhaJogada(int jogadaAtual,int corAtual,int jogadaAnterior){
	int posicaoX=0,posicaoY=0;
	textbackground(YELLOW);
	textcolor(BLACK);
	if(jogadaAnterior!=SEM_JOGADA){
		posicaoDesenho(jogadaAnterior,&posicaoX,&posicaoY);
		cputsxy(posicaoX,posicaoY,"-----");
		posicaoY++;
		cputsxy(posicaoX,posicaoY,"|");
		cputsxy(posicaoX+4,posicaoY,"|");
		posicaoY++;
		cputsxy(posicaoX,posicaoY,"-----");
	}
	posicaoDesenho(jogadaAtual,&posicaoX,&posicaoY);
	textbackground(BROWN);
	cputsxy(posicaoX,posicaoY,"-----");
	cputsxy(posicaoX,posicaoY+1,"|   |");
	cputsxy(posicaoX,posicaoY+2,"-----");
	if(corAtual==BRANCO){
		textbackground(WHITE);
	}
	if(corAtual==PRETO){
		textbackground(BLACK);
	}
	cputsxy(posicaoX+1,posicaoY+1,"   ");
	gotoxy(1,1);
}
void desenhaDesfazer(int ultimaJogada,int penultimaJogada,int antepenultimaJogada){
	int posicaoX=0,posicaoY=0;
	textbackground(YELLOW);
	textcolor(BLACK);
	posicaoDesenho(ultimaJogada,&posicaoX,&posicaoY);
	cputsxy(posicaoX,posicaoY,"-----");
	posicaoY++;
	cputsxy(posicaoX,posicaoY,"|   |");
	posicaoY++;
	cputsxy(posicaoX,posicaoY,"-----");
	posicaoDesenho(penultimaJogada,&posicaoX,&posicaoY);
	cputsxy(posicaoX+1,posicaoY+1,"   ");
	if(antepenultimaJogada!=SEM_JOGADA){
		posicaoDesenho(antepenultimaJogada,&posicaoX,&posicaoY);
		textbackground(BROWN);
		cputsxy(posicaoX,posicaoY,"-----");
		posicaoY++;
		cputsxy(posicaoX,posicaoY,"|");
		cputsxy(posicaoX+4,posicaoY,"|");
		posicaoY++;
		cputsxy(posicaoX,posicaoY,"-----");
	}
	gotoxy(1,1);
}
void desenhaMeioDesfazer(int ultimaJogada,int penultimaJogada){
	int posicaoX=0,posicaoY=0;
	textbackground(YELLOW);
	textcolor(BLACK);
	posicaoDesenho(ultimaJogada,&posicaoX,&posicaoY);
	cputsxy(posicaoX,posicaoY,"-----");
	posicaoY++;
	cputsxy(posicaoX,posicaoY,"|   |");
	posicaoY++;
	cputsxy(posicaoX,posicaoY,"-----");
	posicaoDesenho(penultimaJogada,&posicaoX,&posicaoY);
	textbackground(BROWN);
	cputsxy(posicaoX,posicaoY,"-----");
	posicaoY++;
	cputsxy(posicaoX,posicaoY,"|");
	cputsxy(posicaoX+4,posicaoY,"|");
	posicaoY++;
	cputsxy(posicaoX,posicaoY,"-----");
	gotoxy(1,1);
}
void poeFoco(int posicao){
	int posicaoX=0,posicaoY=0;
	textbackground(LIGHTBLUE);
	textcolor(BLACK);
	posicaoDesenho(posicao,&posicaoX,&posicaoY);
	cputsxy(posicaoX+1,posicaoY+1,"   ");
	gotoxy(1,1);
}
void tiraFoco(int posicao){
	int posicaoX=0,posicaoY=0;
	textbackground(YELLOW);
	textcolor(BLACK);
	posicaoDesenho(posicao,&posicaoX,&posicaoY);
	cputsxy(posicaoX+1,posicaoY+1,"   ");
	gotoxy(1,1);
}
