#include <stdio.h>
#include <getopt.h>
#include <unistd.h>

#include "ciphersaber.h"


#define PROMPT "Secret Key: "

#define OPT_STR  "edli:"

char* prompt();
void  print_help(int code);


char * progname = NULL;

int main(int argc, char * argv[]){
  int opt;

  int   niter   = C2_KEY_ITER;
  char* infname = "";
  char  mode    = ' ';
  char* ext     = C2_FILE_EXT;

  FILE* in    = NULL;
  FILE* out   = NULL;

  progname = argv[0];

  while((opt = getopt(argc, argv, OPT_STR)) != -1){
    switch(opt){
    case 'l':
      niter = C1_KEY_ITER;
      ext = C1_FILE_EXT;
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
      infname = optarg;
      break;
    case '?':
      print_help(EXIT_FAILURE);
    }
  }

 

  if(!in){
    fprintf(stderr, "[ERROR]: No file was given to en/decrypt.\n");
    print_help(EXIT_FAILURE);
  }

  char * secret = getpass(PROMPT);

  /* get file length, prepare memory and read from in */
  fseek(in, 0, SEEK_END);
  size_t fsize = ftell(in);
  fseek(in, 0, SEEK_SET);
  char *msg = malloc(sizeof(char) * fsize);

  if(!msg){
    fprintf(stderr, "[ERROR]: Failed to allocate memory for message.\n");
  }

  fread(msg, sizeof(char), fsize, in);


  /* warn in case vulnerable ciphersaber1 was selected */
  if(niter == C1_KEY_ITER){
    fprintf(stderr, "[WARNING]: Vulnerable Ciphersaber1 algorithm selected. Please use Ciphersaber2 instead!\n");
  }

  char *cipher = NULL;
  if(mode=='e'){
    printf("Encrypting!\n");
    cipher = encrypt(msg, fsize, secret, niter);
  }else if(mode=='d'){
    printf("Decrypting!\n");
    cipher = decrypt(msg, fsize, secret, niter);
  }else if(mode == ' '){
    fprintf(stderr, "[ERROR]: Neither -d nor -e was given as an option.\n");
    free(msg);
    print_help(EXIT_FAILURE);
  }


  /* open out file for writing */
  char outfname[strlen(infname) + strlen(ext)];
  strcat(outfname, infname);
  strcat(outfname, ext);
  out = fopen(outfname, "wb");

  if(!out){
    fprintf(stderr, "[ERROR]: Could not open output file %s\n", outfname);
    exit(EXIT_FAILURE);
  }

  free(msg);
  if(cipher){
    free(cipher);
  }

  return EXIT_SUCCESS;
}


void print_help(int ecode){
  printf("Usage: %s [ACTION] [FLAG] [-i] filename\n", progname);
  printf("\n");
  printf("ACTION\n");
  printf("\t-e[ncrypt]\t\t-encrypt the file \"filename\" to the file \"filename.cs2\" \n");
  printf("\t-d[ecrypt]\t\t-decrypt the file \"filename\" to the file \"filename.cs2\" \n");
  printf("\n");
  printf("FLAG\n");
  printf("\t-l[egacy]\t\t-enables old vulnerable ciphersaber1 algorithm\n");

  exit(ecode);
}
