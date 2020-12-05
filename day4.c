#include <stdio.h>
#include <stdlib.h>
#include "htab.h"
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include "lines.h"



/* a list of tokens is either a passport or an NPC */  
typedef struct {
  char * key;
  char * value;
  list_t ls;
} tokens_t;

#define tokens_from_list(x) container_of((x), tokens_t, ls)
#define tokens_next(x) ((x) ? tokens_from_list((x)->ls.next) : 0)

#define tokens_key(token, x_) (0 == strncmp(#x_, token->key, strlen(#x_)))
#define tokens_value(token, x_) (0 == strncmp(#x_, token->value, strlen(#x_)))

void tokens_free(tokens_t * tok) {
  if (!tok) return;
  if (tok->key) {
    free(tok->key);
  }
  if (tok->value) {
    free(tok->value);
  }
  tokens_free(tokens_next(tok));
  free(tok);
}

/* a list of documents */
typedef struct {
  tokens_t * tokens;
  list_t ls;
} documents_t;

#define documents_from_list(x) container_of((x), documents_t, ls)
#define documents_next(x) ((x) ? documents_from_list((x)->ls.next) : 0)

void documents_free(documents_t * doc) {
  if (!doc) return;
  if (doc->tokens) {
    tokens_free(doc->tokens);
  }
  documents_free(documents_next(doc));
  free(doc);

}


bool tokens_valid_passport_part1(tokens_t * tok) {
  bool byr = false , iyr = false, eyr = false, hgt = false, hcl = false, ecl = false , pid = false , cid = true;
  // iterate tokens
  tokens_t * tmptok  = tok;

  while (tmptok != NULL) {
    /* set the boolean for the token found */
    if(tokens_key(tmptok, byr)) {
	byr = true;
    } else if (tokens_key(tmptok, iyr)) {
      iyr = true;
    } else if (tokens_key(tmptok, eyr)) {
      eyr = true;
    } else if (tokens_key(tmptok, hgt)) {
      hgt = true;
    } else if (tokens_key(tmptok, hcl)) {
      hcl = true;
    } else if (tokens_key(tmptok, ecl)) {
      ecl = true;
    } else if (tokens_key(tmptok, pid)) {
      pid = true;
    } else if (tokens_key(tmptok, cid)) {
      cid = true;
    }
    tmptok =  tokens_next(tmptok);
  }
  if (byr && iyr && eyr && hgt && hcl && ecl && pid && cid) {
    return true;
  }
  return false;
}


bool valid_byr(char * byr) {
  long int num = strtol(byr, NULL, 10);
  if (strlen(byr) == 4 && num >= 1920 && num <= 2002) 
    return true;
  return false;
}

bool valid_iyr(char * iyr) {
 long int num = strtol(iyr, NULL, 10);
 if (strlen(iyr) == 4 && num >= 2010 && num <= 2020) {
    return true;
 }
  return false;
}

bool valid_eyr(char * eyr) {
 long int num = strtol(eyr, NULL, 10);
 if (strlen(eyr) == 4 && num >= 2020 && num <= 2030) {
    return true;
 }
  return false;
}

bool valid_hgt(char * hgt) {
  size_t len = strlen(hgt);
  if (!strncmp(&hgt[len-2], "cm", 2)) {
    long int num = strtol(hgt, NULL, 10);
      if (num >= 150  && num <=193) {
	return true;
      }
  }
  else if (!strncmp(&hgt[len-2], "in" , 2)) {
      long int num = strtol(hgt, NULL, 10);
      if (num >= 59  && num <= 76) {
	return true;
      }
  }
  return false;
}
bool valid_pid( char * pid) {
  if (strlen(pid) != 9) return false;
  for(int i = 0 ; i < 9; i++) {
    if (!isdigit(pid[i])) return false;
  }
  return true;
}
bool valid_ecl(char * ecl) {
  char * valid_ecls[] = { "amb", "blu", "brn", "gry", "hzl", "oth", "grn"};
  int nitems = sizeof(valid_ecls) / sizeof(valid_ecls[0]);
  for (int i = 0 ; i < nitems; i++) {

    if (strncmp(ecl, valid_ecls[i], 3) == 0) return true;
  }
    return false;
}

bool valid_hcl(char * hcl) {
  if (*hcl != '#') return false;
  hcl++;
  if (strlen(hcl) != 6) return false;
  for(int i = 0 ; i < 6; i++) {
    if (!((hcl[i] >= '0' && hcl[i] <= '9') || (hcl[i] >= 'a' && hcl[i] <= 'f'))) {
      return false;
    }
  }
  
  return true;
}


