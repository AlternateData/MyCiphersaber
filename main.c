#include <stdio.h>
#include <getopt.h>

#include "ciphersaber.h"

#define C1_FILE_EXT ".cs"
#define C2_FILE_EXT ".cs2"


#define OPT_STR  "edli:"

char* prompt();


int main(int argc, char * argv[]){
  char opt;
  char* outfname;
  int uselegacy = 0;
  char mode;
  while((opt = getopt(argc, argv, OPT_STR) != -1)){
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
      outfname = argv[optind];;
      break;
    }
    
  }
  return 0;
}
