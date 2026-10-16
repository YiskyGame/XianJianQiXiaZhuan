#ifndef MAP_MAP_H
#define MAP_MAP_H

#include "core/types.h"
#include <string>
#include <vector>
using namespace std;

// ============================================================
// NPC
// ============================================================
struct MapNpc {
    int x, y;
    char symbol;
    int color;
    string name;
    vector<string> dialogs;
    int questId;
};

// ============================================================
// 宝箱
// ============================================================
struct MapBox {
    int x, y;
    vector<int> items;
    bool opened;
};

// ============================================================
// 地图上的怪物
// ============================================================
struct MapMonster {
    int type, x, y, hp;
    bool alive;
};

// ============================================================
// 地图
// ============================================================
struct GameMap {
    string name;
    vector<string> data;
    int px, py, ex, ey;
    bool exitLocked;
    vector<MapBox> boxes;
    vector<MapMonster> mobs;
    vector<MapNpc> npcs;
    int themeColor;
};

// ============================================================
// 全局地图对象（extern）
// ============================================================
extern GameMap currentMap;

// ============================================================
// 地图操作
// ============================================================
void loadMap(int id);
void drawMap();
bool isMapClear();
void monsterMove(int &mx, int &my, int px, int py, vector<string>& map);

// ============================================================
// 统一通关判定
// ============================================================
void tryExitMap(bool& exited);

// ============================================================
// 探索
// ============================================================
void explore();

#endif // MAP_MAP_H