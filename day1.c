#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "numbers.h"


void day1(int * numbers, size_t len) {
  
  for (int i = 0 ; i < len; i++)
    for (int j = i+1 ; j < len; j++) {
      if (numbers[i] + numbers[j] == 2020) {
	printf("%d\n", numbers[i] *numbers[j]);
	return;
      }
    }
  printf("nothing found!\n");
}
void day1_2(int * numbers, size_t len) {
  for (int i = 0 ; i < len; i++)
    for (int j = i+1 ; j < len; j++) {
      for (int k = j+1 ; k < len ; k++) {
      if (numbers[i] + numbers[j] + numbers[k] == 2020) {
	printf("%d\n", numbers[i] *numbers[j] * numbers[k]);
	return;
      }
      }
    }
}

int main() {
  numbers_t nums = {0};
  char * filename = "day1_input.txt";
  numbers_from_file(filename, &nums);
  /* now we have the numbers, lets see if they sum to 2020 */
  printf("%zu\n", nums.len);
   day1(nums.numbers, nums.len);
    day1_2(nums.numbers, nums.len);
  numbers_free(&nums);
  
  return 0;
}
