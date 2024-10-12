#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/random.h>
#include <gmp.h>

#define E_VAL 65537
#define E_VAL62 "H33"
#define MR_ROUNDS 40
#define MAXLEN 1376 /* ceil(8192/log2(62)) */
#define S_(x) #x
#define S(x) S_(x)

void setrandnum(mpz_t num, int bits) {

  int bytes = (bits%8 == 0) ? bits/8 : bits/8+1; 
  char buf[bytes];
  getrandom(buf, bytes, 0);
  mpz_import(num, bytes, 1, 1, 0, 0, buf);
  mpz_fdiv_q_2exp(num, num, bytes*8-bits);

}

void randnum(int bits) {

  mpz_t num;
  mpz_init(num);
  setrandnum(num, bits);
  gmp_printf("%Zd\n", num);
  mpz_clear(num);

}

void key(int bits) {

  mpz_t num;
  mpz_init(num);
  do {
    setrandnum(num, bits);
  } while (mpz_sizeinbase(num, 2) != bits);
  mpz_out_str(stdout, 62, num);
  printf("\n");
  mpz_clear(num);

}

void setprime(mpz_t num, int bits) {

  do {
    setrandnum(num, bits);
  } while (mpz_sizeinbase(num, 2) != bits || mpz_probab_prime_p(num, MR_ROUNDS) == 0);

}

void prime(int bits) {

  mpz_t num;
  mpz_init(num);
  setprime(num, bits);
  gmp_printf("%Zd\n", num);
  mpz_clear(num);

}

void setprime2(mpz_t num, int bits) {

  mpz_t num2;
  mpz_init(num2);
  do {
    setrandnum(num, bits);
    mpz_fdiv_q_ui(num2, num, 3);
  } while (mpz_sizeinbase(num2, 2) != bits-1 || mpz_probab_prime_p(num, MR_ROUNDS) == 0);
  mpz_clear(num2);

}

void setprime1(mpz_t num, int bits) {

  mpz_t num2;
  mpz_init(num2);
  do {
    setrandnum(num, bits);
    mpz_mul_ui(num2, num, 3);
  }
  while (mpz_sizeinbase(num, 2) != bits || mpz_sizeinbase(num2, 2) != bits+1 || mpz_probab_prime_p(num, MR_ROUNDS) == 0);
  mpz_clear(num2);

}

void setprv(mpz_t priv[2], int bits) {

  mpz_t num1, num2, e_val, lambda;
  mpz_inits(num1, num2, e_val, lambda, '\0');

  if (bits%2 == 0) {
    setprime2(num1, bits/2);
    setprime2(num2, bits/2);
  } else {
    setprime1(num1, (bits+1)/2);
    setprime1(num2, (bits+1)/2);
  }
  mpz_mul(priv[0], num1, num2);

  mpz_sub_ui(num1, num1, 1);
  mpz_sub_ui(num2, num2, 1);
  mpz_lcm(lambda, num1, num2);

  mpz_set_ui(e_val, E_VAL);
  mpz_invert(priv[1], e_val, lambda);

  mpz_clears(num1, num2, e_val, lambda, '\0');

}

void prv(int bits) {

  mpz_t priv[2];
  mpz_inits(priv[0], priv[1], '\0');
  setprv(priv, bits);
  mpz_out_str(stdout, 62, priv[0]);
  printf(" ");
  mpz_out_str(stdout, 62, priv[1]);
  printf("\n");
  mpz_clears(priv[0], priv[1], '\0');

}

void pair(int bits) {

  mpz_t priv[2];
  mpz_inits(priv[0], priv[1], '\0');
  setprv(priv, bits);
  mpz_out_str(stdout, 62, priv[0]);
  printf(" ");
  mpz_out_str(stdout, 62, priv[1]);
  printf("\n");
  mpz_out_str(stdout, 62, priv[0]);
  printf(" %s\n", E_VAL62);
  mpz_clears(priv[0], priv[1], '\0');

}

void pub(void) {

  char c;
  while((c = getchar()) != ' ') putchar(c);
  printf(" %s\n", E_VAL62);

}

void func(char* str_in, int base_in, int base_out) {

  mpz_t key[2], val_in, val_out;
  mpz_inits(key[0], key[1], val_in, val_out, '\0');
  char key0[MAXLEN+1], key1[MAXLEN+1];

  scanf("%"S(MAXLEN)"s %"S(MAXLEN)"s", key0, key1);
  mpz_set_str(key[0], key0, 62);
  mpz_set_str(key[1], key1, 62);
  mpz_set_str(val_in, str_in, base_in);
  mpz_powm_sec(val_out, val_in, key[1], key[0]);
  mpz_out_str(stdout, base_out, val_out);
  printf("\n");

  mpz_clears(key[0], key[1], val_in, val_out, '\0');

}

int main (int argc, char *argv[]) {

  if (strcmp(argv[1], "gen") == 0 && argc == 4) {

    if (strcmp(argv[2], "random") == 0) { randnum(atoi(argv[3])); }
    else if (strcmp(argv[2], "key") == 0) { key(atoi(argv[3])); }
    else if (strcmp(argv[2], "prime") == 0) { prime(atoi(argv[3])); }
    else if (strcmp(argv[2], "private") == 0) { prv(atoi(argv[3])); }
    else if (strcmp(argv[2], "pair") == 0) { pair(atoi(argv[3])); }
    else { printf("error: missing/incorrect argument(s)\n"); }

  } else if (strcmp(argv[1], "pub") == 0) { pub(); } // pipe private key to stdin

  else if (argc == 3) {

    if (strcmp(argv[1], "enc") == 0 || strcmp(argv[1], "dec") == 0) { func(argv[2], 62, 62); } // pipe public/private key to stdin
    else if (strcmp(argv[1], "sign") == 0) { func(argv[2], 16, 62); } // pipe private key to stdin
    else if (strcmp(argv[1], "verify") == 0) { func(argv[2], 62, 16); } // pipe private/public key to stdin
    else { printf("error: missing/incorrect argument(s)\n"); }

  } else { printf("error: missing/incorrect argument(s)\n"); }

}
