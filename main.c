/* My own implementation of the CipherSaber algorithm
 * Author: Louis Kronberg
 *
 */
#include <stdio.h>
#include <stdlib.h>
#define SBOX_LEN 256
char sbox[SBOX_LEN] = {0};

int main(int argc, char *argv[]){
  if(argc != 3){
    fprintf(stderr, "Invalid amount of arguments!\n");
    printf("Usage: %s [plaintext] [key]\n", argv[0]);
    exit(1);
  }

  FILE * ptext_file =  fopen(argv[1], "r"); 
  FILE * key_file   =  fopen(argv[2], "r"); 
  FILE * out_file   =  fopen("out.cs", "w");

  if(ptext_file == NULL){
    fprintf(stderr, "Fatal Error: Failed to open file %s\n", argv[1]);
    exit(1);
  }

  if(key_file == NULL){
    fprintf(stderr, "Fatal Error: Failed to open file %s\n", argv[2]);
    exit(1);
  }

  if(out_file == NULL){
    fprintf(stderr, "Fatal Error: Failed to open file out.cs\n");
    exit(1);
  }

  /* init S-Box */
  for(int i = 0; i < SBOX_LEN; i++){
    sbox[i] = i;
  } 

  fclose(ptext_file); 
  fclose(key_file); 
  fclose(out_file); 
   
  return 0;
}
