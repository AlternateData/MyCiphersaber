/* My own implementation of the CipherSaber algorithm
 * Author: Louis Kronberg
 *
 */
#include <stdio.h>
#include <stdlib.h>
#define BUFF_LEN 256
char sbox[BUFF_LEN] = {0};
char key[BUFF_LEN] = {0};

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

  /* init key */
  char c;
  int i = 0;
  while((c = fgetc(key_file)) != EOF && i < BUFF_LEN){
    key[i] = c;
    i++;
  }
  for(int j =i; i< BUFF_LEN; i++){
    key[j] = key[j % i];
  }

  //printf("%s\n", key);
  
  /* init S-Box */
  for(int i = 0; i < BUFF_LEN; i++){
    sbox[i] = i;
  } 



  fclose(ptext_file); 
  fclose(key_file); 
  fclose(out_file); 
   
  return 0;
}
