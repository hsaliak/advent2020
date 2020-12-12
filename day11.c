#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "ctype.h"
 #include "lines.h"
    

typedef struct {
  size_t width;
  size_t length;
  char * cells;
} board_t;

board_t * board_create(lines_t * lines) {
  size_t width = strlen(lines->line); // exclude the newline.
  // count the number of lines there are
  size_t length  = 0;
  for(lines_t * l = lines ; l != NULL;l = lines_next(l), length++);
  
  char * cells = calloc(sizeof(char) , length * width);
  board_t * board = calloc(sizeof(board_t), 1);
  board->cells = cells;
  // populate cells
  board->width = width;
  for(lines_t * l = lines ; l != NULL;l = lines_next(l), board->length++) {
    if (isspace(l->line[0])) break;
    strncpy(board->cells + board->length * board->width,
	    l->line, board->width);
    }
  assert(board->length == length);
  return board;
}

board_t * board_from(board_t * board) {
  board_t * board2 = calloc(sizeof(board_t), 1);
  board2->width = board->width;
  board2->length = board->length;
  char * cells = calloc(sizeof(char), board->length * board->width);
  board2->cells = cells;
  memcpy(board2->cells, board->cells, sizeof(char)* board->length * board->width);
  return board2; 
}

char board_at(board_t * b, int x, int y) {
  if ((x >= 0 && y>=0) && (x < b->width && y < b->length)) {
    return b->cells[y* b->width + x];
  }
  else return '\0';
}

void board_set(board_t *b, int x, int y, char c) {
  if ((x >= 0 && y>=0) && (x < b->width && y < b->length)) {
    b->cells[y* b->width + x] = c;
  }
}

bool is_empty(board_t * b, int x, int y) {
  char c =  board_at (b, x, y);
  if (!(c == '#')) return true;
  return false;
}
bool is_occupied(board_t * b, int x, int y) {
  char c =  board_at (b, x, y);
  //  printf("is_opccupied [%d,%d,%c]\t\n", x, y, c);
  if (c == '#') return true;
  return false;
}


int adjacents(board_t * b, int x, int y, bool (*op)(board_t *board, int, int)) {
  // count the number of adjacents that meet a criteria.
  int count = 0;
  for (int i = x-1; i <= x+1;i++) {
    for (int j = y-1; j <= y+1; j++) {
      if (i == x && j == y){
	continue;
      }
      count += op(b, i, j);
    }    
  }
  return count;
}

int grid_search(board_t * b, int x, int y) {
  // count the number of seats in the line of sight that meet a criteria.
  int count = 0;
  // step 0 , fix y, linear scan x inc
  for (int i = x+1 ; i < b->width; i ++ ) {
    char c = board_at(b,i,y);
    if (c == '#' || c == 'L') {
      if (c == '#') count +=1;
      break;
    }
  }
  // step1 fix y linear scan x dec
  for (int i = x-1 ; i >= 0; i -- ) {
    char c = board_at(b,i,y);
    if (c == '#' || c == 'L') {
      if (c == '#') count +=1;
      break;
    }
    }
  // step 2 fix x, linear scan y
  for (int i = y+1 ; i < b->length; i++) {
    char c = board_at(b,x,i);
    if (c == '#' || c == 'L') {
      if (c == '#') count +=1;
      break;
    }
  }
    // step 3 fix x, linear scan y dec
  for (int i = y-1 ; i >= 0; i--) {
    char c = board_at(b,x,i);
    if (c == '#' || c == 'L') {
      if (c == '#') count +=1;
      break;
    }
  }
  // step 4 increase x and y till the limit is reached either side
  for(int i = x+1, j = y+1; (i < b->width) && (j < b->length); i++, j++) {
    char c = board_at(b,i,j);
    if (c == '#' || c == 'L') {
      if (c == '#') count +=1;
      break;
    }
  }
  // step 5 reduce x and y till limit
  for (int i = x-1, j = y-1; (i >= 0) && (j>=0); i--, j--) {
    char c = board_at(b,i,j);
    if (c == '#' || c == 'L') {
      if (c == '#') count +=1;
      break;
    }
  }
  // step 6 increase x reduce y 
  for (int i = x+1, j = y-1; (j>=0) && (i < b->width); i++, j--) {
    char c = board_at(b,i,j);
    if (c == '#' || c == 'L') {
      if (c == '#') count +=1;
      break;
    }

  }
  // step 7 reduce x increase y 
  for (int i = x-1,j = y+1; (j < b->length) && (i >= 0); i--, j++) {
    char c = board_at(b,i,j);
    if (c == '#' || c == 'L') {
      if (c == '#') count +=1;
      break;
    }
  }
  assert (count <= 8);
  return count;
}

