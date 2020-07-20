/* 
PROGETTO DI RETI INFORMATICHE - ANNO 2017/18 - MATTEO ZINI 533197
################   lib_server/funzionalita.c   ##################
*/

#include "funzionalita.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "utente.h"
#include "funzioni_socket.h"
#include "../lib_condivisa/utility_socket.h"
#include "../lib_condivisa/utility.h"
#include "../lib_condivisa/strutture_dati.h"
#include "../lib_condivisa/costanti.h"


extern Utente* utenti_registrati;

void disconnetti_client(int socket_client)
{	
	int ret;
	Utente* u;
	
	ret = close(socket_client);
	if(ret != 0) 
		errore("close socket durante disconnessione client");
		
	u = cerca_sock_lista_utenti(utenti_registrati, socket_client);
	
	if(u != NULL) /* se era loggato sloggalo */
	{
		log_out(u);
		printf("Log-out utente %s\n", u->nome);
	}
			
	printf("Disconnesso client con socket %d\n", socket_client);
}


int registra_utente(int socket_client)
{
	Indirizzo* ind_utente = malloc(sizeof(Indirizzo));
	Utente* nuovo_u;
	int ret;
	char username[MAX_LUNGHEZZA_NOME_UTENTE+1];
	
	ret = ricevi_indirizzo(socket_client, ind_utente);
	if(ret < 0)
	{
		errore_rec("ricezione indirizzo udp utente");
		return -1;
	}
	
	ret = ricevi_username(socket_client, username);	
	if(ret < 0)
	{
		errore_rec("ricezione username per la registrazione utente");
		return -1;
	}
	
	
	/* guardo se fosse per caso già loggato con altro nome*/
	nuovo_u = cerca_sock_lista_utenti(utenti_registrati, socket_client);
	if(nuovo_u != NULL) /* se è già loggato devo sloggarlo dal precedente */
	{
		log_out(nuovo_u);
		printf("Log-out utente %s\n", nuovo_u->nome);
	}
	
	nuovo_u = cerca_nome_lista_utenti(utenti_registrati, username);
	
	if(nuovo_u == NULL) /* non esiste già */
	{
		nuovo_u = malloc(sizeof(Utente));
		nuovo_u->pun = NULL;
		nuovo_u->messaggi_pendenti = NULL;
		strcpy(nuovo_u->nome, username);
		inserisci_fondo_lista_utenti(&utenti_registrati, nuovo_u);
	}
	else if(nuovo_u->indirizzo != NULL) /* qualcuno è già loggato con quel nome -> errore*/ 
	{
		ret = invia_comando(socket_client, COD_COMANDO_GIA_LOGGATO);
		if(ret < 0) 
		{
			errore_rec("invio errore doppio login");
			return -1;
		}

		free(ind_utente);
		return 0;	
	}

	/* vale come login */
	nuovo_u->indirizzo = ind_utente;
	nuovo_u->sock = socket_client;
	
	ret = manda_messaggi_pendenti(socket_client, nuovo_u);
	if(ret < 0)
		return -1;

	ret = invia_comando(socket_client, COD_COMANDO_OK);
	if(ret < 0)
	{
		errore_rec("invio conferma di registrazione");
		return -1;
	}
	
	printf("Utente %s connesso. Il suo socket udp è aperto all'indirizzo:\n\t%s\n\t%d\n\n", nuovo_u->nome, nuovo_u->indirizzo->ip, nuovo_u->indirizzo->porta);
	return 0;
}


int deregistra_utente(int socket_client)
{
	int ret;
	Utente* utente;
	char nome[MAX_LUNGHEZZA_NOME_UTENTE+1];
	
	utente = cerca_sock_lista_utenti(utenti_registrati, socket_client);	
	if(utente == NULL)
	{
		ret = invia_comando(socket_client, COD_COMANDO_NON_LOGGATO);
		if(ret < 0) /* l'utente non esiste */
		{
			errore_rec("invio comando non loggato durante deregistrazione");
			return -1;
		}
	}
	
	strcpy(nome, utente->nome); /* metto da parte il nome prima che venga distrutto dalla elimina */
	
	ret = elimina_lista_utenti(&utenti_registrati, socket_client);
	
	ret = invia_comando(socket_client, COD_COMANDO_OK);
	if(ret < 0) 
	{
		errore_rec("invio conferma deregistrazione");
		return -1;
	}
	
	printf("L'utente %s è stato deregistrato con successo\n", nome);
	
	return 0;
}



