#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ARC4_KEY_LEN 256
#define IV_LEN 10
#define C1_KEY_ITER 1
#define C2_KEY_ITER 20

#define C1_FILE_EXT ".cs"
#define C2_FILE_EXT ".cs2"

typedef unsigned char uint8_t;

/* holds state of arc4 cipher */
typedef struct state{
  uint8_t i;
  uint8_t j;
  uint8_t *sbox;
} state;


void init_state(state* state, const char* key, int niter);
char* gen_iv();
char* gen_key(const char* secret, int secretlen, const char * iv);
char* gen_bytes(state* state, int n);

char* decrypt(const char *msg, size_t msglen, const char* secret, int niter);
char* encrypt(const char *msg, size_t msglen, const char* secret, int niter);

char* arc4(const char *msg, size_t msglen, const char * key, int niter);

void swap(uint8_t * lst, uint8_t i, uint8_t j){
  uint8_t tmp = lst[i];
  lst[i] = lst[j];
  lst[j] = tmp;
}


