#include <stdio.h>
#include "htab.h"
#include "lines.h"
#include <stdlib.h>
#include <sys/types.h>

typedef enum  {
	  NOP = 0,
	  ACC = 1,
	  JMP = 2
} INS;

typedef struct {
  INS i;
  int op;
  //  list_t ls;
} instr_t;

typedef struct  {
  u_int64_t accum;
  instr_t * instructions; /* a flexible buffer */
  size_t ins_len;
  size_t ins_cap;
} machine_t;

void machine_init(machine_t * m) {
  m->ins_cap = 8; // start with at least 8.
  m->instructions = calloc(sizeof(instr_t), m->ins_cap);
  m->ins_len = 0;
}

void machine_free(machine_t *m) {
  if (m->instructions)
    free(m->instructions);
  m->ins_len = 0;
  m->ins_cap = 0;
}

void machine_sbrk(machine_t * m, size_t cap) {
  if (cap <= m->ins_cap) return;
  m->instructions = realloc(m->instructions, sizeof(instr_t) * cap);
  assert(m->instructions);
  m->ins_cap =  cap;
}

void machine_ins_fromline(char * line, machine_t * m) {
  if (!line) return;
  if (m->ins_cap == 0) machine_init(m);
  if (m->ins_cap == m->ins_len) {
    // allocate more
    machine_sbrk(m, m->ins_cap * 2);
  }
  instr_t * ins  = &m->instructions[m->ins_len];
  char * input = strdup(line);
  char * start = strsep(&line, " ");
  // grab the instruction that we want to add to
  //  program_t * prg = calloc(sizeof(program_t), 1);
  ins->op =  atoi(line);
  if (strncmp(start, "nop",3)  == 0)
    ins->i = NOP;
  else if (strncmp(start, "acc", 3) == 0) ins->i = ACC;
  else if (strncmp(start, "jmp", 3) == 0) ins->i = JMP;
  else assert(1 == 0); // fail
  m->ins_len++;
  free(input);
}

void machine_print_ins(machine_t * m) {
  if (!m) return;
  printf("Machine - cap:len [cap:len] [%ld:%ld]\n", m->ins_cap, m->ins_len);
  for(int i = 0 ; i < m->ins_len; i++) {
    printf("instr - %d, op - %d\n", m->instructions[i].i,
	   m->instructions[i].op);
  }
}


void machine_ins_readall(lines_t * lines, machine_t * m) {
  for (lines_t * l = lines ; l != NULL; l = lines_next(l)) {
    machine_ins_fromline(l->line, m);
  }

}

void detect_cycle(machine_t *m) {
  int pc = 0;
  m->accum = 0; // set the accumulator to zero.
  bool seen[m->ins_len];
  bzero(seen, sizeof(bool) * m->ins_len);
  for(int i = 0 ; i < m->ins_len;i++) {
    if (pc >= m->ins_len || pc < 0) break;
    if (seen[pc]) break;
    seen[pc] = true; // set this program counter as seen.
    instr_t *itr = &m->instructions[pc];
    switch (itr->i) { 
    case NOP:
      pc++; 
      break;
    case ACC:
      m->accum += itr->op;
      pc++;
      break;
    case JMP:
      pc += itr->op;
      break;
    }
  
  }
  printf("detect-cycle: %d %lu\n", pc, m->accum);
  m->accum = 0;
}

bool check_corrupt(machine_t *m) {
  int pc = 0;
  m->accum = 0; // set the accumulator to zero.
  bool seen[m->ins_len];
  bzero(seen, sizeof(bool) * m->ins_len);
  for(int i = 0 ; i < m->ins_len;i++) {
    if (pc >= m->ins_len || pc < 0) break;
    if (seen[pc]) break;
    seen[pc] = true; // set this program counter as seen.
    instr_t *itr = &m->instructions[pc];
    switch (itr->i) { 
    case NOP:
      pc++; 
      break;
    case ACC:
      m->accum += itr->op;
      pc++;
      break;
    case JMP:
      pc += itr->op;
      break;
    }
  
  }
  if (pc == m->ins_len) {
    printf("Valid Program! [pc:%d] [accum:%lu]\n", pc, m->accum);
    return false;
  }
  m->accum = 0;
  return true;
}

INS toggle(INS i) {
  switch (i) {
  case JMP: return NOP; break;
  case NOP: return JMP; break;
  default:
    return i;
  }
}


void fix_program(machine_t * m) {
  // check for corruption.
  // iterate through the instructions
  // if you see a jmp or nop, toggle it.
  // if the program is valid, you are done.
  // if the program is not valid, then toggle that instruction again and go to the next one.
  if(!check_corrupt(m)) return; // what if the program is valid, return it.
  for(int i = 0 ; i < m->ins_len; i++){
    m->instructions[i].i = toggle(m->instructions[i].i); // toggle the instruction
    if (check_corrupt(m)) {
      m->instructions[i].i = toggle(m->instructions[i].i); // still corrupt toggle it back
    } else {
      return;
    }
  }
  printf("!!no correct program found!!\n");
}
  
int main() {
  char * filename = "day8_input.txt";
  lines_t * lines = read_lines(filename);
  print_lines(lines);
  machine_t m = {0};
  machine_init(&m);
  machine_ins_readall(lines, &m);
  machine_print_ins(&m);
  detect_cycle(&m);
  printf("%d\n", check_corrupt(&m));
  fix_program(&m);
  machine_free(&m);
  free_lines(lines);
  return 0;
}
