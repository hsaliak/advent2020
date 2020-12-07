#pragma once

#include <stdint.h>
#include <malloc.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#define container_of(ptr, type, member) \
  ((ptr) ? ((type *)((char *)(ptr)  - offsetof(type, member))) : 0)

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


#define slice_(TYPE) typedef struct {					\
    size_t len;								\
    size_t cap;								\
    TYPE * data;								\
  } slice_ ##TYPE;							\
  void slice_ ##TYPE##_push(slice_ ##TYPE *  slc , TYPE * item) {	\
  if (!slc->cap) slc->cap = 1; /* initialize */				\
  if (slc->len == 0 || slc->cap == slc->len) {\
  slc->data = realloc((void *) slc->data, slc->cap * sizeof(TYPE)  * 2);	\
  slc->cap = slc->cap * 2;						\
  }\
  memcpy(slc->data + slc->len++, item, sizeof(*item));			\
  }
  




  
  
  
