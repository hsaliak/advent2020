#include <stdio.h>
#include "numbers.h"
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

bool has_comb(int input, int64_t * numbers, size_t len) {
  for (int i = 0 ; i < len; i++)
    for (int j = i+1 ; j < len; j++) {
      int64_t sum = numbers[i] + numbers[j];
      if (sum == input) {
	return true;
      }
      sum = 0;
    }
  return false;
}

void day9_1(size_t preamble, int64_t * numbers, size_t len) {
  assert (len > preamble);
  size_t start = 0;
  for (int i = preamble ; i < len; i++, start++) {
    if (!has_comb(numbers[i], numbers+start, preamble)) {
	printf("missing number- %ld\n", numbers[i]);
	return;
      }
  }
}

int main() {
  numbers_t nums = {0};
  char * filename = "day9_input.txt";
  numbers_from_file(filename, &nums);
  printf("%zu", nums.len);
  for (int i = 0 ; i < nums.len; i++) {
    printf("%ld\n", nums.numbers[i]);
  }
  day9_1(25, nums.numbers, nums.len);
  numbers_free(&nums);
  return 0; }
