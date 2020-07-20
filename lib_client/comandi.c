/* 
PROGETTO DI RETI INFORMATICHE - ANNO 2017/18 - MATTEO ZINI 533197
###################  lib_client/comandi.c   #####################
*/

#include "comandi.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>

#include "../lib_condivisa/utility.h"
#include "../lib_condivisa/utility_socket.h"
#include "../lib_condivisa/costanti.h"
#include "../lib_condivisa/strutture_dati.h"
#include "funzioni_socket.h"

char nome_utente[MAX_LUNGHEZZA_NOME_UTENTE+1];
/* socket */
extern int sock_udp, sock_tcp;
extern Indirizzo ind_udp;

Comando comandi[NUM_COMANDI] =	{	{"!help", &comando_help},
									{"!register", &comando_register},
									{"!deregister", &comando_deregister},
									{"!who", &comando_who},
									{"!send", &comando_send},
									{"!quit", &comando_quit}
								};

void prompt() 
{
	if(nome_utente != NULL)
		printf("%s", nome_utente);
		
	printf(">");
	fflush(stdout);

	return;
}


char* leggi_messaggio()
{
	size_t dimensione = 2, dim_getline = 0; /* 2 perchè la stringa iniziale è \n\0 per unificare la gestione della rimozione del \n finale */
	char* riga, * messaggio;
	
	messaggio = malloc(2);
	messaggio[0] = '\n';  /* così la rimozione dell'ultimo a capo può essere fatta senza distinzioni tra messaggio vuoto e pieno */
	messaggio[1] = '\0';
	
	while(dimensione < MAX_LUNGHEZZA_MESSAGGIO)
	{
		riga = NULL;

		ssize_t letti = getline(&riga, &dim_getline, stdin);
		if(letti == -1)
		{
			free(riga);
			free(messaggio);
			return NULL;
		}

		if(strcmp(riga, ".\n") == 0) 
			break;

		dimensione += letti;
		messaggio = realloc(messaggio, dimensione);
		strcat(messaggio, riga);		
	}
	
	free(riga);
	messaggio[dimensione - 2] = '\0'; /* levo l'ultimo a capo */

	return messaggio;
}


void interprete_comandi()
{
	int i;
	char com[MAX_LUNGHEZZA_NOME_COMANDI+1];

	scanf("%"STR(MAX_LUNGHEZZA_NOME_COMANDI)"s", com);

	for(i = 0; i < NUM_COMANDI; ++i)
		if(strcmp(com, comandi[i].nome) == 0)
		{
			comandi[i].gestore();
			return;
		}
	
	/* se non ho trovato nulla */
	printf("Il comando inserito non è valido\n");
	comandi[COMANDO_HELP].gestore();	
	pulisci_stdin();
}

void comando_help()
{
	printf( "\n"
			"Sono disponibili i seguenti comandi:\n"
			"!help --> mostra l'elenco dei comandi disponibili\n"
			"!register username --> registra il client presso il server\n"
			"!deregister --> de-registra il client presso il server\n"
			"!who --> mostra l'elenco degli utenti disponibili\n"
			"!send username --> invia un messaggio ad un altro utente\n"
			"!quit --> disconnette il client dal server ed esce\n\n"
			);
}

void comando_register() 
{
	char username[MAX_LUNGHEZZA_NOME_UTENTE+1];
	char risposta[DIM_COMANDI+1];
	int ret;
	
	scanf("%"STR(MAX_LUNGHEZZA_NOME_UTENTE)"s", username);
	
	ret = invia_comando(sock_tcp, COD_COMANDO_REGISTER);
	if(ret < 0)
	{
		errore_rec("invio comando di registrazione al server");
		return;
	}
	
	ret = invia_indirizzo(sock_tcp, &ind_udp);
	if(ret < 0)
		errore("invio indirizzo udp per la registrazione");
		
	ret = invia_username(sock_tcp, username);
	if(ret < 0)
		errore("invio username per la registrazione");
		
	ret = ricevi_comando(sock_tcp, risposta);
	if(ret < 0)
		errore("ricezione conferma di registrazione da parte del server");
		
	/* l'username è già registrato */
	if(strcmp(risposta, COD_COMANDO_GIA_LOGGATO) == 0)
	{
		printf("L'utente %s è già online nel sistema\n", username);
		/* si viene sloggati automaticamente */
		strcpy(nome_utente, "");
		return;
	}
	
	printf("Registrazione avvenuta con successo come %s\n", username);
	/* setto la var globale per la prompt */
	strcpy(nome_utente, username);
	
	
	/* se non mi manda ok ma il codice dei messaggi vuol dire che ho altri messaggi */
	while(strcmp(risposta, COD_COMANDO_OK) != 0)
	{
		int ret;
		char* messaggio = NULL;
		/* nell'ordine ricevo: lunghezza username, username, lunghezza messaggio, messaggio */
	
		ret = ricevi_username(sock_tcp, username);
		if(ret < 0)
			errore("ricezione username messaggio offline");
			
		ret = ricevi_messaggio(sock_tcp, &messaggio); /* messaggio è nello heap -> va deallocato poi */
		if(ret < 0)
			errore("ricezione messaggio offline");
		
		/* ricevuto il messaggio, lo stampo */
		printf("%s (msg offline)>\n%s\n\n", username, messaggio);	
		
		free(messaggio);		
		
		/* ho finito o i sono ancora messaggi? */
		ret = ricevi_comando(sock_tcp, risposta);
		if(ret < 0)
			errore("ricezione risposta server dopo ricezione del messaggio");	
	}
	pulisci_stdin();
}



