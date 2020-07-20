all: msg_server msg_client

CFLAGS=-Wall
COMPILA=-c

msg_client: msg_client.o lib_client/comandi.o lib_client/funzioni_socket.o lib_condivisa/utility_socket.o lib_condivisa/utility.o
	gcc $^ -o $@ $(CFLAGS)
	
msg_server: msg_server.o lib_server/utente.o lib_server/funzionalita.o lib_server/funzioni_socket.o lib_condivisa/utility_socket.o lib_condivisa/utility.o
	gcc $^ -o $@ $(CFLAGS)
	
msg_client.o: msg_client.c lib_condivisa/costanti.h lib_condivisa/strutture_dati.h
	gcc $(COMPILA) msg_client.c -o $@ $(CFLAGS)
	
msg_server.o: msg_server.c lib_condivisa/costanti.h lib_condivisa/strutture_dati.h
	gcc $(COMPILA) msg_server.c -o $@ $(CFLAGS)

lib_client/comandi.o: lib_client/comandi.c lib_client/comandi.h lib_condivisa/costanti.h lib_condivisa/strutture_dati.h
	gcc $(COMPILA) lib_client/comandi.c -o $@ $(CFLAGS)
	
lib_client/funzioni_socket.o: lib_client/funzioni_socket.c lib_client/funzioni_socket.h lib_condivisa/strutture_dati.h
	gcc $(COMPILA) lib_client/funzioni_socket.c -o $@ $(CFLAGS)

lib_condivisa/utility_socket.o: lib_condivisa/utility_socket.c lib_condivisa/utility_socket.h lib_condivisa/costanti.h lib_condivisa/strutture_dati.h
	gcc $(COMPILA) lib_condivisa/utility_socket.c -o $@ $(CFLAGS)

lib_condivisa/utility.o: lib_condivisa/utility.c lib_condivisa/utility.h
	gcc $(COMPILA) lib_condivisa/utility.c -o $@ $(CFLAGS)

lib_server/utente.o: lib_server/utente.c lib_server/utente.h lib_condivisa/costanti.h lib_condivisa/strutture_dati.h
	gcc $(COMPILA) lib_server/utente.c -o $@ $(CFLAGS)
	
lib_server/funzionalita.o: lib_server/funzionalita.c lib_server/funzionalita.h lib_condivisa/costanti.h lib_condivisa/strutture_dati.h
	gcc $(COMPILA) lib_server/funzionalita.c -o $@ $(CFLAGS)
	
lib_server/funzioni_socket.o: lib_server/funzioni_socket.c lib_server/funzioni_socket.h lib_condivisa/costanti.h lib_condivisa/strutture_dati.h
	gcc $(COMPILA) lib_server/funzioni_socket.c -o $@ $(CFLAGS)
	
clean: 
	rm ./*.o
	rm ./lib_client/*.o
	rm ./lib_condivisa/*.o
	rm ./lib_server/*.o
	rm msg_server
	rm msg_client
