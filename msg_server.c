/* 
PROGETTO DI RETI INFORMATICHE - ANNO 2017/18 - MATTEO ZINI 533197
#########################   SERVER   ############################
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <time.h>

#include "lib_condivisa/costanti.h"
#include "lib_condivisa/utility.h"
#include "lib_condivisa/utility_socket.h"
#include "lib_condivisa/strutture_dati.h"
#include "lib_server/utente.h"
#include "lib_server/funzionalita.h"
#include "lib_server/funzioni_socket.h"

#define NUM_PARAMETRI 2

struct Utente* utenti_registrati = NULL;

int riduci_id_massimo(fd_set set, int old_max)
{	
	int i, new_max;
	
	for(i = 0; i < old_max; i++)
		if(FD_ISSET(i, &set))
			new_max = i;
	
	return new_max;	
} 


/*
funzione principale dove si svolge la gestione degli utenti
*/
void start_servizio(int sock_ascolto)
{	
	/* strutture dati per la gestoine del multiplexing */
	fd_set set_principale, set_utilizzo;
	int sock_id_max;
	FD_ZERO(&set_principale);
	FD_ZERO(&set_utilizzo);
	
	FD_SET(sock_ascolto, &set_principale);
	sock_id_max = sock_ascolto;
	/*-----------------------------------------*/

	/* una volta qui dentro, errori a parte, non si esce più */
	while(1)
	{
		int sock_i, ret;
		
		/* ripristino il set di utilizzo */
		set_utilizzo = set_principale;
		
		/* aspetto un socket pronto */
		ret = select(sock_id_max + 1, &set_utilizzo, NULL, NULL, NULL);
		if(ret == -1)
			errore("select");
		
		for(sock_i = 0; sock_i <= sock_id_max; ++sock_i)
		{
			if(FD_ISSET(sock_i, &set_utilizzo))
			{
				if(sock_i == sock_ascolto) /* è il socket in ascolto? */
				{
					int ret, socket_client;

					ret = accetta_nuovo_client(sock_ascolto, &socket_client);
					if(ret < 0)
						continue;
					
					/* è andato tutto bene -> inserisco il nuvo socket nel set */
					FD_SET(socket_client, &set_principale);
					if(socket_client > sock_id_max)
						sock_id_max = socket_client;
					
					printf("Connesso nuovo client con socket %d\n", socket_client);			
				}	
				else   /* socket di un client già connesso */
				{
					char comando[DIM_COMANDI+1];
					ssize_t byte_rec;
					
					byte_rec = recv(sock_i, (void*)comando, DIM_COMANDI, 0);
					if(byte_rec == -1) 
						errore("receive comando");
					comando[DIM_COMANDI] = '\0';
					
					if(byte_rec == DIM_COMANDI) /* è giunto un comando */
					{
						if(strcmp(comando, COD_COMANDO_REGISTER) == 0)
							ret = registra_utente(sock_i);
						else if(strcmp(comando, COD_COMANDO_DEREGISTER) == 0)
							ret = deregistra_utente(sock_i);
						else if(strcmp(comando, COD_COMANDO_WHO) == 0)
							ret = manda_lista_utenti(sock_i);
						else if(strcmp(comando, COD_COMANDO_SEND) == 0)
							ret = gestore_invio_messaggi(sock_i);
						else
						{
							printf("Comando sconusciuto: disconnessione forzata client con socket%d\n", sock_i);
							ret = -2;
						}
					}

					if(byte_rec == 0 || ret < 0) /* disconnessione o errore nella gestione di una operazione precedente*/
					{	
						disconnetti_client(sock_i);
						
						FD_CLR(sock_i, &set_principale);
						if(sock_i == sock_id_max)
							sock_id_max = riduci_id_massimo(set_principale, sock_id_max);
							
						continue;
					}	
				}
			}
		}
	}
}



int main(int argc, char* argv[]) 
{
	/* parametri da linea di comando */
	int porta_ascolto;
	/* socket */
	int sock_ascolto;
	
	printf("\nProgetto di reti informatiche - Applicazione di messaggistica\n"
		     "-------------------------- Server ---------------------------\n\n\n");
	
	if(argc != NUM_PARAMETRI) 
	{
		printf("Inserire i parametri correttamente\n");
		return 1;
	}
	porta_ascolto = atol(argv[1]);
	
	sock_ascolto = crea_socket_ascolto(porta_ascolto);
	
	start_servizio(sock_ascolto);
	
	return 0;
}
