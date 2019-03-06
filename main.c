/* My own implementation of the CipherSaber algorithm
 * Author: Louis Kronberg
 * Date: 26.02.2019
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>


#define ARC4_KEY_LEN 256
#define IV_LEN 10
#define KEY_SCHED_REP 20

typedef unsigned char uint8_t;


/* utility mehthods */
void swap(uint8_t * lst, uint8_t i, uint8_t j){
  uint8_t tmp = lst[i];
  lst[i] = lst[j];
  lst[j] = tmp;
}


/* cryptographic functions */
void init_sbox(uint8_t *sbox, const char * key){
  for(int i = 0; i < ARC4_KEY_LEN; i++){
    sbox[i] = i;
  } 

   uint8_t j = 0;
   for(int i = 0; i < KEY_SCHED_REP; i++){
    for(int i = 0; i < ARC4_KEY_LEN; i++){
	j = (j + sbox[i] + key[i]);
	swap(sbox, i, j);
    }
   }
}

void add_iv(char * key, char * iv){
  /*compute the length of the key */
  int i = 0;
  while(key[i] != '\n')
    i++;

  /* load iv into buffer*/
  for(int j = i; j - i  < IV_LEN; j++){
    key[j]  = iv[j - i];
  }

  /* fill up the remaining space with the key */
  i+=IV_LEN;
  for(int j = i; j < ARC4_KEY_LEN; j++){
    key[j] = key[j % i];
  }
}

void arcfour(char * in, int in_len, const char * key){
  uint8_t sbox[ARC4_KEY_LEN] = {0};

  init_sbox(sbox, key);
  uint8_t i = 0;
  uint8_t j = 0;
  uint8_t t = 0;
  for(int k = 0; k < in_len; k++){
    i++; 
    j += sbox[i];
    swap(sbox, i, j);
    t = sbox[i] + sbox[j];
    in[k] = in[k] ^ sbox[t];
 }
}

int main(int argc, char *argv[]){

  int ret = 0;
  if(argc != 4){
    fprintf(stderr, "Invalid amount of arguments!\n");
    printf("Usage: %s [encrypt|decrypt] [plaintext] [outfile]\n", argv[0]);
    exit(1);
  }

  FILE * ptext_file =  fopen(argv[2], "rb"); 
  if(ptext_file == NULL){
    fprintf(stderr, "Fatal Error: Failed to open file %s\n", argv[2]);
    fclose(ptext_file); 
    exit(1);
  }

  FILE * out_file   =  fopen(argv[3], "wb");
  if(out_file == NULL){
    fprintf(stderr, "Fatal Error: Failed to open file out.cs\n");
    fclose(ptext_file);
    fclose(out_file); 
    exit(1);
  }

  /* read in ptext_file into buffer of appropiate size */
  fseek(ptext_file, 0, SEEK_END);
  long len = ftell(ptext_file);
  rewind(ptext_file);
  char * msg = malloc(sizeof(char)*len);

  if(msg == NULL){
    /* failure to allocate this memory is fatal. Abort. */
    fprintf(stderr, "Fatal Error: Failed to allocate memory for file contents of %s\n", argv[2]);
    fclose(ptext_file); 
    fclose(out_file); 
    free(msg);
    exit(1);
  }

  /* this will hold the initialisation vector */
  char iv[IV_LEN] = {0};

  if(strcmp(argv[1], "encrypt") == 0){

    /* seed prng used for initialisation vector */
    srand(time(NULL));

    /* create initialisation vector  */
    for(int i = 0; i < IV_LEN; i++) {
      iv[i] = rand() % ARC4_KEY_LEN;
    }

    fwrite(iv, 1, IV_LEN, out_file);

  } else if(strcmp(argv[1], "decrypt") == 0){

    /* decrease the length by the space taken up by iv*/
    len  = len - IV_LEN;
    if(len < 1){
      fprintf(stderr, "Fatal Error: After reading in iv, no message left.\n");
      fclose(ptext_file); 
      fclose(out_file); 
      free(msg);
      exit(1);
    }
    
    /* fetch initialisation vector by reading the first 10 bytes into iv*/
    fread(iv, IV_LEN, 1, ptext_file);
  } else {
    fprintf(stderr, "Fatal Error: Invalid option %s\n", argv[1]);
  }

  fread(msg, len, 1, ptext_file);

  /* this will hold the cipher key */
  char key[ARC4_KEY_LEN] = {0};
  printf("Secret Key: ");
  fgets(key, ARC4_KEY_LEN, stdin);
  add_iv(key, iv);
  /* write en/decrypted message back into msg and onto disk*/
  arcfour(msg, len, key);
  fwrite(msg, 1, len, out_file);
  
  fclose(ptext_file); 
  fclose(out_file); 
  free(msg);
   
  return ret;
}
