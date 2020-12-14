#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "lines.h"


typedef struct { int *busnos;
  int timing;
  int len;
  int cap;}
bus_info_t;

void bus_info_append(char * str, bus_info_t * b) {
  if (!str) return;
  if (b->cap == 0) {
    b->busnos = realloc(b->busnos, sizeof(int) * 8);
    b->cap = 8;
  } else if (b->cap == b->len) {
    b->busnos = realloc(b->busnos, sizeof(int) * b->cap * 2);
    b->cap *=2;
  }
  if (str[0] == 'x')  b->busnos[b->len++] = 0; else {
  b->busnos[b->len++] = atoi(str);
  }
}

void bus_info_clear(bus_info_t * b) {
  free(b->busnos);
  b->busnos =NULL;
  b->timing = 0;
  b->cap = 0;
  b->len = 0;
}

void bus_info_print(bus_info_t * b) {
  printf("[timing %d] [len %d] [ cap %d]\n", b->timing, b->len, b->cap);
  for(int i = 0 ; i < b->len; i ++) {
    printf("T:%d\n", b->busnos[i]);
  }
}
void day13_1(lines_t *lines) {
  bus_info_t b = {0};

  sscanf(&lines->line[0], "%d\n", &b.timing);
  assert(lines_next(lines) != NULL);
  char * timings = lines_next(lines)->line;
  for(char * l = timings; l != NULL;) {
    char * token = strsep(&l, ",");
    bus_info_append(token, &b);
  }
  int earliest_bus =0, earliest_timing = INT32_MAX;
  for (int i = 0 ; i < b.len; i++) {
    if (b.busnos[i] == 0) continue;
    int timing = (b.busnos[i] * (1 + (b.timing / b.busnos[i]))) - b.timing;
    if (timing < earliest_timing) {
      earliest_timing = timing;
      earliest_bus = b.busnos[i];
    }
  }

   bus_info_print(&b);
  printf("Earliest [%d:%d] - answer [%d]\n", earliest_bus, earliest_timing,
	 earliest_bus * earliest_timing);
  bus_info_clear(&b);
}


/* extract the brute force solution out into a function */
int64_t brute_kernel(int * nums, int len, uint64_t start, uint64_t increment) {
  for(uint64_t counter = start; counter < UINT64_MAX; counter += increment) {
    bool found = true;
  for (int i = 0 ; i < len; i++) {
    if (nums[i] == 0) continue;
    if ((counter + i) % nums[i] !=0) {
      found = false;
      break;
    }
  }
  if (found) {
    start = counter;
    break;
  }
  }
  //  first = chinese_remainder(b.busnos,b.len);
  return start;
}

void day13_2_brute(lines_t *lines) {
  bus_info_t b = {0};

  sscanf(&lines->line[0], "%d\n", &b.timing);
  assert(lines_next(lines) != NULL);
  char * timings = lines_next(lines)->line;
  for(char * l = timings; l != NULL;) {
    char * token = strsep(&l, ",");
    bus_info_append(token, &b);
  }
  assert(b.busnos[0] != 0);
  uint64_t first = b.busnos[0];
  uint64_t increment = first;
  first = brute_kernel(b.busnos, b.len, 0, increment);
  printf("%ld\n", first);
  bus_info_clear(&b);

}

void day13_2(lines_t *lines) {
  bus_info_t b = {0};

  sscanf(&lines->line[0], "%d\n", &b.timing);
  assert(lines_next(lines) != NULL);
  char * timings = lines_next(lines)->line;
  for(char * l = timings; l != NULL;) {
    char * token = strsep(&l, ",");
    bus_info_append(token, &b);
  }
  assert(b.busnos[0] != 0);
  uint64_t first = b.busnos[0];
  uint64_t increment = first;
  /* strategy here is to consume the items one by one, after consuming 1 bus, we
     start from the common point, and multiply the bus timing with the previous
     bus timing to advance the increment range
  */

  for(int i = 1; i < b.len; i++) {
    if (b.busnos[i] ==  0) continue;
    printf("%ld,%ld, %d\n", first, increment, b.busnos[i]);
    uint64_t res = brute_kernel(b.busnos, i+1, first, increment);
    printf("res[%ld]\n", res);
    first = res;
    if (i < b.len-1) {
      increment = increment * b.busnos[i];
      printf("incrementing increment to %ld -- %d\n", increment, i);
    } else {
      printf("%ld\n", res);
      break;
    }
    // we found the first, now what is the lcm can we increase it
  }

  bus_info_clear(&b);

}

int main() {
  char * filename = "day13_input.txt";
  lines_t * lines = read_lines(filename);
  day13_2(lines);
  free_lines(lines);
}

