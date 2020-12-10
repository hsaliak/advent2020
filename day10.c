#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include "numbers.h"

// input is the adapters in my bags
// can take an output of 1,2, or 3 volts LOWER produce the outage
// there is a built in - rated for 3 volts higher than the
// highest in my bag.
// 3, 9, 6 -> i have 12
// charging outlet has an effective rate of 0
// if you use every adapter - what is the distribution of
// differences between the outlet, the adapter and the device.
// here built in will be 19 + 3 = 22
// to use every adapter
//

int compare_int64(const void * lhs, const void * rhs) {
  return  *(int64_t *)lhs - * (int64_t *)rhs;
}

int compare_desc(const void * lhs, const void * rhs) {
  return  *(int64_t *)rhs - * (int64_t *)lhs;
}

void day10_1(int64_t * numbers, size_t len) {
  // sort the numbers
  qsort(numbers, len, sizeof(int64_t), compare_int64);
  // int64_t built_in = numbers[len-1] + 3;
  int64_t outlet = 0;
  int64_t difference[len+1];
  for (int64_t i = 0,  prev = outlet ; i < len; prev = numbers[i], i++) {
    difference[i] = numbers[i] - prev;
    printf("%ld\n", difference[i]);
  }
  difference[len] = 3; // last one
  // sort them
  qsort(difference, len+1, sizeof(int64_t), compare_int64); // sort them so the ones and threes are on either end.
  int one_sum = 0;
  int three_sum = 0;
  int count = 0;
  while (difference[count] == 1) {
    one_sum++;
    count++;
  }
  count = len;  // set it to the last item, remeber that this array's length is len+1
  while(difference[count] == 3) {
      three_sum++;
      count--;
    }
  printf("difference %d :%d multiplied %d\n", one_sum, three_sum, one_sum * three_sum);
}

bool is_parent(int64_t parent, int64_t child) { // returns true if 
  return child - parent <= 3;
}

uint64_t reaches_source(int64_t * numbers, size_t idx, size_t len,
			uint64_t * cache) {
  if (idx >= len) return 1; // we have reached the end. there is only 1 way
  uint64_t routes = 0 ; // number of ways to reach this particular item
  if (is_parent(0, numbers[idx]))  {
      routes =  1; // this is the last item, only one way to reach 0.
  }

  if (cache[numbers[idx]]) return cache[numbers[idx]];
  // is the sum of the number of ways to reach it's parents.  
  if ((idx+1 <= len) && is_parent(numbers[idx+1], numbers[idx]))
    routes += cache[numbers[idx+1]] ? cache[numbers[idx+1]] : reaches_source(numbers, idx+1, len, cache);
  if ((idx+2 <= len) && is_parent(numbers[idx+2], numbers[idx]))
    routes += cache[numbers[idx+2]] ? cache[numbers[idx+2]] : reaches_source(numbers, idx+2, len, cache)  ;
  if ((idx+3 <= len) && is_parent(numbers[idx+3], numbers[idx]))
    routes+= cache[numbers[idx+3]] ? cache[numbers[idx+3]] : reaches_source(numbers, idx+3, len, cache) ;
  cache[numbers[idx]] = routes;
  return routes;
}

void day10_2(int64_t * numbers, size_t len) {
  // its a tree walk.
  qsort(numbers, len, sizeof(int64_t), compare_desc);
  uint64_t * cache =  calloc(numbers[0] + 1, sizeof(uint64_t)); // create a cache
  assert(cache);
    uint64_t routes = reaches_source(numbers, 0, len, cache);
  free(cache);
  printf("routes => %lu\n", routes);
}

int main() {
  char * filename = "day10_input.txt";
  numbers_t n = {0};
  numbers_from_file(filename, &n);;
  day10_2(n.numbers, n.len);
  numbers_free(&n);
}
