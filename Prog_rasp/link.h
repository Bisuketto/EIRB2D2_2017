#ifndef _LINK_H_
#define _LINK_H_

struct lelement{
  int x;
  int y;
  int weight;
  struct lelement *parent;
  struct lelement *next;
};

struct link{
  struct lelement *head;
};

void lnk__empty(struct link *);
struct lelement * lnk__first(const struct link *);
int llm__is_end_mark(const struct lelement *);
void lnk__add_head(struct link *, struct lelement *);
struct lelement * lnk__remove_head(struct link *);
struct lelement * llm__next(const struct lelement *);
int lnk__add_after(struct lelement *, struct lelement *);
struct lelement * lnk__remove_after(struct lelement *);
void lnk__add_tail(struct link *, struct lelement *);
struct lelement * lnk__remove_tail(struct link *);
void lnk__free(struct link *);

#endif 
