#ifndef DATA_QUESTS_H
#define DATA_QUESTS_H

#include "core/types.h"
#include <string>
using namespace std;

// ============================================================
// 任务
// ============================================================
struct Quest {
    int id;
    string name, desc;
    int reqItemId, reqItemCount, rewardGold, rewardItemId;
    bool completed, taken;
};

// ============================================================
// 初始化
// ============================================================
void initQuests();

// ============================================================
// 任务进度检查
// ============================================================
void checkQuestProgress(int monsterId);

#endif // DATA_QUESTS_H