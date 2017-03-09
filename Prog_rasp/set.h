#ifndef _SET_H_
#define _SET_H_
#include "link.h"

struct set;

struct set * set__empty();
int set__is_empty(const struct set*);
int set__add(struct set*, int, int, int, struct lelement*);
struct lelement* set__remove(struct set *, int, int);
int set__find(const struct set *, int, int);
unsigned int set__size(const struct set *);
void set__free(struct set *);

#endif //_SET_H_
