/* My own implementation of the CipherSaber algorithm
 * Author: Louis Kronberg
 * Date: 26.02.2019
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string.h>


#define ARC4_KEY_LEN 256
#define IV_LEN 10



/* utility mehthods */
void swap(uint8_t * lst, uint8_t i, uint8_t j){
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
void init_sbox(uint8_t *sbox, const char * key){
  for(int i = 0; i < ARC4_KEY_LEN; i++){
    sbox[i] = i;
  } 

  uint8_t j = 0;
  for(int i = 0; i < ARC4_KEY_LEN; i++){
    j = (j + sbox[i] + key[i]);
    swap(sbox, i, j);
  }
}

void init_key(char * key, char * iv){

  /*compute the length of the key */
  int i = 0;
  while(key[i] != '\n')
    i++;

  printf("%s(%i)\n", key, i);

  /* load iv into buffer*/
  for(int j = i; j - i  < IV_LEN; j++){
    key[j]  = iv[j - i];
  }

  printf("Key: %s\n",key );

  /* fill up the remaining space with the key */
  i+=IV_LEN;
  for(int j = i; j < ARC4_KEY_LEN; j++){
    key[j] = key[j % i];
  }
  printf("Final Key: %s\n", key);
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
  printf("ptext_file has: %i\n", (int)len);
  rewind(ptext_file);
  char * msg = malloc(sizeof(char)*len);

  if(msg == NULL){
    /* failure to allocate this memory is fatal. Abort. */
    fprintf(stderr, "Fatal Error: Failed to allocate memory for file contents of %s\n", argv[2]);
    ret = 1;
    goto cleanup;
  }


  /* this will hold the initialisation vector */
  char iv[IV_LEN] = {0};

  if(streq(argv[1], "encrypt")){


    printf("\n");
    /* seed prng used for initialisation vector */
    srand(time(NULL));

    /* create initialisation vector  */
    for(int i = 0; i < IV_LEN; i++) {
      iv[i] = rand() % ARC4_KEY_LEN;
      printf("%i (%c) ", iv[i], iv[i]);
    }
    printf("\n");

    int written = fwrite(iv, 1, IV_LEN, out_file);
    printf("Bytes written: %i\n", written);
    printf("%s\n", iv);
    //goto cleanup;

  } else if(streq(argv[1], "decrypt")){

    printf("Decrypting!\n");
    /* decrease the length by the space take up by iv*/
    len  = len -  IV_LEN;
    if(len < 1){
      fprintf(stderr, "Fatal Error: After reading in iv, no message left. Ciphertext is ill formated.\n");
      ret = 1;
      goto cleanup;
    }
    
    /* fetch initialisation vector by reading the first 10 bytes into key */
    for(int i = 0; i < IV_LEN; i++){
      iv[i] = fgetc(ptext_file);
      printf("%i (%c) ", iv[i], iv[i]);
    }
    printf("\n");
  } else {
    fprintf(stderr, "Fatal Error: Invalid option %s\n", argv[1]);
  }

  /* this will hold the cipher key */
  char key[ARC4_KEY_LEN] = {0};
  fgets(key, ARC4_KEY_LEN, stdin);
  init_key(key, iv);
  /* write en/decrypted message back into msg and onto disk*/
  fread(msg, len, 1, ptext_file);
  arcfour(msg, len, key);
  int written = fwrite(msg, 1, len, out_file);
  printf("Bytes written: %i\n", written);
  

  //printf("%i",streq("aldkfjlaj\0", "aldjfladjff\0"));
  
 cleanup:
  fclose(ptext_file); 
  //fclose(key_file); 
  fclose(out_file); 
  free(msg);
   
  return ret;
}
