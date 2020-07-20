/* 
PROGETTO DI RETI INFORMATICHE - ANNO 2017/18 - MATTEO ZINI 533197
##############   lib_client/funzioni_socket.c   #################
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "../lib_condivisa/utility.h"
#include "funzioni_socket.h"


int crea_socket_UDP(uint16_t porta_locale)
{
	int ret, sock;
	struct sockaddr_in ind_UDP;
	
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock == -1)
		errore("apertura socket UDP");
		
	memset(&ind_UDP, 0, sizeof(ind_UDP));
	ind_UDP.sin_family = AF_INET;	
	ind_UDP.sin_port = htons(porta_locale);
	ind_UDP.sin_addr.s_addr = INADDR_ANY;
	/* 
	la struttura è temporanea e verrà deallocata senza creare problemi: il kernel ha già memorizzato per conto suo i miei dati
	https://stackoverflow.com/questions/12680835/can-i-call-bind-with-a-temp-addr-struct 
	*/
	
	ret = bind(sock, (struct sockaddr*)&ind_UDP, sizeof(ind_UDP));
	if(ret != 0)
		errore("bind socket UDP");
		
	return sock;
}

int crea_socket_TCP(char* ip_server, int porta_server)
{
	int ret, sock;
	struct sockaddr_in ind_server;
	
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == -1)
		errore("apertura socket TCP");
		
	memset(&ind_server, 0, sizeof(ind_server));
	ind_server.sin_family = AF_INET;	
	ind_server.sin_port = htons(porta_server);
	ret = inet_pton(AF_INET, ip_server, &ind_server.sin_addr);
	if(ret == 0)
		errore("l'ip non è valido");
		
	/* 
	la struttura è temporanea e verrà deallocata senza creare problemi: il kernel ha già memorizzato per conto suo i miei dati
	https://stackoverflow.com/questions/12680835/can-i-call-bind-with-a-temp-addr-struct 
	*/
	
	ret = connect(sock, (struct sockaddr*)&ind_server, sizeof(ind_server));
	if(ret != 0)
		errore("connect socket TCP");
		
	return sock;
}



int invia_messaggio_istantaneo(int sock, Indirizzo* indirizzo, char* username_mittente, char* messaggio)
{
	ssize_t byte_inv;
	int ret, dim_buffer;
	char* buffer;
	uint16_t lun_username, lun_messaggio, lun_username_n, lun_messaggio_n;
	struct sockaddr_in ind_dest;
	
	lun_username = strlen(username_mittente);
	lun_messaggio = strlen(messaggio);
	lun_username_n = htons(lun_username);
	lun_messaggio_n = htons(lun_messaggio);
	dim_buffer = 2*sizeof(uint16_t) + lun_username + lun_messaggio;

	/* sistemo il destinatario */
	memset(&ind_dest, 0, sizeof(struct sockaddr_in));
	ind_dest.sin_family = AF_INET;	
	ind_dest.sin_port = htons(indirizzo->porta);
	ret = inet_pton(AF_INET, indirizzo->ip, &ind_dest.sin_addr);
	if(ret == 0)
		return -1;

	/* creao il pacchetto */
	buffer = malloc(dim_buffer);
	/* metto la roba al suo posto: <lunghezza_username, lunghezza_messaggio, username, messaggio> */
	memcpy(buffer, &lun_username_n, sizeof(uint16_t));
	memcpy(buffer + sizeof(uint16_t), &lun_messaggio_n, sizeof(uint16_t));
	memcpy(buffer + 2*sizeof(uint16_t), username_mittente, lun_username);
	memcpy(buffer + 2*sizeof(uint16_t) + lun_username, messaggio, lun_messaggio);
	
	byte_inv = sendto(sock, (void*)buffer, dim_buffer, 0, (struct sockaddr*)&ind_dest, sizeof(struct sockaddr_in));
	if(byte_inv < dim_buffer)
		return -1;

	return 0;
}


int ricevi_messaggio_istantaneo(int sock, char* username_mittente, char** messaggio)
{
	ssize_t byte_rec;
	int dim_buffer;
	char* buffer;
	uint16_t lun_username, lun_messaggio;
	struct sockaddr_in ind_prov;
	socklen_t dim_ind_prov = sizeof(struct sockaddr_in);
	
	dim_buffer = 2 * sizeof(uint16_t);
	buffer = malloc(dim_buffer);
	
	byte_rec = recvfrom(sock, (void*)buffer, dim_buffer, MSG_PEEK, (struct sockaddr*)&ind_prov, &dim_ind_prov);
	if(byte_rec < dim_buffer)
		return -1;
		
	memcpy(&lun_username, buffer, sizeof(uint16_t));
	memcpy(&lun_messaggio, buffer + sizeof(uint16_t), sizeof(uint16_t));
	lun_username = ntohs(lun_username);
	lun_messaggio = ntohs(lun_messaggio);
	
	dim_buffer = dim_buffer + lun_messaggio + lun_username; 
	free(buffer);
	buffer = malloc(dim_buffer);
	
	byte_rec = recvfrom(sock, (void*)buffer, dim_buffer, 0, (struct sockaddr*)&ind_prov, &dim_ind_prov);
	if(byte_rec < dim_buffer)
		return -1;
	
	*messaggio = malloc(lun_messaggio + 1);
	memcpy(username_mittente, buffer + 2*sizeof(uint16_t), lun_username);
	memcpy(*messaggio, buffer + 2*sizeof(uint16_t) + lun_username, lun_messaggio);
	
	(*messaggio)[lun_messaggio] = '\0';
	username_mittente[lun_username] = '\0';
	
	free(buffer);	

	return 0;
}
