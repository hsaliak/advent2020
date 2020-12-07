#pragma once
#include "htab.h" 

typedef struct  {
  char * line;
  list_t ls;
} lines_t;

#define lines_from_list(x) container_of((x), lines_t, ls)
#define lines_next(x) ((x) ? lines_from_list((x)->ls.next) : 0)
void free_lines(lines_t *);
lines_t * read_lines(char * );
void print_lines(lines_t * );
