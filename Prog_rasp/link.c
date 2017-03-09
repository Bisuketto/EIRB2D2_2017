#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "link.h"

static struct lelement lsentinel = {-1, -1, -1, &lsentinel, &lsentinel};

/*
Initialyze the struture link
 */
void lnk__empty(struct link *link){
 link->head = &lsentinel;
}

/*
Return the element at the head of the list
 */
struct lelement * lnk__first(const struct link *link){
  return link->head;
}

/*
Check if lelement is the tail of the list
 */
int llm__is_end_mark(const struct lelement *e){
  return (e->next == e);
}

/*
Add a node at the head of the list
 */
void lnk__add_head(struct link *link, struct lelement *e){
  e->next = link->head;
  link->head = e;
}

/*
Remove the node at the head of the list
 */
struct lelement * lnk__remove_head(struct link *link){
  struct lelement *e = link->head;
  if(llm__is_end_mark(e))
    return NULL;
  link->head = e->next;
  e->next = NULL;
  return e;
}

/*
Return the next node after the node given in parameter
 */
struct lelement * llm__next(const struct lelement *e){
  return e->next;
}

/*
Add the node e_add after the node e_bef
 */
int lnk__add_after(struct lelement *e_add, struct lelement *e_bef){
  if(llm__is_end_mark(e_bef))
    return 0;
  e_add->next = e_bef->next;
  e_bef->next = e_add;
  return 1;
}

/*
Remove the node after the node e_bef
 */
struct lelement * lnk__remove_after(struct lelement *e_bef){
  struct lelement *e = e_bef->next;
  if(llm__is_end_mark(e_bef) || llm__is_end_mark(e_bef->next))
    return NULL;
  e_bef->next = e_bef->next->next;
  e->next = NULL;
  return e;
}

/*
Add the element at the tail of the list
*/
void lnk__add_tail(struct link *link, struct lelement *e){
  struct lelement *e2 = link->head;
  if(e2 == e2->next){
    e->next = link->head;
    link->head = e;
  }
  else{
    while(! llm__is_end_mark(e2->next)){
      e2 = e2->next;
    }
    e->next = e2->next;
    e2->next = e;
  }
}

/*
Remove the element at the tail of the list
*/
struct lelement * lnk__remove_tail(struct link *link){
  struct lelement *e = link->head;
  struct lelement *tmp;
  if(e == e->next){
    return NULL;
  }
  else if(e->next == e->next->next){
    link->head = e->next;
    return e;
  }
  else{
    while(! llm__is_end_mark(e->next->next)){
      e = e->next;
    }
    tmp = e->next;
    e->next = e->next->next;
    tmp->next = NULL;
    return tmp;
  }
}

/*
Free
*/
void lnk__free(struct link *link){
  while(! llm__is_end_mark(link->head))
    free(lnk__remove_head(link));
}
