#include "data/achievements.h"
#include "core/globals.h"
#include "core/utils.h"
#include "item/bag.h"
#include "entity/stats.h"
#include "entity/player.h"

// ============================================================
// 初始化成就
// ============================================================
void initAchievements() {
	achievements.clear();
	achievements.push_back({"初战告捷",   "击败第一个敌人",                        false,  50, 301, false});
	achievements.push_back({"猎杀者",     "累计击败20个敌人",                    false, 100, 302, false});
	achievements.push_back({"屠魔勇士",   "击败5个Boss",                         false, 500,  16, false});
	achievements.push_back({"灵兽之\"友\"", "击败10只灵兽",                      false, 300, 310, false});
	achievements.push_back({"腰缠万贯",   "累计获得1000金币",                    false, 100, 401, false});
	achievements.push_back({"富甲一方",   "累计获得10000金币",                   false, 1000, 306, false});
	achievements.push_back({"锻造学徒",   "完成5次锻造",                         false, 150, 409, false});
	achievements.push_back({"锻造大师",   "完成20次锻造",                        false, 400,  12, false});
	achievements.push_back({"炼丹新手",   "累计合成/购买15次",                   false, 150, 402, false});
	achievements.push_back({"炼丹宗师",   "累计合成/购买60次",                   false, 400, 311, false});
	achievements.push_back({"购物狂人",   "从商店购买10件物品",                  false, 100, 308, false});
	achievements.push_back({"探险先锋",   "通关3张地图",                         false, 300, 105, false});
	achievements.push_back({"征服者",     "通关全部19张地图",                    false, 2000, 40, false});
	achievements.push_back({"不死小强",   "死亡后复活继续",                      false, 200, 306, false});
	achievements.push_back({"技能达人",   "任意技能达到15级",                    false, 500, 320, false});
	achievements.push_back({"暴击爱好者", "累计触发50次暴击",                    false, 300, 311, false});
	achievements.push_back({"宝藏猎人",   "集齐藏宝图碎片",                      false, 500, 708, false});
	achievements.push_back({"铁匠的传人", "完成铁匠的报恩任务",                  false, 800,  29, false});
	achievements.push_back({"挥金如土",   "累计花费66666金币",                   false, 1000, 40, false});
	achievements.push_back({"勇者之巅",   "累计击败300个敌人",                   false, 800,  29, false});
	achievements.push_back({"技能大师",   "累计使用500次技能",                   false, 600,  21, false});
	// ---------- 隐藏成就 ----------
	achievements.push_back({"极限反杀",   "在杀死任意BOSS时血量不大于5",         false, 500, 16, true});
	achievements.push_back({"脚滑了！",   "被某处陷阱/熔岩杀死",                 false, 200, 306, true});
	achievements.push_back({"坚毅之人",   "在不食用任何丹药的情况下杀死任意地图的BOSS", false, 800, 310, true});
	achievements.push_back({"无敌之人",   "在不食用任何丹药的情况下杀死任意地图的所有敌人（新手关除外）", false, 2000, 40, true});
	achievements.push_back({"狂妄之人",   "使用木剑且不穿戴任何防具/服装/饰品的情况下杀死任意地图的BOSS", false, 1500, 29, true});
}

// ============================================================
// 统一成就发放
// ============================================================
void unlockAchievement(AchId id) {
	int i = (int)id;
	if (i < 0 || i >= (int)achievements.size()) return;
	if (achievements[i].unlocked) return;
	achievements[i].unlocked = true;
	player.gold += achievements[i].rewardGold;
	if (achievements[i].rewardItemId > 0)
		addBag(achievements[i].rewardItemId);
	printf("\n成就解锁：%s\n", achievements[i].name.c_str());
	Sleep(800);
}

// ============================================================
// 检查成就
// ============================================================
void checkAchievements() {
	if (!achievements[ACH_FIRST_BLOOD].unlocked && player.killCount >= 1)
		unlockAchievement(ACH_FIRST_BLOOD);
	if (!achievements[ACH_HUNTER].unlocked && player.killCount >= 20)
		unlockAchievement(ACH_HUNTER);
	if (!achievements[ACH_BOSS_SLAYER].unlocked && player.bossKillCount >= 5)
		unlockAchievement(ACH_BOSS_SLAYER);
	if (!achievements[ACH_SPIRIT_FRIEND].unlocked && player.spiritKillCount >= 10)
		unlockAchievement(ACH_SPIRIT_FRIEND);
	if (!achievements[ACH_RICH_1K].unlocked && player.goldEarned >= 1000)
		unlockAchievement(ACH_RICH_1K);
	if (!achievements[ACH_RICH_10K].unlocked && player.goldEarned >= 10000)
		unlockAchievement(ACH_RICH_10K);
	if (!achievements[ACH_FORGE_5].unlocked && player.forgeCount >= 5)
		unlockAchievement(ACH_FORGE_5);
	if (!achievements[ACH_FORGE_20].unlocked && player.forgeCount >= 20)
		unlockAchievement(ACH_FORGE_20);
	if (!achievements[ACH_ALCHEMY_5].unlocked
		&& (player.forgeCount + player.purchaseCount) >= 15)
		unlockAchievement(ACH_ALCHEMY_5);
	if (!achievements[ACH_ALCHEMY_20].unlocked
		&& (player.forgeCount + player.purchaseCount) >= 60)
		unlockAchievement(ACH_ALCHEMY_20);
	if (!achievements[ACH_SHOPAHOLIC].unlocked && player.purchaseCount >= 10)
		unlockAchievement(ACH_SHOPAHOLIC);
	if (!achievements[ACH_EXPLORER].unlocked && player.mapsCleared >= 3)
		unlockAchievement(ACH_EXPLORER);
	if (!achievements[ACH_CONQUEROR].unlocked && player.mapsCleared >= 19)
		unlockAchievement(ACH_CONQUEROR);
	bool skillMax = false;
	for (int lv : player.skillLv) if (lv >= 15) {
		skillMax = true;
		break;
	}
	if (!achievements[ACH_SKILL_MAX].unlocked && skillMax)
		unlockAchievement(ACH_SKILL_MAX);
	if (!achievements[ACH_SPENDTHRIFT].unlocked && player.totalSpent >= 66666)
		unlockAchievement(ACH_SPENDTHRIFT);
	if (!achievements[ACH_HERO_PEAK].unlocked && player.killCount >= 300)
		unlockAchievement(ACH_HERO_PEAK);
}
