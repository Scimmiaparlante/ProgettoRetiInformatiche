/* 
PROGETTO DI RETI INFORMATICHE - ANNO 2017/18 - MATTEO ZINI 533197
##############   lib_server/funzioni_socket.h   #################
*/


#ifndef LIB_SERVER_FUNZIONI_SOCKET
#define LIB_SERVER_FUNZIONI_SOCKET

#include "utente.h"

int crea_socket_ascolto(int porta_ascolto);
int accetta_nuovo_client(int sock_ascolto, int* socket_client);
int manda_messaggi_pendenti(int socket_client, Utente* ut);

#endif
