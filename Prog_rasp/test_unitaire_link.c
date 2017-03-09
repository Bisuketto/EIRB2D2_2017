#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "link.h"

void test__lnk__empty(){
  printf("Test de  lnk__empty ...");
  //lnk__empty : liste vide
  struct link l1;
  lnk__empty(&l1);
  assert(l1.head == l1.head->next);
  printf("ok\n");
}

void test__lnk__first(){
  printf("Test de  lnk__first ...");
  //lnk__first : liste vide
  struct link l1;
  lnk__empty(&l1);
  assert(lnk__first(&l1) == l1.head);
  //lnk__first : liste non vide
  struct link l2;
  struct lelement e2;
  lnk__empty(&l2);
  lnk__add_head(&l2, &e2);
  assert(lnk__first(&l2) == l2.head);
  printf("ok\n");
}

void test__llm__is_end_mark(){
  printf("Test de  llm__is_end_mark ...");
  //llm__is_end_mark : liste vide
  struct link l1;
  lnk__empty(&l1);
  assert(llm__is_end_mark(l1.head));
  //llm__is_end_mark : liste non vide
  struct link l2;
  struct lelement e2;
  lnk__empty(&l2);
  lnk__add_head(&l2, &e2);
  assert(llm__is_end_mark(l1.head->next));
  printf("ok\n");
}

void test__lnk__add_head(){
  printf("Test de  lnk__add_head ...");
  //lnk__add_head : ajout d'un élément
  struct link l1;
  struct lelement e1;
  lnk__empty(&l1);
  lnk__add_head(&l1, &e1);
  assert(l1.head == &e1);
  printf("ok\n");
}

void test__lnk__remove_head(){
  printf("Test de  lnk__remove_head ...");
  //lnk__remove_head : retirer un élément
  struct link l1;
  struct lelement e1;
  lnk__empty(&l1);
  lnk__add_head(&l1, &e1);  
  assert(lnk__remove_head(&l1) == &e1);
  //lnk__remove_head : retirer la sentinelle
  struct link l2;
  lnk__empty(&l2);
  assert(lnk__remove_head(&l2) == NULL);
  printf("ok\n");
}

void test__llm__next(){
  printf("Test de  llm__next ...");
  //llm__next : sentinelle
  struct link l1;
  lnk__empty(&l1);
  assert(llm__next(l1.head) == l1.head);
  //llm__next : noeud normal
  struct link l2;
  struct lelement e2;
  lnk__empty(&l2);
  lnk__add_head(&l2, &e2);
  assert(llm__next(l2.head) == l2.head->next);
  printf("ok\n");
}

void test__lnk__add_after(){
  printf("Test de  lnk__add_after ...");
  //lnk__add_after : ajout d'un noeud apres un noeud normal
  struct link l1;
  struct lelement e1;
  lnk__empty(&l1);
  lnk__add_head(&l1, &e1);
  assert(lnk__add_after(&e1, l1.head));
  //lnk__add_after : ajout d'un noeud apres la sentinelle
  struct link l2;
  struct lelement e2;
  lnk__empty(&l2);
  assert(! lnk__add_after(&e2, l2.head));
  printf("ok\n");
}


void test__lnk__remove_after(){
  printf("Test de  lnk__remove_after ...");
  //lnk__remove_after : retirer un élément apres un autre élément 
  struct link l1;
  struct lelement e1;
  struct lelement e2;
  lnk__empty(&l1);
  lnk__add_head(&l1, &e1);
  lnk__add_after(&e2, l1.head);
  assert(lnk__remove_after(l1.head) == &e2);
  //lnk__remove_after : retirer un élément après la sentinelle
  struct link l2;
  lnk__empty(&l2);
  assert(lnk__remove_after(l2.head) == NULL);
  //lnk__remove_after : retirer un élément quand l'élément suivant est la sentinelle
  struct link l3;
  struct lelement e3;
  lnk__empty(&l3);
  lnk__add_head(&l3, &e3);
  lnk__add_after(&e3, l3.head);
  assert(lnk__remove_after(&e3) == NULL);
  printf("ok\n");
}

void test__lnk__add_tail(){
  printf("Test de  lnk__add_tail ...");
  //lnk__add_tail : ajoute un élément à la fin de la liste 
  struct link l1;
  struct lelement e1;
  struct lelement e2;
  lnk__empty(&l1);
  lnk__add_head(&l1, &e1);
  lnk__add_tail(&l1, &e2);
  assert(l1.head->next == &e2);
  //lnk__add_tail : ajoute un élément quand la liste est vide
  struct link l2;
  struct lelement e3;
  lnk__empty(&l2);
  lnk__add_tail(&l2, &e3);
  assert(l2.head == &e3);
  printf("ok\n");
}

void test__lnk__remove_tail(){
  printf("Test de  lnk__remove_tail ...");
  //lnk__remove_tail : retirer un élément apres quand la liste est vide 
  struct link l1;
  lnk__empty(&l1);
  assert(lnk__remove_tail(&l1) == NULL);
  //lnk__remove_tail : retirer un élément avec une liste ne contenant qu'un seul élément
  struct link l2;
  struct lelement e2;
  lnk__empty(&l2);
  lnk__add_head(&l2, &e2);
  assert(lnk__remove_tail(&l2) == &e2);
  //lnk__remove_tail : retirer un élément dans un liste contenant au moins deux éléments
  struct link l3;
  struct lelement e3;
  struct lelement e4;
  lnk__empty(&l3);
  lnk__add_head(&l3, &e4);
  lnk__add_head(&l3, &e3);
  assert(lnk__remove_tail(&l3) == &e4);
  printf("ok\n");
}

int main(void){
  test__lnk__empty();
  test__lnk__add_head();
  test__lnk__remove_head();
  test__lnk__add_after();
  test__lnk__remove_after();
  test__lnk__add_tail();
  test__lnk__remove_tail();
  test__lnk__first();
  test__llm__is_end_mark();
  test__llm__next();
  return EXIT_SUCCESS;
}
