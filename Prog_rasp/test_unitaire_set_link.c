#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include  <assert.h>
#include "set.h"

struct set{
  struct link *l;
};

void test__set__is_empty(){
  printf("Test de  set__is_empty ...");
//  set__is_empty: ensemble vide
  struct set *s1 = set__empty();
  assert(set__is_empty(s1));
//  set__is_empty: ensemble  non  vide
  struct set *s2 = set__empty();
  set__add(s2, 1, 1, 1, NULL);
  assert(! set__is_empty (s2));
  printf("ok\n");
}

void test__set__size(){
  printf("Test de  set__size ...");
//  set__size: taille ensemble  vide
  struct set *s1 = set__empty();
  assert(! set__size(s1));
//  set__size: ensemble avec ajout d'un nouvel élément 
  struct set *s2 = set__empty();
  set__empty (s2);
  set__add (s2, 1, 1, 1, NULL);
  unsigned int l1 = set__size(s2);
  set__add (s2, 2, 1, 1, NULL);
  unsigned int l2 = set__size(s2);
  assert( l1 == l2-1 );
//  set__size: ensemble avec ajout d'un élément négatif
  struct set *s3 = set__empty();
  set__empty(s3);
  set__add(s3, 1, 1, 1, NULL);
  unsigned int l3 = set__size(s3);
  set__add(s3, -1, 1, 1, NULL);
  unsigned int l4 = set__size(s3);
  assert(l3 == l4);
  printf ("ok\n");
}

void test__set__find(){
  printf("Test de  set__find ...");
//  set__find: ensemble  vide
  struct set *s1 = set__empty();
  assert(! set__find(s1, 1, 1));
//  set__find: ensemble avec l'element
  struct set *s2 = set__empty();
  set__add(s2, 1, 1, 1, NULL);
  assert(set__find(s2, 1, 1));
//  set__find: ensemble sans l'élément 
  struct set *s3 = set__empty();
  set__add(s3, 1, 1, 1, NULL);
  assert(! set__find(s3, 2, 1));
//  set__find: recherche d'un élément négatif (la butée)
  struct set *s4 = set__empty();
  set__add(s4, 1, 1, 1, NULL);
  assert(! set__find(s4, -1, -1));
  printf("ok\n");
}

void test__set__add(){
  printf("Test de  set__add ...");
//  set__remove: ajout d'un element
  struct set *s1 = set__empty();
  set__add(s1, 1, 1, 1, NULL);
  assert(set__find(s1, 1, 1) == 1);
//  set__size: ensemble avec ajout d'un négatif
  struct set *s2 = set__empty();
  assert(! set__add(s2, -1, 1, 1, NULL));
  printf("ok\n");
}

void test__set__remove(){
  printf("Test de  set__remove ...");
//  set__remove: ensemble  vide
  struct set *s1 = set__empty();
  assert(set__remove(s1, 1, 1) == NULL);
//  set__remove: ensemble avec l'element présent
  struct set *s2 = set__empty();
  set__add(s2, 1, 1, 1, NULL);
  struct lelement *e2 = s2->l->head;
  assert(set__remove(s2, 1, 1) == e2);
  assert(! set__size(s2));
//  set__remove: ensemble avec l'element absent 
  struct set *s3 = set__empty();
  struct lelement e3;
  set__add(s3, 1, 1, 1, &e3);
  assert(set__remove(s3, 2, 1) == NULL);
  assert(set__size(s3) == 1);
//  set__remove: ensemble avec element negatif (la butée)
  struct set *s4 = set__empty();
  set__add(s4, 1, 1, 1, NULL);
  assert(set__remove(s4, -1, 1) == NULL);
  printf("ok\n");
}

int main(void){
  test__set__find();
  test__set__add();
  test__set__remove();
  test__set__is_empty();
  test__set__size();
  return EXIT_SUCCESS;
}
