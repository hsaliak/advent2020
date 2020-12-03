#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "htab.h"

typedef struct {
  char * row;
  size_t llen;
  list_t ls;
} map_t;

#define map_from_list(x_) (map_t *) container_of((x_), map_t, ls)
#define map_next(x_) map_from_list((x_)->ls.next)

void map_freelist(map_t * m) {
  free(m->row);
  if(m->ls.next) {
    map_freelist(map_next(m));
  }
  free(m);
}


int map_from_file(char * filename, map_t ** map_pptr) {
  FILE * stream = fopen(filename, "r");
  *map_pptr = NULL; // initialize it to 0.
  if (stream == NULL) {
    perror("fopen");
    return -1;
  }
  // read lines and then add them
  char * line;
  map_t * mptr;
  list_t *listptr;
  while(fscanf(stream, "%ms\n", &line) != EOF) {
    mptr = calloc(sizeof(map_t), 1);
    assert(mptr);
    mptr->row = line;
    mptr->llen = strlen(line);
    if (!*map_pptr) {
      /* initialize the first item */
      *map_pptr = mptr;
      listptr = &mptr->ls;
    } else {
      list_insert(listptr, &mptr->ls);
      listptr = &mptr->ls; /* same some on the insert */
    }
  }
  return 0;
}

void print_map(map_t * mptr) {
  while(mptr != NULL) {
    printf("%s\n", mptr->row);
    mptr = map_next(mptr);
  }
}

int traverse(map_t * mptr, int right, int down) {
  int start = 0;
  int count = 0;
  /* iterate until we hit the bottom */
  while (true) {
    // go down by the steps needed.
    for(int i = 0  ; i < down ; i++) {
      mptr = map_next(mptr);
      if(mptr == NULL) {
	goto exit; // you can fall off, if you do, just return
      }
    }
    start = start + right;
    if (mptr->row[start % mptr->llen] == '#') {
      count++;
    }
  }
 exit:
  return count; 
}

int main() {
  char * input = "day3_input.txt";
  map_t * mptr;
  int rv;
  rv = map_from_file(input, &mptr);
  if (rv != 0) {
    return -1;
  }
  //  print_map(mptr);
  uint64_t count1, count2, count3, count4, count5, mult;
  count1 = traverse(mptr, 1, 1);
  count2 = traverse(mptr, 3, 1);
  count3 = traverse(mptr, 5, 1);
  count4 = traverse(mptr, 7, 1);
  count5 = traverse(mptr, 1, 2);
  mult = count1 * count2 * count3 * count4 * count5;
  printf("%lu %lu %lu %lu %lu  %lu\n", count1, count2, count3, count4, count5, mult);

  map_freelist(mptr);
  return 0;
}
