#include <stdio.h>
#include <stdlib.h>
#include "passwords.h"

int main() {
  password_t * pwd = NULL;
  passwords_from_file("day2_input.txt", &pwd);
  printf("%d\n", passwords_valid_1(pwd));
  printf("%d\n", passwords_valid_2(pwd));
  password_freelist(pwd);
  return 0;
}

  
