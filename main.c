#include <stdio.h>
#include <getopt.h>
#include <unistd.h>

#include "ciphersaber.h"


#define PROMPT "Secret Key: "

#define OPT_STR  "edli:"

void  print_help(int code);
void cleanup_globals();

char* progname = NULL;
int   niter = C2_KEY_ITER;
char* infname;
char  mode = ' ';
char* extension = C2_FILE_EXT;


FILE* in     = NULL;
FILE* out;
char *msg    = NULL;
char* cipher = NULL;
char *outfname = "out.txt";

int main(int argc, char * argv[]){
  int opt;

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
    cleanup_globals();
    print_help(EXIT_FAILURE);
  }


  if(!in){
    fprintf(stderr, "[ERROR]: No file was given to en/decrypt.\n");
    cleanup_globals();
    print_help(EXIT_FAILURE);
  }


  /* get file length, prepare memory and read from in */
  fseek(in, 0, SEEK_END);
  size_t fsize = ftell(in);
  fseek(in, 0, SEEK_SET);
  msg = malloc(sizeof(*msg) * fsize);

  if(!msg){
    fprintf(stderr, "[ERROR]: Failed to allocate memory for message.\n");
    cleanup_globals();
    exit(EXIT_FAILURE);
  }

  fread(msg, sizeof(char), fsize, in);

  /* obtain secret from cmdline */
  char* secret = getpass(PROMPT);

  if(mode=='e'){
    cipher = encrypt(msg, fsize, secret, niter);

    /* put together the file name */
    outfname = malloc(sizeof(*outfname) * (strlen(infname) + strlen(extension)));
    strcat(outfname, infname);
    strcat(outfname, extension);
  }else if(mode=='d'){
    cipher = decrypt(msg, fsize, secret, niter);

    outfname = malloc(strlen(infname) - strlen(extension));
    memcpy(outfname, infname, strlen(infname) - strlen(extension));
  }

  if(!cipher){
    fprintf(stderr, "[ERROR]: Internal method %s() returned NULL. This is a bug.\n", mode=='e' ? "encrypt" : "decrypt");
    cleanup_globals();
    exit(EXIT_FAILURE);
  }

  /* write contents of cipher to outfile */
  out = fopen(outfname, "wb");
  if(!out){
    fprintf(stderr, "[ERROR]: Could not open output file %s\n", outfname);
    cleanup_globals();
    exit(EXIT_FAILURE);
  }

  fwrite(cipher, 1, fsize, out);

  /*  
   * NOTE: Valgrind reports that memory allocated to cipher definitely is being leaked.
   * However it is being freed in the call to the following function. 
   */

  cleanup_globals();
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

void cleanup_globals(){
  if(in)
    fclose(in);
  if(out)
    fclose(out);
  if(cipher)
    free(cipher);
  if(msg)
    free(msg);
  if(outfname)
    free(outfname);
  outfname = NULL;
}
