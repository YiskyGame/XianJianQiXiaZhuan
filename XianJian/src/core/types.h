#ifndef CORE_TYPES_H
#define CORE_TYPES_H

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
#include <sstream>
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

// 多存档
const int MAX_SLOTS = 3;

// 玩家设置
const string SETTINGS_FILE = "settings.dat";

// ============================================================
// 方向
// ============================================================
extern const int dx[4];
extern const int dy[4];

// ============================================================
// 物品类型
// ============================================================
enum ItemType {
	WEAPON, ARMOR, CLOTHING, POTION, MATERIAL,
	ACCESSORY, THROWING, QUEST, SPECIAL
};

enum ItemEffect {
	EFF_NONE, ADD_HP, ADD_ATK, ADD_DEF, ADD_CRIT,
	ADD_CRITDMG, HEAL, CURE, REVIVE, ADD_MP,
	TEMP_ATK, TEMP_DEF
};

enum Element {
	ELE_NONE, ELE_FIRE, ELE_ICE, ELE_THUNDER, ELE_POISON,
	ELE_HOLY, ELE_DARK, ELE_WIND, ELE_EARTH
};

// ============================================================
// 成就 ID
// ============================================================
enum AchId {
	ACH_FIRST_BLOOD = 0,
	ACH_HUNTER,
	ACH_BOSS_SLAYER,
	ACH_SPIRIT_FRIEND,
	ACH_RICH_1K,
	ACH_RICH_10K,
	ACH_FORGE_5,
	ACH_FORGE_20,
	ACH_ALCHEMY_5,
	ACH_ALCHEMY_20,
	ACH_SHOPAHOLIC,
	ACH_EXPLORER,
	ACH_CONQUEROR,
	ACH_UNDYING,
	ACH_SKILL_MAX,
	ACH_CRIT_LOVER,
	ACH_TREASURE_HUNTER,
	ACH_BLACKSMITH_HEIR,
	ACH_SPENDTHRIFT,
	ACH_HERO_PEAK,
	ACH_SKILL_MASTER,
	ACH_LIMIT_REVERSAL,
	ACH_SLIP,
	ACH_PERSISTENT,
	ACH_INVINCIBLE,
	ACH_ARROGANT,
	ACH_COUNT
};

// ============================================================
// 战斗状态
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

#endif // CORE_TYPES_H
