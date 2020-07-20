/* 
PROGETTO DI RETI INFORMATICHE - ANNO 2017/18 - MATTEO ZINI 533197
#########################   CLIENT   ############################
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

#include "lib_condivisa/costanti.h"
#include "lib_condivisa/utility.h"
#include "lib_condivisa/strutture_dati.h"
#include "lib_client/comandi.h"
#include "lib_client/funzioni_socket.h"

#define NUM_PARAMETRI 4
#define SOCKET_STDIN 0

/* socket */
int sock_udp, sock_tcp;
Indirizzo ind_udp;


void attendi_input(int sock_tcp, int sock_udp)
{
	/* strutture dati per la gestoine del multiplexing */
	const int sock_stdin = 0;
	const int select_max_id = ( (sock_tcp > sock_udp) ? sock_tcp : sock_udp ) + 1;
	
	fd_set set_principale, set_utilizzo;
	FD_ZERO(&set_principale);
	FD_ZERO(&set_utilizzo);
	
	FD_SET(sock_tcp, &set_principale);
	FD_SET(sock_udp, &set_principale);
	FD_SET(SOCKET_STDIN, &set_principale);
	/*-----------------------------------------*/

	while(1)
	{
		int sock_i, ret;
		
		prompt();
		
		/* ripristino il set di utilizzo */
		set_utilizzo = set_principale;
		
		/* aspetto un socket pronto */
		ret = select(select_max_id, &set_utilizzo, NULL, NULL, NULL);
		if(ret == -1)
			errore("select");
		
		for(sock_i = 0; sock_i < select_max_id; ++sock_i)
			if(FD_ISSET(sock_i, &set_utilizzo))
			{
				if(sock_i == sock_stdin) /* è il socket in ascolto? */
				{
					interprete_comandi();
				}	
				else if(sock_i == sock_tcp) /* socket col server: non dovrebe esserci motivo per cui mi scriva, al massimo potrebbe essersi disconnesso */
				{
					ssize_t byte_rec;
					char msg;
					
					byte_rec = recv(sock_i, (void*)&msg, 1, 0);
					if(byte_rec == -1) 
						errore("receive");
					if(byte_rec == 0) /*disconnessione*/
						errore("chiusura connessione da parte del server");
				}
				else /* è il socket udp, mi stanno mandando un messaggio, evviva */
				{
					char* messaggio = NULL;
					char username_mittente[MAX_LUNGHEZZA_NOME_UTENTE+1];
					
					ret = ricevi_messaggio_istantaneo(sock_udp, username_mittente, &messaggio);
					if(ret < 0)
						errore("ricezione messaggio istantaneo");
						
					printf("\n%s (msg istantaneo)>\n%s\n\n", username_mittente, messaggio);
				}
			}
	}
}



int main(int argc, char* argv[]) 
{
	/* parametri da linea di comando */
	int porta_locale, porta_server;
	char ip_server[INET_ADDRSTRLEN], ip_locale[INET_ADDRSTRLEN];

	
	
	printf("\nProgetto di reti informatiche - Applicazione di messaggistica\n"
		   "-------------------------- Client ---------------------------\n\n\n");

	/* leggo i parametri di ingresso */
	if(argc != NUM_PARAMETRI + 1) 
	{
		printf("Errore: inserire i parametri correttamente\n\n");
		return 1;
	}
	strcpy(ip_locale, argv[1]);
	porta_locale = atol(argv[2]);
	strcpy(ip_server, argv[3]);
	porta_server = atol(argv[4]);
	
	strcpy(ind_udp.ip, ip_locale);
	ind_udp.porta = porta_locale;
	
	/* inizializzo socket e connessione con il server */
	sock_udp = crea_socket_UDP(porta_locale);
	sock_tcp = crea_socket_TCP(ip_server, porta_server);
	
	printf("Connessione al server %s (porta %d) effettuata con successo\n", ip_server, porta_server);
	printf("Ricezione messaggi istntanei sulla porta %d\n\n", porta_locale);
		 
	comando_help();
	/* inizializzazioni completate */
	
	attendi_input(sock_tcp, sock_udp);

	return 0;
}
