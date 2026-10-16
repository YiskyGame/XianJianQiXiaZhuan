// ============================================================
// 鲜贱骑虾转 6.0 修复版
// 修复内容见每个函数前的 // Fxx: 注释
// ============================================================

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <ctime>
#include <random>
#include <windows.h>
#include <conio.h>
using namespace std;

// ============================================================
// 全局常量
// ============================================================
const int MAX_BAG = 80;
const int MAX_SKILLS = 8;
const int MAX_ACHIEVEMENTS = 60;
const int MAX_QUESTS = 30;
const int MAX_MAPS = 19;

int currentLevel = 1;
const int dx[4] = {-1, 1, 0, 0};
const int dy[4] = {0, 0, -1, 1};

// ============================================================
// 枚举
// ============================================================
enum ItemType {
    WEAPON, ARMOR, CLOTHING, POTION, MATERIAL,
    ACCESSORY, THROWING, QUEST, SPECIAL
};

enum ItemEffect {
    EFF_NONE, ADD_HP, ADD_ATK, ADD_DEF, ADD_CRIT,
    ADD_CRITDMG, HEAL, CURE, REVIVE, ADD_MP,
    TEMP_ATK, TEMP_DEF   // F10: 临时效果
};

enum Element {
    ELE_NONE, ELE_FIRE, ELE_ICE, ELE_THUNDER, ELE_POISON,
    ELE_HOLY, ELE_DARK, ELE_WIND, ELE_EARTH
};

// F14: 用枚举替代成就魔法数字
enum AchId {
    ACH_FIRST_BLOOD = 0,   // 初战告捷
    ACH_HUNTER,            // 猎杀者
    ACH_BOSS_SLAYER,       // 屠魔勇士
    ACH_SPIRIT_FRIEND,     // 灵兽之友
    ACH_RICH_1K,           // 腰缠万贯
    ACH_RICH_10K,          // 富甲一方
    ACH_FORGE_5,           // 锻造学徒
    ACH_FORGE_20,          // 锻造大师
    ACH_ALCHEMY_5,         // 炼丹新手（F8: 改为综合合成/购买）
    ACH_ALCHEMY_20,        // 炼丹宗师（F8: 改为综合合成/购买）
    ACH_SHOPAHOLIC,        // 购物狂人
    ACH_EXPLORER,          // 探险先锋
    ACH_CONQUEROR,         // 征服者
    ACH_UNDYING,           // 不死小强
    ACH_SKILL_MAX,         // 技能达人
    ACH_CRIT_LOVER,        // 暴击爱好者
    ACH_TREASURE_HUNTER,   // 宝藏猎人
    ACH_BLACKSMITH_HEIR,   // 铁匠的传人
    ACH_SPENDTHRIFT,       // 挥金如土
    ACH_HERO_PEAK,         // 勇者之巅
    ACH_SKILL_MASTER,      // 技能大师
    ACH_LIMIT_REVERSAL,    // 极限反杀（隐藏）
    ACH_SLIP,              // 脚滑了！（隐藏）
    ACH_PERSISTENT,        // 坚毅之人（隐藏）
    ACH_INVINCIBLE,        // 无敌之人（隐藏）
    ACH_ARROGANT,          // 狂妄之人（隐藏）
    ACH_COUNT
};

// ============================================================
// 工具函数
// ============================================================
void setColor(int c) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), c);
}
void resetColor() { setColor(7); }

// F16: read() 处理方向键，读两次丢弃特殊键
int read() {
    int c = _getch();
    if (c == 0 || c == 224) {
        _getch();   // 丢弃方向键第二字节
        return -1;
    }
    return c;
}

// ============================================================
// 结构体定义
// ============================================================
struct Achievement {
    string name, desc;
    bool unlocked;
    int rewardGold, rewardItemId;
    bool hidden;
};
vector<Achievement> achievements;

struct Item {
    int id;
    string name, desc;
    ItemType type;
    int price, effectVal;
    ItemEffect effect;
    vector<pair<int, int>> recipe;
    int stackMax, quality;
    Element element;
    int requiredLevel;
    bool isQuestItem;

    Item()
        : id(0), type(MATERIAL), price(0), effectVal(0),
          effect(EFF_NONE), stackMax(99), quality(0),
          element(ELE_NONE), requiredLevel(0), isQuestItem(false) {}
};
vector<Item> itemDB;
map<string, int> itemNameToID;

struct Skill {
    string name, desc;
    int type;       // 0攻击 1治疗 2辅助 3减益 4被动
    int value;      // 数值
    int costMp;     // 灵力消耗
    int special;    // 特殊标记
    int cd;         // 冷却回合
};

struct CharacterClass {
    string name, title;
    int baseHp, baseAtk, baseDef, baseCrit;
    float baseCritDmg;
    vector<Skill> skills;
    string passiveName, passiveDesc;
};
vector<CharacterClass> classes;

struct Quest {
    int id;
    string name, desc;
    int reqItemId, reqItemCount, rewardGold, rewardItemId;
    bool completed, taken;
};
vector<Quest> quests;

struct MapNpc {
    int x, y;
    char symbol;
    int color;
    string name;
    vector<string> dialogs;
    int questId;
};

struct Monster {
    int id;
    string name;
    int hp, maxHp, atk, def, exp, gold;
    vector<pair<int, int>> loot;
    char symbol;
    int color;
    bool isSpirit;
    Element element;
    int skillChance;
};
vector<Monster> monsterDB;

struct MapBox {
    int x, y;
    vector<int> items;
    bool opened;
};

struct MapMonster {
    int type, x, y, hp;
    bool alive;
};

struct GameMap {
    string name;
    vector<string> data;
    int px, py, ex, ey;
    bool exitLocked;
    vector<MapBox> boxes;
    vector<MapMonster> mobs;
    vector<MapNpc> npcs;
    int themeColor;
} currentMap;

// ============================================================
// F13: Player 结构体精简
//   - 移除 battleAgainstBoss / bossBattleStartHp / currentWeaponId
//   - 移除 hasArmorEquipped / hasClothingEquipped / hasAccessoryEquipped
//   - 移除 questProgress[] / achievement[]（从未真正使用）
//   - 新增 skillCd[]（F4）
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
    vector<int> skillCd;      // F4: 技能冷却

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

    // 战斗状态
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
} player;

// ============================================================
// 前向声明
// ============================================================
int  getTotalAtk();
int  getTotalDef();
int  getTotalCrit();
float getTotalCritDmg();
int  getMaxHp();

void gainExp(int);
void checkAchievements();
void checkQuestProgress(int);
void randomEventExtended();
void showAchievements();
void showQuests();
int  countBag(int);
void addBag(int id, int cnt = 1);
bool removeBag(int id, int cnt = 1);

// F14: 统一成就发放
void unlockAchievement(AchId id);
// F15: 统一通关判定
void tryExitMap(bool& exited);
// F10: 统一丹药使用
void usePotion(int id, bool inBattle);
// 在第 1 部分"前向声明"区域补上
void monsterMove(int &mx, int &my, int px, int py, vector<string>& map);

// 界面
void titleScreen();
void chooseClass();
void mainMenu();
void status();
void shop();
void bagUI();
void forgingUI();
void itemQueryUI();
void auctionHouse();
void showLeaderboard();

// 地图
void loadMap(int id);
void drawMap();
void explore();
bool isMapClear();

// 战斗
bool battle(Monster& mon, int& mx, int& my);

// 存档
void saveGame();
void loadGame();

// ============================================================
// 物品注册工具
// ============================================================
string safeItemName(int id) {
    if (id <= 0) return "[未知物品]";
    if ((size_t)id >= itemDB.size()) return "[未知物品]";
    if (itemDB[id].id != id) return "[未知物品]";
    if (itemDB[id].name.empty() || itemDB[id].name == "[空]") return "[空物品]";
    return itemDB[id].name;
}

void regItem(int id, string name, string desc, ItemType type,
             int price, int effVal,
             ItemEffect eff = EFF_NONE, int quality = 0,
             Element ele = ELE_NONE, int reqLv = 0) {
    if ((int)itemDB.size() <= id) itemDB.resize(id + 1);
    Item it;
    it.id = id;
    it.name = name;
    it.desc = desc;
    it.type = type;
    it.price = price;
    it.effectVal = effVal;
    it.effect = eff;
    it.stackMax = (type == MATERIAL || type == POTION || type == THROWING) ? 99 : 1;
    it.quality = quality;
    it.element = ele;
    it.requiredLevel = reqLv;
    it.isQuestItem = false;
    it.recipe.clear();
    itemDB[id] = it;
    itemNameToID[name] = id;
}

void addRecipe(int target, int matId, int cnt) {
    if (target >= 0 && target < (int)itemDB.size())
        itemDB[target].recipe.push_back({matId, cnt});
}

// ============================================================
// 物品效果描述
// ============================================================
string getItemEffectDesc(int id) {
    if (id <= 0) return "";
    if ((size_t)id >= itemDB.size()) return "";
    if (itemDB[id].id != id) return "";

    Item &it = itemDB[id];
    string desc;
    switch (it.effect) {
        case ADD_ATK:    desc = "攻击+" + to_string(it.effectVal); break;
        case ADD_DEF:    desc = "防御+" + to_string(it.effectVal); break;
        case ADD_HP:     desc = "生命+" + to_string(it.effectVal); break;
        case ADD_CRIT:   desc = "暴击率+" + to_string(it.effectVal) + "%"; break;
        case ADD_CRITDMG:desc = "暴伤+" + to_string(it.effectVal) + "%"; break;
        case HEAL:
            if (id == 325) desc = "恢复全部生命和灵力";
            else desc = "恢复" + to_string(it.effectVal) + "生命";
            break;
        case CURE:       desc = "解除中毒"; break;
        case ADD_MP:     desc = "灵力+" + to_string(it.effectVal); break;
        case REVIVE:
            if (it.effectVal > 0)
                desc = "复活并恢复" + to_string(it.effectVal) + "%生命";
            else
                desc = "复活并恢复全部生命";
            break;
        case TEMP_ATK:   desc = "临时攻击+20% 3回合"; break;
        case TEMP_DEF:   desc = "临时防御+20% 3回合"; break;
        default: break;
    }
    if (id == 302) desc += " 灵力+15";
    if (it.element != ELE_NONE) {
        string ele[] = {"", "火", "冰", "雷", "毒", "圣", "暗", "风", "土"};
        if (!desc.empty()) desc += " ";
        if (it.element >= 1 && it.element <= 8)
            desc += ele[it.element] + "属性";
    }
    if (it.quality > 0 && it.quality <= 4) {
        string qual[] = {"", "[精良]", "[稀有]", "[史诗]", "[传说]"};
        desc = qual[it.quality] + " " + desc;
    }
    return desc;
}

// ============================================================
// 地图名
// ============================================================
string mapChineseNames[20] = {
    "", "新手村", "妖兽森林", "幽暗洞穴", "龙脉遗迹",
    "幽冥鬼域", "天外魔域", "冰霜雪原", "火焰山", "雷霆崖",
    "圣光神殿", "暗影深渊", "虚空裂隙", "元素圣殿", "时间回廊",
    "最终决战之地", "遗忘古战场", "龙族秘境", "仙灵岛", "魔界之门"
};

