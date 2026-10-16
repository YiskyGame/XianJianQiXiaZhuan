#include "core/globals.h"
#include "data/items.h"
#include "data/classes.h"
#include "data/monsters.h"
#include "data/achievements.h"
#include "data/quests.h"
#include "entity/player.h"

// ============================================================
// 方向常量定义
// ============================================================
const int dx[4] = {-1, 1, 0, 0};
const int dy[4] = {0, 0, -1, 1};

// ============================================================
// 全局变量定义
// ============================================================
vector<Item> itemDB;
map<string, int> itemNameToID;

vector<CharacterClass> classes;

vector<Quest> quests;

vector<Achievement> achievements;

vector<Monster> monsterDB;

Player player;

int currentLevel = 1;
int currentSlot = 1;
bool needLoadSave = false;
