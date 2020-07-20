/* 
PROGETTO DI RETI INFORMATICHE - ANNO 2017/18 - MATTEO ZINI 533197
#####################   strutture_dati.h   ######################
*/

#ifndef STRUTTURE_DATI
#define STRUTTURE_DATI

#include <netinet/in.h>
#include <stdint.h>

#include "costanti.h"

typedef struct Indirizzo
{
	char ip[INET_ADDRSTRLEN];
	uint16_t porta;
} Indirizzo;


typedef struct Messaggio
{
	char mittente[MAX_LUNGHEZZA_NOME_UTENTE+1];
	char* corpo;
	struct Messaggio* pun;
} Messaggio;


#endif
