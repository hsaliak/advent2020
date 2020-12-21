#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "htab.h"
#include "lines.h"
#include "subprojects/xxHash-0.6.5/xxhash.h"

typedef struct point {
  int x;
  int y;
  int z;
  int w;

  list_t ls; /* so that we can store this somewhere */
  hash_entry_t ht;
} point_t;

XXH64_hash_t point_hash(int x, int y, int z, int w) {
  int hashes[4];
  hashes[0] = x;
  hashes[1] = y;
  hashes[2] = z;
  hashes[3] = w;
  XXH64_hash_t hash = XXH64(hashes, sizeof(int) * 4, 0);
  assert(hash);
  return hash;
}

void point_sethash(point_t *p) {
  p->ht.hashval = point_hash(p->x, p->y, p->z, p->w);
}

  
typedef struct pocket_dimension {
  point_t * points_;   /* for storage */
  hash_entry_ptr_t *  active_points; 
} pocket_dimension_t;


void pocket_dimension_set_active_point(pocket_dimension_t * pd, int x, int y, int z, int w) {
  // this entire thing can be an implementation detail.
  // create a point
  point_t * point = calloc(sizeof(point_t), 1);
  point->x = x;
  point->y = y;
  point->z = z;
  point->z = w;
  if (!pd->points_) {
    pd->points_ = point;
  } else {
    ls_insert(pd->points_, point);
  }
  // now we have a handle now make it hashable.
  point_sethash(point); // now
  ht_insert(pd->active_points, point); 
}

void point_clear(point_t * points) {
  if (!points) return;
  point_clear(list_next(points, point_t));
  free(points);
}

void pocket_dimension_clear(pocket_dimension_t *pd){
  // clear the points you are holding
  point_clear(pd->points_);
  // destroy hashtable
  hashtable_destroy(pd->active_points);
}

void points_from_lines(lines_t * lines, struct pocket_dimension * pd) {
  // only set the active points.
  // at the start, z is always zero.
  // each line creates n points
  int linecount = 0;
  int y = 0;
  // first figure out how many lines we have
  for (lines_t *l = lines; l != NULL; l = list_next(l, lines_t)) {
    linecount++;
  }
  // how wide are the lines?
  // we are now ready to create the plane.

  //  plane_t * plane =   plane_make(pd->xmax, pd->ymax, 0);
  for (lines_t *l = lines; l != NULL; l = list_next(l, lines_t)) {
    if (isspace(l->line[0])) continue; // ignore newlines.
    for (int x = 0; !isspace(l->line[x]); x++) {
      if (l->line[x] == '#') {
	pocket_dimension_set_active_point(pd, x, y, 0, 0);
      }
    }
    y++;
  }

}


void pocket_dimension_print(pocket_dimension_t * pd) {
  int active_count =0;
  for (point_t * p = pd->points_ ; p != NULL ; p = list_next(p, point_t)) {
    if(ht_search(pd->active_points, p)) {
      active_count++;
    }
  }
  printf("active count: %d\n", active_count);
}

int pocket_dimension_active_neighbors_for_point(pocket_dimension_t * pd, point_t * p) {
  // for each of the neighbour of the point check if it is in the hash table of active points and return that
  int actives = 0;
  for (int x = (p->x-1); x <= (p->x + 1); x++) {
    for (int y = (p->y-1); y <= (p->y+1) ; y++) {
      for (int z = (p->z -1); z <= (p->z+1); z++) {
	for (int w = (p->w -1); w <= (p->w+1); w++) {
	  if (x == p->x && y == p->y && z == p->z && w == p->w) continue;
	  XXH64_hash_t h =  point_hash(x, y, z, w);
	  if (ht_searchval(pd->active_points, h)) {
	    actives++;
	  }
	}
      }
    }
  }
  return actives;
}

point_t * point_create(int x, int y, int z, int w) {
  point_t * np = calloc(sizeof(point_t), 1);
  assert(np);
  np->x = x;
  np->y = y;
  np->z = z;
  np->w = w;
  point_sethash(np);
  return np;
}

