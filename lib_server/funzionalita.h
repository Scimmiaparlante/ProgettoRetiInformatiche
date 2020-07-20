/* 
PROGETTO DI RETI INFORMATICHE - ANNO 2017/18 - MATTEO ZINI 533197
################   lib_server/funzionalita.h   ##################
*/

#ifndef LIB_SERVER_FUNZIONALITA
#define LIB_SERVER_FUNZIONALITA

void disconnetti_client(int socket_client);
int registra_utente(int socket_client);
int deregistra_utente(int socket_client);
int manda_lista_utenti(int socket_client);
int gestore_invio_messaggi(int socket_client);

#endif
