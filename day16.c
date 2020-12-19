#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "lines.h"
#include "htab.h"

typedef struct  {
  char * name;
  int range[4]; // first-second third-fourth
  list_t ls;
  bool found;
} field_t;

void field_print(field_t *f) {
  printf("%s: %d-%d, %d-%d\n", f->name, f->range[0], f->range[1], f->range[2], f->range[3]);
}

bool in_range(field_t * f, int val) {
  if (!f) return false;
  //  field_print(f);
  if (val < f->range[0] ||  val > f->range[1]) { // outside first range
    if (val < f->range[2] || val > f->range[3]) { 
      return false || in_range(list_next(f, field_t), val);  
    }
  }
  return  true; 
}

bool in_range2(field_t * f, int val) {
  if (!f) return false;
  //  field_print(f);
  if (val < f->range[0] ||  val > f->range[1]) { // outside first range
    if (val < f->range[2] || val > f->range[3]) { 
      return false;
    }
  }
  return  true; 
}

// clear fields
void field_free(field_t * field) {
  if(!field) return;
  free(field->name);
  field_free(list_next(field, field_t));
  free(field);
}

field_t * read_field(lines_t * line) {
  if (!line) return NULL;
  field_t * f= calloc(sizeof(field_t), 1);
  char * l = line->line;
  assert(f);
  char * start = strsep(&l, ":");
  f->name = strdup(start);
  assert(f->name);
  // get the first character.
  char * c = strsep(&l, "-");
  f->range[0] = atoi(c);
  f->range[1] = atoi(l);
  c = strsep(&l, "r");
  f->range[2] = atoi(l);
  strsep(&l, "-");
  f->range[3] = atoi(l);
  return f;
}

bool is_your_ticket(char * s)  {
  return (strncmp(s, "your ticket", strlen("your ticket")) ==0);
}
bool is_nearby_ticket(char *s) {
  return (strncmp(s, "nearby ticket", strlen("nearby ticket")) ==0);
}

bool is_field(char *s) {
  if (strlen(s) > 0 && isalpha(s[0])) return true;
  else return false;
}

int invalid_ticket(char * line, field_t * f) {
  // check every item.
  char tempbuf[strlen(line)];
  strncpy(tempbuf, line, strlen(line));
  char * needle = tempbuf;
  do {
    int val = atoi(needle);
    if (!in_range(f, val)) {
      return val;
    }
    strsep(&needle, ",");// move it to the next item.
  } while(needle != NULL);
  return 0;
}

bool error_rate(lines_t *line, field_t * f) {
  // we have a list of fields, check if a ticket is valid or not.
  int error_sum  = 0;
  for (lines_t * l = line; l != NULL; l = lines_next(l)) {
    int error =  invalid_ticket(l->line, f);
    //    printf("%d: %s\n", error, l->line);
    error_sum += error;
  }
  printf("%d\n", error_sum);
  return (error_sum != 0); // returns true if there is an error

}
void process_ticket(lines_t * lines) {
  field_t * fields = NULL;
  for (lines_t * l= lines ; l != NULL; l  = lines_next(l)) {
    if (is_your_ticket(l->line)) {
      l = lines_next(l); // skip this one.
    } else if (is_nearby_ticket(l->line)) {
      error_rate(lines_next(l), fields);
      printf("nt\n");
      break;
    } else if (is_field(l->line)) {
      printf("if\n");
      if (fields == NULL) {
	fields = read_field(l);
      } else {
	field_t * f = read_field(l);
	list_insert(&(fields->ls), &(f->ls));
      }
    }
  }
  field_free(fields);
}
// part 2: 
// get a list of valid tickets
// do BFS

typedef struct  {
  int * ints;
  int ncol; /* number of items in a row */
  int nrow; /* number of rows */
} ticket_block;

void ticket_block_free(ticket_block *tb) {
  if (!tb) return;
  free(tb->ints);
  free(tb);
}


