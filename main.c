/* My own implementation of the CipherSaber algorithm
 * Author: Louis Kronberg
 * Date: 26.02.2019
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>


#define ARC4_KEY_LEN 256
#define IV_LEN 10



/* utility mehthods */
void swap(uint8_t * lst, int i, int j){
  uint8_t tmp = lst[i];
  lst[i] = lst[j];
  lst[j] = tmp;
}

int streq(const char* lhs, const char* rhs){
  int i = 0;
  while(lhs[i] != '\0' && rhs[i] != '\0'){
    if(lhs[i] != rhs[i]){
      return 0;
    }
    i++;
  }

  if(lhs[i] != rhs[i])
    return 0;
  return 1;
}


/* cryptographic functions */
void init_sbox(uint8_t *sbox, const uint8_t * key){
  for(int i = 0; i < ARC4_KEY_LEN; i++){
    sbox[i] = i;
  } 

  unsigned  char j = 0;
  for(int i = 0; i < ARC4_KEY_LEN; i++){
    j = (j + sbox[i] + key[i]);
    swap(sbox, i, j);
  }
}

void init_key(FILE * key_file, uint8_t * iv, uint8_t * key){

  /* load key from file into buffer*/
  char c;
  int i = 0;
  for(;i < IV_LEN; i++){
    key[i]  = iv[i];
  }

  while((c = fgetc(key_file)) != EOF && i < ARC4_KEY_LEN){
    key[i] = c;
    i++;
  }
  
  /* fill up the remaining space with the key */
  for(int j = i; j < ARC4_KEY_LEN; j++){
    key[j] = key[j % i];
  }

  printf("%s\n", key);

}

void arcfour(char * in, int in_len, const uint8_t * key){
  uint8_t sbox[ARC4_KEY_LEN] = {0};
  init_sbox(sbox, key);
  uint8_t i = 0;
  uint8_t j = 0;
  for(int k = 0; k < in_len; k++){
    i++; 
    j += sbox[i];
    swap(sbox, i, j);
    in[k] = in[k] ^ sbox[sbox[i] + sbox[j]];
 }

}



int main(int argc, char *argv[]){
  int ret = 0;
  if(argc != 4){
    fprintf(stderr, "Invalid amount of arguments!\n");
    printf("Usage: %s [encrypt|decrypt] [plaintext] [key]\n", argv[0]);
    exit(1);
  }

  FILE * ptext_file =  fopen(argv[2], "rb"); 
  if(ptext_file == NULL){
    fprintf(stderr, "Fatal Error: Failed to open file %s\n", argv[2]);
    fclose(ptext_file); 
    exit(1);
  }

  FILE * key_file   =  fopen(argv[3], "rb"); 
  if(key_file == NULL){
    fprintf(stderr, "Fatal Error: Failed to open file %s\n", argv[3]);
    fclose(key_file); 
    fclose(ptext_file);
    exit(1);
  }

  FILE * out_file   =  fopen("out.cs", "wb");
  if(out_file == NULL){
    fprintf(stderr, "Fatal Error: Failed to open file out.cs\n");
    fclose(ptext_file);
    fclose(out_file); 
    fclose(key_file);
    exit(1);
  }

  /* read in ptext_file into buffer of appropiate size */
  fseek(ptext_file, 0, SEEK_END);
  long len = ftell(ptext_file);
  fseek(ptext_file, 0, SEEK_SET);
  char * msg = malloc(sizeof(char)*len);

  if(msg == NULL){
    /* failure to allocate this memory is fatal. Abort. */
    fprintf(stderr, "Fatal Error: Failed to allocate memory for file contents of %s\n", argv[2]);
    ret = 1;
    goto cleanup;
  }

  fread(msg, len, 1, ptext_file);

  /* this will hold the cipher key */
  uint8_t key[ARC4_KEY_LEN] = {0};
  /* this will hold the initialisation vector */
  uint8_t iv[IV_LEN] = {0};

  if(streq(argv[1], "encrypt")){

    printf("Encrypting!\n");

    /* seed prng used for initialisation vector */
    srand(time(NULL));

    /* create initialisation vector  */
    for(int i = 0; i < IV_LEN; i++) {
      iv[i] = rand() % ARC4_KEY_LEN;
    }
  } else if(streq(argv[1], "decrypt")){

    printf("Decrypting!\n");
    /* decrease the length by the space take up by iv*/
    msg += IV_LEN;
    len -= IV_LEN;
    if(len < 1){
      fprintf(stderr, "Fatal Error: After reading in iv, no message left. Ciphertext is ill formated.\n");
      ret = 1;
      goto cleanup;
    }
    fseek(ptext_file, 0, SEEK_SET);
    
    /* fetch initialisation vector by reading the first 10 bytes into key */
    char c = 0;
    for(int i = 0; i < IV_LEN; i++){
      c = fgetc(ptext_file);
      if(c == EOF){
	fprintf(stderr, "Fatal Error: Message too short - Could not read initialisation Vector\n");
        ret = 1;
	goto cleanup;
      }
      iv[i] = c;
    }
  } else {
    fprintf(stderr, "Fatal Error: Invalid option %s\n", argv[1]);
  }

  /* write en/decrypted message back into msg and onto disk*/
  init_key(key_file, iv, key);
  arcfour(msg, len, key);
  fwrite(msg, 1, len, out_file);

  

  //printf("%i",streq("aldkfjlaj\0", "aldjfladjff\0"));
  
 cleanup:
  fclose(ptext_file); 
  fclose(key_file); 
  fclose(out_file); 
  free(msg);
   
  return ret;
}