// ============================================================
// 境界名
// ============================================================
string getRealmName(int level, const string& className) {
    if (level <= 10) return "炼气期";
    else if (level <= 20) return "筑基期";
    else if (level <= 30) return "金丹期";
    else if (level <= 40) return "元婴期";
    else if (level <= 50) return "化神期";
    else if (level <= 60) return "渡劫期";
    else if (level <= 70) return "太虚期";
    else if (level <= 80) return "大乘期";
    else return className + "大帝";
}
// ============================================================
// 初始化物品
// F12: 移除 fixRecipes()（历史遗留死代码）
// F13: itemDB 用 assign 初始化更清晰
// ============================================================
void initItems() {
    itemDB.clear();
    itemDB.assign(800, Item());
    for (int i = 0; i < 800; i++) {
        itemDB[i].id = i;
        itemDB[i].name = "[空]";
    }

    // ---------- 武器 ----------
    regItem(0, "空", "占位", MATERIAL, 0, 0);
    regItem(1, "木剑", "攻击+1", WEAPON, 10, 1, ADD_ATK);
    regItem(2, "铁剑", "攻击+2", WEAPON, 30, 2, ADD_ATK, 1);
    regItem(3, "青锋剑", "攻4 暴击2%", WEAPON, 80, 4, ADD_ATK, 2, ELE_THUNDER);
    regItem(4, "烈焰刀", "攻6 暴伤+0.2", WEAPON, 150, 6, ADD_ATK, 2, ELE_FIRE);
    regItem(5, "寒冰剑", "攻7 冰冻", WEAPON, 200, 7, ADD_ATK, 3, ELE_ICE);
    regItem(6, "雷神锤", "攻10 暴击5%", WEAPON, 350, 10, ADD_ATK, 3, ELE_THUNDER);
    regItem(7, "骨玉权杖", "攻8", WEAPON, 280, 8, ADD_ATK, 3, ELE_DARK);
    regItem(8, "龙吟剑", "攻12 暴击10%", WEAPON, 500, 12, ADD_ATK, 4, ELE_HOLY);
    regItem(9, "幽冥刺", "攻7 暴伤+0.3", WEAPON, 220, 7, ADD_ATK, 3, ELE_DARK);
    regItem(10, "破天斧", "攻16 暴击8%", WEAPON, 600, 16, ADD_ATK, 4, ELE_NONE);
    regItem(11, "月影刃", "攻9 暴击6%", WEAPON, 380, 9, ADD_ATK, 3, ELE_DARK);
    regItem(12, "星陨刀", "攻15 暴伤+0.5", WEAPON, 700, 15, ADD_ATK, 4, ELE_THUNDER);
    regItem(13, "噬魂杖", "攻10 灵力+20", WEAPON, 450, 10, ADD_ATK, 3, ELE_DARK);
    regItem(14, "神木弓", "攻8 暴击7%", WEAPON, 320, 8, ADD_ATK, 2, ELE_NONE);
    regItem(15, "九环大刀", "攻14 暴击4%", WEAPON, 550, 14, ADD_ATK, 4, ELE_NONE);
    regItem(16, "太乙剑", "攻18 暴12% 暴伤0.3", WEAPON, 900, 18, ADD_ATK, 5, ELE_HOLY);
    regItem(17, "陨星锤", "攻20 无视10%防", WEAPON, 1200, 20, ADD_ATK, 5, ELE_THUNDER);
    regItem(18, "凤凰翎", "攻13 暴9% 暴伤0.4", WEAPON, 680, 13, ADD_ATK, 4, ELE_FIRE);
    regItem(19, "血饮剑", "攻11", WEAPON, 420, 11, ADD_ATK, 3, ELE_DARK);
    regItem(20, "玄武盾刀", "攻15 防5", WEAPON, 780, 15, ADD_ATK, 4, ELE_NONE);
    regItem(21, "七星剑", "攻20 暴击15%", WEAPON, 1100, 20, ADD_ATK, 5, ELE_HOLY);
    regItem(22, "乾坤圈", "攻16 暴伤0.6", WEAPON, 950, 16, ADD_ATK, 5, ELE_NONE);
    regItem(24, "碧水剑", "攻10 暴5% 水", WEAPON, 400, 10, ADD_ATK, 3, ELE_ICE);
    regItem(25, "赤焰枪", "攻14 火伤", WEAPON, 750, 14, ADD_ATK, 4, ELE_FIRE);
    regItem(26, "狂风刀", "攻12 风", WEAPON, 600, 12, ADD_ATK, 3, ELE_WIND);
    regItem(27, "地裂锤", "攻18 眩晕", WEAPON, 850, 18, ADD_ATK, 4, ELE_EARTH);
    regItem(28, "天雷戟", "攻23 暴伤0.8", WEAPON, 1500, 23, ADD_ATK, 5, ELE_THUNDER);
    regItem(29, "诛仙剑", "攻28 暴20% 暴伤1.0", WEAPON, 3000, 28, ADD_ATK, 5, ELE_HOLY);
    regItem(30, "弑神枪", "攻32 暴15% 无视20%防", WEAPON, 5000, 32, ADD_ATK, 5, ELE_DARK);
    regItem(31, "冰晶刃", "攻20 冰冻几率增加", WEAPON, 850, 20, ADD_ATK, 4, ELE_ICE, 22);
    regItem(32, "炎魔刀", "攻22 火", WEAPON, 900, 22, ADD_ATK, 4, ELE_FIRE, 24);
    regItem(33, "风灵剑", "攻18 暴8% 风", WEAPON, 700, 18, ADD_ATK, 3, ELE_WIND, 20);
    regItem(34, "地母杖", "攻25 防8 土", WEAPON, 1000, 25, ADD_ATK, 4, ELE_EARTH, 27);
    regItem(35, "圣光锤", "攻30 对暗伤害+50%", WEAPON, 2000, 30, ADD_ATK, 5, ELE_HOLY, 35);
    regItem(36, "暗影匕首", "攻19 暴12% 暗", WEAPON, 750, 19, ADD_ATK, 4, ELE_DARK, 21);
    regItem(37, "雷公鞭", "攻26 雷 麻痹", WEAPON, 1300, 26, ADD_ATK, 5, ELE_THUNDER, 30);
    regItem(38, "冰火双剑", "攻28 冰火双属", WEAPON, 1800, 28, ADD_ATK, 5, ELE_ICE, 32);
    regItem(39, "乾坤棍", "攻33 暴15% 暴伤0.7", WEAPON, 2500, 33, ADD_ATK, 5, ELE_NONE, 38);
    regItem(40, "末日之刃", "攻45 暴20% 暴伤1.2", WEAPON, 8000, 45, ADD_ATK, 5, ELE_DARK, 50);
    regItem(41, "龙骨剑", "攻24 暴10% 对龙族伤害+30%", WEAPON, 1600, 24, ADD_ATK, 4, ELE_NONE, 30);
    regItem(42, "玄冰杖", "攻20 冰属性伤害+20%", WEAPON, 1200, 20, ADD_ATK, 4, ELE_ICE, 28);
    regItem(43, "业火刀", "攻26 火属性伤害+20%", WEAPON, 1400, 26, ADD_ATK, 4, ELE_FIRE, 30);
    regItem(44, "幻影之刃", "攻22 暴15% 闪避+5%", WEAPON, 1300, 22, ADD_ATK, 4, ELE_WIND, 26);
    regItem(45, "龙魂戟", "攻35 暴10% 暴伤0.8", WEAPON, 2800, 35, ADD_ATK, 5, ELE_NONE, 45);

    // ---------- 武器配方 ----------
    addRecipe(2, 1, 1);   addRecipe(2, 401, 2);
    addRecipe(3, 2, 1);   addRecipe(3, 403, 3);
    addRecipe(4, 3, 1);   addRecipe(4, 402, 1);   addRecipe(4, 405, 2);
    addRecipe(5, 3, 1);   addRecipe(5, 403, 2);   addRecipe(5, 406, 1);
    addRecipe(6, 2, 1);   addRecipe(6, 409, 1);   addRecipe(6, 402, 2);
    addRecipe(7, 3, 1);   addRecipe(7, 404, 3);   addRecipe(7, 405, 2);
    addRecipe(8, 6, 1);   addRecipe(8, 408, 2);   addRecipe(8, 410, 1);
    addRecipe(9, 4, 1);   addRecipe(9, 406, 2);   addRecipe(9, 407, 1);
    addRecipe(10, 6, 1);  addRecipe(10, 409, 2);  addRecipe(10, 404, 3);
    addRecipe(16, 8, 1);  addRecipe(16, 410, 2);  addRecipe(16, 415, 3);
    addRecipe(17, 10, 1); addRecipe(17, 409, 3);  addRecipe(17, 414, 2);
    addRecipe(29, 16, 1); addRecipe(29, 410, 5);  addRecipe(29, 408, 5);
    addRecipe(30, 17, 1); addRecipe(30, 409, 5);  addRecipe(30, 410, 5);
    addRecipe(31, 5, 1);  addRecipe(31, 413, 3);  addRecipe(31, 401, 4);
    addRecipe(32, 4, 1);  addRecipe(32, 414, 3);  addRecipe(32, 405, 3);
    addRecipe(33, 14, 1); addRecipe(33, 416, 3);  addRecipe(33, 407, 2);
    addRecipe(34, 27, 1); addRecipe(34, 417, 3);  addRecipe(34, 404, 4);
    addRecipe(35, 21, 1); addRecipe(35, 415, 4);  addRecipe(35, 408, 3);
    addRecipe(36, 9, 1);  addRecipe(36, 406, 4);  addRecipe(36, 407, 3);
    addRecipe(37, 6, 1);  addRecipe(37, 409, 3);  addRecipe(37, 415, 3);
    addRecipe(38, 5, 1);  addRecipe(38, 4, 1);    addRecipe(38, 413, 2);  addRecipe(38, 414, 2);
    addRecipe(39, 22, 1); addRecipe(39, 415, 4);  addRecipe(39, 417, 2);
    addRecipe(40, 30, 1); addRecipe(40, 408, 8);  addRecipe(40, 410, 8);  addRecipe(40, 415, 5);
    addRecipe(41, 8, 1);  addRecipe(41, 408, 5);  addRecipe(41, 415, 3);
    addRecipe(42, 5, 1);  addRecipe(42, 413, 4);  addRecipe(42, 416, 2);
    addRecipe(43, 4, 1);  addRecipe(43, 414, 4);  addRecipe(43, 405, 3);
    addRecipe(44, 14, 1); addRecipe(44, 416, 4);  addRecipe(44, 407, 3);
    addRecipe(45, 30, 1); addRecipe(45, 408, 6);  addRecipe(45, 410, 6);  addRecipe(45, 420, 3);

    // ---------- 防具 ----------
    regItem(101, "布衣", "防御+1", ARMOR, 10, 1, ADD_DEF);
    regItem(102, "皮甲", "防御+3", ARMOR, 30, 3, ADD_DEF, 1);
    regItem(103, "锁子甲", "防御+5 生命+30", ARMOR, 80, 5, ADD_DEF, 2);
    regItem(104, "玄铁甲", "防御+8 生命+50", ARMOR, 160, 8, ADD_DEF, 2);
    regItem(105, "金丝软甲", "防御+12 生命+80", ARMOR, 300, 12, ADD_DEF, 3);
    regItem(106, "龙鳞铠", "防御+15 生命+120", ARMOR, 500, 15, ADD_DEF, 4, ELE_HOLY);
    regItem(107, "圣灵盾", "防御+20 生命+200", ARMOR, 800, 20, ADD_DEF, 4, ELE_HOLY);
    regItem(108, "天蚕衣", "防御+10 暴击+3%", ARMOR, 250, 10, ADD_DEF, 3);
    regItem(109, "冥王甲", "防御+18 生命+150", ARMOR, 650, 18, ADD_DEF, 4, ELE_DARK);
    regItem(110, "不灭金身", "防御+25 生命+300", ARMOR, 1200, 25, ADD_DEF, 5, ELE_HOLY);
    regItem(111, "冰霜铠甲", "防御+14 生命+60", ARMOR, 400, 14, ADD_DEF, 3, ELE_ICE);
    regItem(112, "烈焰战甲", "防御+16 生命+100", ARMOR, 550, 16, ADD_DEF, 4, ELE_FIRE);
    regItem(113, "雷神战袍", "防御+19 暴击+5%", ARMOR, 700, 19, ADD_DEF, 4, ELE_THUNDER);
    regItem(114, "风行者衣", "防御+12 闪避提升 生命+50", ARMOR, 350, 12, ADD_DEF, 3, ELE_WIND);
    regItem(115, "大地之铠", "防御+22 生命+250", ARMOR, 950, 22, ADD_DEF, 5, ELE_EARTH);
    regItem(116, "暗夜披风", "防御+15 暴击+5% 暗", ARMOR, 500, 15, ADD_DEF, 4, ELE_DARK);
    regItem(117, "圣光铠甲", "防御+20 生命+200", ARMOR, 900, 20, ADD_DEF, 5, ELE_HOLY);
    regItem(118, "龙骨铠甲", "防御+28 生命+280 对龙族伤害减免", ARMOR, 1500, 28, ADD_DEF, 5, ELE_NONE, 35);
    regItem(119, "幽冥战袍", "防御+22 生命+180 暗属性伤害+15%", ARMOR, 1200, 22, ADD_DEF, 4, ELE_DARK, 30);
    regItem(120, "混沌之铠", "防御+35 生命+400 全属性抗性+10%", ARMOR, 3000, 35, ADD_DEF, 5, ELE_NONE, 50);

    // ---------- 服装 ----------
    regItem(201, "道袍", "生命+40", CLOTHING, 50, 40, ADD_HP, 1);
    regItem(202, "夜行衣", "暴击+5%", CLOTHING, 70, 5, ADD_CRIT, 2, ELE_DARK);
    regItem(203, "锦绣华服", "生命+80", CLOTHING, 120, 80, ADD_HP, 2);
    regItem(204, "隐匿斗篷", "闪避提升", CLOTHING, 90, 0, EFF_NONE, 2, ELE_DARK);
    regItem(205, "碧落仙袍", "生命+150 灵力+30", CLOTHING, 280, 150, ADD_HP, 3, ELE_HOLY);
    regItem(206, "九幽魔衣", "暴击+10% 暴伤+0.3", CLOTHING, 340, 10, ADD_CRIT, 4, ELE_DARK);
    regItem(207, "凤凰羽衣", "生命+200 复活一次", CLOTHING, 600, 200, ADD_HP, 4, ELE_FIRE);
    regItem(208, "冰蚕丝袍", "生命+120 冰抗", CLOTHING, 300, 120, ADD_HP, 3, ELE_ICE);
    regItem(209, "雷纹战衣", "暴击+8% 生命+80", CLOTHING, 400, 8, ADD_CRIT, 3, ELE_THUNDER);
    regItem(210, "大地长袍", "生命+200 防御+5", CLOTHING, 500, 200, ADD_HP, 4, ELE_EARTH);
    regItem(211, "圣光法袍", "生命+180 灵力+40 圣属性伤害+15%", CLOTHING, 700, 180, ADD_HP, 4, ELE_HOLY, 30);
    regItem(212, "暗影斗篷", "暴击+12% 闪避提升", CLOTHING, 600, 12, ADD_CRIT, 4, ELE_DARK, 28);
    regItem(213, "风灵之裳", "生命+140 闪避大幅提升", CLOTHING, 450, 140, ADD_HP, 3, ELE_WIND, 25);
    regItem(214, "龙鳞战袍", "生命+300 防御+8", CLOTHING, 1000, 300, ADD_HP, 5, ELE_NONE, 40);
    regItem(215, "混沌法衣", "生命+250 灵力+50 全属性伤害+10%", CLOTHING, 1200, 250, ADD_HP, 5, ELE_NONE, 45);

    // ---------- 丹药 ----------
    // F10: 神力丹/护体丹改为 TEMP_ATK/TEMP_DEF
    regItem(301, "小还丹", "恢复20生命", POTION, 20, 20, HEAL);
    regItem(302, "大还丹", "恢复50生命 灵力+15", POTION, 60, 50, HEAL, 1);
    regItem(303, "筑基丹", "永久攻击+1", POTION, 150, 1, ADD_ATK, 2);
    regItem(304, "培元丹", "永久防御+1", POTION, 150, 1, ADD_DEF, 2);
    regItem(305, "暴击丹", "永久暴击+2%", POTION, 200, 2, ADD_CRIT, 2);
    regItem(306, "洗髓丹", "永久生命+15", POTION, 250, 15, ADD_HP, 3);
    regItem(308, "解毒丹", "解除中毒", POTION, 30, 0, CURE);
    regItem(309, "回灵丹", "恢复25灵力", POTION, 40, 25, ADD_MP);
    regItem(310, "千年灵芝丹", "生命+40 攻击+1", POTION, 600, 40, ADD_HP, 4);
    regItem(311, "血菩提丹", "暴击+4% 暴伤+0.2", POTION, 700, 4, ADD_CRIT, 4);
    regItem(312, "聚灵丹", "灵力上限+15", POTION, 300, 15, ADD_MP, 3);
    regItem(313, "火灵丹", "火伤+10%", POTION, 400, 0, EFF_NONE, 3, ELE_FIRE);
    regItem(314, "冰心丹", "免疫冰冻3回合", POTION, 200, 0, EFF_NONE, 2, ELE_ICE);
    regItem(315, "雷元丹", "雷伤+10%", POTION, 420, 0, EFF_NONE, 3, ELE_THUNDER);
    regItem(316, "风灵丹", "闪避提升", POTION, 350, 0, EFF_NONE, 2, ELE_WIND);
    regItem(317, "土灵丹", "生命+100 防御+2", POTION, 500, 100, ADD_HP, 4, ELE_EARTH);
    regItem(318, "圣光丹", "对暗伤害+20%", POTION, 450, 0, EFF_NONE, 3, ELE_HOLY);
    regItem(319, "暗影丹", "暴伤+30%", POTION, 480, 0, EFF_NONE, 4, ELE_DARK);
    regItem(320, "龙血丹", "生命+60 攻击+2 暴击+3%", POTION, 800, 60, ADD_HP, 4, ELE_NONE, 30);
    regItem(321, "涅槃丹", "复活并恢复50%生命", POTION, 1000, 50, REVIVE, 5);
    regItem(322, "神力丹", "临时攻击+20% 3回合", POTION, 300, 20, TEMP_ATK, 3);
    regItem(323, "护体丹", "临时防御+20% 3回合", POTION, 300, 20, TEMP_DEF, 3);
    regItem(324, "天元丹", "永久全属性+1", POTION, 2000, 1, ADD_ATK, 5);
    regItem(325, "九转金丹", "恢复全部生命和灵力", POTION, 1500, 0, HEAL, 5);

    // ---------- 丹药配方 ----------
    addRecipe(301, 402, 1); addRecipe(301, 405, 1);
    addRecipe(302, 402, 2); addRecipe(302, 404, 1);
    addRecipe(303, 402, 3); addRecipe(303, 403, 1);
    addRecipe(304, 402, 2); addRecipe(304, 401, 2);
    addRecipe(305, 402, 2); addRecipe(305, 401, 1);
    addRecipe(306, 402, 4); addRecipe(306, 405, 2);
    addRecipe(308, 405, 2); addRecipe(308, 406, 1);
    addRecipe(309, 402, 1); addRecipe(309, 401, 1);
    addRecipe(310, 402, 5); addRecipe(310, 411, 1); addRecipe(310, 412, 1);
    addRecipe(311, 402, 4); addRecipe(311, 412, 2); addRecipe(311, 401, 2);
    addRecipe(312, 402, 3); addRecipe(312, 405, 2);
    addRecipe(313, 402, 3); addRecipe(313, 414, 2);
    addRecipe(314, 402, 2); addRecipe(314, 413, 2);
    addRecipe(315, 402, 3); addRecipe(315, 409, 2);
    addRecipe(316, 402, 3); addRecipe(316, 416, 2);
    addRecipe(317, 402, 4); addRecipe(317, 417, 2); addRecipe(317, 404, 2);
    addRecipe(318, 402, 4); addRecipe(318, 415, 2);
    addRecipe(319, 402, 4); addRecipe(319, 406, 2); addRecipe(319, 407, 1);
    addRecipe(320, 402, 5); addRecipe(320, 408, 2); addRecipe(320, 412, 2);
    addRecipe(321, 402, 8); addRecipe(321, 410, 3); addRecipe(321, 420, 2);
    addRecipe(322, 402, 3); addRecipe(322, 405, 2);
    addRecipe(323, 402, 3); addRecipe(323, 404, 2);
    addRecipe(324, 402, 10); addRecipe(324, 420, 3); addRecipe(324, 411, 3);
    addRecipe(325, 402, 8);  addRecipe(325, 420, 2); addRecipe(325, 410, 3);

    // ---------- 材料 ----------
    regItem(401, "灵兽皮毛", "柔软坚韧", MATERIAL, 15, 0);
    regItem(402, "灵兽内丹", "炼丹核心", MATERIAL, 30, 0, EFF_NONE, 1);
    regItem(403, "灵兽角", "锋利坚硬", MATERIAL, 25, 0);
    regItem(404, "灵兽骨", "坚固的材料", MATERIAL, 20, 0);
    regItem(405, "灵兽血", "蕴含灵力", MATERIAL, 18, 0);
    regItem(406, "灵兽眼", "可洞察弱点", MATERIAL, 22, 0, EFF_NONE, 1);
    regItem(407, "七彩翎羽", "稀有羽毛", MATERIAL, 40, 0, EFF_NONE, 2);
    regItem(408, "龙鳞碎片", "神龙之鳞", MATERIAL, 80, 0, EFF_NONE, 3);
    regItem(409, "天外陨铁", "陨石精华", MATERIAL, 100, 0, EFF_NONE, 3);
    regItem(410, "凤凰尾羽", "涅槃之羽", MATERIAL, 120, 0, EFF_NONE, 4);
    regItem(411, "千年灵芝", "炼丹至宝", MATERIAL, 200, 0, EFF_NONE, 4);
    regItem(412, "血菩提", "强化气血", MATERIAL, 180, 0, EFF_NONE, 4);
    regItem(413, "玄冰晶", "极寒之物", MATERIAL, 60, 0, EFF_NONE, 2);
    regItem(414, "炎阳石", "炽热核心", MATERIAL, 65, 0, EFF_NONE, 2);
    regItem(415, "星辰砂", "蕴含星力", MATERIAL, 90, 0, EFF_NONE, 3);
    regItem(416, "风灵石", "风之精华", MATERIAL, 55, 0, EFF_NONE, 2);
    regItem(417, "大地之心", "大地结晶", MATERIAL, 70, 0, EFF_NONE, 2);
    regItem(418, "暗影石", "暗之碎片", MATERIAL, 80, 0, EFF_NONE, 3);
    regItem(419, "圣光石", "光之结晶", MATERIAL, 85, 0, EFF_NONE, 3);
    regItem(420, "混沌之石", "全属性材料", MATERIAL, 150, 0, EFF_NONE, 5);
    regItem(421, "龙骨", "巨龙之骨", MATERIAL, 120, 0, EFF_NONE, 4);
    regItem(422, "龙魂结晶", "蕴含龙魂之力", MATERIAL, 200, 0, EFF_NONE, 5);
    regItem(423, "千年寒铁", "极寒之铁", MATERIAL, 100, 0, EFF_NONE, 3);
    regItem(424, "太阳精金", "炽热精金", MATERIAL, 100, 0, EFF_NONE, 3);
    regItem(425, "风灵玉", "风之精髓", MATERIAL, 80, 0, EFF_NONE, 3);
    regItem(426, "玄龟甲", "坚固的甲壳", MATERIAL, 90, 0, EFF_NONE, 3);
    regItem(427, "幻影丝", "虚幻之丝", MATERIAL, 70, 0, EFF_NONE, 3);
    regItem(428, "星辰铁", "星辰之铁", MATERIAL, 130, 0, EFF_NONE, 4);
    regItem(429, "魔晶", "魔界之晶", MATERIAL, 150, 0, EFF_NONE, 4);
    regItem(430, "仙灵草", "仙灵之气凝聚", MATERIAL, 160, 0, EFF_NONE, 4);

    // ---------- 暗器 ----------
    regItem(501, "飞镖", "造成20伤害", THROWING, 10, 20);
    regItem(502, "毒镖", "30伤害并中毒", THROWING, 25, 30, EFF_NONE, 1);
    regItem(503, "霹雳弹", "50伤害", THROWING, 40, 50, EFF_NONE, 1);
    regItem(505, "轰天雷", "80伤害", THROWING, 60, 80, EFF_NONE, 2);
    regItem(506, "追魂钉", "60伤害 降低防御", THROWING, 50, 60, EFF_NONE, 2);
    regItem(507, "火焰弹", "70伤害 火", THROWING, 55, 70, EFF_NONE, 2);
    regItem(510, "圣光箭", "90伤害 对暗额外伤害", THROWING, 80, 90, EFF_NONE, 3);
    regItem(511, "冰魄针", "85伤害 冰冻", THROWING, 75, 85, EFF_NONE, 3);
    regItem(512, "雷火弹", "100伤害 雷火双属性", THROWING, 100, 100, EFF_NONE, 4);
    regItem(513, "暗影镖", "95伤害 暗", THROWING, 90, 95, EFF_NONE, 3);
    regItem(514, "龙牙镖", "110伤害 破甲", THROWING, 120, 110, EFF_NONE, 4);
    regItem(515, "天雷子", "150伤害 大范围", THROWING, 180, 150, EFF_NONE, 5);

    // ---------- 饰品 ----------
    regItem(601, "灾厄之牙", "暴击+5%", ACCESSORY, 45, 5, ADD_CRIT, 2);
    regItem(602, "幸运符", "暴伤+10%", ACCESSORY, 55, 10, ADD_CRITDMG, 2);
    regItem(603, "守护项链", "防御+3", ACCESSORY, 50, 3, ADD_DEF, 1);
    regItem(604, "力量戒指", "攻击+2", ACCESSORY, 60, 2, ADD_ATK, 1);
    regItem(605, "灵玉", "生命+30", ACCESSORY, 70, 30, ADD_HP, 2);
    regItem(606, "魔瞳之眼", "暴击+8% 暴伤+15%", ACCESSORY, 130, 8, ADD_CRIT, 3);
    regItem(607, "龙魂戒", "攻击+5 暴击+3%", ACCESSORY, 150, 5, ADD_ATK, 3);
    regItem(610, "冰晶坠", "冰属性攻击+10%", ACCESSORY, 120, 0, EFF_NONE, 3, ELE_ICE);
    regItem(611, "火灵石", "火属性攻击+10%", ACCESSORY, 120, 0, EFF_NONE, 3, ELE_FIRE);
    regItem(612, "雷灵珠", "雷属性攻击+10%", ACCESSORY, 130, 0, EFF_NONE, 3, ELE_THUNDER);
    regItem(614, "大地印记", "防御+8 生命+80", ACCESSORY, 180, 8, ADD_DEF, 4, ELE_EARTH);
    regItem(615, "暗影徽章", "暴击伤害+20%", ACCESSORY, 160, 20, ADD_CRITDMG, 4, ELE_DARK);
    regItem(616, "圣光十字章", "对暗伤害+25%", ACCESSORY, 200, 0, EFF_NONE, 4, ELE_HOLY);
    regItem(617, "风灵坠", "风属性攻击+10% 闪避+3%", ACCESSORY, 150, 0, EFF_NONE, 3, ELE_WIND);
    regItem(618, "龙之护符", "攻击+8 防御+5 生命+100", ACCESSORY, 250, 8, ADD_ATK, 4, ELE_NONE, 30);
    regItem(619, "混沌珠", "全属性+5 全属性伤害+10%", ACCESSORY, 500, 5, ADD_ATK, 5, ELE_NONE, 45);
    regItem(620, "生命之泉", "生命+200 每回合恢复5生命", ACCESSORY, 280, 200, ADD_HP, 4);
    regItem(621, "力量护腕", "攻击+10", ACCESSORY, 200, 10, ADD_ATK, 4);
    regItem(622, "守护之心", "防御+10 生命+150", ACCESSORY, 250, 10, ADD_DEF, 4);
    regItem(623, "破妄之眼", "暴击+15% 命中+10%", ACCESSORY, 350, 15, ADD_CRIT, 5);
    regItem(624, "天罡护符", "全属性抗性+15%", ACCESSORY, 400, 0, EFF_NONE, 4);
    regItem(625, "轮回之链", "死亡后复活一次(被动)", ACCESSORY, 600, 0, EFF_NONE, 5);

    // ---------- 任务物品 ----------
    regItem(701, "村长手信", "交给铁匠的信", QUEST, 0, 0);
    regItem(702, "妖兽核心", "任务需要", QUEST, 0, 0);
    regItem(703, "破碎的玉佩", "隐藏任务线索", QUEST, 0, 0);
    regItem(704, "宗门令牌", "进入宗门凭证", QUEST, 0, 0);
    regItem(705, "藏宝图碎片1", "藏宝图部分", SPECIAL, 10, 0);
    regItem(706, "藏宝图碎片2", "藏宝图部分", SPECIAL, 10, 0);
    regItem(707, "藏宝图碎片3", "藏宝图部分", SPECIAL, 10, 0);
    regItem(708, "完整的藏宝图", "指向神秘宝藏", SPECIAL, 100, 0, EFF_NONE, 1);
    regItem(709, "龙族信物", "龙族证明", QUEST, 0, 0);
    regItem(710, "魔界之钥", "开启魔界之门", QUEST, 0, 0);
    regItem(711, "玄铁剑", "村长年轻时用的剑，虽旧但材质极佳", QUEST, 0, 0);
    regItem(712, "铁匠的承诺", "铁匠欠你一个人情", QUEST, 0, 0);

    // ---------- 拍卖品 ----------
    regItem(801, "龙魂玉", "传说中龙族的至宝，全属性+10", ACCESSORY, 0, 10, ADD_ATK, 5, ELE_NONE, 50);
    regItem(802, "九转还魂丹", "死亡后满状态复活一次", POTION, 0, 0, REVIVE, 5);
    regItem(803, "天罡战甲", "防御+50 生命+500 全抗性+20%", ARMOR, 0, 50, ADD_DEF, 5, ELE_NONE, 50);
    regItem(804, "轩辕剑", "攻击+50 暴击30% 暴伤1.5", WEAPON, 0, 50, ADD_ATK, 5, ELE_HOLY, 50);
    regItem(805, "混沌之心", "全属性+15 全伤害+20%", ACCESSORY, 0, 15, ADD_ATK, 5, ELE_NONE, 55);
    regItem(806, "涅槃羽衣", "生命+400 死亡后原地复活一次", CLOTHING, 0, 400, ADD_HP, 5, ELE_FIRE, 45);
    regItem(807, "盘古斧", "攻击+45 无视30%防御 暴击25%", WEAPON, 0, 45, ADD_ATK, 5, ELE_NONE, 50);
    regItem(808, "玄黄甲", "防御+40 生命+400 反伤30%", ARMOR, 0, 40, ADD_DEF, 5, ELE_EARTH, 48);
    regItem(809, "永恒之戒", "全属性+8 每回合恢复10生命", ACCESSORY, 0, 8, ADD_ATK, 5, ELE_NONE, 40);
    regItem(810, "太虚神丹", "永久全属性+3", POTION, 0, 3, ADD_ATK, 5);
    regItem(811, "九天玄女剑", "攻击+38 暴击35% 暴伤1.2", WEAPON, 0, 38, ADD_ATK, 5, ELE_WIND, 45);
    regItem(812, "不动明王铠", "防御+45 生命+450 免疫眩晕", ARMOR, 0, 45, ADD_DEF, 5, ELE_HOLY, 48);
    regItem(813, "幻影披风", "闪避+30% 暴击+15%", CLOTHING, 0, 15, ADD_CRIT, 5, ELE_DARK, 42);
    regItem(814, "轮回珠", "死亡后保留所有经验并满状态复活", ACCESSORY, 0, 0, EFF_NONE, 5);
    regItem(815, "昊天锤", "攻击+40 暴击20% 暴伤1.8", WEAPON, 0, 40, ADD_ATK, 5, ELE_THUNDER, 48);
    regItem(816, "星辉法袍", "生命+350 灵力+100 全属性伤害+15%", CLOTHING, 0, 350, ADD_HP, 5, ELE_NONE, 44);
    regItem(817, "龙渊剑", "攻击+42 暴击28% 对龙族伤害+50%", WEAPON, 0, 42, ADD_ATK, 5, ELE_NONE, 47);
    regItem(818, "金刚不坏体", "防御+35 生命+500 物理减免30%", ARMOR, 0, 35, ADD_DEF, 5, ELE_EARTH, 45);
    regItem(819, "灵犀指环", "全属性+6 暴击+20%", ACCESSORY, 0, 6, ADD_ATK, 5, ELE_NONE, 38);
    regItem(820, "神仙醉", "永久攻击+5 防御+5 生命+100", POTION, 0, 5, ADD_ATK, 5);
}

// ============================================================
// F12: 只保留 sanitizeAllRecipes，移除 fixRecipes
// ============================================================
void sanitizeAllRecipes() {
    for (auto &it : itemDB) {
        it.recipe.erase(remove_if(it.recipe.begin(), it.recipe.end(),
            [](pair<int, int>& r) {
                return r.first <= 0
                    || r.first >= (int)itemDB.size()
                    || itemDB[r.first].id != r.first;
            }), it.recipe.end());
    }
}

