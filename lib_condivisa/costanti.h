/* 
PROGETTO DI RETI INFORMATICHE - ANNO 2017/18 - MATTEO ZINI 533197
########################   costanti.h  ##########################
*/

#ifndef COSTANTI
#define COSTANTI



/*	define dei codici di comando */

#define DIM_COMANDI 3

#define COD_COMANDO_REGISTER "reg"
#define COD_COMANDO_DEREGISTER "der"
#define COD_COMANDO_WHO "who"
#define COD_COMANDO_SEND "sen"

#define COD_COMANDO_OK "ok!"

#define COD_COMANDO_MESSAGGI_DISPONIBILI "mes"
#define COD_COMANDO_GIA_LOGGATO "glo"
#define COD_COMANDO_NON_LOGGATO "nlo"
#define COD_COMANDO_UTENTE_DISPONIBILE "udi"
#define COD_COMANDO_UTENTE_INESISTENTE "uin"

/* define macro per utilizzare le costanti numeriche concatenate nelle stringhe
	https://stackoverflow.com/questions/25410690/scanf-variable-length-specifier
	https://stackoverflow.com/questions/2751870/how-exactly-does-the-double-stringize-trick-work
*/
#define STR2(x) #x
#define STR(X) STR2(X)


/* define valori massimi campi */
#define MAX_LUNGHEZZA_NOME_UTENTE 25
/* lunghezza massima di un messaggio */
#define MAX_LUNGHEZZA_MESSAGGIO 64000


#endif
