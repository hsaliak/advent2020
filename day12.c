
#include <stdio.h>
#include <stdlib.h>
#include "lines.h"

typedef struct {
  char move;
  int distance;
} step_t;

typedef enum  {
		NORTH = 0,
		EAST = 1,
		SOUTH = 2,
		WEST = 3
} DIRECTION_t;
  
typedef struct {
  int units[4];
  DIRECTION_t facing;

} ship_t;

ship_t steer(ship_t sh, step_t s) {
  assert (s.move == 'L' || s.move == 'R');
  int rotation = s.distance / 90; // its either 0 , 1 or 2 or 3 or 4.
  if (s.move == 'L') rotation = -rotation;
  sh.facing = (sh.facing + rotation) % 4;
  return sh;
}

ship_t advance(ship_t sh, int distance) {
  switch (sh.facing) { 
  case EAST:
    sh.units[EAST] += distance;
    return sh;
  case WEST:
    sh.units[WEST] += distance;
    return sh;
  case NORTH:
    sh.units[NORTH] += distance;
    return sh;
  case SOUTH:
    sh.units[SOUTH] += distance;
  default:
    return sh;
  }
  return sh;
}

ship_t move(ship_t sh, step_t s) {
  switch(s.move) {
  case 'N':
    sh.units[NORTH] += s.distance;
    return sh;
  case 'S':
    sh.units[SOUTH] += s.distance;
    return sh;

      case 'E':
    sh.units[EAST] += s.distance;
    return sh;
      case 'W':
    sh.units[WEST] += s.distance;
    return sh;
  case 'L':
    return  steer(sh, s);
  case 'R':
      return steer(sh, s);
  case 'F':
    return advance(sh, s.distance);
  default:
    return sh;
  }
}


void loop(lines_t * lines, ship_t ship) {
  step_t step;
  for(lines_t * l  = lines; l != NULL; l = lines_next(l)) {
    sscanf(l->line, "%c%d\n", &step.move, &step.distance);
    ship = move(ship, step);
  }
  printf("%d\n", abs(ship.units[NORTH] - ship.units[SOUTH]) + abs(ship.units[EAST] - ship.units[WEST]));
}

ship_t move_to_waypoint(ship_t ship, ship_t waypoint, int multiplier) {
  for (int i = 0 ; i < 4;i ++ ) {
    ship.units[i] += waypoint.units[i] * multiplier;
  }

  return ship;
}

ship_t rotate_around_ship(ship_t ship, ship_t waypoint, step_t step) {
  // start at 0
  int angle =0;
  if (step.move == 'L') angle = 360 - step.distance;
  else
    angle = step.distance;
  int rotation = angle / 90;
  ship_t wret = {0};
  for (int i = 0 ; i < 4; i++) {
    wret.units[(i + rotation) % 4]  = waypoint.units[i];
  }
  return wret;
}

void loop2(lines_t * lines, ship_t ship, ship_t waypoint) {
  // the waypoint is relative to the ship
  step_t step;
  for(lines_t * l  = lines; l != NULL; l = lines_next(l)) {
    sscanf(l->line, "%c%d\n", &step.move, &step.distance);
    switch (step.move) {
    case 'F':
      // move the ship to waypoint
      ship = move_to_waypoint(ship, waypoint, step.distance);
      break;
    case 'N':
      // move the waypoint
      waypoint = move(waypoint, step);
      break;
    case 'S':
      waypoint = move(waypoint, step);
      break;
    case 'E':
      waypoint = move(waypoint, step);
      break;
    case 'W':
      waypoint = move(waypoint, step);
      break;
    case 'L':
    case 'R':
      // rotate waypoint around ship wtf
      waypoint = rotate_around_ship(ship, waypoint, step);
      break;
  default:
    break;
    }
  }
    printf("%d\n", abs(ship.units[NORTH] - ship.units[SOUTH]) + abs(ship.units[EAST] - ship.units[WEST]));
}

int main() {
  char * filename = "day12_input.txt";
  lines_t * lines = read_lines(filename);
  ship_t ship = {0};
  ship.facing = EAST;
  loop(lines, ship);
  ship_t waypoint  = {0};
  waypoint.units[EAST] = 10;
  waypoint.units[NORTH] = 1;
  loop2(lines, ship, waypoint);
  free_lines(lines);
  
}
