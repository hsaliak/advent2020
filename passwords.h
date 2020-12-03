#pragma once
#include <stdbool.h>
#include "htab.h" 

typedef struct {
  int at_least;
  int at_most;
  char c;
  char * passstr; /* dynamically allocated, takes ownership  */
  list_t ls; /* to make a linked list */
} password_t;

/* returns length of passwords read from file */
int passwords_from_file(char *, password_t ** );
bool password_is_valid_1(password_t *);
bool password_is_valid_2(password_t *);
#define password_from_list(x_) (password_t *)container_of((x_), password_t, ls)
// #define password_next(x_) ((x_) ? password_from_list((x_)->ls.next) : 0)
#define password_next(x_) (password_from_list((x_)->ls.next))

int passwords_valid_1(password_t *);
int passwords_valid_2(password_t *);
void password_free(password_t *);
void password_freelist(password_t *);
