#include "entity/player.h"
#include "entity/stats.h"
#include "core/globals.h"
#include "core/utils.h"
#include "data/items.h"
#include "data/classes.h"
#include "item/bag.h"

// ============================================================
// 初始化玩家
// ============================================================
void initPlayer(int cid, string pname) {
    if (cid < 0 || cid >= (int)classes.size()) cid = 0;
    player.name = pname;
    player.classId = cid;
    player.level = 1;
    player.exp = 0;
    player.maxExp = 50;
    player.maxHp = classes[cid].baseHp;
    player.hp = player.maxHp;
    player.atk = classes[cid].baseAtk;
    player.def = classes[cid].baseDef;
    player.crit = classes[cid].baseCrit;
    player.critDmg = classes[cid].baseCritDmg;
    player.gold = 200;
    player.mp = 50;
    player.maxMp = 50;
    player.weapon = player.armor = player.clothing = player.accessory = 0;
    player.bag.clear();
    player.skillLv.assign(classes[cid].skills.size(), 0);
    player.skillCd.assign(classes[cid].skills.size(), 0);
    memset(player.statusDur, 0, sizeof(player.statusDur));
    player.dungeonLevel = 1;
    player.passiveBonus = (cid == 0 ? 5 : 0);
    player.firstAttack = true;
    player.killCount = player.bossKillCount = player.spiritKillCount = 0;
    player.forgeCount = player.purchaseCount = 0;
    player.goldEarned = player.mapsCleared = 0;
    player.treasurePieces = 0;
    player.totalSpent = 0;
    player.skillUseCount = 0;
    player.totalCritCount = 0;
    player.trapDeathCount = 0;
    player.usedPotionInBattle = false;
    player.usedPotionInCurrentMap = false;

    Element valid[] = {ELE_FIRE, ELE_ICE, ELE_THUNDER, ELE_POISON, ELE_HOLY, ELE_DARK, ELE_WIND, ELE_EARTH};
    player.playerElement = valid[rand() % 8];
    string en[] = {"无", "火", "冰", "雷", "毒", "圣", "暗", "风", "土"};
    printf("你与 %s 属性产生了共鸣！\n", en[player.playerElement].c_str());
    Sleep(1500);

    addBag(1, 1);
    addBag(301, 3);
    addBag(701, 1);
    addBag(711, 1);
}

// ============================================================
// 升级
// ============================================================
void gainExp(int e) {
    if (player.level >= 90) {
        printf("已达满级（90级），无法再获得经验。\n");
        Sleep(500);
        return;
    }
    player.exp += e;
    while (player.exp >= player.maxExp) {
        player.exp -= player.maxExp;
        player.level++;
        if (player.level > 90) {
            player.level = 90;
            player.exp = 0;
            printf("恭喜你达到了满级 90 级！\n");
            Sleep(800);
            break;
        }
        player.maxExp = 50 + player.level * 25;
        player.maxHp += 12;
        player.hp += 12;
        player.atk += 2;
        player.def += 1;
        player.maxMp += 5;
        player.mp += 5;

        string realm = getRealmName(player.level, classes[player.classId].name);
        printf("升级！当前等级 %d - %s\n", player.level, realm.c_str());

        int sc = classes[player.classId].skills.size();
        if (sc > 0) {
            int r = rand() % sc;
            if (player.skillLv[r] < 15) {
                player.skillLv[r]++;
                printf("技能 [%s] 提升至 Lv.%d\n",
                       classes[player.classId].skills[r].name.c_str(),
                       player.skillLv[r]);
            } else {
                printf("技能 [%s] 已达最高等级\n",
                       classes[player.classId].skills[r].name.c_str());
            }
        }
        Sleep(400);
    }
}

