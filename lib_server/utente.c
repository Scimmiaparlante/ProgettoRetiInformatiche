/* 
PROGETTO DI RETI INFORMATICHE - ANNO 2017/18 - MATTEO ZINI 533197
###################   lib_server/utente.c   #####################
*/

#include <stdlib.h>
#include <string.h>
#include "utente.h"


void inserisci_fondo_lista_utenti(Utente** lista, Utente* c)
{
	Utente* p, * q;
	q = NULL;
	c->pun = NULL;
	
	for(p = *lista; p != NULL; p = p->pun)
		q = p;

	if(q == NULL)
		*lista = c;
	else
		q->pun = c;
}

Utente* cerca_sock_lista_utenti(Utente* lista, int sock)
{
	Utente* ret;
	
	for(ret = lista; ret != NULL; ret = ret->pun)
		if(ret->sock == sock)
			break;
			
	return ret;
}

Utente* cerca_nome_lista_utenti(Utente* lista, char* nome)
{
	Utente* ret;
	
	for(ret = lista; ret != NULL; ret = ret->pun)
		if(strcmp(ret->nome, nome) == 0)
			break;
			
	return ret;
}

int elimina_lista_utenti(Utente** lista, int sock)
{
	Utente* p, * q = NULL;
	
	for(p = *lista; p != NULL && p->sock != sock; p = p->pun)
		q = p;
	
	if(p == NULL) /* lista vuota o elemento assente */
		return -1; 
	else if(q == NULL)
		*lista = p->pun;
	else
		q->pun = p->pun;
		
	/* non dovrebbero eserci messaggi ma un controllo si fa comunque */
	Messaggio* m;
	while(p->messaggi_pendenti != NULL)
	{
		m = p->messaggi_pendenti;
		p->messaggi_pendenti = p->messaggi_pendenti->pun;
		free(m);
	}
	
	free(p->indirizzo);
	free(p);
		
	return 0;
}

/* -------------- */


void log_out(Utente* u)
{
	if(u != NULL)
	{	/* setto valori non validi */
		free(u->indirizzo);
		u->indirizzo = NULL;
		u->sock = -1; 
		
	}
}

int stato_utente(Utente* u)
{
	return ((u->indirizzo != NULL) ? 1 : 0);
}

void inserisci_messaggio_utente(Utente* u, char* username, char* messaggio)
{
	Messaggio* p, *q = NULL;
	Messaggio* m = malloc(sizeof(Messaggio));
	m->corpo = malloc(strlen(messaggio) + 1);
	
	strcpy(m->mittente, username);
	strcpy(m->corpo, messaggio);
	m->pun = NULL;
	
	for(p = u->messaggi_pendenti; p != NULL; p = p->pun)
		q = p;
		
	if(q == NULL)
		u->messaggi_pendenti = m;
	else
		q->pun = m;
}


int estrai_messaggio_utente(Utente* u, char* username, char** messaggio)
{
	Messaggio* m = u->messaggi_pendenti;
	
	if(m == NULL)
		return -1;
	
	*messaggio = malloc(strlen(m->corpo) + 1);
	strcpy(username, m->mittente);
	strcpy(*messaggio, m->corpo);
	(*messaggio)[strlen(m->corpo)] = '\0';
	
	u->messaggi_pendenti = u->messaggi_pendenti->pun;
	free(m->corpo);
	free(m);
	
	return 0;
}
