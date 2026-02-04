#include <windows.h>
#include "constantes.h"
#include "jogo.h"
int main(){
#ifdef ALTA_PRIORIDADE
	SetPriorityClass(GetCurrentProcess(),HIGH_PRIORITY_CLASS);
#else
	SetPriorityClass(GetCurrentProcess(),IDLE_PRIORITY_CLASS);
#endif
	jogo();
	return 0;
}
