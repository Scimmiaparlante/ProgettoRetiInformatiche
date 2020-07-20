/* 
PROGETTO DI RETI INFORMATICHE - ANNO 2017/18 - MATTEO ZINI 533197
##############   lib_server/funzioni_socket.c   #################
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "funzioni_socket.h"

#include "../lib_condivisa/utility_socket.h"
#include "../lib_condivisa/utility.h"
#include "../lib_condivisa/costanti.h"
#include "../lib_condivisa/strutture_dati.h"
#include "funzionalita.h"

#define MAX_LUNGHEZZA_BACKLOG 10

int crea_socket_ascolto(int porta_ascolto)
{
	int ret, sock;
	struct sockaddr_in ind_ascolto;
	
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == -1)
		errore("apertura socket TCP di ascolto");
		
	memset(&ind_ascolto, 0, sizeof(ind_ascolto));
	ind_ascolto.sin_family = AF_INET;	
	ind_ascolto.sin_port = htons(porta_ascolto);
	ind_ascolto.sin_addr.s_addr = INADDR_ANY;
	/* 
	la struttura è temporanea e verrà deallocata senza creare problemi: il kernel ha già memorizzato per conto suo i miei dati
	https://stackoverflow.com/questions/12680835/can-i-call-bind-with-a-temp-addr-struct 
	*/
	
	ret = bind(sock, (struct sockaddr*)&ind_ascolto, sizeof(ind_ascolto));
	if(ret != 0)
		errore("bind socket TCP di ascolto");
	
	ret = listen(sock, MAX_LUNGHEZZA_BACKLOG);
	if(ret != 0) 
		errore("listen socket TCP di ascolto");
		
	return sock;
}

int accetta_nuovo_client(int sock_ascolto, int* socket_client)
{
	struct sockaddr_in ind_client;
	int ret;
	struct timeval timeout;
	
	socklen_t len = sizeof(ind_client);
	*socket_client = accept(sock_ascolto, (struct sockaddr*)&ind_client, &len);
	if(*socket_client == -1)
	{
		errore_rec("Errore accept nuovo client");
		ret = close(*socket_client);
		if(ret == -1)
			errore("fallimento irrecuperabile nella chiusura della connessione (accept)");
		return -1;
	}
		
	/* recupero l'ip del nuovo client ----  CODICE RIMOSSO PER CAMBIAMENTO SPECIFICHE -----
	cret = inet_ntop(AF_INET, &ind_client.sin_addr, ip_nuovo_client, INET_ADDRSTRLEN);
	if(cret == NULL)
	{
		errore_rec("IP nuovo client corrotto");
		ret = close(*socket_client);
		if(ret == -1)
			errore("fallimento irrecuperabile nella chiusura della connessione (IP)");
		return -2;
	}*/
	
	/* metto un timeout al socket, così da non farsi bloccare da un client */
	memset(&timeout, 0, sizeof(timeout));
	timeout.tv_sec = 20;
	setsockopt(*socket_client, SOL_SOCKET, SO_RCVTIMEO, (void*) &timeout, (socklen_t) sizeof(timeout));
	timeout.tv_sec = 2;
	setsockopt(*socket_client, SOL_SOCKET, SO_SNDTIMEO, (void*) &timeout, (socklen_t) sizeof(timeout));
	
	return 0;
}


int manda_messaggi_pendenti(int socket_client, Utente* ut)
{
	Messaggio m;
	
	while(estrai_messaggio_utente(ut, m.mittente, &m.corpo) == 0)
	{
		int ret;
		
		/* comunico che ci sono ancora messaggi */
		ret = invia_comando(socket_client, COD_COMANDO_MESSAGGI_DISPONIBILI);
		if(ret < 0)
		{
			errore_rec("invio segnale che ci sono messaggi pendenti");	
			return -1;
		}
	
		/* trasmetto il prossimo */
		ret = invia_username(socket_client, m.mittente);
		if(ret < 0)
		{
			errore_rec("invio username messaggio offline");
			return -1;
		}
			
		ret = invia_messaggio(socket_client, m.corpo);
		if(ret < 0)
		{
			errore("invio corpo messaggio offline");
			return -1;
		}
		
		free(m.corpo);
	}
	return 0;
}
