/* 
PROGETTO DI RETI INFORMATICHE - ANNO 2017/18 - MATTEO ZINI 533197
###################   lib_server/utente.h   #####################
*/

#ifndef UTENTE
#define UTENTE

#include "../lib_condivisa/costanti.h"
#include "../lib_condivisa/strutture_dati.h"

typedef struct Utente
{
	char nome[MAX_LUNGHEZZA_NOME_UTENTE+1];
	int sock;
	struct Messaggio* messaggi_pendenti;
	struct Indirizzo* indirizzo;
	struct Utente* pun;
} Utente;

void inserisci_fondo_lista_utenti(Utente** lista, Utente* c);
Utente* cerca_sock_lista_utenti(Utente* lista, int sock);
Utente* cerca_nome_lista_utenti(Utente* lista, char* nome);
int elimina_lista_utenti(Utente** lista, int sock);

void log_out(Utente* u);
int stato_utente(Utente* u);
void inserisci_messaggio_utente(Utente* u, char* username, char* messaggio);
int estrai_messaggio_utente(Utente* u, char* username, char** messaggio);

#endif
