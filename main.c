#include <stdio.h>
#include <getopt.h>
#include <unistd.h>

#include "ciphersaber.h"


#define PROMPT "Secret Key: "

#define OPT_STR  "edli:"

void  print_help(int code);


char * progname = NULL;
int   niter;
char* infname;
char  mode;
char* extension;



int main(int argc, char * argv[]){
  int opt;

  niter   = C2_KEY_ITER;
  mode    = ' ';
  extension     = C2_FILE_EXT;

  FILE* in    = NULL;
  FILE* out   = NULL;

  progname = argv[0];

  while((opt = getopt(argc, argv, OPT_STR)) != -1){
    switch(opt){
    case 'l':
      niter = C1_KEY_ITER;
      extension = C1_FILE_EXT;
      fprintf(stderr, "[WARNING]: Vulnerable Ciphersaber1 algorithm selected. Please use Ciphersaber2 instead!\n");
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

  if(mode == ' '){
    fprintf(stderr, "[ERROR]: Neither -d nor -e was given as an option. These are essential.\n");
    print_help(EXIT_FAILURE);
  }


  if(!in){
    fprintf(stderr, "[ERROR]: No file was given to en/decrypt.\n");
    print_help(EXIT_FAILURE);
  }


  /* get file length, prepare memory and read from in */
  fseek(in, 0, SEEK_END);
  size_t fsize = ftell(in);
  fseek(in, 0, SEEK_SET);
  char *msg = malloc(sizeof(char) * fsize);

  if(!msg){
    fprintf(stderr, "[ERROR]: Failed to allocate memory for message.\n");
    exit(EXIT_FAILURE);
  }

  fread(msg, sizeof(char), fsize, in);

  char* secret = getpass(PROMPT);
  char* cipher = NULL;

  char *outfname;
  if(mode=='e'){
    cipher = encrypt(msg, fsize, secret, niter);

    outfname = malloc(sizeof(char) * (strlen(infname) + strlen(extension)));
    strcat(outfname, infname);
    strcat(outfname, extension);
  }else if(mode=='d'){
    cipher = decrypt(msg, fsize, secret, niter);

    outfname = malloc(strlen(infname) - strlen(extension));
    memcpy(outfname, infname, strlen(infname) - strlen(extension));
  }

  if(!cipher){
    fprintf(stderr, "[ERROR]: Internal method %s() returned NULL\n", mode=='e' ? "encrypt" : "decrypt");
    exit(EXIT_FAILURE);
  }


  out = fopen(outfname, "wb");
  if(!out){
    fprintf(stderr, "[ERROR]: Could not open output file %s\n", outfname);
    exit(EXIT_FAILURE);
  }

  fwrite(cipher, 1, fsize, out);


  /* clean up */
  fclose(in);
  fclose(out);
  free(msg);

  fclose(in);
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