// ============================================================
// 初始化职业
// ============================================================
void initClasses() {
    classes.resize(8);

    classes[0] = {"剑修", "青云剑仙", 120, 6, 2, 10, 1.5,
        {
            {"剑气纵横", "攻击力*2",         0, 200, 10, 0, 2},
            {"万剑归宗", "3次0.8倍攻击",     0,  80, 15, 3, 3},
            {"剑意护体", "防御+50%，持续3回合", 2, 50, 20, 8, 5},
            {"破军",     "无视防御1.5倍伤害",  0, 150, 12, 0, 2},
            {"天剑",     "攻击*3，必暴",      0, 300, 25, 6, 4},
            {"剑心通明", "暴击率+20%，持续3回合", 2, 20, 15, 0, 4}
        },
        "剑心", "暴击率额外增加5%"};

    classes[1] = {"丹修", "丹霞圣手", 120, 4, 3, 5, 1.2,
        {
            {"回春术",   "恢复40生命",       1,  40, 15, 0, 2},
            {"毒爆",     "1.5倍伤害+中毒",   0, 150, 12, 4, 3},
            {"炼丹大师", "不消耗回合使用丹药", 2,   0,  5, 0, 5},
            {"生命链接", "伤害30%转化为生命", 0, 100, 12, 2, 3},
            {"万物复苏", "恢复60HP，清除负面", 1, 60, 25, 0, 4},
            {"毒雾弥漫", "全体中毒，持续伤害", 0,  80, 20, 4, 4}
        },
        "药体", "丹药效果提升20%"};

    classes[2] = {"暗影", "幽冥刺客", 85, 8, 1, 20, 2.0,
        {
            {"暗杀",     "必暴，暴伤+50%",     0, 100, 15, 6, 3},
            {"影遁",     "闪避并反击",        2,   0, 10, 9, 4},
            {"淬毒",     "普攻附带中毒",       4,   0,  0, 4, 0},
            {"分身斩",   "攻击两次",          0, 100, 12, 3, 3},
            {"幽冥绝杀", "攻击*2.5，无视50%防御", 0, 250, 20, 0, 4},
            {"暗影步",   "提升闪避，下次攻击必暴", 2, 0, 12, 0, 5}
        },
        "隐匿", "首次攻击必暴"};

    classes[3] = {"体修", "金刚怒目", 150, 5, 5, 5, 1.3,
        {
            {"金刚掌",   "1.8倍伤害+眩晕",     0, 180, 10, 1, 3},
            {"铁壁",     "防御翻倍，持续2回合", 2, 100, 15, 8, 4},
            {"舍身击",   "消耗10%生命，3倍伤害", 0, 300,  5, 0, 3},
            {"反伤",     "反弹50%伤害，持续2回合", 2, 50, 15, 7, 4},
            {"不灭金身", "3回合免疫死亡，回复30%HP", 2, 30, 30, 0, 8},
            {"狮子吼",   "攻击*1.2，降低敌人防御", 0, 120, 10, 0, 2}
        },
        "坚韧", "生命低于30%时防御翻倍"};

    classes[4] = {"符修", "天符真人", 90, 7, 2, 8, 1.4,
        {
            {"五雷符",   "雷属性群体攻击1.5倍", 0, 150, 18, 5, 3},
            {"金刚符",   "防御提升30%，持续3回合", 2, 30, 20, 8, 4},
            {"破魔符",   "无视防御1.2倍伤害",  0, 120, 12, 0, 2},
            {"定身符",   "造成1.4倍伤害并眩晕", 0, 140, 15, 1, 3},
            {"天师符",   "攻击*2，暴击率额外+20%", 0, 200, 22, 0, 4},
            {"符阵*万象", "全体攻击1.0倍，中毒2回合", 0, 100, 25, 4, 5}
        },
        "符印", "每回合恢复2点灵力"};

    classes[5] = {"阵修", "阵法大师", 110, 5, 4, 6, 1.3,
        {
            {"八卦阵",   "3回合内防御翻倍且反伤30%", 2, 100, 25, 7, 5},
            {"四象阵",   "攻击力提升30%，持续3回合", 2,  30, 18, 0, 4},
            {"困龙阵",   "对敌人造成1.8倍伤害并降低攻击", 0, 180, 15, 0, 3},
            {"幻阵",     "使敌人陷入混乱（眩晕2回合）", 0, 100, 12, 1, 3},
            {"破阵子",   "攻击*2.2，无视30%防御", 0, 220, 20, 0, 4},
            {"聚灵阵",   "恢复全体(自身)50生命和20灵力", 1, 50, 28, 0, 5}
        },
        "阵基", "战斗开始时获得一个随机增益"};

    classes[6] = {"音修", "琴心剑魄", 95, 4, 3, 7, 1.3,
        {
            {"清心曲",   "恢复自身50生命并解除负面", 1, 50, 20, 0, 3},
            {"破阵曲",   "对全体敌人造成0.8倍攻击并降低防御", 0, 80, 15, 0, 3},
            {"安魂曲",   "让敌人沉睡（眩晕2回合）", 0, 100, 12, 1, 3},
            {"高山流水", "治疗全体(自身)80生命", 1, 80, 25, 0, 4},
            {"广陵散",   "攻击*2.0，附带吸血30%", 0, 200, 18, 2, 4},
            {"十面埋伏", "攻击*3.0，但消耗15%当前生命", 0, 300, 30, 0, 6}
        },
        "余音", "每次使用技能后恢复5点生命"};

    classes[7] = {"毒修", "万毒尊者", 100, 6, 2, 6, 1.4,
        {
            {"毒液喷射", "1.6倍伤害+中毒3回合", 0, 160, 12, 4, 3},
            {"蚀骨之毒", "降低敌人防御30%，持续3回合", 3, 30, 10, 0, 3},
            {"瘟疫蔓延", "全体敌人中毒4回合", 0, 70, 18, 4, 4},
            {"剧毒新星", "造成2.0倍伤害，如果敌人已中毒则伤害+50%", 0, 200, 20, 4, 4},
            {"万毒噬心", "攻击*2.5，并附加3种负面效果", 0, 250, 25, 4, 5},
            {"毒体",     "被动：中毒伤害减半，且免疫中毒", 4, 0, 0, 0, 0}
        },
        "淬毒", "普通攻击有30%几率附加中毒"};
}
// ============================================================
// 初始化怪物
// ============================================================
void addMonster(int id, string n, int hp, int atk, int def,
                int exp, int gold, char sym, int col,
                bool spirit, Element ele, int skillChance,
                vector<pair<int, int>> loot) {
    if ((int)monsterDB.size() <= id) monsterDB.resize(id + 1);
    monsterDB[id] = {id, n, hp, hp, atk, def, exp, gold,
                     loot, sym, col, spirit, ele, skillChance};
}

void initMonsters() {
    // ---------- 普通怪 ----------
    addMonster(0, "新手怪", 35, 3, 0, 20, 10, 'm', 10, false, ELE_NONE, 10,
               {{1, 60}, {301, 30}});
    addMonster(1, "普通怪", 50, 5, 1, 35, 20, 'm', 11, false, ELE_NONE, 15,
               {{2, 40}, {401, 50}, {403, 20}});
    addMonster(2, "黄风怪", 70, 7, 2, 50, 30, 'M', 14, false, ELE_WIND, 20,
               {{3, 30}, {402, 30}, {404, 20}});
    addMonster(3, "石魔", 80, 6, 8, 45, 25, 'E', 8, false, ELE_EARTH, 10,
               {{404, 60}, {417, 10}, {401, 30}});
    addMonster(4, "火精", 60, 9, 2, 40, 30, 'm', 12, false, ELE_FIRE, 25,
               {{402, 30}, {414, 30}, {405, 20}});
    addMonster(5, "冰精", 65, 8, 3, 42, 28, 'm', 11, false, ELE_ICE, 25,
               {{402, 30}, {413, 30}, {405, 20}});
    addMonster(6, "雷精", 70, 10, 1, 48, 35, 'm', 13, false, ELE_THUNDER, 30,
               {{402, 30}, {409, 10}, {401, 30}});
    addMonster(7, "暗影怪", 75, 11, 2, 55, 40, 'm', 5, false, ELE_DARK, 20,
               {{406, 40}, {418, 20}, {404, 30}});
    addMonster(8, "光灵", 55, 7, 4, 35, 25, 'm', 15, false, ELE_HOLY, 15,
               {{401, 40}, {419, 20}, {402, 20}});
    addMonster(9, "毒雾妖", 60, 8, 3, 45, 30, 'm', 10, true, ELE_POISON, 30,
               {{405, 50}, {406, 30}, {401, 30}});

    // ---------- 灵兽 ----------
    addMonster(10, "灵兽·风狼", 200, 15, 8, 120, 80, 'L', 13, true, ELE_WIND, 25,
               {{401, 80}, {402, 60}, {403, 40}, {405, 20}});
    addMonster(11, "灵兽·火狐", 180, 18, 6, 110, 70, 'L', 12, true, ELE_FIRE, 30,
               {{401, 70}, {402, 50}, {404, 30}, {406, 20}});
    addMonster(12, "灵兽·冰蟒", 250, 20, 10, 160, 100, 'L', 9, true, ELE_ICE, 25,
               {{401, 60}, {403, 40}, {404, 40}, {407, 10}});
    addMonster(13, "灵兽·雷鹰", 220, 22, 7, 140, 90, 'L', 11, true, ELE_THUNDER, 20,
               {{401, 50}, {406, 30}, {407, 20}, {410, 5}});
    addMonster(14, "灵兽·玄龟", 350, 14, 20, 180, 120, 'L', 13, true, ELE_EARTH, 15,
               {{401, 40}, {404, 60}, {405, 40}, {412, 10}});
    addMonster(15, "灵兽·金翅大鹏", 400, 26, 12, 240, 200, 'L', 14, true, ELE_WIND, 20,
               {{407, 50}, {410, 30}, {415, 20}, {601, 10}});
    addMonster(16, "灵兽·九尾狐", 420, 24, 14, 280, 240, 'L', 13, true, ELE_FIRE, 30,
               {{401, 80}, {406, 50}, {412, 30}});
    addMonster(17, "灵兽·冰凤", 500, 30, 16, 350, 300, 'L', 11, true, ELE_ICE, 30,
               {{407, 60}, {410, 40}, {413, 30}, {610, 10}});
    addMonster(18, "灵兽·雷龙", 550, 35, 15, 400, 380, 'L', 13, true, ELE_THUNDER, 25,
               {{408, 30}, {409, 30}, {415, 30}, {612, 10}});
    addMonster(19, "灵兽·暗黑凤凰", 600, 40, 20, 500, 500, 'L', 5, true, ELE_DARK, 30,
               {{410, 30}, {418, 20}, {415, 20}, {615, 10}});
    addMonster(20, "灵兽·青龙", 650, 38, 22, 450, 420, 'L', 10, true, ELE_WIND, 25,
               {{408, 40}, {421, 30}, {422, 20}, {618, 10}});
    addMonster(21, "灵兽·白虎", 580, 32, 18, 380, 350, 'L', 15, true, ELE_HOLY, 25,
               {{408, 30}, {419, 30}, {415, 20}, {623, 5}});
    addMonster(22, "灵兽·玄武", 700, 25, 35, 420, 400, 'L', 6, true, ELE_EARTH, 15,
               {{426, 40}, {417, 30}, {420, 10}, {622, 10}});
    addMonster(23, "灵兽·朱雀", 520, 34, 16, 400, 380, 'L', 12, true, ELE_FIRE, 30,
               {{410, 30}, {414, 30}, {424, 20}, {611, 10}});

    // ---------- Boss ----------
    addMonster(100, "Boss·黑山老妖", 450, 25, 15, 300, 250, 'B', 12, false, ELE_DARK, 30,
               {{5, 30}, {104, 20}, {408, 10}, {601, 20}});
    addMonster(101, "Boss·千年树妖", 550, 30, 18, 400, 350, 'B', 2, false, ELE_EARTH, 35,
               {{8, 25}, {107, 15}, {411, 20}, {412, 20}});
    addMonster(102, "Boss·炎魔", 650, 35, 20, 550, 500, 'B', 12, false, ELE_FIRE, 40,
               {{12, 20}, {112, 20}, {414, 30}, {604, 20}});
    addMonster(103, "Boss·冰霜巨龙", 800, 45, 25, 750, 700, 'B', 11, true, ELE_ICE, 40,
               {{18, 20}, {111, 20}, {413, 30}, {610, 15}});
    addMonster(104, "Boss·雷神", 750, 48, 22, 700, 650, 'B', 14, false, ELE_THUNDER, 35,
               {{17, 20}, {113, 20}, {409, 30}, {612, 15}});
    addMonster(105, "Boss·风之领主", 700, 40, 18, 650, 600, 'B', 10, false, ELE_WIND, 40,
               {{26, 20}, {114, 20}, {416, 30}});
    addMonster(106, "Boss·大地魔像", 900, 30, 40, 800, 750, 'B', 6, false, ELE_EARTH, 20,
               {{27, 20}, {115, 20}, {417, 30}, {614, 15}});
    addMonster(107, "Boss·暗影君主", 850, 55, 28, 900, 850, 'B', 5, false, ELE_DARK, 40,
               {{19, 20}, {119, 20}, {418, 30}, {615, 15}});
    addMonster(108, "Boss·圣光天使", 800, 45, 30, 850, 800, 'B', 15, false, ELE_HOLY, 30,
               {{21, 20}, {117, 20}, {419, 30}, {616, 15}});
    addMonster(109, "最终Boss·混沌天魔", 1500, 70, 45, 2000, 2000, 'B', 13, false, ELE_DARK, 50,
               {{40, 20}, {120, 20}, {420, 20}, {619, 10}});
    addMonster(110, "Boss·龙骨守卫", 700, 40, 30, 600, 550, 'B', 8, false, ELE_NONE, 30,
               {{41, 20}, {118, 20}, {421, 30}, {618, 10}});
    addMonster(111, "Boss·魔界领主", 900, 55, 32, 850, 800, 'B', 5, false, ELE_DARK, 35,
               {{45, 15}, {120, 15}, {429, 30}, {625, 10}});
    addMonster(112, "Boss·仙灵守护", 750, 42, 28, 700, 650, 'B', 15, false, ELE_HOLY, 25,
               {{42, 20}, {211, 20}, {430, 30}, {624, 10}});
}