void comando_deregister() 
{
	int ret;
	char risposta[DIM_COMANDI+1];
	
	ret = invia_comando(sock_tcp, COD_COMANDO_DEREGISTER);
	if(ret < 0)
		errore("invio comando di deregistrazione");
		
	ret = ricevi_comando(sock_tcp, risposta);
	if(ret < 0)
		errore("ricezione risposta al comando di deregistrazione");
	
	strcpy(nome_utente, "");
	printf("Deregistrazione avvenuta con successo\n");
	
	pulisci_stdin();
}


void comando_who() 
{
	int ret;
	char risposta[DIM_COMANDI + 1];
	
	ret = invia_comando(sock_tcp, COD_COMANDO_WHO);
	if(ret < 0)
		errore("invio comando who");

	printf("Client registrati:\n");

	ret = ricevi_comando(sock_tcp, risposta);
	if(ret < 0)
		errore("ricezione risposta comando who");
		
	while(strcmp(risposta, COD_COMANDO_OK) != 0)
	{
		char username[MAX_LUNGHEZZA_NOME_UTENTE+1];
		uint16_t stato;
		char stato_stampa[10];
		
		ret = ricevi_username(sock_tcp, username);		
		if(ret < 0)
			errore("ricezione username comando who");
	
		ret = ricevi_stato_utente(sock_tcp, &stato);
		if(ret < 0)
			errore("ricezione stato comando who");
		
		if(stato == 1)
			strcpy(stato_stampa, "online");
		else
			strcpy(stato_stampa, "offline");
			
		printf("\t%s(%s)\n", username, stato_stampa);		
		
		ret = ricevi_comando(sock_tcp, risposta);
		if(ret < 0)
			errore("ricezione risposta comando who");
	}
	
	pulisci_stdin();
}


void comando_send() 
{
	int ret;
	uint16_t stato_utente;
	char username[MAX_LUNGHEZZA_NOME_UTENTE+1];
	char* messaggio;
	char risposta[DIM_COMANDI + 1];
	
	scanf("%" STR(MAX_LUNGHEZZA_NOME_UTENTE) "s", username);
	
	ret = invia_comando(sock_tcp, COD_COMANDO_SEND); /* invio comando e user */
	if(ret < 0)
		errore("invio comando send");
		
	ret = invia_username(sock_tcp, username);
	if(ret < 0)
		errore("invio username send");
		
	/* vedo se tutto è ok */
	ret = ricevi_comando(sock_tcp, risposta);
	if(ret < 0)
		errore("ricezione risposta alla send");
	
	if(strcmp(risposta, COD_COMANDO_NON_LOGGATO) == 0)
	{
		printf("Non sei loggato, non puoi inviare messaggi\n");
		pulisci_stdin();
		return;
	}
	else if(strcmp(risposta, COD_COMANDO_UTENTE_INESISTENTE) == 0)
	{
		printf("L'utente destinatario \"%s\" è inesistente\n", username);
		pulisci_stdin();
		return;
	}
	
	ret = ricevi_stato_utente(sock_tcp, &stato_utente);
	if(ret < 0)
		errore("ricezione stato utente send");
		
	/* leggo il messaggio */
	pulisci_stdin();
	messaggio = leggi_messaggio();
	if(messaggio == NULL)
		errore("lettura messaggio");
	
	if(stato_utente == 0)
	{
		ret = invia_messaggio(sock_tcp, messaggio);
		if(ret < 0)
			errore("invio messaggio offline");
			
		printf("Messaggio offline inviato\n");
	}
	else if(stato_utente == 1)
	{
		Indirizzo* ind_destinatario = malloc(sizeof(Indirizzo));
		ret = ricevi_indirizzo(sock_tcp, ind_destinatario);
		if(ret < 0)
			errore("ricezione indirizzo messaggio istantaneo");
			
		ret = invia_messaggio_istantaneo(sock_udp, ind_destinatario, nome_utente, messaggio);
		if(ret < 0)
			errore("invio messaggio istantaneo");		
	
		printf("\nMessaggio istantaneo inviato\n\n");
	}	
}


void comando_quit() 
{
	int ret;
	
	/* chiudo i socket */
	ret = close(sock_tcp);
	if(ret != 0) 
		errore("close socket tcp");	
		
	ret = close(sock_udp);
	if(ret != 0) 
		errore("close socket udp");	
		
	printf("\nClient disconnesso\n\n");

	exit(0);
}
