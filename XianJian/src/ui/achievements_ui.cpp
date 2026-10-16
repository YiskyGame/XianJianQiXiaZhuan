#include "ui/ui.h"
#include "core/globals.h"
#include "core/utils.h"
#include "data/achievements.h"
#include "data/quests.h"
#include "entity/player.h"
#include "item/bag.h"

// ============================================================
// 成就界面
// ============================================================
void showAchievements() {
    system("cls");
    printf("===== 成就系统 =====\n");
    printf("总成就数：%d\n\n", (int)achievements.size());
    int visibleCount = 0, unlockedCount = 0;
    for (auto& a : achievements) {
        if (a.hidden && !a.unlocked) continue;
        visibleCount++;
        if (a.unlocked) unlockedCount++;
        printf("[%s] %s - %s (奖励：%d金币 + %s)\n",
               a.unlocked ? "已解锁" : "未解锁",
               a.name.c_str(), a.desc.c_str(),
               a.rewardGold,
               safeItemName(a.rewardItemId).c_str());
        if (a.hidden && a.unlocked) printf("  *隐藏成就*\n");
    }
    printf("\n========================================\n");
    printf("已解锁：%d / %d（可见成就）\n",
           unlockedCount, visibleCount);
    system("pause");
}

// ============================================================
// 任务界面
// ============================================================
void showQuests() {
    system("cls");
    printf("===== 任务列表 =====\n");
    for (auto& q : quests) {
        if (!q.taken)
            printf("[未接取] %s - %s\n", q.name.c_str(), q.desc.c_str());
        else if (!q.completed) {
            printf("[进行中] %s - %s", q.name.c_str(), q.desc.c_str());
            if (q.reqItemId > 0)
                printf(" (物品进度: %d/%d)",
                       countBag(q.reqItemId), q.reqItemCount);
            printf("\n");
        } else {
            printf("[已完成] %s\n", q.name.c_str());
        }
    }
    system("pause");
}