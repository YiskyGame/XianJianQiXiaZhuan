#ifndef ITEM_BAG_H
#define ITEM_BAG_H

#include "core/types.h"

// ============================================================
// 背包操作
// ============================================================
void addBag(int id, int cnt = 1);
bool removeBag(int id, int cnt = 1);
int  countBag(int id);

#endif // ITEM_BAG_H