void pocket_dimension_cycle(pocket_dimension_t * pd) {
  // 1) what are the points to consider/ consider each point and its neighbours. (done)
  // 2) collect the list of points to consider by looking at the active points form the previous cycle (done)
  // 3) add it to a hash table and check that table, so you do not add duplicates. (done)
  // 4) next, for each of those points, apply the rules (based on the old hash table) you can use the function above. 
  // 5) if the point is inactive, then you remove it from the hash table and list
  // 6) if the point is active, you keep it, else you remove it.
  // 7) swap the hash table and linked list out.
  //  point_t * new_points = NULL;
  point_t * neighbor_points = NULL;
  hash_entry_ptr_t * new_active_points = hashtable_new();
  for(point_t * p  = pd->points_; p != NULL; p = list_next(p, point_t)) {
    if (ht_search(pd->active_points, p)) {
	  // lets look for its neighbors and add it to the list.
      for (int x = p->x-1; x <= p->x + 1; x++) {
	for (int y = p->y-1; y <= p->y+1 ; y++) {
	  for (int z = p->z -1; z <= p->z+1; z++) {
	    for (int w = p->w -1; w <= p->w+1; w++) {
	      if (x == p->x && y == p->y && z == p->z && w == p->w) continue;
	      // create this point, its a neighbour
	      // create the hash
	      XXH64_hash_t h =  point_hash(x, y, z, w);
	      if (!ht_searchval(new_active_points, h)) { // every point is added to new active points.
		point_t * np = point_create(x,y,z, w);
		if (!neighbor_points) {
		  neighbor_points = np;
		} else {
		  ls_insert(neighbor_points, np);
		}
		ht_insert(new_active_points, np);
	      }
	    }
	  }
	}
      }
    }
  }
  // now everything is populated.
  point_t * old_point = NULL;
  for (point_t * p = neighbor_points; p != NULL ; p = list_next(p, point_t)) {
    if (old_point) { // clear out from the previous loop
      free(old_point);
      old_point = NULL;
    }
    if (ht_search(pd->active_points, p)) { // an active point, see if 2 or 3 of it's neighbors are also active.
      int npt = pocket_dimension_active_neighbors_for_point(pd, p);
      if (npt == 2 || npt == 3) {
	ht_insert(new_active_points, p);
      } else {
	ht_delete(new_active_points, p);
	ls_remove(neighbor_points, p);
	if (p == neighbor_points) { // did we remove the head? update there reference.
	  neighbor_points = list_next(p, point_t); 
	}
	old_point = p;
      }
    } else { // not an active point..
      if (pocket_dimension_active_neighbors_for_point(pd, p) ==3) {
	ht_insert(new_active_points, p);

      } else {
	ht_delete(new_active_points, p);
	list_remove(&neighbor_points->ls, &p->ls);
	if (p == neighbor_points) { // did we remove the head? update there reference.
	  neighbor_points = list_next(p, point_t);
	}
	old_point = p;
      }
    }
  }
  if (old_point) { // clear out from the previous loop
    free(old_point);
    old_point = NULL;
  }

  // here is where you need to remove the point. // or predicate with ->active_point.
  hashtable_destroy(pd->active_points);
  point_clear(pd->points_);
  pd->active_points  = new_active_points;
  pd->points_ = neighbor_points;
} 

int main(int argc, char *argv[])
{
  lines_t * lines = read_lines("day17_input.txt");
  //  print_lines(lines);
  // read in all the points from the lines
  struct pocket_dimension pd = {0};
  pd.active_points = hashtable_new();
  points_from_lines(lines, &pd);
  //  pocket_dimension_print(&pd);
  for(int i = 0; i < 6 ;i++) {
  pocket_dimension_cycle(&pd);
  }
  pocket_dimension_print(&pd);
  pocket_dimension_clear(&pd);
  free_lines(lines);
  return 0;
}

