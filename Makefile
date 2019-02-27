CC=gcc
CFLAGS= -g -Wall

ciphersaber : main.o	
	$(CC)  $(CFLAGS) -o ciphersaber main.o
main.o : 
	$(CC)  $(CFLAGS) -c main.c
clean  :
	$(RM) main.o
