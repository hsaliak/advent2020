#include <stdlib.h>
#include <string.h>
#include "lines.h"

void free_lines(lines_t *l) {
  if (!l) return;
  free_lines(lines_next(l));
  free(l->line);
  free(l);
}

lines_t * read_lines(char * filename) {
  lines_t * lines = NULL;
  FILE * stream   = fopen(filename, "r");
  if (!stream) {
    perror("fopen");
    return NULL;
  }
  char * line = NULL;
  size_t sz = 0;
  list_t * parent = NULL; 
  while(getline(&line, &sz, stream) != -1) {
    lines_t * templn;
    templn = calloc(sizeof(lines_t), 1);
    assert(templn);
    templn->line = line;
    if (lines == NULL) {
      lines = templn;
      parent = &lines->ls; /* initialize parent */
    } else {
      list_insert(parent, &templn->ls);
      parent = parent->next; /* optimization */
    }
    line = NULL; /* re-set it, the lines_t structure has taken ownership */
  }
  if (line != NULL) free(line);
  fclose(stream);
  return lines;
}
 
void print_lines(lines_t * lines) {
  while (lines != NULL) {
    if (!strncmp(lines->line, "\n",1))  printf("newline\n");
    else  
    printf("%s", lines->line);
    lines = lines_next(lines);
    }
  }

