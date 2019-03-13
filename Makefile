CC=gcc
CFLAGS= -g -Wall
LFLAGS=

ciphersaber : main.o ciphersaber.o	
	$(CC) $(CFLAGS) $(LFLAGS) -o ciphersaber main.o ciphersaber.o

ciphersaber.o : ciphersaber.c
	$(CC) $(CFLAGS) $(LFLAGS) -c ciphersaber.c

main.o : main.c ciphersaber.h
	$(CC) $(CFLAGS) $(LFLAGS) -c main.c

clean :
	$(RM) main.o ciphersaber.o
