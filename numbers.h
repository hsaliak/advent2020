#pragma once

/* numbers_t is a flexible structure for a bunch of numbers */
typedef struct {
  int * numbers;
  size_t len;
  size_t cap;

} numbers_t;

int numbers_grow(numbers_t *);

int numbers_append(numbers_t *, int);

void numbers_free(numbers_t *);

int numbers_from_file(char *, numbers_t *);
