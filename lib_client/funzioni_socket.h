/* 
PROGETTO DI RETI INFORMATICHE - ANNO 2017/18 - MATTEO ZINI 533197
##############   lib_client/funzioni_socket.h   #################
*/

#ifndef LIB_COND_FUNZIONI_SOCKET
#define LIB_COND_FUNZIONI_SOCKET

#include "../lib_condivisa/strutture_dati.h"

int crea_socket_UDP(uint16_t porta_locale);
int crea_socket_TCP(char* ip_server, int porta_server);

int invia_messaggio_istantaneo(int sock, Indirizzo* indirizzo, char* username_mittente, char* messaggio);
int ricevi_messaggio_istantaneo(int sock, char* username_mittente, char** messaggio);

#endif
