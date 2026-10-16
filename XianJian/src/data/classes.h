#ifndef DATA_CLASSES_H
#define DATA_CLASSES_H

#include "core/types.h"
#include <string>
#include <vector>
using namespace std;

// ============================================================
// 技能
// ============================================================
struct Skill {
	string name, desc;
	int type;
	int value;
	int costMp;
	int special;
	int cd;
};

// ============================================================
// 职业
// ============================================================
struct CharacterClass {
	string name, title;
	int baseHp, baseAtk, baseDef, baseCrit;
	float baseCritDmg;
	vector<Skill> skills;
	string passiveName, passiveDesc;
};

// ============================================================
// 初始化
// ============================================================
void initClasses();

#endif // DATA_CLASSES_H
