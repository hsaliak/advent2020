#pragma once

#include <stdint.h>
#include <malloc.h>
#include <stdbool.h>
#include <assert.h>

#define container_of(ptr, type, member) \
  ((type *)((char *)(ptr)  - offsetof(type, member)))

#define HASHSIZ 4096


// a singly linked list 
typedef struct list {
  struct list * next;
} list_t;

typedef list_t *list_ptr_t;

// define the list api
void list_insert(list_ptr_t, list_ptr_t); // add to the chain
list_ptr_t
    list_remove(list_ptr_t,
                list_ptr_t); // remove the item from the list and return it

bool list_is_parent(list_ptr_t);
bool list_is_tail(list_ptr_t); // if the next element is null.
void list_insert_keepnext(list_ptr_t, list_ptr_t);

/*************** end list api ***************/

typedef struct hash_entry {
  uint64_t hashval;
  list_ptr_t lst;
  //  struct hash_entry * next; // so that we can chain into the bucket.
} hash_entry_t;

typedef hash_entry_t *hash_entry_ptr_t;

hash_entry_ptr_t htab_next(hash_entry_ptr_t );

/* search for hash_entry in the given hash table. */
hash_entry_t * htab_search(hash_entry_ptr_t *, hash_entry_t *);

/* insert item into a hash table */
void htab_insert(hash_entry_ptr_t * htab, hash_entry_t * hptr);
int  htab_delete(hash_entry_ptr_t *htab, hash_entry_t *hptr);

/* TODO: htab_delete */

/* create a hash table */
hash_entry_ptr_t * hashtable_new(void);

/* destroy allocated memory for the hash table */
void hashtable_destroy(hash_entry_ptr_t *hptr);


/*
typedef struct int_slice {
  int * items;
  ssize_t len;
  ssize_t cap;
} int_slice_t;

int int_slice_push(int_slice_t * is, int item) {
  if (is->len == is->cap) {
    void * rv;
    rv = realloc(is->items, sizeof(int) * 2 * is->cap);
    assert(rv != NULL);

    is->items = (int *) rv;
  }
  is->items[is->len++] = item;
  return 0;
}

int int_slice_pop(int_slice_t* is) {
  assert(is->len > 0);
  is->len -=1; // reduce the length 
  return is->items[is->len];
}
*/