int valid_documents_part1(documents_t * doc) {
  int count = 0;
while (doc != NULL) {
  if (tokens_valid_passport_part1(doc->tokens)) {
    count++;
  }
  doc = documents_next(doc);
 }
  
  return count;
}

bool tokens_valid_passport_part2(tokens_t * tok) {
  bool byr = false , iyr = false, eyr = false, hgt = false, hcl = false, ecl = false , pid = false , cid = true;
  // iterate tokens
  tokens_t * tmptok  = tok;

  while (tmptok != NULL) {
    //    printf("\t%s:%s\n", tmptok->key, tmptok->value);
    /* set the boolean for the token found */
    if(tokens_key(tmptok, byr) && valid_byr(tmptok->value)) {
	byr = true;
    } else if (tokens_key(tmptok, iyr) && valid_iyr(tmptok->value)) {
      iyr = true;
    } else if (tokens_key(tmptok, eyr) && valid_eyr(tmptok->value)) {
      eyr = true;
    } else if (tokens_key(tmptok, hgt) && valid_hgt(tmptok->value)) {
      hgt = true;
    } else if (tokens_key(tmptok, hcl) && valid_hcl(tmptok->value)) {
      hcl = true;
    } else if (tokens_key(tmptok, ecl) && valid_ecl(tmptok->value)) {
      ecl = true;
    } else if (tokens_key(tmptok, pid) && valid_pid(tmptok->value)) {
      pid = true;
    } else if (tokens_key(tmptok, cid) ) {
      cid = true;
    }
    tmptok =  tokens_next(tmptok);
  }
  if (byr && iyr && eyr && hgt && hcl && ecl && pid && cid) {
    return true;
  }
  return false;
}

int valid_documents_part2(documents_t * doc) {
  int count = 0;
while (doc != NULL) {
  if (tokens_valid_passport_part2(doc->tokens)) {
    count++;
  }
  doc = documents_next(doc);
 }
  
  return count;
}

/* reads lines and then returns a document or NULL */
/* advances lines past the point where it is read */

tokens_t * token_fromstr(char ** str) {
  tokens_t * token = NULL;
  char * key, * value;
  key = strsep(str, ":");
  if (!str)  return NULL; /* no delimiter found */
  token =calloc(sizeof(tokens_t),1);
  token->key = strdup(key);
  value = strsep(str, " \n");
  assert(value);
  token->value = strdup(value);
  //  printf("\ttokenizing\t:");
  //  printf("\t%s-%s\n", token->key, token->value);
  return token;
}

/* returns true if there is a blank line */
bool blankline(char * line) {
  if (strlen(line) == 0) return true;
  if (isspace(line[0])) return true;
  return false;
}

/* we break up every line into a key value pair and stop when we
   hit a new line */
documents_t * tokenize(lines_t ** lines) {
  if (!*lines)
    return NULL;
  documents_t * doc = NULL;
  tokens_t * tok = NULL;
  char * line = (*lines)->line;
  while (true) {
    if (line == NULL || *line == '\0') {
      // pick up the next line
      *lines = lines_next(*lines);
      if (!*lines || blankline((*lines)->line)) { /* end processing here */
	break;
      }
      line = (*lines)->line; /* grab the next line for processing */
    }
    tokens_t * tmptok;
    tmptok = token_fromstr(&line); // extract a token from pair.
    assert(tmptok);
    if (!tok) { 
      tok = tmptok;
    }  else {
      list_insert_keepnext(&tok->ls, &tmptok->ls);
    }
  }

  // allocate a doc and put the token inside it.
  if (tok != NULL) {
  doc = calloc(sizeof(documents_t), 1);
  doc->tokens = tok;
  assert(doc);
  }
  return doc;
}

documents_t * documents_from_lines(lines_t * lines) {
  documents_t *doc = NULL;
  documents_t *tmpdoc;
  while ((tmpdoc  = tokenize(&lines)) != NULL) {
    !doc ? doc = tmpdoc : list_insert(&doc->ls, &tmpdoc->ls);
    lines = lines_next(lines);
  }
  return doc;
}

int main() {
  char * filename = "day4_input.txt";
  lines_t * lines = NULL;
  lines = read_lines(filename);
  //  print_lines(lines);
  documents_t * docs;
  docs = documents_from_lines(lines);
  int count = valid_documents_part1(docs);
  int count2 = valid_documents_part2(docs);
  printf("%d - %d\n", count, count2);
  documents_free(docs);
  free_lines(lines);
  return 0;
}
