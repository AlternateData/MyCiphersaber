#include "ciphersaber.h"

void swap(uint8_t * lst, uint8_t i, uint8_t j){
  uint8_t tmp = lst[i];
  lst[i] = lst[j];
  lst[j] = tmp;
}

char* gen_key(const char *secret,int secretlen, const char *iv){
  if(!secret || !iv){
    return NULL;
  }

  /* shorten key if too little space for initialisation vector */
  if(secretlen > ARC4_KEY_LEN){
    secretlen = ARC4_KEY_LEN - IV_LEN;
  }

  char* key = malloc(sizeof(*key) * ARC4_KEY_LEN);
  memcpy(key, secret, secretlen);
  memcpy(key+secretlen, iv, IV_LEN);
  for(int i = secretlen + IV_LEN; i < ARC4_KEY_LEN;i++){
    key[i] = key[i % (secretlen + IV_LEN)];
  }
  return key;

}

void init_state(state *  state, const char *key, int niter){
  if(!state || !key){
    return;
  }

  state->i= 0;
  state->j= 0;
  state->sbox = malloc(sizeof(uint8_t) * ARC4_KEY_LEN);

  for(int i = 0; i < ARC4_KEY_LEN;i++){
    state->sbox[i] = i;
  }

  /* key scheduling */
  uint8_t j = 0;
  for(int k = 0; k  < niter; k++){
    for(int i = 0; i < ARC4_KEY_LEN; i++){
      j = j + state->sbox[i] + key[i];
      swap(state->sbox, i, j);
    }
  }
}

char* gen_bytes(state*state, int n){
  char *bytes = malloc(sizeof(*bytes) * n);
  if(!bytes){
    return NULL;
  }

  if(!state){
    return NULL;
  }

  uint8_t index = 0;
  uint8_t *sbox = state->sbox;
  for(int k = 0; k < n; k++){
    state->i++;
    state->j += sbox[state->i];
    swap(sbox, state->i, state->j);

    index = sbox[state->i] + sbox[state->j];
    bytes[k] = sbox[index];
  }
  return bytes;
}

char* encrypt(const char *msg, size_t msglen,const char* secret, int niter){


  char* cipher = malloc(sizeof(*cipher)* (msglen + IV_LEN));
  char iv[IV_LEN] = {0};
  if(!cipher){
    return NULL;
  }

  /* generate initialisation vector */
  srand(time(NULL));
  for(int i = 0; i < IV_LEN; i++) {
    iv[i] = rand() % ARC4_KEY_LEN;
  }

  /* generate key and encrypt the message */
  char *key  = gen_key(secret, strlen(secret), iv);
  char* arc4cipher = arc4(msg, msglen,key, niter);

  if(!arc4cipher){
    return NULL;
  }

  /* load initialisation vector into cipher */
  memcpy(cipher, iv, IV_LEN);
  /* copy the message directly after the instruction vector into cipher */
  memcpy(cipher+IV_LEN, arc4cipher, msglen);

  free(key);
  free(arc4cipher);
  return cipher;
}

char* decrypt(const char *msg, size_t msglen, const char* secret, int niter){
  char iv[IV_LEN] = {0};
  char* cipher = malloc(sizeof(*cipher)* (msglen - IV_LEN));
  if(!cipher){
    return NULL;
  }

  /* load initialisation vector from message */
  memcpy(iv, msg, IV_LEN);
  char *key  = gen_key(secret, strlen(secret), iv);
  cipher = arc4(msg + IV_LEN, msglen,key, niter);

  free(key);
  return cipher;
}


char* arc4(const char *msg, size_t msglen,const char * key, int niter){

  if(!msg || !key){
    return NULL;
  }

  char* cipher = malloc(sizeof(char)* msglen);
  state* state = malloc(sizeof(state));
  /* setup arc4 key and init state for arc4 */
  init_state(state, key, niter);

  /* generate random bytes */
  char * bytes = gen_bytes(state, msglen);
  /*  generate the cipher text */
  for(size_t i= 0; i < msglen; i++){
    cipher[i] = msg[i] ^ bytes[i];
  }
  
  free(state->sbox);
  free(state);
  free(bytes);

  return cipher;
}
