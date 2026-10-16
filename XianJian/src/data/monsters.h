#ifndef DATA_MONSTERS_H
#define DATA_MONSTERS_H

#include "core/types.h"
#include <string>
#include <vector>
using namespace std;

// ============================================================
// 怪物
// ============================================================
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

// ============================================================
// 初始化
// ============================================================
void addMonster(int id, string n, int hp, int atk, int def,
				int exp, int gold, char sym, int col,
				bool spirit, Element ele, int skillChance,
				vector<pair<int, int>> loot);

void initMonsters();

#endif // DATA_MONSTERS_H
