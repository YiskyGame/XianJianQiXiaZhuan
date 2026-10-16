#ifndef ENTITY_PLAYER_H
#define ENTITY_PLAYER_H

#include "core/types.h"
#include <string>
#include <vector>
using namespace std;

// ============================================================
// 玩家
// ============================================================
struct Player {
    string name;
    int classId, level, exp, maxExp;
    int hp, maxHp, atk, def, crit;
    float critDmg;
    int gold, mp, maxMp;
    int weapon, armor, clothing, accessory;

    vector<pair<int, int>> bag;
    vector<int> skillLv;
    vector<int> skillCd;

    int statusDur[10];
    int dungeonLevel;
    Element playerElement;
    int passiveBonus;

    int killCount, bossKillCount, spiritKillCount;
    int forgeCount, purchaseCount;
    int goldEarned, mapsCleared;

    bool firstAttack;
    int treasurePieces;
    int totalSpent;
    int skillUseCount;
    int totalCritCount;
    int trapDeathCount;

    bool usedPotionInBattle;
    bool usedPotionInCurrentMap;

    Player()
        : classId(0), level(1), exp(0), maxExp(50),
          hp(100), maxHp(100), atk(5), def(2), crit(5), critDmg(1.5f),
          gold(200), mp(50), maxMp(50),
          weapon(0), armor(0), clothing(0), accessory(0),
          dungeonLevel(1), playerElement(ELE_FIRE), passiveBonus(0),
          killCount(0), bossKillCount(0), spiritKillCount(0),
          forgeCount(0), purchaseCount(0), goldEarned(0), mapsCleared(0),
          firstAttack(true), treasurePieces(0),
          totalSpent(0), skillUseCount(0), totalCritCount(0), trapDeathCount(0),
          usedPotionInBattle(false), usedPotionInCurrentMap(false) {
        memset(statusDur, 0, sizeof(statusDur));
    }
};

// ============================================================
// 玩家初始化 / 升级 / 换职业
// ============================================================
void initPlayer(int cid, string pname);
void gainExp(int e);
void applyPlayerClass(int newClassId);

// ============================================================
// 丹药使用（统一入口）
// ============================================================
void usePotion(int id, bool inBattle);

#endif // ENTITY_PLAYER_H