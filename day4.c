#include <stdio.h>
#include <stdlib.h>
#include "htab.h"

typedef struct  {
  char * line;
  list_t ls;
} lines_t;

#define lines_from_list(x) container_of((x), lines_t, ls)
#define lines_next(x) ((x) ? lines_from_list((x)->ls.next) : 0)

void free_lines(lines_t *l) {
  if (!l) return;
  free(l->line);
  free_lines(lines_next(l));
}

lines_t * read_lines(char * filename) {
  lines_t * lines = NULL;

  return lines;
}
int main() {
  char * filename = "day4_input.txt";
  lines_t * lines = NULL;
  lines = read_lines(filename);
  free_lines(lines);
  return 0;
}
