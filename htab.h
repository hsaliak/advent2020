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

#define list_container(ptr, type) container_of(ptr, type, ls)
#define list_next(ptr, type) ((ptr) ? list_container((ptr)->ls.next, type): 0)


/*************** end list api ***************/


typedef struct hash_entry {
  uint64_t hashval;
  list_t lst;
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





  
  
  