int manda_lista_utenti(int socket_client)
{
	int ret;
	Utente* u;
	uint16_t stato;
	
	for(u = utenti_registrati; u != NULL; u = u->pun)
	{
		ret = invia_comando(socket_client, COD_COMANDO_UTENTE_DISPONIBILE);
		if(ret < 0) 
		{
			errore_rec("invio avviso disponibilità prossimo elemento lista utenti");
			return -1;
		}
		
		ret = invia_username(socket_client, u->nome);
		if(ret < 0)
		{
			errore_rec("invio username lista utenti");
			return -1;
		}
		
		stato = stato_utente(u);
		
		ret = invia_stato_utente(socket_client, stato);
		if(ret < 0)
		{
			errore_rec("invio stato utente lista utenti");
			return -1;
		}
	}
	
	ret = invia_comando(socket_client, COD_COMANDO_OK);
	if(ret < 0) 
	{
		errore_rec("invio avviso fine lista utenti");
		return -1;
	}
	
	printf("Lista utenti inviata correttamenteal client con socket %d\n\n", socket_client);

	return 0;
}


int gestore_invio_messaggi(int socket_client)
{
	int ret;
	Utente* mittente, * destinatario;
	char username[MAX_LUNGHEZZA_NOME_UTENTE+1];
	uint16_t stato_destinatario;
	char* messaggio = NULL;
	
	ret = ricevi_username(socket_client, username);
	if(ret < 0) 
	{
		errore_rec("ricezione username destinatario messaggio");
		return -1;
	}
	
	mittente = cerca_sock_lista_utenti(utenti_registrati, socket_client);
	destinatario = cerca_nome_lista_utenti(utenti_registrati, username);
	
	if(mittente == NULL) /* il mittente non è loggato -> errore */
	{	
		ret = invia_comando(socket_client, COD_COMANDO_NON_LOGGATO);
		if(ret < 0) 
			errore_rec("invio errore per invio messaggio senza login");
			
		return 0;
	}
	else if(destinatario == NULL) /* il destinatario non esiste -> errore */
	{
		ret = invia_comando(socket_client, COD_COMANDO_UTENTE_INESISTENTE);
		if(ret < 0) 
			errore_rec("invio errore per invio messaggio a utente inesistente");

		return 0;
	}
	else
	{
		ret = invia_comando(socket_client, COD_COMANDO_OK);
		if(ret < 0) 
		{
			errore_rec("invio ok per invio messaggio");
			return -1;
		}
	}
	
	stato_destinatario = stato_utente(destinatario);
	ret = invia_stato_utente(socket_client, stato_destinatario);
	if(ret < 0) 
	{
		errore_rec("invio stato destinatario messaggio");
		return -1;
	}
	
	if(stato_destinatario == 0) /* messaggio offline */
	{
		ret = ricevi_messaggio(socket_client, &messaggio); /* restituisce un puntatore allo heap */
		if(ret < 0) 
		{
			errore_rec("ricezione messaggio offline");
			return -1;
		}

		printf("Messaggio da \"%s\" a \"%s\":\n\"%s\"\n\n", mittente->nome, destinatario->nome, messaggio);
		
		inserisci_messaggio_utente(destinatario, mittente->nome, messaggio);
		free(messaggio);
	}
	else /* messaggio istantaneo */
	{
		ret = invia_indirizzo(socket_client, destinatario->indirizzo);
		if(ret < 0) 
		{
			errore_rec("invio indirizzo messaggio istantaneo");
			return -1;
		}	
		printf("Mandato a %s l'indirizzo di %s\n\n", mittente->nome, destinatario->nome);
	}

	return 0;
}