ticket_block * ticket_block_create(lines_t * lines) {
  if (!lines) return NULL;
  int ncols = 0;
  int nrows = 0;
  for (int i = 0; lines->line[i] != '\0';i++ ) {
    if (lines->line[i] == ',') ncols++;
  }
  ncols++;

  for(lines_t * li = lines ; li != NULL; li = lines_next(li), nrows++);
  int * ints = calloc(sizeof(int) , ncols * nrows);
  // lets parse them
  int i = 0;
  for(lines_t * li = lines; li != NULL; li = lines_next(li)) {
    char * l = li->line;
    do {
      ints[i++] = atoi(l);
      strsep(&l, ",");// move it to the next item.
    } while(l != NULL);
  }
  
  ticket_block  * tb  = calloc(sizeof(ticket_block), 1);
  tb->ints = ints;
  tb->nrow = nrows;
  tb->ncol = ncols;
  return tb;
  
}

field_t *  field_for_col(field_t *  f, ticket_block * tb, int col) {
  bool is_valid = true;
  for (int j = 0; j < tb->nrow; j++) {
    if (!in_range2(f,  tb->ints[col + tb->ncol *j])) {
      is_valid = false;
      break;
    }
  }
  if (is_valid) return f;
  return NULL;
}


bool fields_matching_columns(field_t * f, ticket_block * tb, int ncol,
			     char ** fields) {
  if (f == NULL) return false;
  if (ncol >= tb->ncol) {
    return true;
  }
  if (ncol == 0) { // set found = false
    for(field_t * ff = f; ff != NULL; ff = list_next(ff, field_t)) {
      ff->found = false;
    }
  }
  // we look for fields that can match the first column,
  // second column and so on.
  for(field_t *  ff = f; ff != NULL; ff = list_next(ff, field_t)) {
    if (ff->found) continue;
    if (field_for_col(ff, tb, ncol)) { // we found something
      ff->found = true; 
      fields[ncol] = ff->name;
      bool connected  = fields_matching_columns(f, tb, ++ncol, fields);
      // do i need to backtrack? 
      if (!connected) { // rollback, this is not what we want.
	ff->found = false; 
	fields[--ncol] = NULL; 
      } else {
	printf("%s ->  %d\n", f->name, ncol);
	return true;
      }
    }
  }
  return false;
}
 
		      
void process_ticket2(lines_t * lines) {
  field_t * fields = NULL;
  lines_t *  valid_tickets = NULL;
  int nfields =0;
  for (lines_t * l= lines ; l != NULL; l  = lines_next(l)) {
    if (is_your_ticket(l->line)) {
      l = lines_next(l); // skip this one.
    } else if (is_nearby_ticket(l->line)) {
      l = lines_next(l); // advance once
      while (l != NULL) {
	if (!invalid_ticket(l->line, fields)) {
	  if (!valid_tickets) {
	    valid_tickets = calloc(sizeof(lines_t), 1);
	    valid_tickets->line = strdup(l->line);
	  } else {
	    lines_t *  tmp = calloc(sizeof(lines_t), 1);
	    tmp->line = strdup(l->line);
	    list_insert(&(valid_tickets->ls), &(tmp->ls));
	  }
	}
	l = lines_next(l);
      }
      break;
    } else if (is_field(l->line)) {
      nfields++;
      if (fields == NULL) {
	fields = read_field(l);
      } else {
	field_t * f = read_field(l);
	list_insert(&(fields->ls), &(f->ls));
      }
    }
  }

  ticket_block * tb = ticket_block_create(valid_tickets);
  int found[tb->ncol];
  memset(found, 0, sizeof(int) * tb->ncol);

  char * field_names[tb->ncol];
  memset(field_names, 0, sizeof(char *) * tb->ncol);
  fields_matching_columns(fields, tb, 0, field_names);
  for (int i = 0 ; i < tb->ncol; i++) {
    printf("%s => %d\n", field_names[i], i );
  }
  // all the valid tickets are in lines.
  ticket_block_free(tb);
  free_lines(valid_tickets);
  field_free(fields);
}

  
int main() {
  char * filename = "day16_input.txt";
  lines_t * lines = read_lines(filename);
  process_ticket2(lines);
  free_lines(lines);
  return 0;
}
  
