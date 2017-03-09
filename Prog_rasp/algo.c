#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "set.h"
#define WALL 1

struct set{
  struct link *l;
};

static int col = 0;

struct infos{
  int line;
  int column;
  int departure[2];
  int arrival[2];
};

void display_infos(const struct infos *infos){
  printf("column: %d\nline : %d\n", infos->column, infos->line);
  printf("départ: (%d, %d)\n", infos->departure[0], infos->departure[1]);
  printf("arrivée: (%d, %d)\n", infos->arrival[0], infos->arrival[1]);
}

FILE * open_map(){
  FILE * f = fopen("map", "r");
  if (f == NULL) {
    printf("cannot open file\n");
    return NULL;
  }
  return f;
}

void close_map(FILE * f){
  fclose(f);
}

void check(char c, struct infos *infos){
  if(c == 'D'){
    infos->departure[0] = infos->column;
    infos->departure[1] = infos->line;
  }
  else if(c == 'A'){
    infos->arrival[0] = infos->column;
    infos->arrival[1] = infos->line;
  }
}
void infos_map(FILE * f, struct infos *infos){
  char c = '0';
  int nb_column = 0;
  rewind(f);
  infos->line = 0;
  infos->column = 0;
  while(c != '*'){
    check(c, infos);
    c = fgetc(f);
  }
  while(c != '\n'){
    check(c, infos);
    infos->column++;
    nb_column++;
    c = fgetc(f);
  }
  while((c != '*') || (c != 'X') || (c != 'A') || (c != 'D')){
    if(c == '\n'){
      infos->line++;
      infos->column = -1;
    }
    if(c == EOF)
      break;
    check(c, infos);
    infos->column++;
    c = fgetc(f);
  }
  infos->column = nb_column;
  infos->line--;
}
  
void wall_map(FILE * f, int map[0][col]){
  rewind(f);
  char c = '0';
  int line = 0;
  int column = 0;
  
  while(c != '*')
    c = fgetc(f);

  while((c == '*') || (c == 'X') || (c == 'A') || (c == 'D') || (c == '\n')){
    if(c == '\n'){
      line++;
      column = -1;
    }
    else
      column++;
    if(c == 'X'){
      map[line][column] = 1;
    }
    else if(column != -1)
      map[line][column] = 0;
    c = fgetc(f);
    if(c == EOF)
      break;
  }
}

void display_solution(struct set* close, const struct infos *infos){
  struct lelement *e = close->l->head;
  while(e->next->next != e->next) 
    e = e->next;
  printf("(%d, %d)\n", infos->arrival[0], infos->arrival[1]);
  while((e->x != infos->departure[0]) || (e->y != infos->departure[1])){
    printf("(%d, %d)\n", e->x, e->y);
    e = e->parent;
  }
  printf("(%d, %d)\n", e->x, e->y);
}

void initialize(struct set* open, int x, int y){
  set__add(open, x, y, 0, NULL);
}

struct lelement* lightweight(struct set *open){
  struct lelement *e = open->l->head;
  struct lelement e_min;
  e_min.x = e->x;
  e_min.y = e->y;
  e_min.weight = e->weight;
  
  while(e->next != e){
    if(e->weight < e_min.weight){
        e_min.x = e->x;
	e_min.y = e->y;
	e_min.weight = e->weight;
    }
    e = e->next;
  }
  return set__remove(open, e_min.x, e_min.y);
}

int abs(int x){
  if(x < 0)
    return -x;
  return x;
}

int distance(int x1, int y1, int x2, int y2){
  return 10 * (abs(x1 - x2) + abs(y1 - y2));
}

void add_successor(struct set* close, struct set* open, struct lelement* father, int x1, int y1, const struct infos *infos, int map[][col]){
  struct lelement *e = open->l->head;
  int w = 0;
  int k = 0;
  if(map[y1][x1] != WALL){
    w = distance(x1, y1, infos->arrival[0], infos->arrival[1]) + 10;
    while(e->next != e){
      if((e->x == x1) && (e->y == y1)){
	k = 1;
	if(e->weight > w){
	  e->weight = w;
	  e->parent = father;
	}
      }
      e = e->next;
    }
    if((k == 0) && !(set__find(close, x1, y1))){
      set__add(open, x1, y1, w, father);
    }
  }
}
  
void successor(struct set* close, struct set* open, struct lelement* father, const struct infos *infos, int map[][col]){
  if(father->x > 0)
    add_successor(close, open, father, father->x -1, father->y, infos, map);
  if(father->x < infos->column - 1){
    add_successor(close, open, father, father->x + 1, father->y, infos, map);}
  if(father->y > 0)
    add_successor(close, open, father, father->x, father->y - 1, infos, map);
  if(father->y < infos->line - 1)
    add_successor(close, open, father, father->x, father->y + 1, infos, map);
}

void display_set(const struct set *set){
  struct lelement *e = set->l->head;
  while(e != e->next){
    printf("(%d, %d)\n", e->x, e->y);
    e = e->next;
  }
  printf("________________________________________________\n");
}

void search(struct set* open, struct set* close, FILE * f){
  struct infos infos;
  infos_map(f, &infos);
  display_infos(&infos);
  col = infos.column;
  int map[infos.line][infos.column];
    
  wall_map(f, map);
  
  initialize(open, infos.departure[0], infos.departure[1]);
  
  struct lelement *e_min;
  while((set__size(open)) && !(set__find(open, infos.arrival[0], infos.arrival[1]))){
    e_min = lightweight(open);
    set__add(close, e_min->x, e_min->y, e_min->weight, e_min->parent);
    successor(close, open, e_min, &infos, map);
    //display_set(open);
  }

  if(set__size(open))
    display_solution(close, &infos);
  else
    printf("Pas de solution\n");
}
  
int main (){
  struct set *open = set__empty();
  struct set *close = set__empty();
  FILE * f = open_map();
  
  search(open, close, f);
    
  set__free(open);
  set__free(close);
  close_map(f);
  
  return EXIT_SUCCESS;
}