void board_print(board_t * b) {
  int total_occupied = 0;
  for(int i =0 ; i < b->length; i++) {
    for (int j = 0 ; j < b->width; j++) {
      total_occupied += is_occupied(b, j, i);
      printf("%c ", board_at(b,j, i));
    }
    printf("\n"); 
  }
  printf("%d\n", total_occupied);
}


void board_calc_grid(board_t * b) {
  for(int i =0 ; i < b->length; i++) {
    for (int j = 0 ; j < b->width; j++) {
      if (!isspace(board_at(b, j, i))) {
      int count = grid_search(b, j, i);
      printf("[%c %d,%d] - %d ", board_at(b,j,i), j, i, count);
      }
    }
    printf("\n"); 
  }

}

void board_free(board_t * b) {
  if (b) {
    free(b->cells);
    free(b);
  }
  
}

bool board_equal(board_t * b1, board_t *b2) {
  return b1->length == b2->length && b1->width == b2->width &&
    !memcmp(b1->cells,b2->cells, sizeof(char) * b1->length * b1->width);
    }
bool board_round_adjacents(board_t ** b) {
  board_t * board2 = board_from(*b); // copy board
  board_t * tmp = *b;
  for(int i =0 ; i < tmp->length; i++) {
    for (int j = 0 ; j < tmp->width; j++) {
      if (!isspace(board_at(tmp, j, i)) && board_at(tmp, j,i) != '.') {
	if (is_empty(tmp, j, i)) {
	    int count = adjacents(tmp, j, i, is_occupied);
	    if (count == 0) {
	      board_set(board2, j, i, '#');
	    }
	  } else {
	    int count = adjacents(tmp, j, i, is_occupied);
	    if (count >=4){
	      board_set(board2, j, i, 'L');
	    }
	}
      }
    }
  }

  bool stable = board_equal(*b, board2);
  board_free(*b);
  *b = board2;
  return stable;
}

bool board_round_grid_search(board_t ** b) {
  board_t * board2 = board_from(*b); // copy board
  board_t * tmp = *b;
  for(int i =0 ; i < tmp->length; i++) {
    for (int j = 0 ; j < tmp->width; j++) {
      if (!isspace(board_at(tmp, j, i)) && board_at(tmp, j,i) != '.') {
	if (is_empty(tmp, j, i)) {
	    int count = grid_search(tmp, j, i);
	    if (count == 0) {
	      board_set(board2, j, i, '#');
	    }
	  } else {
	    int count = grid_search(tmp, j, i);
	    if (count >=5){
	      board_set(board2, j, i, 'L');
	    }
	}
      }
    }
  }

  bool stable = board_equal(*b, board2);
  board_free(*b);
  *b = board2;
  return stable;
}  

int main() {
  char * filename = "day11_input.txt";
  lines_t * lines = read_lines(filename);
  board_t * b = board_create(lines);
  //  while(!board_round_adjacents(&b)); uncomment for part 1
  while(!board_round_grid_search(&b));
  board_print(b);
  board_free(b);
  free_lines(lines);
}
