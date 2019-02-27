/* My own implementation of the CipherSaber algorithm
 * Author: Louis Kronberg
 * Date: 26.02.2019
 */
#include <stdio.h>
#include <stdlib.h>


#define BUFF_LEN 256
#define IV_LEN 10


char key[BUFF_LEN] = {0};
/* These correspond to the i and j counters mentioned in the specs.
 * They are used to generate random bytes.
 */
int counter_i;
int counter_j;

void swap(unsigned char * lst, int i, int j){
  unsigned char tmp = lst[i];
  lst[i] = lst[j];
  lst[j] = tmp;
}

void init_sbox(unsigned char *sbox, const unsigned char * key){
  for(int i = 0; i < BUFF_LEN; i++){
    sbox[i] = i;
  } 

  unsigned  char j = 0;
  for(int i = 0; i < BUFF_LEN; i++){
    j = (j + sbox[i] + key[i]);
    swap(sbox, i, j);
  }
}

void init_key(FILE * key_file, unsigned char * iv, unsigned char * key){

  /* load key from file into buffer*/
  char c;
  int i = 0;
  for(;i < IV_LEN; i++){
    key[i]  = iv[i];
  }

  while((c = fgetc(key_file)) != EOF && i < BUFF_LEN){
    key[i] = c;
    i++;
  }
  
  /* fill up the remaining space with the key */
  for(int j = i; j < BUFF_LEN; j++){
    key[j] = key[j % i];
  }

  //printf("%s\n", key);

}

void arcfour(char * in, int in_len, const unsigned char * key, char * out){
  unsigned char sbox[BUFF_LEN] = {0};
  init_sbox(sbox, key);
  unsigned char i = 0;
  unsigned char j = 0;
  unsigned char t = 0;
  for(int k = 0; k < in_len; k++){
    i++; 
    j += sbox[i];
    swap(sbox, i, j);
    t = sbox[i] + sbox[j];
    out[k] = in[k] ^ sbox[t];
  }

}

char* encrypt(FILE*);
char* decrypt(FILE*);

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

  

  fclose(ptext_file); 
  fclose(key_file); 
  fclose(out_file); 
   
  return 0;
}
