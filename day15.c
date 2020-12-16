#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>

/* lets try to do this without all that jazz */
typedef struct  counter {
  size_t count;
  size_t previous;
  bool seen;
} counter_t;


counter_t *  counter_for(counter_t * counters, size_t clen, int lookfor, int count) {
  // create a counter for the value
  assert(lookfor < clen);
  counter_t * counter = &counters[lookfor];
  if (!counter->seen) {
    counter->seen = true;
    counter->previous =  -1;
    counter->count = count;
  } else {
    counter->previous = counter->count;
    counter->count = count;
  }
  return counter;
}
counter_t * counter_get(counter_t * counters, size_t clen, int lookfor) {
  assert(lookfor < clen);
  return &counters[lookfor];
}


void day15_1() {
  int starting[6] = {2,1,10,11,0,6};
  size_t clen = 30000000;
  counter_t * counters = calloc(sizeof(counter_t), clen);
  int turn = 0;
  int last = 0;
  turn += 6;
  for(int i = 0 ; i < 6; i++) {
    counter_for(counters, clen, starting[i], i);
    last = starting[i];
    printf("%d\n", last);
  }
  int spoken = 0;
  for(uint64_t i = turn; i <  clen ; i++) {
    // examine the prev number.
    counter_t * c = counter_get(counters, clen, last);
    if (c->previous == -1) { // is it the last time it was spoken
      spoken = 0;
    } else { // times is not 1
      spoken = c->count -c->previous;
    }
    // set the value of spoken to the time it was last spoken.
    last = spoken;
    // then update it here.
    c = counter_for(counters, clen, spoken, i);
  }
  printf("[spoken: %d\n", spoken);
  free(counters);

}
  
int main() {
  // lets create a hash table
  day15_1();

}
