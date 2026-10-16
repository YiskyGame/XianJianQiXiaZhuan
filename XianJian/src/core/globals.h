#ifndef CORE_GLOBALS_H
#define CORE_GLOBALS_H

#include "core/types.h"
#include <string>
#include <vector>
#include <map>
using namespace std;

// ============================================================
// 结构体前向声明（避免头文件循环依赖）
// ============================================================
struct Item;
struct Monster;
struct CharacterClass;
struct Quest;
struct Achievement;
struct Player;
struct MapBox;
struct MapMonster;
struct MapNpc;
struct GameMap;
struct GameSettings;

// ============================================================
// 全局变量（在 globals.cpp 中定义）
// ============================================================
extern vector<Item> itemDB;
extern map<string, int> itemNameToID;

extern vector<CharacterClass> classes;

extern vector<Quest> quests;

extern vector<Achievement> achievements;

extern vector<Monster> monsterDB;

extern Player player;

extern int currentLevel;
extern int currentSlot;
extern bool needLoadSave;

// 注意：currentMap 和 g_settings 因为需要完整类型，
// 它们的 extern 声明放在 map.h / settings.h 里。
// 这里不重复声明，避免重复定义。

#endif // CORE_GLOBALS_H
