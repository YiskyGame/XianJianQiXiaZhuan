#ifndef CORE_UTILS_H
#define CORE_UTILS_H

#include "core/types.h"
#include <string>
using namespace std;

// ============================================================
// 控制台颜色
// ============================================================
void setColor(int c);
void resetColor();

// ============================================================
// 输入（处理方向键）
// ============================================================
int read();

// ============================================================
// 物品工具
// ============================================================
string safeItemName(int id);
string getItemEffectDesc(int id);

// ============================================================
// 地图名 / 境界名
// ============================================================
extern string mapChineseNames[20];
string getRealmName(int level, const string& className);

#endif // CORE_UTILS_H
