#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "htab.h"
#include "passwords.h"

/* does not handle next pointer, for that, see the next item */
void password_free(password_t * pwd) {
  free(pwd->passstr);
  free(pwd);
}

bool password_is_valid_1(password_t *pwd) {
  assert(pwd != NULL);
  char needle = pwd->c;
  char * haystack = pwd->passstr;
  int count = 0;
  size_t len = strlen(haystack);
  for (int i = 0 ; i < len; i ++) {
    if (needle == haystack[i]) {
      count++;
    }
  }
  if (count >= pwd->at_least && count <= pwd->at_most) {
    return true;
  }
  return false;
}

bool password_is_valid_2(password_t *pwd) {
  assert(pwd != NULL);
  char needle = pwd->c;
  char * haystack = pwd->passstr;
  int count = 0;
  size_t len = strlen(haystack);
  assert(pwd->at_least > 0 && pwd->at_most <= len);
  if (needle == haystack[pwd->at_least-1])
      count++;
  if (needle == haystack[pwd->at_most-1])
    count++;
  return count == 1;
}

int passwords_valid_1(password_t *pwd) {
  int valid = 0;
  while (pwd != NULL) {
    if (password_is_valid_1(pwd)) {
      valid++;
    }
    pwd = password_next(pwd);
  }
  return valid; 
}

int passwords_valid_2(password_t *pwd) {
  int valid = 0;
  while (pwd != NULL) {
    if (password_is_valid_2(pwd)) {
      valid++;
    }
    pwd = password_next(pwd);
  }
  return valid; 
}

/* frees the entire password list */
void password_freelist(password_t * pwd) { 
if (pwd == NULL) return;
  if (password_next(pwd) != NULL) {
    password_freelist(password_next(pwd));
  }
  password_free(pwd);
}

/* initializes and chains passwords that are read from a file */
int passwords_from_file(char * filename, password_t ** passwords) {
  FILE * stream = fopen(filename, "r");
  if (stream == NULL) {
    perror("fopen");
    return -1; /*signal error */
  }
  /* placeholders for password */
  if (*passwords != NULL) {
    fprintf(stderr, "warning: stomping over password argument in %s",__FUNCTION__);
    *passwords = NULL;
  }
  int at_least, at_most;
  char pc;
  char * passstr;
  list_t * listptr;
  while(fscanf(stream, "%d-%d %c: %ms", &at_least, &at_most, &pc,
	       &passstr) != EOF) {
    /* initialize a password */
    password_t * pwd = calloc(sizeof(password_t),1);
    pwd->at_least = at_least;
    pwd->at_most = at_most;
    pwd->c = pc;
    pwd->passstr = passstr;
  /* add it to the list */
  /* grab a pointer to the tail */
  if(*passwords == NULL) {
    /* we are at the head, grab the pointers we need */
    *passwords = pwd;
    listptr = &pwd->ls;
  } else { 
  /* else, append to list */
    list_insert(listptr, &pwd->ls);
    listptr = &pwd->ls; /* convenience, so we can insert at tail */
  }
  }
  return 0;
}



