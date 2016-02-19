#ifndef ZLIST_H
#define ZLIST_H

#include "screentriangle.h"

int ZList_init();
void ZList_clear();
int ZList_add(ScreenPoint *p1, ScreenPoint *p2, ScreenPoint *p3, color24 c);
void ZList_render();

#endif //ZLIST_H