// ============================================================
// 切换职业
// ============================================================
void applyPlayerClass(int newClassId) {
    if (newClassId < 0 || newClassId >= (int)classes.size()) return;

    int savedLevel = player.level, savedExp = player.exp;
    int savedMaxExp = player.maxExp, savedGold = player.gold;
    int savedWeapon = player.weapon, savedArmor = player.armor;
    int savedClothing = player.clothing, savedAccessory = player.accessory;
    int savedKill = player.killCount, savedBoss = player.bossKillCount;
    int savedSpirit = player.spiritKillCount;
    int savedForge = player.forgeCount, savedPurchase = player.purchaseCount;
    int savedGoldEarned = player.goldEarned, savedMaps = player.mapsCleared;
    int savedDungeon = player.dungeonLevel;
    Element savedElem = player.playerElement;
    int savedTreasure = player.treasurePieces;
    int savedTotalSpent = player.totalSpent;
    int savedSkillUse = player.skillUseCount;
    int savedCritCount = player.totalCritCount;
    int savedTrapDeath = player.trapDeathCount;
    int savedStatus[10];
    memcpy(savedStatus, player.statusDur, sizeof(player.statusDur));

    player.classId = newClassId;
    int lvHp = (savedLevel - 1) * 12;
    int lvAtk = (savedLevel - 1) * 2;
    int lvDef = (savedLevel - 1) * 1;
    int lvMp = (savedLevel - 1) * 5;
    player.maxHp = classes[newClassId].baseHp + lvHp;
    player.atk = classes[newClassId].baseAtk + lvAtk;
    player.def = classes[newClassId].baseDef + lvDef;
    player.crit = classes[newClassId].baseCrit;
    player.critDmg = classes[newClassId].baseCritDmg;
    player.maxMp = 50 + lvMp;
    player.mp = player.maxMp;
    player.hp = player.maxHp;

    player.passiveBonus = (newClassId == 0 ? 5 : 0);

    player.skillLv.assign(classes[newClassId].skills.size(), 0);
    player.skillCd.assign(classes[newClassId].skills.size(), 0);

    player.level = savedLevel;
    player.exp = savedExp;
    player.maxExp = savedMaxExp;
    player.gold = savedGold;
    player.weapon = savedWeapon;
    player.armor = savedArmor;
    player.clothing = savedClothing;
    player.accessory = savedAccessory;
    player.killCount = savedKill;
    player.bossKillCount = savedBoss;
    player.spiritKillCount = savedSpirit;
    player.forgeCount = savedForge;
    player.purchaseCount = savedPurchase;
    player.goldEarned = savedGoldEarned;
    player.mapsCleared = savedMaps;
    player.dungeonLevel = savedDungeon;
    player.playerElement = savedElem;
    player.treasurePieces = savedTreasure;
    player.totalSpent = savedTotalSpent;
    player.skillUseCount = savedSkillUse;
    player.totalCritCount = savedCritCount;
    player.trapDeathCount = savedTrapDeath;
    memcpy(player.statusDur, savedStatus, sizeof(savedStatus));
}

// ============================================================
// 统一丹药使用
// ============================================================
void usePotion(int id, bool inBattle) {
    if (id <= 0 || (size_t)id >= itemDB.size()) return;
    Item& it = itemDB[id];
    float mul = (player.classId == 1) ? 1.2f : 1.0f;

    switch (it.effect) {
        case HEAL: {
            int heal = (int)(it.effectVal * mul);
            player.hp = min(getMaxHp(), player.hp + heal);
            printf("恢复 %d 生命\n", heal);
            if (id == 302) {
                player.mp = min(player.maxMp, player.mp + 15);
                printf("额外恢复 15 灵力\n");
            }
            break;
        }
        case REVIVE:
            player.hp = getMaxHp() * it.effectVal / 100;
            player.mp = player.maxMp;
            printf("复活！恢复 %d%% 生命和全部灵力\n", it.effectVal);
            break;
        case ADD_ATK:
            player.atk += it.effectVal;
            printf("攻击力永久 +%d\n", it.effectVal);
            break;
        case ADD_DEF:
            player.def += it.effectVal;
            printf("防御力永久 +%d\n", it.effectVal);
            break;
        case ADD_CRIT:
            player.crit += it.effectVal;
            printf("暴击率永久 +%d%%\n", it.effectVal);
            break;
        case ADD_HP:
            player.maxHp += it.effectVal;
            printf("生命上限永久 +%d\n", it.effectVal);
            break;
        case CURE:
            player.statusDur[0] = 0;
            printf("中毒已解除\n");
            break;
        case ADD_MP:
            player.mp = min(player.maxMp, player.mp + it.effectVal);
            printf("灵力恢复 %d\n", it.effectVal);
            break;
        case TEMP_ATK:
            player.statusDur[7] = 3;
            printf("临时攻击 +20%%，持续 3 回合\n");
            break;
        case TEMP_DEF:
            player.statusDur[2] = 3;
            printf("临时防御 +20%%，持续 3 回合\n");
            break;
        default:
            printf("该物品无法直接使用。\n");
            break;
    }

    if (inBattle) player.usedPotionInBattle = true;
    player.usedPotionInCurrentMap = true;
}