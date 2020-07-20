/* 
PROGETTO DI RETI INFORMATICHE - ANNO 2017/18 - MATTEO ZINI 533197
##############   lib_condivisa/utility_socket.c   ###############
*/

#include "utility_socket.h"
#include "strutture_dati.h"
#include "costanti.h"

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>


int ricevi_username(int sock, char* username)
{
	uint16_t dim_username;
	ssize_t byte_rec;

	byte_rec = recv(sock, (void*)&dim_username, sizeof(uint16_t), 0);
	if(byte_rec < sizeof(uint16_t)) 
		return -1;

	dim_username = ntohs(dim_username);

	byte_rec = recv(sock, (void*)username, dim_username, 0);
	if(byte_rec < dim_username) 
		return -1;

	username[dim_username] = '\0';

	return 0;
}


int ricevi_messaggio(int sock, char** messaggio)
{	
	uint16_t dim_messaggio, dim_messaggio_n;
	ssize_t byte_rec;

	byte_rec = recv(sock, (void*)&dim_messaggio_n, sizeof(uint16_t), 0);
	if(byte_rec < sizeof(uint16_t)) 
		return -1;

	dim_messaggio = ntohs(dim_messaggio_n);
	*messaggio = malloc(dim_messaggio+1);

	byte_rec = recv(sock, (void*)*messaggio, dim_messaggio, 0);
	if(byte_rec < dim_messaggio) 
		return -1;

	(*messaggio)[dim_messaggio] = '\0';

	return 0;
}


int ricevi_comando(int sock, char* comando)
{
	ssize_t byte_rec;

	byte_rec = recv(sock, (void*)comando, DIM_COMANDI, 0);
	if(byte_rec < DIM_COMANDI) 
		return -1;
	comando[DIM_COMANDI] = '\0';

	return 0;
}


int invia_username(int sock, char* username)
{
	uint16_t dim_username, dim_username_n;
	ssize_t byte_inv;

	dim_username = strlen(username);
	dim_username_n = htons(dim_username);

	byte_inv = send(sock, (void*)&dim_username_n, sizeof(uint16_t), 0);
	if(byte_inv < sizeof(uint16_t)) 
		return -1;

	byte_inv = send(sock, (void*)username, dim_username, 0);
	if(byte_inv < dim_username) 
		return -1;

	return 0;
}

int invia_messaggio(int sock, char* messaggio)
{
	uint16_t dim_messaggio, dim_messaggio_n;
	ssize_t byte_inv;
	
	dim_messaggio = strlen(messaggio);
	dim_messaggio = (dim_messaggio > 0) ? dim_messaggio : 1; /* se è vuota mando il carattere di terminazione così il server non si addormenta sulla sned */ 
	dim_messaggio_n = htons(dim_messaggio);

	byte_inv = send(sock, (void*)&dim_messaggio_n, sizeof(uint16_t), 0);
	if(byte_inv < sizeof(uint16_t)) 
		return -1;

	byte_inv = send(sock, (void*)messaggio, dim_messaggio, 0);
	if(byte_inv < dim_messaggio) 
		return -1;

	return 0;
}


int invia_comando(int sock, char* comando)
{
	ssize_t byte_inv;

	byte_inv = send(sock, (void*)comando, DIM_COMANDI, 0);
	if(byte_inv < DIM_COMANDI) 
		return -1;

	return 0;
}

/* ------------------------------------------------------------- */

int ricevi_indirizzo(int sock, Indirizzo* indirizzo)
{
	uint16_t msg_porta;
	char msg_ip[INET_ADDRSTRLEN];
	int byte_rec;

	/* nuovo codice per la ricezione dell'ip del client dopo il cambiamento delle specifiche */
	byte_rec = recv(sock, (void*)msg_ip, INET_ADDRSTRLEN-1, 0);
	if(byte_rec < INET_ADDRSTRLEN-1) 
		return -1;

	/* rimetto il carattere di fine stringa */
	msg_ip[INET_ADDRSTRLEN-1] = '\0';

	/* riczione dela porta del client */
	byte_rec = recv(sock, (void*)&msg_porta, sizeof(uint16_t), 0);
	if(byte_rec < sizeof(uint16_t))
		return -1;

	msg_porta = ntohs(msg_porta);

	strcpy(indirizzo->ip, msg_ip);
	indirizzo->porta = msg_porta;

	return 0;
}

int invia_indirizzo(int sock, Indirizzo* indirizzo)
{
	ssize_t byte_inv;
	uint16_t messaggio_porta;
	const int lunghezza_porta = sizeof(uint16_t);
	
	messaggio_porta = (uint16_t) indirizzo->porta;
	messaggio_porta = htons(messaggio_porta);
	
	/* il ricevente sa che questa prima comunicazione avrà lunghezza 16 byte essendo l'ip in formato stringa*/
	/* ometto il carattere di fine stringa, verrà rimesso a destinazione */
	byte_inv = send(sock, (void*)indirizzo->ip, INET_ADDRSTRLEN-1, 0);
	if(byte_inv < INET_ADDRSTRLEN-1) 
		return -1;

	/* mando la mia porta UDP: il ricevente sa che questa prima comunicazione avrà lunghezza 16, essendo l'invio della porta */
	byte_inv = send(sock, (void*)&messaggio_porta, lunghezza_porta, 0);
	if(byte_inv < lunghezza_porta) 
		return -1;

	return 0;
}


/* ------------------------------------------------------------- */


int invia_stato_utente(int sock, uint16_t stato)
{
	ssize_t byte_inv;
	uint16_t stato_n = htons(stato);

	byte_inv = send(sock, (void*)&stato_n, sizeof(uint16_t), 0);
	if(byte_inv < sizeof(uint16_t)) 
		return -1;

	return 0;
}

int ricevi_stato_utente(int sock, uint16_t* stato)
{
	ssize_t byte_rec;
	uint16_t stato_n;

	byte_rec = recv(sock, (void*)&stato_n, sizeof(uint16_t), 0);
	if(byte_rec < sizeof(uint16_t)) 
		return -1;

	*stato = ntohs(stato_n); 

	return 0;
}