// ============================================================
// 初始化成就
// F14: 顺序与 AchId 严格对应
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
// 初始化玩家
// F3: 初始物品（木剑 + 小还丹x3 + 村长手信 + 玄铁剑）
// F6: passiveBonus 按职业设置
// F4: skillCd 初始化
// ============================================================
void initPlayer(int cid, string pname) {
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

    // 随机属性共鸣
    Element valid[] = {ELE_FIRE, ELE_ICE, ELE_THUNDER, ELE_POISON, ELE_HOLY, ELE_DARK, ELE_WIND, ELE_EARTH};
    player.playerElement = valid[rand() % 8];
    string en[] = {"无", "火", "冰", "雷", "毒", "圣", "暗", "风", "土"};
    printf("你与 %s 属性产生了共鸣！\n", en[player.playerElement].c_str());
    Sleep(1500);

    // F3: 初始物品
    addBag(1, 1);     // 木剑
    addBag(301, 3);   // 小还丹 x3
    addBag(701, 1);   // 村长手信
    addBag(711, 1);   // 玄铁剑
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
        player.maxHp += 12;  player.hp += 12;
        player.atk += 2;
        player.def += 1;
        player.maxMp += 5;   player.mp += 5;

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
// 属性计算
// ============================================================
int getTotalAtk() {
    int sum = player.atk;
    if (player.weapon > 0) {
        if ((size_t)player.weapon < itemDB.size()
            && itemDB[player.weapon].id == player.weapon
            && itemDB[player.weapon].effect == ADD_ATK) {
            int bonus = itemDB[player.weapon].effectVal;
            if (itemDB[player.weapon].element == player.playerElement)
                bonus = (int)(bonus * 1.3);
            sum += bonus;
        }
    }
    if (player.accessory > 0) {
        if ((size_t)player.accessory < itemDB.size()
            && itemDB[player.accessory].id == player.accessory
            && itemDB[player.accessory].effect == ADD_ATK)
            sum += itemDB[player.accessory].effectVal;
    }
    if (player.statusDur[7]) sum = (int)(sum * 1.2);  // 攻击提升 buff
    return sum;
}

int getTotalDef() {
    int sum = player.def;
    if (player.armor > 0) {
        if ((size_t)player.armor < itemDB.size()
            && itemDB[player.armor].id == player.armor
            && itemDB[player.armor].effect == ADD_DEF)
            sum += itemDB[player.armor].effectVal;
    }
    if (player.accessory > 0) {
        if ((size_t)player.accessory < itemDB.size()
            && itemDB[player.accessory].id == player.accessory
            && itemDB[player.accessory].effect == ADD_DEF)
            sum += itemDB[player.accessory].effectVal;
    }
    if (player.statusDur[2])  // 防御提升 buff
        sum = (int)(sum * (player.classId == 3 ? 2.0 : 1.5));
    if (player.classId == 3 && player.hp < getMaxHp() * 0.3)
        sum *= 2;
    return sum;
}

int getTotalCrit() {
    int sum = player.crit + player.passiveBonus;
    if (player.weapon > 0
        && (size_t)player.weapon < itemDB.size()
        && itemDB[player.weapon].effect == ADD_CRIT)
        sum += itemDB[player.weapon].effectVal;
    if (player.clothing > 0
        && (size_t)player.clothing < itemDB.size()
        && itemDB[player.clothing].effect == ADD_CRIT)
        sum += itemDB[player.clothing].effectVal;
    if (player.accessory > 0
        && (size_t)player.accessory < itemDB.size()
        && itemDB[player.accessory].effect == ADD_CRIT)
        sum += itemDB[player.accessory].effectVal;
    if (player.statusDur[8]) sum += 20;
    return min(100, sum);
}

// F1: 使用 critDmg + 装备暴伤加成
float getTotalCritDmg() {
    float sum = player.critDmg;
    if (player.accessory > 0
        && (size_t)player.accessory < itemDB.size()
        && itemDB[player.accessory].effect == ADD_CRITDMG)
        sum += itemDB[player.accessory].effectVal / 100.0f;
    if (player.weapon > 0
        && (size_t)player.weapon < itemDB.size()
        && itemDB[player.weapon].id == player.weapon) {
        // 部分武器描述里带"暴伤"效果（示例：太乙剑/星陨刀等）
        // 这里不额外加成，只做统一计算
    }
    return sum;
}

int getMaxHp() {
    int sum = player.maxHp;
    if (player.armor > 0
        && (size_t)player.armor < itemDB.size()
        && itemDB[player.armor].id == player.armor
        && itemDB[player.armor].effect == ADD_HP)
        sum += itemDB[player.armor].effectVal;
    if (player.clothing > 0
        && (size_t)player.clothing < itemDB.size()
        && itemDB[player.clothing].id == player.clothing
        && itemDB[player.clothing].effect == ADD_HP)
        sum += itemDB[player.clothing].effectVal;
    if (player.accessory > 0
        && (size_t)player.accessory < itemDB.size()
        && itemDB[player.accessory].id == player.accessory
        && itemDB[player.accessory].effect == ADD_HP)
        sum += itemDB[player.accessory].effectVal;
    if (player.classId == 3 && player.skillLv[7] > 0)
        sum = (int)(sum * 1.2);
    return sum;
}

// ============================================================
// 背包
// ============================================================
void addBag(int id, int cnt) {
    if (id <= 0) return;
    if ((size_t)id >= itemDB.size()) return;
    if (itemDB[id].id != id) return;

    int currentCount = 0;
    for (auto& p : player.bag) currentCount += p.second;
    if (currentCount + cnt > MAX_BAG) {
        printf("背包已满！无法添加更多物品。\n");
        Sleep(500);
        return;
    }

    for (auto &p : player.bag) {
        if (p.first == id) { p.second += cnt; return; }
    }
    player.bag.push_back({id, cnt});
}

bool removeBag(int id, int cnt) {
    if (id <= 0) return false;
    if ((size_t)id >= itemDB.size()) return false;
    if (itemDB[id].id != id) return false;
    for (auto it = player.bag.begin(); it != player.bag.end(); ++it) {
        if (it->first == id) {
            if (it->second >= cnt) {
                it->second -= cnt;
                if (it->second == 0) player.bag.erase(it);
                return true;
            }
            return false;
        }
    }
    return false;
}

int countBag(int id) {
    if (id <= 0) return 0;
    if ((size_t)id >= itemDB.size()) return 0;
    if (itemDB[id].id != id) return 0;
    for (auto &p : player.bag)
        if (p.first == id) return p.second;
    return 0;
}

// ============================================================
// F6: 切换职业
//   - 不再覆盖 passiveBonus
//   - 初始化 skillCd
// ============================================================
void applyPlayerClass(int newClassId) {
    if (newClassId < 0 || newClassId >= (int)classes.size()) return;

    // 保存需要保留的字段
    int savedLevel = player.level, savedExp = player.exp, savedMaxExp = player.maxExp, savedGold = player.gold;
    int savedWeapon = player.weapon, savedArmor = player.armor;
    int savedClothing = player.clothing, savedAccessory = player.accessory;
    int savedKill = player.killCount, savedBoss = player.bossKillCount, savedSpirit = player.spiritKillCount;
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

    // 重算属性
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

    // F6: 按新职业设置 passiveBonus，不再覆盖回去
    player.passiveBonus = (newClassId == 0 ? 5 : 0);

    player.skillLv.assign(classes[newClassId].skills.size(), 0);
    player.skillCd.assign(classes[newClassId].skills.size(), 0);

    // 恢复保留字段
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
// F14: 统一成就发放
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
// F10: 统一丹药使用
//   - 神力丹/护体丹改为真临时（走 statusDur）
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
        // F10: 真·临时效果
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
// ============================================================
// 战斗系统
// ============================================================

enum BattleState {
    BST_PLAYER_TURN,
    BST_PLAYER_SKILL_SELECT,
    BST_PLAYER_ITEM_SELECT,
    BST_PLAYER_THROW_SELECT,
    BST_MONSTER_TURN,
    BST_VICTORY,
    BST_DEFEAT
};

struct BattleContext {
    Monster* mon;
    int mHp, mMaxHp, mAtk, mDef;
    string mName;
    int charge;
    int poisonDmg, poisonTurn;
    BattleState state;
    int monsterStunTurns, monsterId;
};

// ============================================================
// F9: pDir == -1 视为无效输入，直接空过
// F1: 暴击使用 critDmg
// F2: firstAttack 在 battle() 开头重置
// ============================================================
void processPlayerAttack(BattleContext& ctx) {
    system("cls");
    printf("选择攻击方向: [J]上 [U]右 [I]下 [K]左\n");
    int dir = read();
    int pDir = (dir == 'j' || dir == 'J') ? 0
             : (dir == 'u' || dir == 'U') ? 1
             : (dir == 'i' || dir == 'I') ? 2
             : (dir == 'k' || dir == 'K') ? 3 : -1;

    // F9: 非法输入 → 空过
    if (pDir == -1) {
        printf("无效输入，本回合空过。\n");
        Sleep(600);
        ctx.state = BST_MONSTER_TURN;
        return;
    }

    int mBlock = rand() % 4;
    if (pDir == mBlock) {
        printf("怪物格挡了你的攻击！\n");
        Sleep(600);
        ctx.state = BST_MONSTER_TURN;
        return;
    }

    int dmg = getTotalAtk();
    bool isCrit = false;

    // F2: firstAttack 已在 battle() 开头重置
    if (player.classId == 2 && player.firstAttack) {
        isCrit = true;
        player.firstAttack = false;
    } else if (rand() % 100 < getTotalCrit()) {
        isCrit = true;
    }

    if (isCrit) {
        // F1: 使用 critDmg，不再硬编码 2 倍
        float critMul = getTotalCritDmg();
        dmg = (int)(dmg * critMul);
        player.totalCritCount++;
        printf("暴击！(x%.2f) ", critMul);
        if (ctx.monsterId < 100) {
            int ns = rand() % 3 + 1;
            if (ctx.monsterStunTurns > 0)
                ctx.monsterStunTurns = max(ctx.monsterStunTurns, ns);
            else
                ctx.monsterStunTurns = ns;
            printf("怪物眩晕 %d 回合！\n", ns);
        }
    }

    dmg = max(1, dmg - ctx.mDef);
    ctx.mHp -= dmg;
    printf("造成 %d 点伤害\n", dmg);

    if (player.classId == 2 && player.skillLv[2] > 0) {
        ctx.poisonTurn = 3;
        ctx.poisonDmg = getTotalAtk() / 3;
        printf("淬毒：怪物中毒！\n");
    }
    if (!isCrit && rand() % 100 < 12 && ctx.monsterId < 100) {
        int ns = rand() % 3 + 1;
        if (ctx.monsterStunTurns > 0)
            ctx.monsterStunTurns = max(ctx.monsterStunTurns, ns);
        else
            ctx.monsterStunTurns = ns;
        printf("怪物被击晕！剩余 %d 回合\n", ctx.monsterStunTurns);
    }
    Sleep(600);
    ctx.state = BST_MONSTER_TURN;
}

// ============================================================
// F1: 技能暴击使用 critDmg
// F4: 技能冷却生效
// ============================================================
void processSkillSelect(BattleContext& ctx) {
    system("cls");
    auto& skl = classes[player.classId].skills;
    printf("===== 技能列表 =====\n");
    for (size_t i = 0; i < skl.size(); i++) {
        int cd = (i < player.skillCd.size()) ? player.skillCd[i] : 0;
        printf("[%d] %s Lv.%d (MP:%d CD:%d) - %s\n",
               (int)i + 1, skl[i].name.c_str(), player.skillLv[i],
               skl[i].costMp, cd, skl[i].desc.c_str());
    }
    printf("[C] 返回\n");
    char ch = read();

    if (ch == 'c' || ch == 'C') {
        ctx.state = BST_PLAYER_TURN;
        return;
    }

    int idx = ch - '1';
    if (idx < 0 || idx >= (int)skl.size()) {
        printf("无效选择！\n");
        Sleep(500);
        ctx.state = BST_PLAYER_TURN;
        return;
    }
    if (player.skillLv[idx] == 0) {
        printf("该技能尚未领悟，无法使用！\n");
        Sleep(500);
        ctx.state = BST_PLAYER_TURN;
        return;
    }
    // F4: 冷却检查
    if (idx < (int)player.skillCd.size() && player.skillCd[idx] > 0) {
        printf("技能冷却中（剩余 %d 回合）！\n", player.skillCd[idx]);
        Sleep(500);
        ctx.state = BST_PLAYER_TURN;
        return;
    }
    if (player.mp < skl[idx].costMp) {
        printf("灵力不足！\n");
        Sleep(500);
        ctx.state = BST_PLAYER_TURN;
        return;
    }

    player.mp -= skl[idx].costMp;
    player.skillUseCount++;

    // F4: 设置冷却
    if (idx < (int)player.skillCd.size()) {
        player.skillCd[idx] = skl[idx].cd;
    }

    // 技能大师成就
    if (!achievements[ACH_SKILL_MASTER].unlocked && player.skillUseCount >= 500)
        unlockAchievement(ACH_SKILL_MASTER);

    Skill& s = skl[idx];
    int lv = player.skillLv[idx];
    double scale = 1.0 + 0.2 * (lv - 1);
    int dmg = 0;
    bool skillCrit = false;

    if (s.type == 0) {
        // 体修舍身击
        if (player.classId == 3 && s.name == "舍身击") {
            int sc = max(1, player.hp / 10);
            player.hp -= sc;
            printf("舍身一击！消耗 %d 生命值。\n", sc);
        }

        if (s.special == 6) {
            dmg = (int)(getTotalAtk() * 2 * scale);
            skillCrit = true;
        } else if (s.special == 3) {
            // 多段攻击
            int hits = 3, total = 0;
            for (int i = 0; i < hits; i++) {
                int hit = max(1, (int)(getTotalAtk() * 0.8 * scale) - ctx.mDef);
                ctx.mHp -= hit;
                total += hit;
            }
            printf("三连击造成 %d 点伤害！\n", total);
            Sleep(500);
            ctx.state = BST_MONSTER_TURN;
            return;
        } else if (s.special == 4) {
            dmg = (int)(getTotalAtk() * 1.5 * scale);
            ctx.poisonTurn = 3;
            ctx.poisonDmg = getTotalAtk() / 2;
        } else if (s.special == 1) {
            dmg = (int)(getTotalAtk() * 1.8 * scale);
            if (ctx.monsterId < 100)
                ctx.monsterStunTurns = max(ctx.monsterStunTurns, 2);
        } else {
            dmg = (int)(getTotalAtk() * (s.value / 100.0) * scale);
        }

        // 暴击判定
        if (!skillCrit && s.special != 3 && s.special != 6) {
            if (rand() % 100 < getTotalCrit()) {
                // F1: 使用 critDmg
                dmg = (int)(dmg * getTotalCritDmg());
                skillCrit = true;
                player.totalCritCount++;
                printf("暴击！");
                if (ctx.monsterId < 100)
                    ctx.monsterStunTurns = max(ctx.monsterStunTurns, rand() % 3 + 1);
            }
        } else if (skillCrit) {
            player.totalCritCount++;
        }

        if (s.special != 3) {
            dmg = max(1, dmg - ctx.mDef);
            ctx.mHp -= dmg;
            printf("造成 %d 点伤害！\n", dmg);
        }
        // 生命链接吸血
        if (s.special == 2) {
            int heal = dmg * 0.3;
            player.hp = min(getMaxHp(), player.hp + heal);
            printf("吸血恢复 %d 点生命值！\n", heal);
        }

    } else if (s.type == 1) {
        // 治疗
        int heal = (int)(s.value * scale);
        if (player.classId == 1) heal = (int)(heal * 1.2);
        player.hp = min(getMaxHp(), player.hp + heal);
        printf("恢复 %d 点生命值！\n", heal);

    } else if (s.type == 2) {
        // 辅助
        if (s.special == 8) {
            player.statusDur[2] = 3;
            printf("防御提升 3 回合！\n");
        } else if (s.special == 7) {
            player.statusDur[4] = 2;
            printf("反伤激活 2 回合！\n");
        } else if (s.special == 9) {
            player.statusDur[3] = 2;
            printf("闪避提升 2 回合！\n");
        } else if (s.special == 0) {
            player.statusDur[7] = 3;
            printf("攻击提升 3 回合！\n");
        }
    }

    // 暴击爱好者
    if (!achievements[ACH_CRIT_LOVER].unlocked && player.totalCritCount >= 50)
        unlockAchievement(ACH_CRIT_LOVER);

    Sleep(600);
    ctx.state = BST_MONSTER_TURN;
}

// ============================================================
// F10: 战斗中使用丹药（统一走 usePotion）
// ============================================================
void processItemSelect(BattleContext& ctx) {
    system("cls");
    printf("===== 可用丹药 =====\n");
    vector<int> pots;
    for (auto& p : player.bag) {
        if (p.first > 0 && p.first < (int)itemDB.size()
            && itemDB[p.first].id == p.first
            && itemDB[p.first].type == POTION) {
            pots.push_back(p.first);
            printf("[%d] %s x%d\n", (int)pots.size(),
                   safeItemName(p.first).c_str(), p.second);
        }
    }
    if (pots.empty()) {
        printf("没有可用丹药！\n");
        Sleep(500);
        ctx.state = BST_PLAYER_TURN;
        return;
    }
    printf("[C] 返回\n");
    char ch = read();
    if (ch == 'c' || ch == 'C') {
        ctx.state = BST_PLAYER_TURN;
        return;
    }
    int i = ch - '1';
    if (i >= 0 && i < (int)pots.size()) {
        int id = pots[i];
        removeBag(id, 1);
        // F10: 统一走 usePotion
        usePotion(id, true);
        printf("使用 %s\n", safeItemName(id).c_str());
        Sleep(500);
        // 丹修"炼丹大师"被动：不消耗回合
        ctx.state = (player.classId == 1 && player.skillLv[2] > 0)
                    ? BST_PLAYER_TURN : BST_MONSTER_TURN;
    }
}

// ============================================================
// 暗器
// ============================================================
void processThrowSelect(BattleContext& ctx) {
    system("cls");
    printf("===== 暗器 =====\n");
    vector<int> thr;
    for (auto& p : player.bag) {
        if (p.first > 0 && p.first < (int)itemDB.size()
            && itemDB[p.first].id == p.first
            && itemDB[p.first].type == THROWING) {
            thr.push_back(p.first);
            printf("[%d] %s x%d\n", (int)thr.size(),
                   safeItemName(p.first).c_str(), p.second);
        }
    }
    if (thr.empty()) {
        printf("没有暗器！\n");
        Sleep(500);
        ctx.state = BST_PLAYER_TURN;
        return;
    }
    printf("[C] 返回\n");
    char ch = read();
    if (ch == 'c' || ch == 'C') {
        ctx.state = BST_PLAYER_TURN;
        return;
    }
    int i = ch - '1';
    if (i >= 0 && i < (int)thr.size()) {
        int id = thr[i];
        removeBag(id, 1);
        ctx.mHp -= itemDB[id].effectVal;
        printf("使用 %s，造成 %d 伤害\n",
               safeItemName(id).c_str(), itemDB[id].effectVal);
        Sleep(500);
        ctx.state = BST_MONSTER_TURN;
    }
}

// ============================================================
// 怪物技能
// ============================================================
void monsterSkill(BattleContext& ctx) {
    int id = ctx.monsterId;
    if (id >= 10 && id < 100) {
        int dmg = (int)(ctx.mAtk * 1.5);
        printf("%s 使用强力撕咬！无法格挡！\n", ctx.mName.c_str());
        int real = max(1, dmg - getTotalDef());
        player.hp -= real;
        printf("受到 %d 伤害\n", real);
    } else if (id >= 100 && id < 200) {
        if (rand() % 2 == 0) {
            int dmg = (int)(ctx.mAtk * 2.0);
            printf("%s 使用暗影冲击！无法格挡！\n", ctx.mName.c_str());
            int real = max(1, dmg - getTotalDef());
            player.hp -= real;
            printf("受到 %d 伤害\n", real);
        } else {
            ctx.mDef = (int)(ctx.mDef * 1.05);
            printf("%s 使用黑暗护盾，防御提升！\n", ctx.mName.c_str());
        }
    }
}

// ============================================================
// F5: 怪物回合不再递减状态持续时间（已改在 battle() 主循环减一次）
// F11: 符修回蓝移到 battle() 开头
// ============================================================
void processMonsterTurn(BattleContext& ctx) {
    // 怪物眩晕
    if (ctx.monsterStunTurns > 0) {
        ctx.monsterStunTurns--;
        if (ctx.monsterStunTurns == 0) {
            printf("怪物从眩晕中恢复！\n");
            if (ctx.charge > 0) {
                int dmg = getTotalAtk() * (1 + ctx.charge / 2);
                dmg = max(1, dmg - ctx.mDef);
                ctx.mHp -= dmg;
                printf("蓄力爆发！造成 %d 点伤害！\n", dmg);
                ctx.charge = 0;
                Sleep(500);
                if (ctx.mHp <= 0) { ctx.state = BST_VICTORY; return; }
            }
        } else {
            printf("怪物处于眩晕状态，无法行动（剩余 %d 回合）。\n",
                   ctx.monsterStunTurns);
            Sleep(600);
        }
        ctx.state = BST_PLAYER_TURN;
        return;
    }

    // 影遁反击
    if (player.statusDur[3] > 0) {
        player.statusDur[3]--;
        printf("影遁触发！闪避并反击\n");
        int dmg = max(1, getTotalAtk() - ctx.mDef);
        ctx.mHp -= dmg;
        printf("反击造成 %d 伤害\n", dmg);
        Sleep(500);
        ctx.state = (ctx.mHp <= 0) ? BST_VICTORY : BST_PLAYER_TURN;
        return;
    }

    int skillChance = (ctx.monsterId >= 200) ? 50
                    : (ctx.monsterId >= 100) ? 40
                    : (ctx.monsterId >= 10)  ? 30 : 0;
    if (skillChance && rand() % 100 < skillChance) {
        monsterSkill(ctx);
    } else {
        printf("怪物攻击！请格挡 [J]上 [U]右 [I]下 [K]左\n");
        char def = read();
        int pDir = (def == 'j' || def == 'J') ? 0
                 : (def == 'u' || def == 'U') ? 1
                 : (def == 'i' || def == 'I') ? 2
                 : (def == 'k' || def == 'K') ? 3 : -1;
        int mDir = rand() % 4;
        if (pDir == mDir) {
            printf("格挡成功！\n");
        } else {
            int dmg = max(1, ctx.mAtk - getTotalDef());
            player.hp -= dmg;
            printf("受到 %d 伤害\n", dmg);
            // 反伤
            if (player.statusDur[4] > 0) {
                int reb = dmg / 2;
                ctx.mHp -= reb;
                printf("反伤 %d 伤害\n", reb);
                player.statusDur[4]--;
            }
        }
    }
    Sleep(600);
    ctx.state = BST_PLAYER_TURN;
}

// ============================================================
// F2: firstAttack 每场战斗重置
// F5: 状态持续时间在 battle 主循环减一次
// F7: 击杀计数只在 battle 里加一次
// F11: 符修每回合回蓝（无论怪物是否眩晕）
// ============================================================
bool battle(Monster& mon, int& mx, int& my) {
    BattleContext ctx;
    ctx.mon = &mon;
    ctx.mHp = mon.hp;
    ctx.mMaxHp = mon.hp;
    ctx.mAtk = mon.atk;
    ctx.mDef = mon.def;
    ctx.mName = mon.name;
    ctx.charge = 0;
    ctx.poisonDmg = ctx.poisonTurn = 0;
    ctx.state = BST_PLAYER_TURN;
    ctx.monsterStunTurns = 0;
    ctx.monsterId = mon.id;

    // F2: 每场战斗重置首次攻击
    player.firstAttack = true;
    player.usedPotionInBattle = false;

    bool isBoss = (mon.id >= 100 && mon.id < 200);

    // 阵修"阵基"被动
    if (player.classId == 5) {
        int r = rand() % 3;
        if (r == 0) player.statusDur[2] = 3;
        else if (r == 1) player.statusDur[7] = 3;
        else player.statusDur[4] = 2;
        printf("阵基触发，获得随机增益！\n");
    }

    while (ctx.mHp > 0 && player.hp > 0) {
        // F5: 状态持续时间只在主循环减一次
        for (int i = 0; i < 10; i++) {
            if (player.statusDur[i] > 0) player.statusDur[i]--;
        }
        // F4: 技能冷却递减
        for (size_t i = 0; i < player.skillCd.size(); i++) {
            if (player.skillCd[i] > 0) player.skillCd[i]--;
        }
        // F11: 符修每回合回蓝（无论怪物是否眩晕）
        if (player.classId == 4) {
            player.mp = min(player.maxMp, player.mp + 2);
        }

        // 中毒伤害
        if (ctx.poisonTurn > 0) {
            ctx.mHp -= ctx.poisonDmg;
            ctx.poisonTurn--;
        }
        if (ctx.mHp <= 0) { ctx.state = BST_VICTORY; break; }

        // 玩家眩晕/冰冻检查（此时 statusDur 已递减，剩余 > 0 表示仍生效）
        bool playerStun   = (player.statusDur[1] > 0);
        bool playerFrozen = (player.statusDur[5] > 0);

        system("cls");
        setColor(15);
        printf("===== 战斗 ===== %s  VS  %s\n",
               player.name.c_str(), ctx.mName.c_str());
        printf("生命: %d/%d  灵力: %d/%d  攻击: %d  防御: %d  暴击: %d%%  暴伤: %.2f\n",
               player.hp, getMaxHp(), player.mp, player.maxMp,
               getTotalAtk(), getTotalDef(),
               getTotalCrit(), getTotalCritDmg());
        printf("怪物生命: %d/%d  眩晕剩余: %d 回合\n",
               ctx.mHp, ctx.mMaxHp, ctx.monsterStunTurns);
        if (ctx.charge) printf("蓄力层数: %d\n", ctx.charge);

        switch (ctx.state) {
            case BST_PLAYER_TURN:
                if (playerStun || playerFrozen) {
                    printf("你无法行动！\n");
                    Sleep(800);
                    ctx.state = BST_MONSTER_TURN;
                    break;
                }
                printf("[1]攻击  [2]技能  [3]丹药  [4]暗器");
                if (ctx.monsterStunTurns > 0)
                    printf("  [5]蓄力  [0]结束回合");
                printf("\n");
                {
                    char cmd = read();
                    if (cmd == '1') {
                        processPlayerAttack(ctx);
                    } else if (cmd == '2') {
                        ctx.state = BST_PLAYER_SKILL_SELECT;
                    } else if (cmd == '3') {
                        ctx.state = BST_PLAYER_ITEM_SELECT;
                    } else if (cmd == '4') {
                        ctx.state = BST_PLAYER_THROW_SELECT;
                    } else if (cmd == '5' && ctx.monsterStunTurns > 0) {
                        if (ctx.charge >= 3) {
                            printf("蓄力已达最大层数！\n");
                            Sleep(400);
                        } else {
                            ctx.charge++;
                            printf("蓄力 +1，当前层数: %d\n", ctx.charge);
                            Sleep(400);
                        }
                    } else if (cmd == '0' && ctx.monsterStunTurns > 0) {
                        ctx.state = BST_MONSTER_TURN;
                    }
                }
                break;
            case BST_PLAYER_SKILL_SELECT:
                processSkillSelect(ctx);
                break;
            case BST_PLAYER_ITEM_SELECT:
                processItemSelect(ctx);
                break;
            case BST_PLAYER_THROW_SELECT:
                processThrowSelect(ctx);
                break;
            case BST_MONSTER_TURN:
                processMonsterTurn(ctx);
                break;
            default:
                break;
        }
        if (ctx.mHp <= 0) ctx.state = BST_VICTORY;
        if (player.hp <= 0) {
            // 体修不灭金身
            if (player.statusDur[6] > 0) {
                player.hp = 1;
                player.statusDur[6] = 0;
                printf("不灭金身触发！\n");
                Sleep(500);
            } else {
                ctx.state = BST_DEFEAT;
            }
        }
    }

    // 战败
    if (player.hp <= 0) {
        system("cls");
        printf("你被击败了……\n");
        // 战败惩罚：扣 10% 金币
        int penalty = player.gold / 10;
        player.gold -= penalty;
        printf("损失 %d 金币。\n", penalty);
        player.hp = getMaxHp();
        player.mp = player.maxMp;
        Sleep(2000);
        return false;
    }

    // 胜利
    system("cls");
    printf("击败 %s！获得 %d 经验，%d 金币\n",
           ctx.mName.c_str(), mon.exp, mon.gold);
    gainExp(mon.exp);
    player.gold += mon.gold;

    for (auto& l : mon.loot) {
        if (rand() % 100 < l.second) {
            addBag(l.first);
            printf("获得物品: %s\n", safeItemName(l.first).c_str());
        }
    }

    // F7: 击杀计数只在这里加一次
    player.killCount++;
    player.goldEarned += mon.gold;
    if (mon.isSpirit) player.spiritKillCount++;
    if (mon.name.find("Boss") != string::npos) player.bossKillCount++;
    checkAchievements();
    checkQuestProgress(mon.id);

    // 隐藏成就（Boss 战）
    if (isBoss) {
        // 极限反杀
        if (player.hp <= 5 && !achievements[ACH_LIMIT_REVERSAL].unlocked)
            unlockAchievement(ACH_LIMIT_REVERSAL);
        // 坚毅之人
        if (!player.usedPotionInBattle && !achievements[ACH_PERSISTENT].unlocked)
            unlockAchievement(ACH_PERSISTENT);
        // 狂妄之人：木剑 + 无防具/服装/饰品
        bool noArmor = (player.armor == 0
                     && player.clothing == 0
                     && player.accessory == 0);
        if (player.weapon == 1 && noArmor
            && !achievements[ACH_ARROGANT].unlocked)
            unlockAchievement(ACH_ARROGANT);
    }

    Sleep(1500);
    mx = my = 0;
    return true;
}

// ============================================================
// F15: 统一通关判定
// ============================================================
void tryExitMap(bool& exited) {
    exited = false;
    if (currentMap.px != currentMap.ex || currentMap.py != currentMap.ey) return;
    if (currentMap.exitLocked) return;

    printf("通过 %s！\n", currentMap.name.c_str());
    Sleep(1500);
    player.mapsCleared++;

    // 无敌之人（新手村除外）
    if (currentMap.name != mapChineseNames[1]) {
        if (!player.usedPotionInCurrentMap
            && !achievements[ACH_INVINCIBLE].unlocked)
            unlockAchievement(ACH_INVINCIBLE);
    }
    checkAchievements();
    exited = true;
}
// ============================================================
// 地图：19 张地图
// ============================================================
void loadMap(int id) {
    currentMap.boxes.clear();
    currentMap.mobs.clear();
    currentMap.npcs.clear();
    currentMap.exitLocked = true;
    // 统一默认值
    currentMap.px = 2; currentMap.py = 3;
    currentMap.ex = 7; currentMap.ey = 68;
    currentMap.themeColor = 10;

    switch (id) {
        case 1:
            currentMap.name = mapChineseNames[1];
            currentMap.data = {
                "*******************************************************",
                "*                                                     *",
                "*                                                     *",
                "*                                                     *",
                "*                                                     *",
                "*                                                     *",
                "*                                                     *",
                "*                                                     *",
                "*******************************************************"};
            currentMap.px = 4; currentMap.py = 3;
            currentMap.ex = 7; currentMap.ey = 1;
            currentMap.boxes.push_back({3, 28, {2, 102, 301}, false});
            currentMap.mobs.push_back({0, 5, 25, 35, true});
            currentMap.npcs.push_back({3, 35, 'N', 14, "村长",
                {"年轻人，请帮我把信交给铁匠。", "铁匠就在村子东边。"}, 1});
            currentMap.npcs.push_back({6, 10, 'N', 10, "猎人",
                {"最近森林里的灵兽皮毛很值钱。", "给我5张皮毛，我会给你报酬。"}, 2});
            break;
        case 2:
            currentMap.name = mapChineseNames[2];
            currentMap.data = {
                "**********************************************************************",
                "*         ~~~                      ######                            *",
                "*         ~ ~                      #                                 *",
                "*         ~~~                           #                            *",
                "*                                  ######                            *",
                "*                                                                    *",
                "*                                                                    *",
                "*                                                                    *",
                "*                                                                    *",
                "**********************************************************************"};
            currentMap.px = 2; currentMap.py = 4;
            currentMap.ex = 7; currentMap.ey = 68;
            currentMap.boxes.push_back({4, 55, {3, 401}, false});
            currentMap.boxes.push_back({6, 25, {103, 402}, false});
            currentMap.mobs.push_back({1, 3, 35, 50, true});
            currentMap.mobs.push_back({1, 5, 60, 50, true});
            currentMap.mobs.push_back({10, 6, 45, 90, true});
            currentMap.npcs.push_back({4, 30, 'N', 11, "采药人",
                {"前面的洞穴里有珍贵的灵芝。", "帮我采3株，我会给你报酬。"}, 3});
            break;
        case 3:
            currentMap.name = mapChineseNames[3];
            currentMap.data = {
                "*****************************************************************",
                "*     ~~~~~~               ~~~~~~~~~                            *",
                "*                                                               *",
                "*     ~    ~~~~            ~       ~~~~                         *",
                "*     ~~~~~~               ~~~~~~~~  ~                          *",
                "*            ~~~~                            X                 *",
                "*                                                               *",
                "*                                                               *",
                "*                                                               *",
                "*****************************************************************"};
            currentMap.px = 3; currentMap.py = 2;
            currentMap.ex = 8; currentMap.ey = 63;
            currentMap.boxes.push_back({2, 40, {4, 403}, false});
            currentMap.boxes.push_back({5, 52, {104, 404}, false});
            currentMap.mobs.push_back({2, 3, 35, 70, true});
            currentMap.mobs.push_back({11, 5, 50, 80, true});
            currentMap.mobs.push_back({100, 7, 58, 180, true});
            currentMap.npcs.push_back({5, 10, 'N', 13, "探险家",
                {"这洞穴深处有强大的怪物。", "打败黑山老妖，我会奖赏你。"}, 4});
            currentMap.themeColor = 8;
            break;
        case 4:
            currentMap.name = mapChineseNames[4];
            currentMap.data = {
                "**********************************************************************",
                "*         ####          ~~~~                   ~~~~                  *",
                "*                       ~                         ~                  *",
                "*         ####             ~                   ~                     *",
                "*                       ~~~~                   ~~~~                  *",
                "*                                                       ~~~~~~~~~~  *",
                "*                                                                     *",
                "*                                                                     *",
                "*                                                                     *",
                "**********************************************************************"};
            currentMap.px = 2; currentMap.py = 5;
            currentMap.ex = 7; currentMap.ey = 68;
            currentMap.boxes.push_back({3, 60, {8, 408}, false});
            currentMap.boxes.push_back({5, 30, {12, 409}, false});
            currentMap.mobs.push_back({12, 4, 55, 110, true});
            currentMap.mobs.push_back({13, 6, 50, 100, true});
            currentMap.mobs.push_back({101, 7, 35, 220, true});
            currentMap.npcs.push_back({5, 25, 'N', 14, "考古学家",
                {"找到完整的藏宝图，你将获得巨大财富。", "收集三块藏宝图碎片可以合成完整的藏宝图。"}, 5});
            currentMap.themeColor = 14;
            break;
        case 5:
            currentMap.name = mapChineseNames[5];
            currentMap.data = {
                "**********************************************************************",
                "*  ~~~~~        ##              ~~~~~~~~~        ##                 *",
                "*               ##                               ##                 *",
                "*        ~~     ##      X               ~~~~     ##        F        *",
                "*  ~~~~~        ##              ~~~~~~~~         ##                 *",
                "*        ~~~~   ##                      ~~~~    ##                 *",
                "*                                                                     *",
                "*                                                                     *",
                "*                                                                     *",
                "**********************************************************************"};
            currentMap.px = 2; currentMap.py = 3;
            currentMap.ex = 7; currentMap.ey = 68;
            currentMap.boxes.push_back({3, 50, {14, 411}, false});
            currentMap.boxes.push_back({5, 25, {107, 412}, false});
            currentMap.mobs.push_back({14, 3, 45, 150, true});
            currentMap.mobs.push_back({102, 5, 60, 280, true});
            currentMap.mobs.push_back({100, 6, 65, 180, true});
            currentMap.npcs.push_back({2, 20, 'N', 12, "幽灵商人",
                {"帮我消灭暗影刺客，我将赐你宝物。"}, 9});
            currentMap.themeColor = 5;
            break;
        case 6:
            currentMap.name = mapChineseNames[6];
            currentMap.data = {
                "***************************************************************************",
                "*  ~~~~~        #######            ~~~~~            #######               *",
                "*      ~              #            ~                #                     *",
                "*  ~   ~~~~     #     #    X       ~   ~~~~         #     #  ~~~~         *",
                "*  ~~~~~  ~     #                  ~~~~~  ~               #  ~  ~         *",
                "*        ~~~~   #######                  ~~~~       #######  ~~~~         *",
                "*                                                                           *",
                "*                                                                           *",
                "*                                                                           *",
                "***************************************************************************"};
            currentMap.px = 2; currentMap.py = 4;
            currentMap.ex = 7; currentMap.ey = 73;
            currentMap.boxes.push_back({3, 65, {16, 415}, false});
            currentMap.boxes.push_back({4, 45, {110, 416}, false});
            currentMap.mobs.push_back({15, 3, 50, 180, true});
            currentMap.mobs.push_back({103, 5, 60, 350, true});
            currentMap.mobs.push_back({104, 6, 40, 320, true});
            currentMap.mobs.push_back({109, 7, 30, 600, true});
            currentMap.npcs.push_back({4, 30, 'N', 11, "流浪剑客",
                {"击败冰霜巨龙，证明你的实力！"}, 6});
            currentMap.themeColor = 12;
            break;
        case 7:  // 冰霜雪原：铁匠被困
            currentMap.name = mapChineseNames[7];
            currentMap.data = {
                "**********************************************************************",
                "*  ~~~~~~~~~~~~~           XXXX            ~~~~~~~~~~~~~             *",
                "*  ~                                                    ~             *",
                "*      ~~~~~   ~                            ~   ~~~~~                 *",
                "*  ~       ~   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~   ~   ~   ~             *",
                "*  ~~~~~   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~   ~~~~~             *",
                "*                                                                     *",
                "*                                                                     *",
                "*                                                                     *",
                "**********************************************************************"};
            currentMap.px = 2; currentMap.py = 3;
            currentMap.ex = 7; currentMap.ey = 68;
            currentMap.boxes.push_back({3, 60, {31, 413}, false});
            currentMap.boxes.push_back({5, 40, {111, 402}, false});
            currentMap.mobs.push_back({5, 1, 15, 65, true});
            currentMap.mobs.push_back({5, 2, 25, 65, true});
            currentMap.mobs.push_back({5, 3, 35, 65, true});
            currentMap.mobs.push_back({12, 4, 45, 110, true});
            currentMap.mobs.push_back({17, 6, 55, 300, true});
            currentMap.npcs.push_back({3, 25, 'N', 13, "铁匠",
                {"年轻人！快救救我！我被这群冰霜怪物困在这里好久了！",
                 "你身上有村长的信？太好了！快给我看看！",
                 "什么？村长他...还活着？太好了！",
                 "这把玄铁剑...这是村长的佩剑啊！当年我们并肩作战时...",
                 "作为报答，我用这把玄铁剑给你锻造一件神兵！"}, 1});
            currentMap.themeColor = 11;
            break;
        case 8:
            currentMap.name = mapChineseNames[8];
            currentMap.data = {
                "**********************************************************************",
                "*  FFFFFF        ~~~~~~~~~           FFFFFF        ~~~~~~~~~         *",
                "*  F    F        ~       ~                F        ~       ~         *",
                "*  F    FFFF     ~       ~~~~        F    FFFF     ~       ~~~~      *",
                "*                ~       ~  ~        F             ~       ~  ~      *",
                "*  FFFFFFFFFF F  ~~~~   ~~~~         FFFFFFFFFF F  ~~~~   ~~~~      *",
                "*                                                                     *",
                "*                                                                     *",
                "*                                                                     *",
                "**********************************************************************"};
            currentMap.px = 2; currentMap.py = 4;
            currentMap.ex = 7; currentMap.ey = 68;
            currentMap.boxes.push_back({2, 55, {25, 414}, false});
            currentMap.boxes.push_back({5, 30, {112, 402}, false});
            currentMap.mobs.push_back({4, 3, 45, 60, true});
            currentMap.mobs.push_back({11, 5, 50, 80, true});
            currentMap.mobs.push_back({102, 6, 40, 280, true});
            currentMap.npcs.push_back({3, 20, 'N', 12, "丹药师",
                {"炼成5颗大还丹，我教你新配方。"}, 7});
            currentMap.themeColor = 12;
            break;
        case 9:
            currentMap.name = mapChineseNames[9];
            currentMap.data = {
                "**********************************************************************",
                "*  ~~~~~~~~~~           ********           ~~~~~~~~~~                *",
                "*                       *      *                    ~                *",
                "*  ~  * **  ~~~  ~~~~  ~*      ~~~~~~~~~~~~  ****  ~                *",
                "*  ~  *  *  ~                      ~           *  ~                *",
                "*  ~~~~  ~~~~                      ~~~~~~~~~  ~~~~                *",
                "*                                                                     *",
                "*                                                                     *",
                "*                                                                     *",
                "**********************************************************************"};
            currentMap.px = 2; currentMap.py = 3;
            currentMap.ex = 7; currentMap.ey = 68;
            currentMap.boxes.push_back({2, 60, {17, 409}, false});
            currentMap.mobs.push_back({6, 3, 45, 70, true});
            currentMap.mobs.push_back({13, 5, 55, 100, true});
            currentMap.mobs.push_back({104, 6, 65, 320, true});
            currentMap.npcs.push_back({4, 50, 'N', 14, "雷神使者",
                {"击败雷神，证明你有资格获得雷神锤。"}, -1});
            currentMap.themeColor = 14;
            break;
        case 10:
            currentMap.name = mapChineseNames[10];
            currentMap.data = {
                "**********************************************************************",
                "*  ########           ~~~~~~~~~~           ########                  *",
                "*         #           ~        ~                  #                  *",
                "*  #  **  ###########   ~  ~  ~   ########  **  #                  *",
                "*  #  **               ~  ~  ~               **                     *",
                "*  ########           ~~~~~~~~~~           ########                  *",
                "*                                                                     *",
                "*                                                                     *",
                "*                                                                     *",
                "**********************************************************************"};
            currentMap.px = 2; currentMap.py = 3;
            currentMap.ex = 7; currentMap.ey = 68;
            currentMap.boxes.push_back({2, 55, {21, 419}, false});
            currentMap.boxes.push_back({4, 40, {117, 402}, false});
            currentMap.mobs.push_back({8, 3, 45, 55, true});
            currentMap.mobs.push_back({108, 5, 60, 350, true});
            currentMap.mobs.push_back({15, 6, 50, 180, true});
            currentMap.npcs.push_back({5, 30, 'N', 15, "圣殿骑士",
                {"净化暗影深渊，你将获得圣光祝福。"}, -1});
            currentMap.themeColor = 15;
            break;
        case 11:
            currentMap.name = mapChineseNames[11];
            currentMap.data = {
                "**********************************************************************",
                "*  ~~~~~~~~~           **********           ~~~~~~~~~                *",
                "*                      *        *                   ~                *",
                "*  ~  ****  ~~~~~  ~~~~   **   *~~~~~~~~~~  *  *  ~                *",
                "*  ~  *  *  ~                               *  *                   *",
                "*  ~~~~  ~~~~                       ~~~~  ~~~  ~~~~                *",
                "*                                                                     *",
                "*                                                                     *",
                "*                                                                     *",
                "**********************************************************************"};
            currentMap.px = 2; currentMap.py = 3;
            currentMap.ex = 7; currentMap.ey = 68;
            currentMap.boxes.push_back({2, 60, {19, 418}, false});
            currentMap.mobs.push_back({7, 3, 45, 75, true});
            currentMap.mobs.push_back({107, 5, 60, 380, true});
            currentMap.mobs.push_back({109, 6, 65, 600, true});
            currentMap.npcs.push_back({4, 35, 'N', 12, "暗影刺客",
                {"击败我，你才有资格面对最终Boss。"}, -1});
            currentMap.themeColor = 5;
            break;
        case 12:
            currentMap.name = mapChineseNames[12];
            currentMap.data = {
                "***************************************************************************",
                "*  ~~~~~~            **********            ~~~~~~                        *",
                "*  ~                 *                     ~    ~                        *",
                "*  ~    ~~~~~~~~~~~~~*   **   *~~~~~~~~~~~~~    ~~~~~~~~~                *",
                "*  ~                     **   *                         ~                *",
                "*  ~~~~~~            **********            ~~~~~~       ~~~~              *",
                "*                                                                          *",
                "*                                                                          *",
                "*                                                                          *",
                "***************************************************************************"};
            currentMap.px = 2; currentMap.py = 3;
            currentMap.ex = 7; currentMap.ey = 73;
            currentMap.boxes.push_back({3, 65, {16, 415}, false});
            currentMap.boxes.push_back({5, 50, {17, 409}, false});
            currentMap.mobs.push_back({15, 3, 50, 180, true});
            currentMap.mobs.push_back({18, 5, 65, 300, true});
            currentMap.mobs.push_back({109, 6, 40, 600, true});
            currentMap.npcs.push_back({4, 30, 'N', 11, "虚空旅者",
                {"找到混沌碎片，我将给你终极力量。"}, 10});
            currentMap.themeColor = 5;
            break;
        case 13:
            currentMap.name = mapChineseNames[13];
            currentMap.data = {
                "********************************************************************************",
                "*  ########            ~~~~~~~~~~            ########                          *",
                "*  #      #            ~        ~            #      #                          *",
                "*  #  **  ##############   ~~~~   #############  **  ##########               *",
                "*     **                      ~  ##                          ##               *",
                "*  ########                ~~~               ########          ######          *",
                "*                                                                              *",
                "*                                                                              *",
                "*                                                                              *",
                "********************************************************************************"};
            currentMap.px = 2; currentMap.py = 3;
            currentMap.ex = 7; currentMap.ey = 78;
            currentMap.boxes.push_back({2, 70, {21, 419}, false});
            currentMap.boxes.push_back({4, 50, {110, 415}, false});
            currentMap.mobs.push_back({108, 3, 60, 350, true});
            currentMap.mobs.push_back({19, 5, 70, 450, true});
            currentMap.mobs.push_back({109, 6, 65, 600, true});
            currentMap.npcs.push_back({3, 40, 'N', 14, "元素长老",
                {"击败所有Boss，混沌天魔才会现身。"}, -1});
            currentMap.themeColor = 14;
            break;
        case 14:
            currentMap.name = mapChineseNames[14];
            currentMap.data = {
                "********************************************************************************",
                "*  ~~~~~~~             ********             ~~~~~~~                           *",
                "*  ~     ~             *      *             ~     ~                           *",
                "*        ~~~~~~~~~~~~~~*        ~~~~~~~~~~~~~     ~~~~~~~~~~~~~               *",
                "*                         **  *                            ~                 *",
                "*  ~~~~~~~             ********             ~ ~~~~~        ~~~~               *",
                "*                                                                              *",
                "*                                                                              *",
                "*                                                                              *",
                "********************************************************************************"};
            currentMap.px = 2; currentMap.py = 3;
            currentMap.ex = 7; currentMap.ey = 78;
            currentMap.boxes.push_back({2, 70, {110, 420}, false});
            currentMap.boxes.push_back({5, 50, {110, 415}, false});
            currentMap.mobs.push_back({19, 3, 65, 450, true});
            currentMap.mobs.push_back({107, 5, 70, 380, true});
            currentMap.mobs.push_back({109, 6, 50, 600, true});
            currentMap.npcs.push_back({4, 55, 'N', 15, "时间守护者",
                {"最终决战即将到来，你准备好了吗？"}, -1});
            currentMap.themeColor = 11;
            break;
        case 15:
            currentMap.name = mapChineseNames[15];
            currentMap.data = {
                "*************************************************************************************",
                "*  ~~~~~~           **********           ~~~~~~           **********               *",
                "*                                                                                  *",
                "*       ~  ~~~~~~~~~    **    ~~~~~  ~~~~~    ~~~~~~~~~~~~*   **   *  ~~~~         *",
                "*                       **                                *   **                   *",
                "*  ~~~~~~           **********           ~~~~~~           **********  ~~~~         *",
                "*        ~~~~~~~~~~                                ~~~~~~~~~~                      *",
                "*                                                                                   *",
                "*                                                                                   *",
                "*                                                                                   *",
                "*************************************************************************************"};
            currentMap.px = 2; currentMap.py = 3;
            currentMap.ex = 9; currentMap.ey = 83;
            currentMap.boxes.push_back({3, 75, {29, 420}, false});
            currentMap.boxes.push_back({6, 60, {110, 415}, false});
            currentMap.mobs.push_back({19, 3, 65, 500, true});
            currentMap.mobs.push_back({108, 5, 75, 450, true});
            currentMap.mobs.push_back({109, 8, 60, 600, true});
            currentMap.mobs.push_back({15, 9, 40, 400, true});
            currentMap.npcs.push_back({6, 70, 'N', 13, "先知",
                {"击败混沌天魔，拯救世界！"}, -1});
            currentMap.themeColor = 12;
            break;
        case 16:
            currentMap.name = mapChineseNames[16];
            currentMap.data = {
                "*************************************************************************************",
                "*  ######            ######            ######            ######                 *",
                "*  #    #            #    #            #    #            #    #                 *",
                "*  #    #   ######   #    #   ######   #    #   ######   #    #                 *",
                "*  #    #            #    #            #    #            #    #                 *",
                "*  ######            ######            ######            ######                 *",
                "*                                                                                *",
                "*                                                                                *",
                "*                                                                                *",
                "*                                                                                *",
                "*************************************************************************************"};
            currentMap.px = 2; currentMap.py = 3;
            currentMap.ex = 7; currentMap.ey = 83;
            currentMap.boxes.push_back({3, 70, {41, 421, 422}, false});
            currentMap.boxes.push_back({5, 50, {118, 426}, false});
            currentMap.mobs.push_back({20, 3, 50, 200, true});
            currentMap.mobs.push_back({110, 5, 60, 380, true});
            currentMap.mobs.push_back({15, 7, 45, 180, true});
            currentMap.npcs.push_back({4, 35, 'N', 14, "龙族长老",
                {"击败龙骨守卫，证明你的实力！"}, 11});
            break;
        case 17:
            currentMap.name = mapChineseNames[17];
            currentMap.data = {
                "*************************************************************************************",
                "*  ~~~~~~~~~~           ~~~~~~~~~~           ~~~~~~~~~~                    *",
                "*  ~        ~           ~        ~           ~        ~                    *",
                "*  ~        ~~~~~~~~~~~ ~        ~~~~~~~~~~~ ~        ~~~~~~~~~~~        *",
                "*  ~                     ~                     ~                           *",
                "*  ~~~~~~~~~~           ~~~~~~~~~~           ~~~~~~~~~~                    *",
                "*                                                                          *",
                "*                                                                          *",
                "*                                                                          *",
                "*                                                                          *",
                "*************************************************************************************"};
            currentMap.px = 2; currentMap.py = 3;
            currentMap.ex = 7; currentMap.ey = 83;
            currentMap.boxes.push_back({3, 65, {42, 430}, false});
            currentMap.boxes.push_back({5, 45, {211, 430}, false});
            currentMap.mobs.push_back({23, 3, 55, 200, true});
            currentMap.mobs.push_back({112, 5, 65, 400, true});
            currentMap.mobs.push_back({15, 7, 50, 180, true});
            currentMap.npcs.push_back({4, 30, 'N', 15, "仙灵使者",
                {"寻找仙灵草，炼制仙丹。"}, 12});
            currentMap.themeColor = 15;
            break;
        case 18:
            currentMap.name = mapChineseNames[18];
            currentMap.data = {
                "*************************************************************************************",
                "*  ~~~~~~~            #######            ~~~~~~~                        *",
                "*  ~     ~            #     #            ~     ~                        *",
                "*  ~     ~~~~~~~~~~~~~#     #~~~~~~~~~~~~~     ~~~~~~~~~~~~~            *",
                "*  ~                   #     #                   ~                        *",
                "*  ~~~~~~~            #######            ~~~~~~~                        *",
                "*                                                                          *",
                "*                                                                          *",
                "*                                                                          *",
                "*                                                                          *",
                "*************************************************************************************"};
            currentMap.px = 2; currentMap.py = 3;
            currentMap.ex = 7; currentMap.ey = 83;
            currentMap.boxes.push_back({3, 70, {45, 429, 420}, false});
            currentMap.boxes.push_back({5, 50, {120, 429}, false});
            currentMap.mobs.push_back({21, 3, 55, 200, true});
            currentMap.mobs.push_back({22, 5, 60, 200, true});
            currentMap.mobs.push_back({111, 7, 50, 400, true});
            currentMap.npcs.push_back({4, 40, 'N', 5, "魔界使者",
                {"击败魔界领主，获取魔界之钥。"}, 13});
            currentMap.themeColor = 5;
            break;
        case 19:
            currentMap.name = mapChineseNames[19];
            currentMap.data = {
                "*************************************************************************************",
                "*  ~~~~~~           **********           ~~~~~~           **********      *",
                "*  ~                 *        *           ~                 *        *      *",
                "*  ~    ~~~~~~~~~~~~~*        ~~~~~~~~~~~~~    ~~~~~~~~~~~~~*        ~~~~~~*",
                "*  ~                 *                      ~                 *              *",
                "*  ~~~~~~           **********           ~~~~~~           **********      *",
                "*        ~~~~~~~~~~                                ~~~~~~~~~~              *",
                "*                                                                          *",
                "*                                                                          *",
                "*                                                                          *",
                "*************************************************************************************"};
            currentMap.px = 2; currentMap.py = 3;
            currentMap.ex = 9; currentMap.ey = 83;
            currentMap.boxes.push_back({3, 75, {40, 120, 420}, false});
            currentMap.boxes.push_back({6, 60, {45, 120}, false});
            currentMap.mobs.push_back({19, 3, 65, 500, true});
            currentMap.mobs.push_back({109, 5, 75, 600, true});
            currentMap.mobs.push_back({111, 7, 60, 500, true});
            currentMap.mobs.push_back({112, 8, 50, 450, true});
            currentMap.npcs.push_back({6, 70, 'N', 13, "最终先知",
                {"击败所有强敌，完成终极试炼！"}, 14});
            currentMap.themeColor = 13;
            break;
        default:
            currentMap.name = "未知区域";
            currentMap.data = {"*****", "*   *", "*****"};
            currentMap.px = 1; currentMap.py = 1;
            currentMap.ex = 1; currentMap.ey = 3;
            currentMap.exitLocked = false;
            currentMap.themeColor = 7;
            break;
    }
}

bool isMapClear() {
    for (auto& m : currentMap.mobs) if (m.alive) return false;
    return true;
}

void drawMap() {
    for (int i = 0; i < (int)currentMap.data.size(); i++) {
        for (int j = 0; j < (int)currentMap.data[i].size(); j++) {
            if (i == currentMap.px && j == currentMap.py) {
                setColor(14); cout << 'P'; continue;
            }
            if (!currentMap.exitLocked && i == currentMap.ex && j == currentMap.ey) {
                setColor(10); cout << 'O'; continue;
            }
            bool isBox = false;
            for (auto& b : currentMap.boxes) {
                if (!b.opened && b.x == i && b.y == j) {
                    setColor(6); cout << 'B'; isBox = true; break;
                }
            }
            if (isBox) continue;
            bool isMob = false;
            for (auto& m : currentMap.mobs) {
                if (m.alive && m.x > 0 && m.y > 0 && m.x == i && m.y == j) {
                    setColor(monsterDB[m.type].color);
                    cout << monsterDB[m.type].symbol;
                    isMob = true; break;
                }
            }
            bool isNpc = false;
            for (auto& npc : currentMap.npcs) {
                if (npc.x == i && npc.y == j) {
                    setColor(npc.color); cout << npc.symbol;
                    isNpc = true; break;
                }
            }
            if (isNpc || isMob) continue;
            char c = currentMap.data[i][j];
            if (c == '*') setColor(7);
            else if (c == '~') setColor(9);
            else if (c == '#') setColor(2);
            else if (c == 'X') { setColor(12); cout << 'X'; continue; }
            else if (c == 'F') { setColor(4); cout << 'F'; continue; }
            else setColor(15);
            cout << c;
        }
        cout << endl;
    }
    resetColor();
}

// ============================================================
// 探索
// F15: 使用 tryExitMap 统一通关判定
// F7: 击杀计数不再重复
// ============================================================
void explore() {
    player.usedPotionInCurrentMap = false;
    while (1) {
        system("cls");
        printf("地图：%s  生命：%d/%d  金币：%d\n",
               currentMap.name.c_str(), player.hp, getMaxHp(), player.gold);
        drawMap();
        printf("[W]上 [S]下 [A]左 [D]右  [E]背包\n");
        char cmd = read();
        if (cmd == 'e' || cmd == 'E') { bagUI(); continue; }

        int dir = -1;
        if (cmd == 'w' || cmd == 'W') dir = 0;
        else if (cmd == 's' || cmd == 'S') dir = 1;
        else if (cmd == 'a' || cmd == 'A') dir = 2;
        else if (cmd == 'd' || cmd == 'D') dir = 3;

        if (dir != -1) {
            int nx = currentMap.px + dx[dir];
            int ny = currentMap.py + dy[dir];
            if (nx >= 0 && nx < (int)currentMap.data.size()
                && ny >= 0 && ny < (int)currentMap.data[0].size()) {
                char tile = currentMap.data[nx][ny];
                if (tile == '*' || tile == '#' || tile == '~') continue;

                // 陷阱 / 熔岩
                if (tile == 'X' || tile == 'F') {
                    player.hp = max(1, player.hp - 10);
                    printf("你踩到了%s！扣除 10 点生命。\n",
                           tile == 'X' ? "陷阱" : "熔岩");
                    Sleep(500);
                    if (player.hp <= 0) {
                        printf("你因伤重而倒下……\n");
                        player.trapDeathCount++;
                        // 脚滑了！
                        if (!achievements[ACH_SLIP].unlocked)
                            unlockAchievement(ACH_SLIP);
                        // 不死小强
                        if (!achievements[ACH_UNDYING].unlocked)
                            unlockAchievement(ACH_UNDYING);
                        Sleep(1000);
                        player.hp = getMaxHp();
                        player.mp = player.maxMp;
                        return;
                    }
                }

                // 宝箱
                bool boxOpened = false;
                for (auto& b : currentMap.boxes) {
                    if (!b.opened && b.x == nx && b.y == ny) {
                        b.opened = true;
                        printf("打开宝箱！获得：\n");
                        for (int itemId : b.items) {
                            addBag(itemId);
                            printf("  %s\n", safeItemName(itemId).c_str());
                            if (itemId >= 705 && itemId <= 707) {
                                player.treasurePieces++;
                                if (player.treasurePieces >= 3) {
                                    addBag(708);
                                    printf("三块藏宝图碎片自动合成了完整的藏宝图！\n");
                                    player.treasurePieces = 0;
                                    if (!achievements[ACH_TREASURE_HUNTER].unlocked)
                                        unlockAchievement(ACH_TREASURE_HUNTER);
                                }
                            }
                        }
                        Sleep(1000);
                        boxOpened = true;
                        break;
                    }
                }
                if (boxOpened) {
                    currentMap.px = nx; currentMap.py = ny;
                    if (currentMap.exitLocked && isMapClear()) {
                        currentMap.exitLocked = false;
                        printf("所有怪物已消灭，出口解锁！\n");
                        Sleep(800);
                    }
                    bool exited = false;
                    tryExitMap(exited);
                    if (exited) return;
                    if (rand() % 100 < 15) randomEventExtended();
                    continue;
                }

                // NPC
                bool npcFound = false;
                for (auto& npc : currentMap.npcs) {
                    if (npc.x == nx && npc.y == ny) {
                        npcFound = true;
                        system("cls");
                        printf("你遇到了 %s。\n", npc.name.c_str());
                        Sleep(300);

                        // 铁匠
                        if (npc.name == "铁匠") {
                            bool allDead = true;
                            for (auto& m : currentMap.mobs)
                                if (m.alive) { allDead = false; break; }
                            if (!allDead) {
                                printf("铁匠：'年轻人，我被困在这里太久了！请帮我消灭周围的冰霜怪物！'\n");
                                Sleep(1000);
                                break;
                            }
                            printf("铁匠：'谢谢你！我终于自由了！'\n");
                            Sleep(500);

                            Quest& q = quests[0];
                            if (q.taken && !q.completed) {
                                if (countBag(711) == 0) {
                                    addBag(711, 1);
                                    player.gold += 200;
                                    player.goldEarned += 200;
                                    printf("铁匠：'这是村长年轻时用过的佩剑，你替我还给他吧。'\n");
                                    printf("获得 [玄铁剑] x1！\n");
                                    printf("获得 200 金币！\n");
                                } else {
                                    printf("铁匠：'你已经有玄铁剑了，快回去找村长吧。'\n");
                                }
                            } else if (q.completed) {
                                printf("铁匠：'你已经把剑还给村长了？那就好。'\n");
                            } else {
                                printf("铁匠：'你是新来的冒险者吧？'\n");
                                printf("铁匠：'你可以先去新手村找村长了解一下情况。'\n");
                            }
                            Sleep(500);
                            break;
                        }

                        // 村长
                        if (npc.name == "村长") {
                            for (auto& line : npc.dialogs) {
                                printf("%s：%s\n", npc.name.c_str(), line.c_str());
                                Sleep(800);
                            }
                            Quest& q = quests[0];
                            if (!q.taken) {
                                printf("接受任务 [%s] 吗？(y/n)\n", q.name.c_str());
                                char ans = read();
                                if (ans == 'y' || ans == 'Y') {
                                    q.taken = true;
                                    if (countBag(701) == 0) addBag(701, 1);
                                    printf("任务已接受！获得 [村长手信] x1。\n");
                                }
                            } else if (!q.completed) {
                                if (countBag(711) >= 1) {
                                    removeBag(711, 1);
                                    q.completed = true;
                                    player.gold += q.rewardGold;
                                    player.goldEarned += q.rewardGold;
                                    if (q.rewardItemId > 0)
                                        addBag(q.rewardItemId);
                                    printf("村长：'这是当年我与铁匠并肩作战时的佩剑啊！'\n");
                                    printf("任务完成！获得 %d 金币和 %s。\n",
                                           q.rewardGold,
                                           safeItemName(q.rewardItemId).c_str());
                                    if (!achievements[ACH_BLACKSMITH_HEIR].unlocked)
                                        unlockAchievement(ACH_BLACKSMITH_HEIR);
                                } else {
                                    printf("村长：'你找到铁匠了吗？他应该还在冰霜雪原。'\n");
                                }
                            } else {
                                printf("村长：'谢谢你完成了我交代的事。'\n");
                            }
                            Sleep(500);
                            break;
                        }

                        // 普通 NPC
                        for (auto& line : npc.dialogs) {
                            printf("%s：%s\n", npc.name.c_str(), line.c_str());
                            Sleep(800);
                        }
                        if (npc.questId != -1 && npc.questId <= 15) {
                            if (npc.name != "村长" && npc.name != "铁匠") {
                                Quest& q = quests[npc.questId - 1];
                                if (!q.taken) {
                                    printf("接受任务 [%s] 吗？(y/n)\n", q.name.c_str());
                                    char ans = read();
                                    if (ans == 'y' || ans == 'Y') {
                                        q.taken = true;
                                        printf("任务已接受！\n");
                                    }
                                } else if (!q.completed) {
                                    bool can = false;
                                    if (q.reqItemId > 0) {
                                        if (countBag(q.reqItemId) >= q.reqItemCount) {
                                            removeBag(q.reqItemId, q.reqItemCount);
                                            can = true;
                                        } else {
                                            printf("任务还需要 %d 个 %s。\n",
                                                   q.reqItemCount - countBag(q.reqItemId),
                                                   safeItemName(q.reqItemId).c_str());
                                        }
                                    } else if (q.reqItemId == -1) {
                                        if (player.killCount > 0) can = true;
                                    } else if (q.reqItemId == -2) {
                                        if (player.bossKillCount >= 1) can = true;
                                    } else if (q.reqItemId == -3) {
                                        if (player.bossKillCount >= 2) can = true;
                                    } else if (q.reqItemId == -4) {
                                        if (player.bossKillCount >= 3) can = true;
                                    }
                                    if (can) {
                                        q.completed = true;
                                        player.gold += q.rewardGold;
                                        player.goldEarned += q.rewardGold;
                                        if (q.rewardItemId > 0)
                                            addBag(q.rewardItemId);
                                        printf("任务完成！获得 %d 金币和 %s。\n",
                                               q.rewardGold,
                                               safeItemName(q.rewardItemId).c_str());
                                        checkAchievements();
                                    }
                                } else {
                                    printf("你已经完成了这个任务。\n");
                                }
                            }
                        }
                        Sleep(500);
                        break;
                    }
                }

                if (npcFound) {
                    currentMap.px = nx; currentMap.py = ny;
                    if (currentMap.exitLocked && isMapClear()) {
                        currentMap.exitLocked = false;
                        printf("所有怪物已消灭，出口解锁！\n");
                        Sleep(800);
                    }
                    bool exited = false;
                    tryExitMap(exited);
                    if (exited) return;
                    if (rand() % 100 < 15) randomEventExtended();
                    continue;
                }

                // 怪物
                bool monsterFound = false, monsterDefeated = false;
                for (auto& m : currentMap.mobs) {
                    if (!m.alive) continue;
                    int dist = abs(m.x - nx) + abs(m.y - ny);
                    if (dist <= 1) {
                        monsterFound = true;
                        currentMap.px = nx; currentMap.py = ny;
                        printf("遭遇 %s！\n", monsterDB[m.type].name.c_str());
                        Sleep(500);
                        bool result = battle(monsterDB[m.type], m.x, m.y);
                        if (result) {
                            m.alive = false;
                            m.x = m.y = 0;
                            monsterDefeated = true;
                            printf("%s 已被击败！\n",
                                   monsterDB[m.type].name.c_str());
                            Sleep(800);
                            // F7: 击杀计数已在 battle() 里加过，这里不再加
                            if (currentMap.name == mapChineseNames[7] && isMapClear()) {
                                printf("\n=== 所有冰霜怪物已被消灭！===\n");
                                Sleep(500);
                                printf("远处传来铁匠的声音：'年轻人！你做到了！快过来！'\n");
                                Sleep(800);
                            }
                        } else {
                            printf("你战败了，返回主菜单...\n");
                            Sleep(1500);
                            if (!achievements[ACH_UNDYING].unlocked)
                                unlockAchievement(ACH_UNDYING);
                            return;
                        }
                        break;
                    }
                }
                if (monsterFound) {
                    if (monsterDefeated) {
                        if (currentMap.exitLocked && isMapClear()) {
                            currentMap.exitLocked = false;
                            printf("所有怪物已消灭，出口解锁！\n");
                            Sleep(800);
                        }
                        bool exited = false;
                        tryExitMap(exited);
                        if (exited) return;
                        if (rand() % 100 < 15) randomEventExtended();
                    }
                    continue;
                }

                // 普通移动
                currentMap.px = nx; currentMap.py = ny;
                if (currentMap.exitLocked && isMapClear()) {
                    currentMap.exitLocked = false;
                    printf("所有怪物已消灭，出口解锁！\n");
                    Sleep(800);
                }
                bool exited = false;
                tryExitMap(exited);
                if (exited) return;
                if (rand() % 100 < 15) randomEventExtended();
            }
        }
        // 怪物移动
        for (auto& m : currentMap.mobs)
            if (m.alive)
                monsterMove(m.x, m.y, currentMap.px, currentMap.py, currentMap.data);
    }
}

// ============================================================
// 怪物移动（沿用预发布版）
// ============================================================
void monsterMove(int &mx, int &my, int px, int py, vector<string>& map) {
    if (mx == 0 && my == 0) return;
    int h = map.size(), w = map[0].size();
    if (mx <= 0 || mx >= h - 1 || my <= 0 || my >= w - 1) return;
    vector<int> valid;
    for (int i = 0; i < 4; i++) {
        int nx = mx + dx[i], ny = my + dy[i];
        if (nx > 0 && nx < h - 1 && ny > 0 && ny < w - 1
            && map[nx][ny] != '*' && map[nx][ny] != '#'
            && map[nx][ny] != '~')
            valid.push_back(i);
    }
    if (valid.empty()) return;
    if (abs(mx - px) + abs(my - py) <= 10) {
        vector<pair<int, int>> cand;
        for (int d : valid) {
            int nx = mx + dx[d], ny = my + dy[d];
            cand.push_back({abs(nx - px) + abs(ny - py), d});
        }
        sort(cand.begin(), cand.end());
        int best = cand[0].second;
        mx += dx[best]; my += dy[best];
        return;
    }
    vector<int> safe;
    for (int d : valid) {
        int nx = mx + dx[d], ny = my + dy[d];
        int cnt = 0;
        for (int j = 0; j < 4; j++) {
            int nnx = nx + dx[j], nny = ny + dy[j];
            if (nnx > 0 && nnx < h - 1 && nny > 0 && nny < w - 1
                && map[nnx][nny] != '*' && map[nnx][nny] != '#'
                && map[nnx][nny] != '~')
                cnt++;
        }
        if (cnt > 0) safe.push_back(d);
    }
    int d = safe.empty() ? valid[rand() % valid.size()]
                         : safe[rand() % safe.size()];
    mx += dx[d]; my += dy[d];
}

// ============================================================
// 锻造
// ============================================================
void forgingUI() {
    while (1) {
        system("cls");
        printf("===== 锻造系统 =====\n");
        printf("[1] 查看可锻造物品\n");
        printf("[2] 锻造物品\n");
        printf("[3] 查看锻造配方\n");
        printf("[0] 返回\n");
        char choice = read();
        if (choice == '0') break;
        if (choice == '1') {
            system("cls");
            printf("===== 可锻造物品 =====\n");
            int count = 0;
            for (size_t i = 0; i < itemDB.size(); i++) {
                if (i > 0 && !itemDB[i].recipe.empty()) {
                    printf("%d. %s - %s\n", (int)i,
                           safeItemName(i).c_str(),
                           itemDB[i].desc.c_str());
                    count++;
                    if (count % 10 == 0) {
                        system("pause");
                        system("cls");
                        printf("===== 可锻造物品 =====\n");
                    }
                }
            }
            printf("共 %d 种可锻造物品。\n", count);
            system("pause");
            continue;
        }
        if (choice == '2') {
            system("cls");
            printf("===== 锻造 =====\n");
            printf("请输入要锻造的物品ID（输入0取消）：");
            int id; cin >> id;
            if (id <= 0) continue;
            if (id >= (int)itemDB.size() || itemDB[id].recipe.empty()) {
                printf("该物品无法锻造。\n");
                Sleep(500); continue;
            }
            Item& it = itemDB[id];
            printf("锻造 %s 需要以下材料：\n", safeItemName(id).c_str());
            for (auto& r : it.recipe)
                printf("  %s x%d (当前拥有: %d)\n",
                       safeItemName(r.first).c_str(),
                       r.second, countBag(r.first));
            printf("数量：");
            int amount; cin >> amount;
            if (amount <= 0) continue;
            bool enough = true;
            for (auto& r : it.recipe) {
                if (countBag(r.first) < r.second * amount) {
                    printf("材料不足！需要 %s x%d，当前只有 %d\n",
                           safeItemName(r.first).c_str(),
                           r.second * amount, countBag(r.first));
                    enough = false;
                }
            }
            if (enough) {
                for (auto& r : it.recipe)
                    removeBag(r.first, r.second * amount);
                addBag(id, amount);
                player.forgeCount += amount;
                printf("锻造成功！获得 %d 个 %s\n", amount,
                       safeItemName(id).c_str());
                checkAchievements();
                if (id == 105 && quests[7].taken && !quests[7].completed) {
                    if (countBag(105) >= 1) {
                        quests[7].completed = true;
                        player.gold += quests[7].rewardGold;
                        if (quests[7].rewardItemId > 0)
                            addBag(quests[7].rewardItemId);
                        printf("任务'装备升级'已完成！\n");
                    }
                }
            }
            Sleep(800);
            continue;
        }
        if (choice == '3') {
            system("cls");
            printf("===== 锻造配方 =====\n");
            printf("输入物品ID查看配方（输入0返回）：");
            int id; cin >> id;
            if (id <= 0) continue;
            if (id >= (int)itemDB.size() || itemDB[id].recipe.empty()) {
                printf("该物品没有锻造配方。\n");
                Sleep(500); continue;
            }
            printf("%s 的锻造配方：\n", safeItemName(id).c_str());
            for (auto& r : itemDB[id].recipe)
                printf("  %s x%d\n",
                       safeItemName(r.first).c_str(), r.second);
            system("pause");
        }
    }
}

// ============================================================
// 背包
// F10: 使用 usePotion
// ============================================================
void bagUI() {
    while (1) {
        // 清理无效物品
        player.bag.erase(remove_if(player.bag.begin(), player.bag.end(),
            [](pair<int, int>& p) {
                return p.first <= 0
                    || p.first >= (int)itemDB.size()
                    || itemDB[p.first].id != p.first;
            }), player.bag.end());
        if (player.hp > getMaxHp()) player.hp = getMaxHp();
        if (player.mp > player.maxMp) player.mp = player.maxMp;

        system("cls");
        printf("===== 背包 (%d 件) =====\n", (int)player.bag.size());
        for (size_t i = 0; i < player.bag.size(); i++) {
            int id = player.bag[i].first;
            if (id <= 0 || id >= (int)itemDB.size()
                || itemDB[id].id != id) continue;
            string eff = getItemEffectDesc(id);
            string typeIcon;
            switch (itemDB[id].type) {
                case WEAPON:    typeIcon = "[武]"; break;
                case ARMOR:     typeIcon = "[防]"; break;
                case CLOTHING:  typeIcon = "[衣]"; break;
                case ACCESSORY: typeIcon = "[饰]"; break;
                case POTION:    typeIcon = "[丹]"; break;
                case THROWING:  typeIcon = "[暗]"; break;
                case MATERIAL:  typeIcon = "[材]"; break;
                case QUEST:     typeIcon = "[任]"; break;
                default:        typeIcon = "[ ]"; break;
            }
            string equipped = "";
            if (id == player.weapon || id == player.armor
                || id == player.clothing || id == player.accessory)
                equipped = " ?装备中";
            printf("%2d. %s %s x%d%s\n", (int)i + 1, typeIcon.c_str(),
                   safeItemName(id).c_str(),
                   player.bag[i].second, equipped.c_str());
            if (!eff.empty()) printf("    └ %s\n", eff.c_str());
        }
        printf("\n输入编号使用/装备，[D]丢弃，[C]关闭\n");
        string input;
        cin >> input;
        if (input == "C" || input == "c") break;
        if (input == "D" || input == "d") {
            printf("输入要丢弃的编号：");
            cin >> input;
            int idx = atoi(input.c_str()) - 1;
            if (idx >= 0 && idx < (int)player.bag.size()) {
                int id = player.bag[idx].first;
                if (id <= 0 || id >= (int)itemDB.size()
                    || itemDB[id].id != id) {
                    printf("该物品无效，自动清理。\n");
                    player.bag.erase(player.bag.begin() + idx);
                } else {
                    printf("丢弃 %s ？(y/n)", safeItemName(id).c_str());
                    char ans = read();
                    if (ans == 'y' || ans == 'Y') {
                        removeBag(id, 1);
                        printf("已丢弃\n");
                    }
                }
            }
            Sleep(400);
            continue;
        }
        int idx = atoi(input.c_str()) - 1;
        if (idx >= 0 && idx < (int)player.bag.size()) {
            int id = player.bag[idx].first;
            if (id <= 0 || id >= (int)itemDB.size()
                || itemDB[id].id != id) {
                printf("物品数据错误，已自动移除。\n");
                player.bag.erase(player.bag.begin() + idx);
                Sleep(500);
                continue;
            }
            Item& it = itemDB[id];
            if (it.type == WEAPON) {
                if (player.weapon) addBag(player.weapon);
                player.weapon = id;
                removeBag(id);
                printf("已装备武器：%s (%s)\n",
                       safeItemName(id).c_str(),
                       getItemEffectDesc(id).c_str());
                Sleep(500);
            } else if (it.type == ARMOR) {
                if (player.armor) addBag(player.armor);
                player.armor = id;
                removeBag(id);
                printf("已装备防具：%s (%s)\n",
                       safeItemName(id).c_str(),
                       getItemEffectDesc(id).c_str());
                Sleep(500);
            } else if (it.type == CLOTHING) {
                if (player.clothing) addBag(player.clothing);
                player.clothing = id;
                removeBag(id);
                printf("已装备服装：%s (%s)\n",
                       safeItemName(id).c_str(),
                       getItemEffectDesc(id).c_str());
                Sleep(500);
            } else if (it.type == ACCESSORY) {
                if (player.accessory) addBag(player.accessory);
                player.accessory = id;
                removeBag(id);
                printf("已装备饰品：%s (%s)\n",
                       safeItemName(id).c_str(),
                       getItemEffectDesc(id).c_str());
                Sleep(500);
            } else if (it.type == POTION) {
                removeBag(id);
                // F10: 统一走 usePotion
                usePotion(id, false);
                Sleep(500);
            } else {
                printf("该物品无法直接使用。\n");
                Sleep(500);
            }
        } else {
            printf("无效编号。\n");
            Sleep(400);
        }
    }
}

// ============================================================
// 商店
// ============================================================
bool isValidItem(int id) {
    if (id <= 0) return false;
    if ((size_t)id >= itemDB.size()) return false;
    if (itemDB[id].id != id) return false;
    if (itemDB[id].name.empty() || itemDB[id].name == "[空]"
        || itemDB[id].name == "[未知物品]") return false;
    if (itemDB[id].price <= 0
        && itemDB[id].type != QUEST
        && itemDB[id].type != SPECIAL) return false;
    return true;
}

void shopCategory() {
    while (1) {
        system("cls");
        printf("===== 商店 =====\n");
        printf("[1] 武器\n[2] 防具\n[3] 服装\n[4] 丹药\n");
        printf("[5] 暗器\n[6] 饰品\n[7] 材料\n[0] 返回\n");
        char choice = read();
        if (choice == '0') break;

        vector<int> idList;
        switch (choice) {
            case '1': {
                int arr[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
                             21,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,
                             41,42,43,44,45};
                idList.assign(arr, arr + sizeof(arr) / sizeof(arr[0]));
                break;
            }
            case '2': {
                int arr[] = {101,102,103,104,105,106,107,108,109,110,
                             111,112,113,114,115,116,117,118,119,120};
                idList.assign(arr, arr + sizeof(arr) / sizeof(arr[0]));
                break;
            }
            case '3': {
                int arr[] = {201,202,203,204,205,206,207,208,209,210,
                             211,212,213,214,215};
                idList.assign(arr, arr + sizeof(arr) / sizeof(arr[0]));
                break;
            }
            case '4': {
                int arr[] = {301,302,303,304,305,306,308,309,310,311,312,313,
                             314,315,316,317,318,319,320,321,322,323,324,325};
                idList.assign(arr, arr + sizeof(arr) / sizeof(arr[0]));
                break;
            }
            case '5': {
                int arr[] = {501,502,503,505,506,507,510,511,512,513,514,515};
                idList.assign(arr, arr + sizeof(arr) / sizeof(arr[0]));
                break;
            }
            case '6': {
                int arr[] = {601,602,603,604,605,606,607,610,611,612,614,615,
                             616,617,618,619,620,621,622,623,624,625};
                idList.assign(arr, arr + sizeof(arr) / sizeof(arr[0]));
                break;
            }
            case '7': {
                int arr[] = {401,402,403,404,405,406,407,408,409,410,411,412,413,
                             414,415,416,417,418,419,420,421,422,423,424,425,426,
                             427,428,429,430};
                idList.assign(arr, arr + sizeof(arr) / sizeof(arr[0]));
                break;
            }
            default: continue;
        }

        vector<int> validList;
        for (int id : idList) {
            if (!isValidItem(id)) continue;
            if (itemDB[id].price <= 0) continue;
            if (itemDB[id].requiredLevel > player.level) continue;
            validList.push_back(id);
        }
        if (validList.empty()) {
            printf("当前分类无可用商品！\n");
            Sleep(1500);
            continue;
        }

        const char* catName = choice == '1' ? "武器"
                            : choice == '2' ? "防具"
                            : choice == '3' ? "服装"
                            : choice == '4' ? "丹药"
                            : choice == '5' ? "暗器"
                            : choice == '6' ? "饰品" : "材料";

        while (1) {
            system("cls");
            printf("===== 购买 %s ===== (金币:%d  等级:%d)\n",
                   catName, player.gold, player.level);
            for (size_t i = 0; i < validList.size(); i++) {
                int id = validList[i];
                string eff = getItemEffectDesc(id);
                printf("%2d. %-20s %5d金币", (int)i + 1,
                       itemDB[id].name.c_str(), itemDB[id].price);
                if (!eff.empty()) printf("  [%s]", eff.c_str());
                if (itemDB[id].requiredLevel > 0)
                    printf(" (需%d级)", itemDB[id].requiredLevel);
                printf("\n");
            }
            printf("\n[B]批量购买  [S]批量出售  [C]返回上一级\n");

            string input;
            cin >> input;

            if (input == "b" || input == "B") {
                printf("请输入物品编号：");
                cin >> input;
                int idxRaw = atoi(input.c_str()) - 1;
                if (idxRaw < 0 || (size_t)idxRaw >= validList.size()) {
                    printf("编号不存在！\n"); Sleep(500); continue;
                }
                int id = validList[idxRaw];
                printf("请输入购买数量：");
                cin >> input;
                int amount = atoi(input.c_str());
                if (amount <= 0) { printf("数量必须大于0！\n"); Sleep(500); continue; }
                int totalCost = itemDB[id].price * amount;
                if (player.gold < totalCost) {
                    printf("金币不足！需要 %d，当前 %d\n",
                           totalCost, player.gold);
                    Sleep(500); continue;
                }
                player.gold -= totalCost;
                addBag(id, amount);
                player.purchaseCount += amount;
                player.totalSpent += totalCost;
                printf("成功购买 %d 个 %s\n", amount,
                       itemDB[id].name.c_str());
                checkAchievements();
                Sleep(800);
                continue;
            }

            if (input == "s" || input == "S") {
                system("cls");
                printf("===== 当前分类批量出售 =====\n");
                vector<pair<int, int>> sellList;
                int showNo = 1;
                for (auto& bagItem : player.bag) {
                    int itemId = bagItem.first;
                    if (!isValidItem(itemId)) continue;
                    bool match = false;
                    for (int cid : validList)
                        if (itemId == cid) { match = true; break; }
                    if (match && bagItem.second > 0) {
                        sellList.push_back({showNo, itemId});
                        printf("%d. %s 持有:%d\n", showNo,
                               itemDB[itemId].name.c_str(),
                               bagItem.second);
                        showNo++;
                    }
                }
                if (sellList.empty()) {
                    printf("背包无该分类可出售物品\n");
                    Sleep(1000); continue;
                }
                printf("\n输入物品编号：");
                cin >> input;
                int sellNo = atoi(input.c_str());
                int targetId = -1;
                for (auto& p : sellList)
                    if (p.first == sellNo) { targetId = p.second; break; }
                if (targetId == -1) {
                    printf("不存在该编号物品！\n"); Sleep(800); continue;
                }
                int ownNum = countBag(targetId);
                if (ownNum <= 0) {
                    printf("该物品已不在背包中！\n"); Sleep(800); continue;
                }
                printf("当前持有%d，请输入出售数量：", ownNum);
                cin >> input;
                int sellAmt = atoi(input.c_str());
                if (sellAmt <= 0 || sellAmt > ownNum) {
                    printf("数量无效！\n"); Sleep(600); continue;
                }
                int earnGold = (itemDB[targetId].price / 2) * sellAmt;
                if (removeBag(targetId, sellAmt)) {
                    player.gold += earnGold;
                    printf("出售成功，获得%d金币\n", earnGold);
                }
                Sleep(1000);
                continue;
            }

            if (input == "c" || input == "C") break;

            int idxRaw = atoi(input.c_str()) - 1;
            if (idxRaw < 0 || (size_t)idxRaw >= validList.size()) {
                printf("无效物品编号！\n"); Sleep(500); continue;
            }
            int id = validList[idxRaw];
            if (player.gold >= itemDB[id].price) {
                player.gold -= itemDB[id].price;
                addBag(id);
                player.purchaseCount++;
                player.totalSpent += itemDB[id].price;
                printf("购买成功！获得 %s\n", itemDB[id].name.c_str());
                checkAchievements();
            } else {
                printf("金币不足！\n");
            }
            Sleep(500);
        }
    }
}

void shop() { shopCategory(); }

// ============================================================
// 角色状态
// ============================================================
void status() {
    system("cls");
    printf("===== 角色状态 =====\n");
    string realm = getRealmName(player.level, classes[player.classId].name);
    printf("道号：%s %s  等级：%d - %s  金币：%d\n",
           classes[player.classId].title.c_str(),
           player.name.c_str(),
           player.level, realm.c_str(), player.gold);
    string en[] = {"无","火","冰","雷","毒","圣","暗","风","土"};
    printf("自身属性：%s\n", en[player.playerElement].c_str());
    printf("--------------------------------------------------\n");
    printf("生命：%d / %d  灵力：%d / %d\n",
           player.hp, getMaxHp(), player.mp, player.maxMp);
    printf("攻击：%d  防御：%d  暴击率：%d%%  暴击伤害：%.2f\n",
           getTotalAtk(), getTotalDef(),
           getTotalCrit(), getTotalCritDmg());
    printf("--------------------------------------------------\n");
    printf("【装备栏】\n");
    if (player.weapon) printf("  武器：%s\n", safeItemName(player.weapon).c_str());
    else printf("  武器：无\n");
    if (player.armor) printf("  防具：%s\n", safeItemName(player.armor).c_str());
    else printf("  防具：无\n");
    if (player.clothing) printf("  服装：%s\n", safeItemName(player.clothing).c_str());
    else printf("  服装：无\n");
    if (player.accessory) printf("  饰品：%s\n", safeItemName(player.accessory).c_str());
    else printf("  饰品：无\n");
    printf("--------------------------------------------------\n");
    printf("技能等级：\n");
    for (size_t i = 0; i < classes[player.classId].skills.size(); i++)
        printf("  %s Lv.%d - %s\n",
               classes[player.classId].skills[i].name.c_str(),
               player.skillLv[i],
               classes[player.classId].skills[i].desc.c_str());
    printf("--------------------------------------------------\n");
    printf("额外统计：\n");
    printf("  击杀数：%d  Boss击杀：%d  灵兽击杀：%d\n",
           player.killCount, player.bossKillCount, player.spiritKillCount);
    printf("  锻造次数：%d  购买次数：%d\n",
           player.forgeCount, player.purchaseCount);
    printf("  累计金币：%d  通关地图：%d\n",
           player.goldEarned, player.mapsCleared);
    printf("  藏宝图碎片：%d/3\n", player.treasurePieces);
    printf("--------------------------------------------------\n");
    system("pause");
}

// ============================================================
// 存档
// F17: 只保存成就 unlocked 状态，不再保存 questProgress/achievement
// ============================================================
void saveGame() {
    ofstream f("save.dat");
    if (!f) { printf("存档失败！\n"); Sleep(500); return; }
    f << player.name << "\n";
    f << player.classId << " " << player.level << " "
      << player.exp << " " << player.maxExp << "\n";
    f << player.hp << " " << player.maxHp << " "
      << player.atk << " " << player.def << " " << player.crit << " "
      << player.critDmg << "\n";
    f << player.gold << " " << player.mp << " " << player.maxMp << "\n";
    f << player.weapon << " " << player.armor << " "
      << player.clothing << " " << player.accessory << "\n";
    for (int i = 0; i < 10; i++) f << player.statusDur[i] << " ";
    f << "\n";
    f << player.dungeonLevel << " " << player.passiveBonus << " "
      << player.firstAttack << "\n";
    f << (int)player.playerElement << " "
      << player.killCount << " " << player.bossKillCount << " "
      << player.spiritKillCount << " "
      << player.forgeCount << " " << player.purchaseCount << " "
      << player.goldEarned << " " << player.mapsCleared << "\n";
    f << player.skillLv.size() << "\n";
    for (int lv : player.skillLv) f << lv << " ";
    f << "\n" << player.bag.size() << "\n";
    for (auto& p : player.bag) f << p.first << " " << p.second << "\n";
    f << quests.size() << "\n";
    for (auto& q : quests) f << q.taken << " " << q.completed << "\n";
    f << achievements.size() << "\n";
    for (auto& a : achievements) f << a.unlocked << " ";
    f << "\n";
    f << currentLevel << "\n";
    f << player.treasurePieces << "\n";
    f << player.totalSpent << " " << player.skillUseCount << " "
      << player.totalCritCount << " " << player.trapDeathCount << "\n";
    f.close();
    printf("存档成功！\n");
    Sleep(500);
}

// ============================================================
// 读档
// F18: 不再调用 applyPlayerClass，直接读存档属性
// ============================================================
void loadGame() {
    ifstream f("save.dat");
    if (!f) { printf("未找到存档文件。\n"); Sleep(500); return; }
    string name;
    getline(f, name);
    int classId, level, exp, maxExp;
    f >> classId >> level >> exp >> maxExp;
    int hp, maxHp, atk, def, crit;
    float critDmg;
    f >> hp >> maxHp >> atk >> def >> crit >> critDmg;
    int gold, mp, maxMp;
    f >> gold >> mp >> maxMp;
    int weapon, armor, clothing, accessory;
    f >> weapon >> armor >> clothing >> accessory;
    int statusDur[10];
    for (int i = 0; i < 10; i++) f >> statusDur[i];
    int dungeonLevel, passiveBonus;
    bool firstAttack;
    f >> dungeonLevel >> passiveBonus >> firstAttack;
    int elementInt;
    f >> elementInt;
    int killCount, bossKillCount, spiritKillCount, forgeCount, purchaseCount,
        goldEarned, mapsCleared;
    f >> killCount >> bossKillCount >> spiritKillCount
      >> forgeCount >> purchaseCount >> goldEarned >> mapsCleared;
    int savedSkillCount;
    f >> savedSkillCount;
    vector<int> savedSkillLv(savedSkillCount);
    for (int i = 0; i < savedSkillCount; i++) f >> savedSkillLv[i];
    int bagSize;
    f >> bagSize;
    vector<pair<int, int>> savedBag;
    for (int i = 0; i < bagSize; i++) {
        int id, cnt;
        f >> id >> cnt;
        if (id > 0 && id < (int)itemDB.size() && itemDB[id].id == id)
            savedBag.push_back({id, cnt});
    }
    int questCount;
    f >> questCount;
    if (questCount == (int)quests.size()) {
        for (int i = 0; i < questCount; i++) {
            bool taken, completed;
            f >> taken >> completed;
            quests[i].taken = taken;
            quests[i].completed = completed;
        }
    }
    int achCount;
    f >> achCount;
    vector<bool> achUnlocked(achCount);
    for (int i = 0; i < achCount; i++) {
        int v; f >> v; achUnlocked[i] = (v != 0);
    }
    f >> currentLevel;
    if (currentLevel < 1) currentLevel = 1;
    if (currentLevel > MAX_MAPS) currentLevel = MAX_MAPS;
    int treasurePieces = 0;
    if (f.peek() != EOF) f >> treasurePieces;
    int totalSpent = 0, skillUseCount = 0, totalCritCount = 0, trapDeathCount = 0;
    if (f.peek() != EOF)
        f >> totalSpent >> skillUseCount >> totalCritCount >> trapDeathCount;
    f.close();

    // F18: 直接读存档属性
    player.name = name;
    player.classId = classId;
    player.level = level;
    player.exp = exp;
    player.maxExp = maxExp;
    player.maxHp = maxHp;
    player.hp = min(hp, maxHp);
    player.atk = atk;
    player.def = def;
    player.crit = crit;
    player.critDmg = critDmg;
    player.gold = gold;
    player.maxMp = maxMp;
    player.mp = min(mp, maxMp);
    player.weapon = weapon;
    player.armor = armor;
    player.clothing = clothing;
    player.accessory = accessory;
    memcpy(player.statusDur, statusDur, sizeof(statusDur));
    player.dungeonLevel = dungeonLevel;
    player.passiveBonus = passiveBonus;
    player.firstAttack = firstAttack;
    player.playerElement = (Element)elementInt;
    player.killCount = killCount;
    player.bossKillCount = bossKillCount;
    player.spiritKillCount = spiritKillCount;
    player.forgeCount = forgeCount;
    player.purchaseCount = purchaseCount;
    player.goldEarned = goldEarned;
    player.mapsCleared = mapsCleared;
    player.treasurePieces = treasurePieces;
    player.totalSpent = totalSpent;
    player.skillUseCount = skillUseCount;
    player.totalCritCount = totalCritCount;
    player.trapDeathCount = trapDeathCount;
    player.skillLv = savedSkillLv;
    player.skillCd.assign(player.skillLv.size(), 0);
    player.bag = savedBag;
    for (size_t i = 0; i < achievements.size() && i < achUnlocked.size(); i++)
        achievements[i].unlocked = achUnlocked[i];
    player.hp = min(player.hp, getMaxHp());
    player.mp = min(player.mp, player.maxMp);
    printf("读档成功！\n");
    Sleep(500);
}

// ============================================================
// 随机事件
// ============================================================
void randomEventExtended() {
    int r = rand() % 20;
    if (r == 0) {
        system("cls");
        printf("一位神秘商人出现在你面前，准备用材料换取装备。\n");
        Sleep(800);
        struct TradeItem { int itemId; vector<pair<int, int>> cost; };
        vector<TradeItem> tradeList = {
            {3,   {{401,3},{404,2}}},
            {103, {{401,2},{403,2}}},
            {303, {{402,2},{404,3}}},
            {105, {{401,4},{403,3},{404,2}}},
            {302, {{402,3},{401,2}}},
            {15,  {{401,5},{403,4},{404,4}}},
            {201, {{401,3},{403,1}}},
            {202, {{401,2},{406,1}}},
            {203, {{401,4},{404,3}}},
            {204, {{401,3},{406,2}}},
            {205, {{401,6},{407,2}}},
            {206, {{401,8},{410,1}}},
            {41,  {{408,3},{421,2}}},
            {42,  {{413,3},{416,2}}},
            {43,  {{414,3},{405,2}}},
            {44,  {{416,3},{407,2}}}
        };
        vector<TradeItem> validTrade;
        for (auto& t : tradeList) {
            int tid = t.itemId;
            if (tid <= 0 || (size_t)tid >= itemDB.size()) continue;
            if (itemDB[tid].id != tid) continue;
            if ((tid >= 801 && tid <= 820) || (tid >= 701 && tid <= 712)) continue;
            if (itemDB[tid].recipe.empty()) continue;
            validTrade.push_back(t);
        }
        if (validTrade.empty()) {
            printf("商人无可交易物品。\n");
            Sleep(1000);
            return;
        }
        TradeItem trade = validTrade[rand() % validTrade.size()];
        printf("他想用 [%s] 交换以下材料：\n", safeItemName(trade.itemId).c_str());
        for (auto& c : trade.cost)
            printf("  %s x%d (你拥有: %d)\n",
                   safeItemName(c.first).c_str(), c.second,
                   countBag(c.first));
        printf("\n你愿意交换吗？(y/n) ");
        char ans = read();
        if (ans == 'y' || ans == 'Y') {
            bool enough = true;
            for (auto& c : trade.cost)
                if (countBag(c.first) < c.second) { enough = false; break; }
            if (enough) {
                for (auto& c : trade.cost) removeBag(c.first, c.second);
                addBag(trade.itemId);
                printf("交换成功！你获得了 %s。\n",
                       safeItemName(trade.itemId).c_str());
            } else {
                printf("材料不足，交易失败。\n");
            }
        } else {
            printf("你拒绝了交易。\n");
        }
        Sleep(1000);
    } else if (r == 1) {
        printf("你发现了一个隐藏的宝箱！\n");
        addBag(rand() % 2 ? 408 : 409, 1);
        Sleep(500);
    } else if (r == 2) {
        printf("触发陷阱！损失10生命。\n");
        player.hp = max(1, player.hp - 10);
        Sleep(500);
    } else if (r == 3) {
        printf("你找到了一处疗伤温泉，生命恢复20点。\n");
        player.hp = min(getMaxHp(), player.hp + 20);
        Sleep(500);
    } else if (r == 4) {
        printf("一位流浪汉送给你一颗丹药。\n");
        addBag(301, 1);
        Sleep(500);
    } else if (r == 5) {
        printf("一群强盗袭击了你，你损失了20金币。\n");
        player.gold = max(0, player.gold - 20);
        Sleep(500);
    } else if (r == 6) {
        printf("你捡到了一块藏宝图碎片！\n");
        if (player.treasurePieces < 3) {
            int piece = 705 + player.treasurePieces;
            addBag(piece);
            player.treasurePieces++;
            if (player.treasurePieces >= 3) {
                addBag(708);
                printf("三块藏宝图碎片合成了完整的藏宝图！\n");
                if (!achievements[ACH_TREASURE_HUNTER].unlocked)
                    unlockAchievement(ACH_TREASURE_HUNTER);
            }
        }
        Sleep(500);
    } else if (r == 7) {
        printf("你遇到了一位云游仙人，他传授了你一些经验。\n");
        gainExp(50);
        Sleep(500);
    }
}

// ============================================================
// 选择职业
// ============================================================
void chooseClass() {
    system("cls");
    printf("===== 选择你的修道之路 =====\n");
    for (int i = 0; i < (int)classes.size(); i++) {
        printf("%d. %s (%s)\n", i + 1,
               classes[i].name.c_str(), classes[i].title.c_str());
        printf("   生命:%d 攻击:%d 防御:%d 暴击:%d%% 技能: ",
               classes[i].baseHp, classes[i].baseAtk,
               classes[i].baseDef, classes[i].baseCrit);
        for (int j = 0; j < (int)classes[i].skills.size(); j++)
            printf("%s ", classes[i].skills[j].name.c_str());
        printf("\n   被动: %s - %s\n",
               classes[i].passiveName.c_str(),
               classes[i].passiveDesc.c_str());
    }
    char ch = read();
    int id = ch - '1';
    if (id < 0 || id > 7) id = 0;
    cout << "请输入你的名字: ";
    string name;
    cin >> name;
    initPlayer(id, name);
    printf("欢迎，%s %s！\n",
           classes[id].title.c_str(), name.c_str());
    Sleep(1500);
}

// ============================================================
// F8: 检查成就
//   - "炼丹新手/宗师"改为综合合成/购买
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
    // F8: 炼丹成就改为综合
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
    for (int lv : player.skillLv) if (lv >= 15) { skillMax = true; break; }
    if (!achievements[ACH_SKILL_MAX].unlocked && skillMax)
        unlockAchievement(ACH_SKILL_MAX);
    if (!achievements[ACH_SPENDTHRIFT].unlocked && player.totalSpent >= 66666)
        unlockAchievement(ACH_SPENDTHRIFT);
    if (!achievements[ACH_HERO_PEAK].unlocked && player.killCount >= 300)
        unlockAchievement(ACH_HERO_PEAK);
}

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

// ============================================================
// F19: 数据驱动的任务进度检查
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

// ============================================================
// 拍卖行（沿用预发布版简化版）
// ============================================================
struct AuctionBidder {
    string name;
    int maxBid, currentBid, gold;
    bool isPlayer;
    int bidIncrement;
};
vector<AuctionBidder> bidders;
int auctionWinner = -1;
int auctionFinalPrice = 0;

void initAuctionBidders() {
    bidders.clear();
    vector<pair<string, int>> npcData = {
        {"青云剑宗·李长老", 8000 + rand() % 5000},
        {"天机阁·赵掌柜",   6000 + rand() % 4000},
        {"万宝楼·钱庄主",  10000 + rand() % 8000},
        {"暗影殿·影杀",     5000 + rand() % 3000},
        {"仙灵岛·白仙子",  12000 + rand() % 5000},
        {"龙族·敖烈",      15000 + rand() % 8000},
        {"魔界·暗影王",     7000 + rand() % 4000},
        {"散修·云游子",     3000 + rand() % 2000},
    };
    bidders.push_back({player.name, player.gold, 0, player.gold, true, 100});
    for (auto& npc : npcData)
        bidders.push_back({npc.first, npc.second, 0, npc.second, false,
                           50 + rand() % 150});
    random_device rd;
    mt19937 gen(rd());
    shuffle(bidders.begin() + 1, bidders.end(), gen);
}

int selectAuctionItem() {
    vector<int> items = {801,802,803,804,805,806,807,808,809,810,
                         811,812,813,814,815,816,817,818,819,820};
    return items[rand() % items.size()];
}

bool npcBid(int bidderIdx, int currentPrice, int itemId) {
    AuctionBidder& bidder = bidders[bidderIdx];
    if (bidder.isPlayer) return false;
    if (currentPrice >= bidder.maxBid) return false;
    int priceRatio = (currentPrice * 100) / bidder.maxBid;
    int interest = 50 + rand() % 50;
    if (itemId >= 801 && itemId <= 805) interest += 20;
    if (itemId >= 806 && itemId <= 810) interest += 15;
    if (priceRatio > 80) interest -= 30;
    else if (priceRatio > 60) interest -= 15;
    else if (priceRatio > 40) interest -= 5;
    if (rand() % 100 < interest) {
        int increment = bidder.bidIncrement + rand() % 200;
        int newBid = min(currentPrice + increment, bidder.maxBid);
        if (newBid > currentPrice) {
            bidder.currentBid = newBid;
            return true;
        }
    }
    return false;
}

void doAuctionRound(int itemId) {
    system("cls");
    setColor(14);
    cout << "╔══════════════════════════════════════╗\n";
    cout << "║           拍 卖 大 会                ║\n";
    cout << "╚══════════════════════════════════════╝\n\n";
    resetColor();
    string itemName = safeItemName(itemId);
    int basePrice = 1000 + rand() % 4000;

    cout << "【今日拍品】" << itemName << "\n";
    cout << "  起拍价：" << basePrice << " 金币\n\n";

    int currentPrice = basePrice;
    int lastBidder = -1;
    int consecutivePass = 0;
    bool playerStillActive = true;
    vector<bool> activeBidders(bidders.size(), true);

    while (consecutivePass < 3) {
        system("cls");
        cout << "【拍品】" << itemName
             << "  当前价格：" << currentPrice << " 金币\n\n";

        vector<int> activeList;
        for (size_t i = 0; i < bidders.size(); i++)
            if (activeBidders[i] && bidders[i].maxBid >= currentPrice + 100
                && !bidders[i].isPlayer)
                activeList.push_back(i);

        bool playerBid = false;
        if (activeBidders[0] && bidders[0].maxBid >= currentPrice + 100
            && playerStillActive) {
            cout << "主持人：当前 " << currentPrice
                 << " 金币，要加价吗？\n";
            cout << "你的预算：" << bidders[0].maxBid << "\n";
            cout << "1. 出价  2. 放弃：";
            char choice = read();
            if (choice == '1') {
                int inc = 100 + rand() % 300;
                int newBid = currentPrice + inc;
                if (newBid <= bidders[0].maxBid) {
                    currentPrice = newBid;
                    playerBid = true;
                    lastBidder = 0;
                    consecutivePass = 0;
                    cout << "你出价 " << currentPrice << " 金币！\n";
                    Sleep(500);
                } else {
                    cout << "预算不足！\n"; Sleep(500);
                }
            } else {
                playerStillActive = false;
                activeBidders[0] = false;
            }
        }

        if (!playerBid && !activeList.empty()) {
            int idx = activeList[rand() % activeList.size()];
            if (npcBid(idx, currentPrice, itemId)) {
                currentPrice = bidders[idx].currentBid;
                lastBidder = idx;
                consecutivePass = 0;
                cout << bidders[idx].name << " 出价 "
                     << currentPrice << " 金币！\n";
                Sleep(400);
            } else {
                activeBidders[idx] = false;
                cout << bidders[idx].name << " 放弃了。\n";
                Sleep(300);
            }
        } else if (!playerBid) {
            consecutivePass++;
            cout << "无人加价... (" << consecutivePass << "/3)\n";
            Sleep(500);
        }
    }

    system("cls");
    if (lastBidder >= 0) {
        auctionWinner = lastBidder;
        auctionFinalPrice = currentPrice;
        cout << "成交价：" << auctionFinalPrice << " 金币\n";
        cout << "得主：" << bidders[auctionWinner].name << "\n";
        if (bidders[auctionWinner].isPlayer) {
            if (player.gold >= auctionFinalPrice) {
                player.gold -= auctionFinalPrice;
                addBag(itemId);
                player.totalSpent += auctionFinalPrice;
                cout << "恭喜你拍得 " << itemName << "！\n";
                checkAchievements();
            } else {
                cout << "金币不足！拍卖无效。\n";
            }
        } else {
            cout << "很遗憾，你没有拍得此物。\n";
        }
    } else {
        cout << "流拍。\n";
    }
    cout << "\n按任意键返回...";
    read();
}

void auctionHouse() {
    if (player.gold < 1000) {
        system("cls");
        printf("你的金币不足1000，无法参加拍卖。\n");
        Sleep(1500);
        return;
    }
    system("cls");
    setColor(14);
    cout << "===== 拍 卖 行 =====\n\n";
    resetColor();
    cout << "[1] 参加拍卖\n[2] 查看规则\n[0] 离开\n请选择：";
    char choice = read();
    if (choice == '1') {
        initAuctionBidders();
        int itemId = selectAuctionItem();
        doAuctionRound(itemId);
        auctionWinner = -1;
        auctionFinalPrice = 0;
    } else if (choice == '2') {
        system("cls");
        printf("===== 拍卖规则 =====\n");
        printf("1. 随机出现一件稀世珍宝\n");
        printf("2. 所有竞拍者按预算出价\n");
        printf("3. 每次至少加价 100 金币\n");
        printf("4. 连续3次无人加价，拍卖结束\n");
        printf("5. 出价最高者获得\n\n");
        system("pause");
    }
}

// ============================================================
// 物品查询系统
// ============================================================
string getItemTypeNameCN(int id) {
    if (id <= 0 || (size_t)id >= itemDB.size() || itemDB[id].id != id)
        return "未知";
    switch (itemDB[id].type) {
        case WEAPON:    return "武器";
        case ARMOR:     return "防具";
        case CLOTHING:  return "服装";
        case ACCESSORY: return "饰品";
        case POTION:    return "丹药";
        case THROWING:  return "暗器";
        case MATERIAL:  return "材料";
        case QUEST:     return "任务物品";
        case SPECIAL:   return "特殊物品";
        default:        return "其他";
    }
}

string getQualityName(int q) {
    switch (q) {
        case 0: return "普通";
        case 1: return "精良";
        case 2: return "稀有";
        case 3: return "史诗";
        case 4: return "传说";
        default: return "未知";
    }
}

string getElementNameCN(int ele) {
    static const char* names[] = {"无","火","冰","雷","毒","圣","暗","风","土"};
    if (ele >= 0 && ele <= 8) return names[ele];
    return "无";
}

void displayItemDetail(int id) {
    if (id <= 0 || (size_t)id >= itemDB.size() || itemDB[id].id != id) {
        printf("错误：物品ID无效！\n");
        return;
    }
    Item& it = itemDB[id];
    if (it.name.empty() || it.name == "[空]" || it.name == "[未知物品]") {
        printf("该物品槽位为空！\n");
        return;
    }
    setColor(14);
    printf("===== 物品详细信息 =====\n");
    resetColor();
    printf("\nID：%d\n名称：%s\n类型：%s\n描述：%s\n",
           id, it.name.c_str(),
           getItemTypeNameCN(id).c_str(), it.desc.c_str());
    if (it.quality > 0)
        printf("品质：%s\n", getQualityName(it.quality).c_str());
    if (it.element != ELE_NONE)
        printf("元素属性：%s\n", getElementNameCN(it.element).c_str());
    if (it.requiredLevel > 0)
        printf("需求等级：%d 级\n", it.requiredLevel);
    if (it.price > 0)
        printf("商店价格：%d 金币\n", it.price);
    string eff = getItemEffectDesc(id);
    if (!eff.empty()) printf("效果：%s\n", eff.c_str());
    if (!it.recipe.empty()) {
        printf("可锻造：是\n配方材料：\n");
        for (auto& r : it.recipe)
            printf("  %s x%d\n",
                   safeItemName(r.first).c_str(), r.second);
    } else {
        printf("可锻造：否\n");
    }
    printf("\n");
    system("pause");
}

void searchItemByName() {
    system("cls");
    printf("===== 按名称搜索 =====\n");
    printf("\n请输入关键词（输入0取消）: ");
    string keyword; cin >> keyword;
    if (keyword == "0") return;
    vector<int> results;
    for (size_t i = 0; i < itemDB.size(); i++)
        if (itemDB[i].id == (int)i
            && itemDB[i].name != "[空]"
            && itemDB[i].name != "[未知物品]"
            && itemDB[i].name.find(keyword) != string::npos)
            results.push_back(i);
    if (results.empty()) {
        printf("未找到包含 \"%s\" 的物品。\n", keyword.c_str());
        system("pause"); return;
    }
    printf("\n找到 %d 个匹配物品：\n\n", (int)results.size());
    printf("ID    名称                    类型      品质\n");
    printf("================================================\n");
    for (int id : results)
        printf("%4d  %-20s  %-6s  %s\n",
               id, itemDB[id].name.c_str(),
               getItemTypeNameCN(id).c_str(),
               getQualityName(itemDB[id].quality).c_str());
    printf("================================================\n");
    printf("输入物品ID查看详情（输入0返回）: ");
    int choice; cin >> choice;
    if (choice > 0) displayItemDetail(choice);
}

void searchItemByType() {
    system("cls");
    printf("===== 按类型浏览 =====\n");
    printf("[1]武器 [2]防具 [3]服装 [4]饰品 [5]丹药 [6]暗器 [7]材料 [8]任务 [0]返回\n");
    char choice = read();
    if (choice == '0') return;
    ItemType targetType;
    string typeName;
    switch (choice) {
        case '1': targetType = WEAPON; typeName = "武器"; break;
        case '2': targetType = ARMOR; typeName = "防具"; break;
        case '3': targetType = CLOTHING; typeName = "服装"; break;
        case '4': targetType = ACCESSORY; typeName = "饰品"; break;
        case '5': targetType = POTION; typeName = "丹药"; break;
        case '6': targetType = THROWING; typeName = "暗器"; break;
        case '7': targetType = MATERIAL; typeName = "材料"; break;
        case '8': targetType = QUEST; typeName = "任务物品"; break;
        default: return;
    }
    system("cls");
    printf("===== %s 列表 =====\n\n", typeName.c_str());
    printf("ID    名称                    价格      效果\n");
    printf("================================================\n");
    vector<int> results;
    for (size_t i = 0; i < itemDB.size(); i++)
        if (itemDB[i].id == (int)i
            && itemDB[i].name != "[空]"
            && itemDB[i].name != "[未知物品]"
            && itemDB[i].type == targetType)
            results.push_back(i);
    for (int id : results) {
        string eff = getItemEffectDesc(id);
        if (eff.length() > 20) eff = eff.substr(0, 17) + "...";
        printf("%4d  %-20s  %6d  %s\n",
               id, itemDB[id].name.c_str(),
               itemDB[id].price, eff.c_str());
    }
    printf("================================================\n");
    printf("输入物品ID查看详情（输入0返回）: ");
    int cid; cin >> cid;
    if (cid > 0) displayItemDetail(cid);
}

void itemQueryUI() {
    while (true) {
        system("cls");
        printf("===== 物品查询系统 =====\n");
        printf("[1] 按ID查询\n[2] 按名称搜索\n[3] 按类型浏览\n[0] 返回\n");
        printf("\n请选择: ");
        char choice = read();
        switch (choice) {
            case '1': {
                system("cls");
                printf("请输入物品ID（输入0返回）: ");
                int id; cin >> id;
                if (id > 0) displayItemDetail(id);
                break;
            }
            case '2': searchItemByName(); break;
            case '3': searchItemByType(); break;
            case '0': return;
        }
    }
}

// ============================================================
// 标题界面
// ============================================================
bool needLoadSave = false;

void titleScreen() {
    system("cls");
    setColor(14);
    cout << "      ____  _                     _          ____        _                 \n";
    cout << "     / ___|(_)  ____ _ _ __    __| | ___    / ___| _   _| |__   ___  _ __  \n";
    cout << "     \\___ \\| | / _` | '_ \\  / _` |/ _ \\   \\___ \\| | | | '_ \\ / _ \\| '_ \\ \n";
    cout << "      ___) | || (_| | | | || (_| |  __/    ___) | |_| | |_) | (_) | | | |\n";
    cout << "     |____/|_| \\__,_|_| |_| \\__,_|\\___|   |____/ \\__,_|_.__/ \\___/|_| |_|\n";
    setColor(11);
    cout << "                       《 鲜 贱 骑 虾 转 》\n";
    setColor(10);
    cout << "                         ver 6.0 预发布版二\n";
    setColor(15);
    cout << "            作者：Liuhang, guojunyan2031, 2030chenxilin\n";
    cout << "      ====================================================\n\n";
    setColor(7);

    ifstream f("save.dat");
    bool validSave = false;
    if (f.is_open()) {
        f.seekg(0, ios::end);
        if (f.tellg() > 0) validSave = true;
        f.close();
    }
    if (validSave) {
        cout << "检测到存档文件！\n";
        cout << "[1] 读取存档继续游戏\n";
        cout << "[2] 重新开始新游戏\n";
        cout << "请选择 (1/2): ";
        char choice = getchar();
        if (choice == '1') { needLoadSave = true; return; }
        else if (choice == '2') {
            player = Player();
            remove("save.dat");
            needLoadSave = false;
            return;
        }
    }
    cout << "按任意键开始你的修仙之旅...";
    read();
}

// ============================================================
// 主菜单
// F20: currentLevel 由 loadGame 读入，主菜单不再覆盖
// ============================================================
void mainMenu() {
    while (1) {
        system("cls");
        printf("===== 鲜贱骑虾转 =====\n");
        printf("当前已通关最大关卡：%d\n", currentLevel);
        printf("[1] 游玩关卡\n");
        printf("[2] 商店\n");
        printf("[3] 锻造\n");
        printf("[4] 角色状态\n");
        printf("[5] 背包\n");
        printf("[6] 存档/读档\n");
        printf("[7] 成就\n");
        printf("[8] 任务\n");
        printf("[9] 更新内容\n");
        printf("[A] 拍卖行\n");
        printf("[B] 物品查询\n");
        printf("[0] 退出\n");
        char ch = read();

        if (ch == '1') {
            printf("请输入要进入的关卡编号（1~%d，0退出）: ", currentLevel);
            int mapId; cin >> mapId;
            if (mapId >= 1 && mapId <= currentLevel) {
                loadMap(mapId);
                explore();
                if (isMapClear() && !currentMap.exitLocked) {
                    if (mapId == currentLevel && currentLevel < MAX_MAPS) {
                        currentLevel++;
                        player.dungeonLevel = currentLevel;
                    }
                }
            } else if (mapId != 0) {
                printf("无效关卡！\n"); Sleep(800);
            }
        } else if (ch == '2') {
            shop();
        } else if (ch == '3') {
            forgingUI();
        } else if (ch == '4') {
            status();
        } else if (ch == '5') {
            bagUI();
        } else if (ch == '6') {
            system("cls");
            printf("===== 存档/读档 =====\n");
            printf("[1] 存档\n[2] 读档\n[0] 返回\n");
            char subCh = read();
            if (subCh == '1') saveGame();
            else if (subCh == '2') loadGame();
        } else if (ch == '7') {
            showAchievements();
        } else if (ch == '8') {
            showQuests();
        } else if (ch == '9') {
            system("cls");
            setColor(14);
            cout << "==================== 更 新 内 容 ====================\n\n";
            resetColor();
            cout << "【预发布版二版内容】\n";
            cout << "  - 暴击伤害使用 critDmg（不再硬编码 2 倍）\n";
            cout << "  - 首次攻击每场战斗重置（暗影被动正确）\n";
            cout << "  - 技能冷却 cd 生效\n";
            cout << "  - 状态持续时间不再双重递减\n";
            cout << "  - 击杀计数不再重复\n";
            cout << "  - 丹药使用统一走 usePotion\n";
            cout << "  - 临时丹药（神力丹/护体丹）真临时\n";
            cout << "  - 符修眩晕时也回蓝\n";
            cout << "  - 成就发放统一 unlockAchievement\n";
            cout << "  - 通关判定统一 tryExitMap\n";
            cout << "  - 存档只保存成就 unlocked 状态\n";
            cout << "  - 移除所有死代码与冗余变量\n\n";
            system("pause");
        } else if (ch == 'A' || ch == 'a') {
            auctionHouse();
        } else if (ch == 'B' || ch == 'b') {
            itemQueryUI();
        } else if (ch == '0') {
            exit(0);
        }
    }
}

// ============================================================
// 主函数
// F20: currentLevel 与 player.dungeonLevel 统一
// ============================================================
int main() {
    srand((unsigned)time(nullptr));

    titleScreen();
    initItems();
    initClasses();
    initMonsters();
    sanitizeAllRecipes();
    initAchievements();
    initQuests();

    if (needLoadSave) {
        loadGame();
        // F20: currentLevel 已在 loadGame 里读入
    } else {
        chooseClass();
    }
    mainMenu();
    return 0;
}
