#include <unistd.h>
#include <stdio.h>
#include "htab.h"
#include <assert.h>

/*************** list api ***************/
bool list_is_tail(list_ptr_t elt) {
  if (elt == NULL || elt->next == NULL) return true;
  return false;
}
inline bool list_is_parent(list_ptr_t elt) {
  return !list_is_tail(elt);
}

		    
void list_insert__(list_ptr_t elts, list_ptr_t elt, bool keepnext) {
  if (elts == NULL) {
    return ; 
  }
  while(list_is_parent(elts)){
    elts = elts->next;
  }
  elts->next = elt;
  if (!keepnext)  {
    elt->next = NULL;
  }
  return ;
}

void list_insert(list_ptr_t elts, list_ptr_t elt) {
  return list_insert__(elts, elt, false);
}

/* does not clear out the elt's next ptr, useful for merging lists */
void list_insert_keepnext(list_ptr_t elts, list_ptr_t elt) {
  return list_insert__(elts, elt, true); 
}

list_ptr_t list_remove(list_ptr_t elts, list_ptr_t elt) {
  list_ptr_t retval = NULL;
  if (elts == NULL || elt == NULL) {
    fprintf(stderr,"%s - elts or elt is NULL\n", __FUNCTION__);
    return NULL;
  }
  while(elts != NULL) {
    if (elts->next == elt) {
      elts->next = elts->next->next;
      //      elts->next = elts->next->next;  // swallow that item
      retval = elt;
    }
    elts = elts->next;
  }
  return retval;
}

/*************** end of list api ***************/



/****************************** Hash table api ***************/
// we create a hash type, this is the core of the hash table
// a hashtable can now be implemented on an array of addresses for
// the hash values
#define htab_entry_from_list(x_)\
  (hash_entry_t *) (container_of((x_), hash_entry_t, lst))

hash_entry_ptr_t htab_next(hash_entry_ptr_t hptr) {
  if(list_is_tail(hptr->lst)) {
    return NULL;
  }
  return htab_entry_from_list(hptr->lst->next);
}

hash_entry_t *  htab_search(hash_entry_ptr_t* htab, hash_entry_t *hptr) {
  ssize_t key = hptr->hashval & (HASHSIZ -1);
  printf("getting entry at key %ld\n", key);
  hash_entry_t * begin = htab[key]; // grab the first entry
  if (begin == NULL) {
    return NULL; // key not found
  }
  while (begin->hashval != hptr->hashval && begin != NULL) {
    begin = htab_next(begin); 
  }
  return begin;
}

void htab_insert(hash_entry_ptr_t* htab,  hash_entry_t * hptr) { // we can simplify this by using HASHSIZ
  ssize_t key = hptr->hashval & (HASHSIZ -1);
  if (htab[key] == NULL) {
    htab[key] = hptr;
  } else {
    list_t * lsptr = htab[key]->lst; // grab the list item
    list_insert(lsptr, hptr->lst);
  }
  printf("inserted entry  %lud at key %ld\n", hptr->hashval, key);
}

int  htab_delete(hash_entry_ptr_t *htab, hash_entry_t *hptr) {
  // first obtain the key of the item to be deleted.
  ssize_t key = hptr->hashval & (HASHSIZ-1);
  if (htab[key] == NULL) {
    return -1;
  }
  // search for the item in the buckeet
  // short circuit and check the first item.
  if (htab[key]->hashval == hptr->hashval) {
    htab[key] = htab_next(htab[key]); // set the first item.
    return 0;
  } 
  hash_entry_t * parent = htab[key]; // we already checked that the first item isnt
  hash_entry_ptr_t  next = htab_next(parent);
  while(next != NULL && next->hashval != hptr->hashval) {
    next = htab_next(parent);
  }
  if (next == NULL) {
    return -1; // key not found something is wrong.
  } else { // found it!
    list_remove(parent->lst, next->lst);
  }
  return 0;
}

hash_entry_ptr_t *  hashtable_new(void) {
  return calloc(HASHSIZ, sizeof(hash_entry_t *));
}

void hashtable_destroy(hash_entry_ptr_t * hptr) {
  free(hptr);
}

/*************** end ***************/

