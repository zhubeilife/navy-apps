#include <unistd.h>
#include <stdio.h>
#include "fixedptc.h"

#define D 1.2
#define E 10
#define F 5

void fixedpt_print(fixedpt r)
{
  char num[20];
  fixedpt_str(r, num, -2);
  puts(num);
}

int main() {
  printf("Test fixedptc\n");
  printf("Using %d-bit precision, %d.%d format\n\n", FIXEDPT_BITS, FIXEDPT_WBITS, FIXEDPT_FBITS);

  printf("The most precise number: ");
  fixedpt_print(1);
  printf("The biggest number: ");
  fixedpt_print(0x7fffff00);

  fixedpt d = fixedpt_rconst(D);
  fixedpt e = fixedpt_fromint(E);

  printf("Test value set\n");
  printf("d: %f | %s\n", D, fixedpt_cstr(d, -1));
  printf("e: %d | %s\n", E, fixedpt_cstr(e, -1));

  printf("%f multi %d = %f | %s\n", D, E, D*E, fixedpt_cstr(fixedpt_muli(d, E), -1));
  printf("%f mult %d = %f | %s\n", D, E, D*E, fixedpt_cstr(fixedpt_mul(d, e), -1));

  printf("%f divi %d = %f | %s\n", D, E, D/E, fixedpt_cstr(fixedpt_divi(d, E), -1));
  printf("%f div %d = %f | %s\n", D, E, D/E, fixedpt_cstr(fixedpt_div(d, e), -1));

  printf("float of 0 is: %s\n", fixedpt_cstr(fixedpt_floor(fixedpt_fromint(0)), -1));
  printf("float of 0.5 is: %s\n", fixedpt_cstr(fixedpt_floor(fixedpt_rconst(0.5)), -1));
  printf("float of -0.5 is: %s\n", fixedpt_cstr(fixedpt_floor(fixedpt_rconst(-0.5)), -1));

  printf("ceil of 0 is: %s\n", fixedpt_cstr(fixedpt_ceil(fixedpt_fromint(0)), -1));
  printf("ceil of 0.5 is: %s\n", fixedpt_cstr(fixedpt_ceil(fixedpt_rconst(0.5)), -1));
  printf("ceil of -0.5 is: %s\n", fixedpt_cstr(fixedpt_ceil(fixedpt_rconst(-0.5)), -1));

  printf("\n==================Ctrl C======================\n");

  fixedpt A, B, C;


  printf("Here are some example numbers:\n");

  printf("PI: ");
  fixedpt_print(FIXEDPT_PI);
  printf("e: ");
  fixedpt_print(FIXEDPT_E);
  puts("");

  A = fixedpt_rconst(2.5);
  B = fixedpt_fromint(3);

  fixedpt_print(A);
  puts("+");
  fixedpt_print(B);
  C = fixedpt_add(A, B);
  puts("=");
  fixedpt_print(C);
  puts("");

  fixedpt_print(A);
  puts("*");
  fixedpt_print(B);
  puts("=");
  C = fixedpt_mul(A, B);
  fixedpt_print(C);
  puts("");

  A = fixedpt_rconst(1);
  B = fixedpt_rconst(4);
  C = fixedpt_div(A, B);

  fixedpt_print(A);
  puts("/");
  fixedpt_print(B);
  puts("=");
  fixedpt_print(C);

  printf("exp(1)=");
  fixedpt_print(fixedpt_exp(FIXEDPT_ONE));

  puts("");
  puts("sqrt(pi)=");
  fixedpt_print(fixedpt_sqrt(FIXEDPT_PI));

  puts("");
  puts("sqrt(25)=");
  fixedpt_print(fixedpt_sqrt(fixedpt_rconst(25)));

  puts("");
  puts("sin(pi/2)=");
  fixedpt_print(fixedpt_sin(FIXEDPT_HALF_PI));

  puts("");
  puts("sin(3.5*pi)=");
  fixedpt_print(fixedpt_sin(fixedpt_mul(fixedpt_rconst(3.5), FIXEDPT_PI)));

  puts("");
  puts("4^3.5=");
  fixedpt_print(fixedpt_pow(fixedpt_rconst(4), fixedpt_rconst(3.5)));

  puts("");
  puts("4^0.5=");
  fixedpt_print(fixedpt_pow(fixedpt_rconst(4), fixedpt_rconst(0.5)));

  return 0;
}
