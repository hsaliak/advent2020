#include <stdint.h>
#include <stdio.h>
#include "htab.h"
#include "lines.h"
#include <regex.h>
#include <string.h>
#include <stdlib.h>
#include "subprojects/xxHash-0.6.5/xxhash.h"


/* read bags into a growable slice */
/* set its hash */
/* set references to this hash */



typedef struct {
  size_t count;
  char * bag_color; 
  list_t ls;  
} contains_t;

typedef struct bag {
  char * bag_color;
  hash_entry_t ht;
  contains_t * contains;
  bool can_hold; /* internal */
  list_t ls; /* lets get a bunch of bags */
} bag_t;



void bag_sethash(bag_t * bag) {
  // lets just hash by bag color? 
  size_t len = strlen(bag->bag_color);
  bag->ht.hashval = XXH64(bag->bag_color, len, 0);
}

#define bag_fromhash(x_) ((x_) ? container_of((x_), bag_t, ht) : 0)
#define bag_from_list(x) container_of((x), bag_t, ls)
#define bag_next(x) ((x) ? bag_from_list((x)->ls.next) : 0)

#define contains_t_from_list(x) container_of((x), contains_t, ls)
#define contains_t_next(x) ((x) ? contains_t_from_list((x)->ls.next) : 0)

void contains_free(contains_t * c) {
  if (!c) return;
  if (c->ls.next) contains_free(container_of(c->ls.next, contains_t, ls));
  free(c->bag_color);
  free(c);
}
void bag_free(bag_t * bag) { 
  if (!bag) return;
  if(bag->ls.next) bag_free(bag_next(bag));
  free(bag->bag_color);
  contains_free(bag->contains);
  free(bag);
}

/* strdups a color */
bag_t * bag_copycolor(bag_t * bag, char * color) {
  bag->bag_color = strdup(color);
  return bag;
}


void process_contains(bag_t * bag, char * line)
{
  // grab the regexes
  char * regex = "([[:digit:]][[:space:]][[:alnum:]]+[[:space:]][[:alnum:]]+)";
  regex_t preg;
  if (regcomp(&preg, regex, REG_EXTENDED)) {
    goto exit;
  }
  regmatch_t pmatch[2]; // we only have one capture group
  while(regexec(&preg, line, 2, pmatch, 0) != REG_NOMATCH) {
    if (pmatch[1].rm_so == -1) {
      printf("not supposed to happen\n");
      break;
    }
    size_t len = pmatch[1].rm_eo - pmatch[1].rm_so ;
    //    printf("len - %ld\n", len);
    char match[len+1];
    strncpy(match, line + pmatch[1].rm_so, len);
    match[len] = '\0';
    unsigned int count = atoi(match);
    char * color = match + 2; // first and the space.
    // lets add this package
    contains_t * co = calloc(sizeof(contains_t), 1);
    co->bag_color = strdup(color);
    co->count = count;
    if (!bag->contains){
      bag->contains = co;
    }
    else {
      list_insert(&(bag->contains->ls), &(co->ls));
    }
    line = line+ pmatch[1].rm_eo;
  }
 exit:

  regfree(&preg);
}


// step one, lets read all the input return a slice of bags?

bag_t *  process_bags( lines_t * lines, hash_entry_ptr_t * htab) {
  bag_t * bag = NULL;
  for (lines_t * l = lines; l != NULL; l = lines_next(l)) {
    char * line = l->line;
    size_t len = strstr(line, "bag") - line;
    char color[len];
    bzero(color, len);
    strncpy(color, line, len);
    color[len-1] ='\0';

    // we got the bag color, now create a bag
    bag_t * bptr = calloc(sizeof(bag_t),1);
    if (!bag) {
      bag = bptr;
    } else {
      list_insert(&(bag->ls), &(bptr->ls)); // add it to the list
    }
    bag_copycolor(bptr, color);
    bag_sethash(bptr);
    htab_insert(htab, &bptr->ht);
    // now process what the bag contains
    process_contains(bptr, line);
    }
  return bag;
}


hash_entry_t hash_color(char * color) {
  hash_entry_t he = {
		     .hashval = XXH64(color, strlen(color), 0)
  };
  return he;
}


bool can_contain(char * lookfor, bag_t * bag, hash_entry_ptr_t * allbags) {
  // a recursive function.
  // check if it is the bags
  if (bag->can_hold || strncmp(bag->bag_color, lookfor, strlen(lookfor)) ==0) {
    bag->can_hold = true;
    return true;
  }
  // get each of its child and run can_contain on it.
  for (contains_t * c = bag->contains; c != NULL; c = contains_t_next(c)) {
    // get the pointer to the bag frm all bags

    hash_entry_t hc = hash_color(c->bag_color);
    
    hash_entry_ptr_t hptr = htab_search(allbags, &hc); /* this can happen if there is a bag here that is not in all bags, wtf */
    if(!hptr){
    assert(hptr); // shold not happen
    }
    bag_t * b = bag_fromhash(hptr);
    if (can_contain(lookfor, b, allbags)) {
      bag->can_hold = true;
      return true;
    }
  }
  return false;
}



int day_7_1( bag_t * bag, char * lookfor, hash_entry_ptr_t * allbags) {
  // ok we are going to do a depth first search and see if every bag can hold a gold bag.
  int count = 0;
  /* here's the plan.
     1. for each bag, check if he _bag_ can hold using a dfs - either the color
matches or the bool can_hold is set to true.
     2. call recursive function which 1) checks if it is it, if not call itself
on each of it's children
     3. if the child call returns positive, return positive
     4. if not proceed to next child.
  */
  for(bag_t * bptr = bag; bptr != NULL; bptr = bag_next(bptr)) {
    if (can_contain(lookfor, bptr, allbags)) {
	count++;
    }
    }
  for(bag_t * bptr = bag; bptr != NULL; bptr = bag_next(bptr)) {
    bag->can_hold = false; // reset it
    }
  return count -1 ; /* always -1 to discount the actual bag itself */
}

/* count the number of bags contained within the bag color */
int day_7_2(char * lookfor, hash_entry_ptr_t * allbags) {
  /* for each bag
     2. for each child, count the bags it contains and add it all up, just dfs, how do you obtain -- by searching allbags. */
  // get the bag
  hash_entry_t h = hash_color(lookfor);
  bag_t * bag = bag_fromhash(htab_search(allbags, &h));
  assert(bag);
  int count = 0;
  for( contains_t * c = bag->contains ; c!= NULL ; c = contains_t_next(c)) {
    count += c->count + c->count * day_7_2(c->bag_color, allbags);
  }

  return count;
}

int main(void){
  char * filename = "day7_input.txt";
  lines_t * lines = read_lines(filename);
  hash_entry_ptr_t * colors = hashtable_new();

  bag_t * bag =  process_bags(lines, colors);
    char * lookfor = "shiny gold";
  int count =  day_7_1(bag, lookfor, colors);
  int count2 = day_7_2(lookfor, colors);
  printf("contains count - %d - %d\n", count, count2);
  hashtable_destroy(colors);
  bag_free(bag);
  free_lines(lines);
  return 0;
}
