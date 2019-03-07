#include <stdio.h>
#include <getopt.h>
#include <unistd.h>

#include "ciphersaber.h"


#define PROMPT "Secret Key: "

#define OPT_STR  "edli:"

char* prompt();
void  print_help(int code);


int main(int argc, char * argv[]){
  int opt;
  char* fname = "";
  int uselegacy = 0;
  char mode = ' ';
  FILE * in = NULL;
  FILE*  out= NULL;

  while((opt = getopt(argc, argv, OPT_STR)) != -1){
    switch(opt){
    case 'l':
      uselegacy = 1;
      break;
    case 'e':
      mode = 'e';
      break;
    case 'd':
      mode = 'd';
      break;
    case 'i':
      in = fopen(optarg, "rb");
      if(!in){
	fprintf(stderr, "[ERROR]: Could not open file: %s\n", optarg);
	print_help(EXIT_FAILURE);
      }
      fname = optarg;
      break;
    case '?':
      print_help(EXIT_FAILURE);
    }
  }

  if(!in){
    in  = stdin;
    out = stdout;
    printf("Listening on stin\n");
  }

  /* read in passphrase from stdin */
  char * secret = getpass(PROMPT);

  /* read from in */

  if(uselegacy){
    fprintf(stderr, "[WARNING]: Vulnerable Ciphersaber1 algorithm selected. Please use Ciphersaber2 instead!\n");
  }

  if(mode=='e'){
    printf("Encrypting!\n");
  }else if(mode=='d'){
    printf("Decrypting!\n");
  }else if(mode == ' '){
    fprintf(stderr, "[ERROR]: Neither -d nor -e was given as an Option.\n");
    print_help(EXIT_FAILURE);
  }

  /* write to out */


  return 0;
}


void print_help(int ecode){
  printf("Abort.");
  exit(ecode);
}
