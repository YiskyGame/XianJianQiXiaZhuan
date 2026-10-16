#include "data/quests.h"
#include "core/globals.h"
#include "core/utils.h"
#include "item/bag.h"
#include "entity/player.h"

// ============================================================
// 初始化任务
// ============================================================
void initQuests() {
    quests.clear();
    quests.push_back({1,  "铁匠的困境", "将村长的信和玄铁剑送到冰霜雪原的铁匠手中", 701, 1, 0,    0,   false, false});
    quests.push_back({2,  "收集皮毛",   "收集5张灵兽皮毛交给猎人",                 401, 5, 150,  103, false, false});
    quests.push_back({3,  "采药任务",   "采集3株千年灵芝",                        411, 3, 200,  310, false, false});
    quests.push_back({4,  "力量试炼",   "击败黑山老妖",                            -1, 0, 500,  8,   false, false});
    quests.push_back({5,  "古老宝藏",   "集齐完整的藏宝图",                        708, 1, 1000, 23,  false, false});
    quests.push_back({6,  "屠龙勇士",   "击败冰霜巨龙",                            -2, 0, 1500, 18,  false, false});
    quests.push_back({7,  "炼丹委托",   "炼制5颗大还丹",                          302, 5, 300,  311, false, false});
    quests.push_back({8,  "装备升级",   "锻造一件金丝软甲",                        105, 1, 400,  106, false, false});
    quests.push_back({9,  "清除暗影",   "击败3个暗影刺客",                         -3, 3, 600,  36,  false, false});
    quests.push_back({10, "终极挑战",   "击败混沌天魔",                            -4, 0, 3000, 40,  false, false});
    quests.push_back({11, "龙族试炼",   "击败龙骨守卫",                            -2, 0, 2000, 41,  false, false});
    quests.push_back({12, "仙灵之缘",   "击败仙灵守护",                            -2, 0, 1800, 42,  false, false});
    quests.push_back({13, "魔界入侵",   "击败魔界领主",                            -2, 0, 2500, 45,  false, false});
    quests.push_back({14, "铁匠的报恩", "铁匠用玄铁剑为你重新锻造了一件趁手的兵器", -5, 0, 500,  29,  false, false});
    quests.push_back({15, "终极试炼",   "击败所有隐藏Boss",                        -4, 0, 5000, 120, false, false});
}

// ============================================================
// 任务进度检查（击杀 Boss 触发）
// ============================================================
struct QuestKillReq { int questIdx; int monsterId; };
static const QuestKillReq questKillReqs[] = {
    {3, 100},   // 力量试炼 → 黑山老妖
    {5, 103},   // 屠龙勇士 → 冰霜巨龙
    {9, 109},   // 终极挑战 → 混沌天魔
    {10, 110},  // 龙族试炼 → 龙骨守卫
    {11, 112},  // 仙灵之缘 → 仙灵守护
    {12, 111},  // 魔界入侵 → 魔界领主
};

void checkQuestProgress(int monsterId) {
    for (auto& r : questKillReqs) {
        if (r.monsterId == monsterId) {
            if (r.questIdx < (int)quests.size()
                && quests[r.questIdx].taken
                && !quests[r.questIdx].completed) {
                quests[r.questIdx].completed = true;
                player.gold += quests[r.questIdx].rewardGold;
                player.goldEarned += quests[r.questIdx].rewardGold;
                if (quests[r.questIdx].rewardItemId > 0)
                    addBag(quests[r.questIdx].rewardItemId);
                printf("任务'%s'已完成！\n",
                       quests[r.questIdx].name.c_str());
            }
        }
    }
}