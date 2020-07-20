/* 
PROGETTO DI RETI INFORMATICHE - ANNO 2017/18 - MATTEO ZINI 533197
########################   utility.c   ##########################
*/
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void errore(char* nome)
{	
	int num_err = errno; //vedi manuale per copia
	char err[50] = "Errore ";
	strcat(err, nome);
	strcat(err, "\nperror");
	printf("Errno: %d\n", num_err);
	perror(err); 
	exit(1);
}

void errore_rec(char* nome)
{	
	int num_err = errno; //vedi manuale per copia
	char err[50] = "Errore ";
	strcat(err, nome);
	strcat(err, "\nperror");
	printf("Errno: %d\n", num_err);
	perror(err); 
	printf("\n");
	return;
}

/* ATTENZIONE: SI BLOCCA SE NON TROVA NULLA */
void pulisci_stdin()
{
	char c;	
	
	while((c = getchar()) != '\n' && c != EOF);
	return;
}
