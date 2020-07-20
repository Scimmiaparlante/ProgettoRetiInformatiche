/* 
PROGETTO DI RETI INFORMATICHE - ANNO 2017/18 - MATTEO ZINI 533197
###################  lib_client/comandi.h   #####################
*/

#define NUM_COMANDI 6
#define MAX_LUNGHEZZA_NOME_COMANDI 15 
#define COMANDO_HELP 0

void prompt();
void interprete_comandi();

void comando_help();
void comando_register();
void comando_deregister();
void comando_who();
void comando_send();
void comando_quit();


typedef struct Comando {
	char nome[MAX_LUNGHEZZA_NOME_COMANDI+1];
	void (*gestore)();
} Comando;
