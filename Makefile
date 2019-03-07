CC=gcc
CFLAGS= -g -Wall
LFLAGS=

ciphersaber : main.o	
	$(CC)  $(CFLAGS) $(LFLAGS) -o ciphersaber main.o
main.o : main.c
	$(CC)  $(CFLAGS) $(LFLAGS)  -c main.c
clean  :
	$(RM) main.o
