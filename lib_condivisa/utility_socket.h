/* 
PROGETTO DI RETI INFORMATICHE - ANNO 2017/18 - MATTEO ZINI 533197
##############   lib_condivisa/utility_socket.h   ###############
*/

#ifndef LIB_COND_UTILITY_SOCKET
#define LIB_COND_UTILITY_SOCKET

#include "strutture_dati.h"

int ricevi_username(int sock, char* username);
int ricevi_messaggio(int soc, char** messaggio);
int ricevi_comando(int sock, char* comando);
int invia_username(int sock, char* username);
int invia_messaggio(int sock, char* messaggio);
int invia_comando(int sock, char* comando);

int ricevi_indirizzo(int sock, Indirizzo* indirizzo);
int invia_indirizzo(int sock, Indirizzo* indirizzo);

int invia_stato_utente(int sock, uint16_t stato);
int ricevi_stato_utente(int sock, uint16_t* stato);

#endif
