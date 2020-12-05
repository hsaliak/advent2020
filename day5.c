#include <bits/stdint-uintn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "htab.h"
#include "lines.h"
#include "numbers.h"

#define ROWS 127
#define PASSROWS 7 /* number of rows */
#define PASSCOLS 3
#define COLUMNS 7


int pass_row(lines_t * lines) {
  char * line = lines->line;
  int lower = 0;
  int upper = ROWS;
  for ( int i = 0 ; i < PASSROWS; i++) {
    char c = line[i];
    assert(c  == 'F' || c == 'B');
    if (c == 'F') { // halve the upper 
      upper = lower + (upper - lower) / 2;
    } else if (c == 'B') { // add half of the count to the lower
      lower = (upper - lower) / 2 + lower +1 ;
    }
  }
  assert (lower == upper); // we should have converged
  return upper;
}

int pass_col(lines_t * lines) {
  char * line = lines->line;
  int lower = 0;
  int upper = COLUMNS;
  for (int i = PASSROWS ; i < PASSCOLS+ PASSROWS; i++) {
    char c= line[i];
    if ( c== 'L') {
      upper = lower + (upper - lower) / 2;
    } else if (c == 'R') {
      lower = (upper - lower) /2 + lower +1;
    }
  }
  assert( lower == upper);
  return upper;
}

int compare_ints(const void * lhs, const void * rhs) {
  return  *(int *)lhs - * (int *)rhs;
}

int find_seat(lines_t * lines) {
  // what are the min seat IDs?
  numbers_t nums = {0};
  int seat = 0;
  for (lines_t * ls = lines; ls != NULL; ls = lines_next(ls)) {
    int rowcount = pass_row(ls);
    int colcount = pass_col(ls);
    int id = rowcount * 8 + colcount;
    numbers_append(&nums, id);
  }
  // sort the numbers
  qsort(nums.numbers, nums.len, sizeof(int), compare_ints);
  for(int i = 0 ; i < nums.len -1;i++) {
    if (nums.numbers[i+1] - nums.numbers[i] != 1) {
      seat = nums.numbers[i] +1;
  }
}
  numbers_free(&nums);
  return seat ;
}

int main() {
  char * filename = "day5_input.txt";
  lines_t * lines = read_lines(filename);
  int max_id = 0;
  for (lines_t * ls = lines; ls != NULL; ls = lines_next(ls)) {
    int rowcount = pass_row(ls);
    int colcount = pass_col(ls);
    int id = rowcount * 8 + colcount;
    if (id > max_id) max_id = id;
  }
  printf("%d\n", max_id);
  int my_seat = find_seat(lines);
  printf("seat is %d\n", my_seat);
  free_lines(lines);
  return 0;
}
