#ifndef DATA_ACHIEVEMENTS_H
#define DATA_ACHIEVEMENTS_H

#include "core/types.h"
#include <string>
using namespace std;

// ============================================================
// 成就
// ============================================================
struct Achievement {
    string name, desc;
    bool unlocked;
    int rewardGold, rewardItemId;
    bool hidden;
};

// ============================================================
// 初始化
// ============================================================
void initAchievements();

// ============================================================
// 统一发放
// ============================================================
void unlockAchievement(AchId id);

// ============================================================
// 检查成就
// ============================================================
void checkAchievements();

#endif // DATA_ACHIEVEMENTS_H