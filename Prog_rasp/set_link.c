#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "set.h"

struct set{
  struct link *l;
};

struct set* set__empty(){
  struct set *set = malloc(sizeof(struct set));
  set->l = malloc(sizeof(struct link)); 
  lnk__empty(set->l);
  return set;
}

int set__is_empty(const struct set* set){
  return llm__is_end_mark(set->l->head);
}

unsigned int set__size(const struct set *set){
  struct lelement *e = set->l->head;
  int k = 0;
  while(e->next != e){
    e = e->next;
    k++;
  }
  return k;
}

int set__find(const struct set *set, int x, int y){
  struct lelement *e = set->l->head;
  while(e->next != e){
    if((e->x == x) && (e->y == y))
      return 1;
    e = e->next;
  }
  return 0;
}

int set__add(struct set* set, int x, int y, int weight, struct lelement *parent){
  struct lelement *e_new; 
  if((x < 0) || (y < 0) || (weight < 0))
    return 0;
  e_new = malloc(sizeof(struct lelement));
  e_new->x = x;
  e_new->y = y;
  e_new->weight = weight;
  e_new->parent = parent;
  lnk__add_tail(set->l, e_new);
  return 1;
}

struct lelement* set__remove(struct set *set, int x, int y){
  struct lelement *e = set->l->head;
  if((x < 0) || (y < 0) || (set->l->head == set->l->head->next))
    return NULL;

  if((e->x == x) && (e->y == y)){
    return (lnk__remove_head(set->l));
  }
  
  while(e->next != e->next->next){
    if((e->next->x == x) && (e->next->y == y)){
      return lnk__remove_after(e);
    } 
    e = e->next;
  }
  return NULL;
}

void set__free(struct set *set){
  lnk__free(set->l);
  free(set->l);
  free(set);
}
