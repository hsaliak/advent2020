#include <assert.h>
#include <stdlib.h>
#include <stdio.h>


#include "numbers.h"
int numbers_grow(numbers_t * nums) {
  if (nums->cap ==0) {
    nums->cap = 10;
    nums->numbers = realloc(nums->numbers, sizeof(int64_t) * nums->cap);
    if (nums->numbers == NULL) {
      goto fail;
  }
  }
  if (nums->len == nums->cap) {
    nums->cap = nums->cap * 2;
    nums->numbers = realloc(nums->numbers,nums->cap * sizeof(int64_t));
    if (nums->numbers == NULL) {
      goto fail;
    }
  }
  return 0;
  fail:
  perror("realloc");
  exit(EXIT_FAILURE);
  return -1;
}


int numbers_append(numbers_t *  nums, int64_t num) {
  if (nums->cap == 0 || nums->len == nums->cap) {
    assert(numbers_grow(nums) == 0);
  }
  nums->numbers[nums->len++] = num;
  return num;
}

void numbers_free(numbers_t * nums) {
  free(nums->numbers);
  nums->len = 0;
  nums->cap = 0;
  nums->numbers = NULL;
}

int numbers_from_file(char * filename, numbers_t * nums)  {
  /* fills up nums with numbers from file, caller responsible to free */
  FILE * stream = fopen(filename, "r");
  if (stream == NULL) {
    perror("fopen");
    return -1;
  }
  int64_t num = 0;
  while (fscanf(stream, "%ld\n", &num) != EOF) {
    numbers_append(nums, num);
  }
  fprintf(stderr, "read %zu numbers\n", nums->len);
  fclose(stream);
  return 0;
}

