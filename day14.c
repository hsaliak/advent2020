#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <sys/types.h>
#include "htab.h"
#include "lines.h"
#include "subprojects/xxHash-0.6.5/xxhash.h"

/* a key and value are two cells in an array */
typedef struct {
  uint64_t key;
  uint64_t value;
} memv_t;


typedef struct {
  char bitmask[36];
  list_t ls;
} mask_t;

#define MEMSIZE  8
typedef struct {
  //  char bitmask[36];
  mask_t * mask;
  memv_t *mem;
  size_t mlen;
  size_t mcap;
} memtab_t;

void mask_free(mask_t *mask) {
 if(!mask) return;
  mask_free(list_next(mask, mask_t));
  free(mask);
}

void memtab_writev(memtab_t * mt, memv_t mv) {
  // check if the value already exists, if so update it if not set item
  // has to be called from setv ONLY
  if (mt->mcap == mt->mlen) {
    mt->mem = realloc(mt->mem, sizeof(memv_t) * (mt->mcap + MEMSIZE));
    assert(mt->mem);
    mt->mcap = mt->mcap + MEMSIZE;
  }

  for(int i = 0 ; i < mt->mlen; i++) {
    if (mt->mem[i].key == mv.key) {
      mt->mem[i].value = mv.value;
      return;
    }
  }
  mt->mem[mt->mlen++] = mv;
}

void memtab_setv(memtab_t *mt, memv_t mv) {
  assert(mt->mcap > 0); // ensure that the memtab is initialized.
  // now lets figure out the mask.
  // process mask
  for(int i = 0; i < 36; i++) {
    if (mt->mask->bitmask[i] == '0') {
      // clear bit i 
      mv.value &= ~(1UL << (35-i));
  }else if (mt->mask->bitmask[i] == '1') {
      // set bit i on the value 
      mv.value |= 1UL << (35-i);
  }
  }
  // check if the value already exists, if so update it if not set item
  memtab_writev(mt, mv);
}

void decode_address(memtab_t * mt, char maskstr[36]) {
  mask_free(mt->mask); // get rid of the current masks
  // set it to the mask str
  mt->mask = calloc(sizeof(mask_t),1);
  strncpy(mt->mask->bitmask, maskstr, 36);
  mask_t * parent = NULL;
  for(mask_t * m = mt->mask; m != NULL && strchr(m->bitmask, 'X'); m = list_next(m, mask_t)) {
    assert(m);
    mask_t * m1 = calloc(sizeof(mask_t), 1);
    strncpy(m1->bitmask, m->bitmask, 36);
    mask_t *m2 = calloc(sizeof(mask_t), 1);
    strncpy(m2->bitmask, m->bitmask, 36);

    for(int i = 0; i < 36; i++) {
      // create two bitmasks, one for 0 and one for 1, add it to the list.
      if(m->bitmask[i] == 'X') {
	m1->bitmask[i] = 'd';
	m2->bitmask[i] = 'u';
	list_insert(&(m->ls), &(m1->ls));
	list_insert(&(m->ls), &(m2->ls));
      break;
      }
    }
    if (parent) {
      free(parent);

    }
    parent = m;
  }
  if (parent) {
    mt->mask = list_next(parent, mask_t);
    free(parent);
  }
  for(mask_t * m = mt->mask; m != NULL; m = list_next(m, mask_t)) {
  }

}

void memtab_setv2(memtab_t *mt, memv_t mv) {
  assert(mt->mcap > 0); // ensure that the memtab is initialized.
  // for each bitmask, process this
  for(mask_t * m = mt->mask; m != NULL; m = list_next(m, mask_t)) {
    memv_t temp = mv;

    // process masks, in 2, value is fixed, we need to modify keys
    for(int i = 0; i <36 ; i++) {
      if (m->bitmask[i] == 'd') {

	temp.key &= ~(1UL << (35-i));

      }  else if (m->bitmask[i] == 'u' || m->bitmask[i] == '1') {
	// set bit i on the value 
	temp.key |= 1UL << (35-i);
      }
    }
    memtab_writev(mt, temp);
  }
  return;
}

void memtab_init(memtab_t * mt) {
  mt->mlen = 0;
  mt->mcap = MEMSIZE;
  mt->mem = calloc(sizeof(memv_t), mt->mcap);
  mt->mask = calloc(sizeof(mask_t), 1);
  assert(mt->mem);
}
  

void memtab_clear(memtab_t * mt) {
  //  bzero(mt->mask->bitmask, sizeof(char) * 36);
  mt->mlen = 0;
  mask_free(mt->mask);
  if(mt->mem) 
    free(mt->mem);
}
		  

memv_t memtab_search(memtab_t * mt, uint64_t key) {
  for (int i = 0 ; i < mt->mlen; i++) {
    if (mt->mem[i].key == key) {
      return mt->mem[i];
    }
  }
    // nothing found return sentinel
    memv_t m = {0};
    return m;
}

void process_input(memtab_t * mt, lines_t * lines) {
  // we expect mv to be initialized here.. check
  assert(mt->mcap > 0);
  for (lines_t * l = lines; l != NULL ; l = lines_next(l)) {
      // 1. check if its a bitmask, if so update bitmask of mv
    if (strncmp(l->line, "mask", 3) == 0) {
      char * line = l->line;
      // grab the bitmask.
      strsep(&line, "=");
      if(isspace(line[0])) line++;
      //      strncpy(mt->mask->bitmask, line, 36);
      decode_address(mt, line);
    } else {
      // construct an memv
      memv_t mv;
      char * line = l->line;
      strsep(&line, "[");
      mv.key = atoi(line); // grabbed the key now grab the value
      strsep(&line, "=");
      mv.value = atoi(line);
      memtab_setv2(mt, mv);
    }
  }
}

uint64_t sum(memtab_t * mt) {
  uint64_t s  =0;
  for (int i = 0 ; i < mt->mlen;i++) {
    s += mt->mem[i].value;
  }
  return s;
}

int main() {
  char * filename = "day14_input.txt";
  lines_t * lines = read_lines(filename);
  //  print_lines(lines);

  memtab_t mt;
  memtab_init(&mt);
  process_input(&mt, lines);
  uint64_t s = sum(&mt);
   printf("%ld\n", s);
  memtab_clear(&mt);
  free_lines(lines);  
 
  return 0;
}
