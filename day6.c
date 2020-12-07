#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "htab.h"
#include "lines.h"

typedef struct {
  bool questions[26] ;
  list_t ls;
  
} survey_t;

#define survey_from_list(x) container_of((x), survey_t, ls)
#define survey_next(x) ((x) ? survey_from_list((x)->ls.next) : 0)

void surveys_free(survey_t * surveys) { 
  if (!surveys) return;
  if(surveys->ls.next) surveys_free(survey_next(surveys));
  free(surveys);
}

survey_t * process_survey_input(lines_t * lines) {
  if (!lines) return NULL;
  survey_t * head = NULL;
  for(survey_t * cur = NULL ; lines != NULL ;  lines = lines_next(lines)) {
    if (!strncmp(lines->line, "\n", 1)) {
      /* process the next survey */
	if (cur) { 
	  survey_t * newsurvey = calloc(sizeof(survey_t), 1);
	  list_insert(&cur->ls, &newsurvey->ls); // added the pointer.
	  // advance the cur ponter.
	  cur = survey_next(cur);
	}
    } else { 
      /* initial case */
      if (!cur) { 
	cur = calloc(sizeof(survey_t), 1);
	head = cur;
      }
      if (!cur) exit(EXIT_FAILURE);
      for(int i = 0 ; i  < strlen(lines->line); i ++ ) {
	if (isspace(lines->line[i])) continue; // should not happen but just add a test here.
	assert(lines->line[i] >= 'a' && lines->line[i] <= 'z'); /* check for valid input */
	cur->questions[lines->line[i] - 'a'] = true;
      }
    }
  }
    return head;
}

survey_t * process_survey_input_2(lines_t * lines) {
  if (!lines) return NULL;
  survey_t * head = NULL;
  for(survey_t * cur = NULL ; lines != NULL ;  lines = lines_next(lines)) {
    if (!strncmp(lines->line, "\n", 1)) {
      /* process the next survey */
	if (cur) { 
	  survey_t * newsurvey = calloc(sizeof(survey_t), 1);
	  list_insert(&cur->ls, &newsurvey->ls); // added the pointer.
	  // advance the cur ponter.
	  cur = survey_next(cur);
	  memset(cur->questions, 1, sizeof(bool) * 26); // initialize all the answers to 1.
	}
    } else { 
      /* initial case */
      if (!cur) { 
	cur = calloc(sizeof(survey_t), 1);
	memset(cur->questions, 1, sizeof(bool) * 26); // initialize all the answers to 1.
	head = cur;
      }
      if (!cur) exit(EXIT_FAILURE);
      bool row_answers[26] = {0}; // use this to process each row
      for(int i = 0 ; i  < strlen(lines->line); i ++ ) {
	if (isspace(lines->line[i])) continue; // should not happen but just add a test here.
	assert(lines->line[i] >= 'a' && lines->line[i] <= 'z'); /* check for valid input */
	// we are processing a line. create an empty buffer.
	row_answers[lines->line[i] - 'a'] = true; /* set the value that i care about */
      }
      /* a row has been processed, now apply that to the survey result */
      for (int i = 0 ; i < 26; i ++) {
	cur->questions[i] &=  row_answers[i];
      }
    }
  }
    return head;
}


int count_answers(survey_t * s) {
  int answered = 0;
  for (int i = 0; i < (sizeof(s->questions) /sizeof(s->questions[0])); i++ ) {
    if (s->questions[i]) {
      answered++;
    }
  }
  return answered;
}

int main(void)
{
  char * filename = "day6_input.txt";
  lines_t * lines = read_lines(filename);
  //  print_lines(lines);
  survey_t * s = process_survey_input(lines);
  int count = 0;
    for (survey_t * ss = s; ss != NULL; ss = survey_next(ss)) {
      count = count + count_answers(ss);
  }
  printf("day6_1 - %d\n", count);
  surveys_free(s);
  s = NULL;
  s = process_survey_input_2(lines);
  count = 0;
  for (survey_t * ss = s; ss != NULL; ss = survey_next(ss)) {
    count = count + count_answers(ss);
  }
  printf("day6_2 - %d\n", count);
  surveys_free(s);
  free_lines(lines);
  return 0;
}
