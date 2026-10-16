#ifndef DATA_ITEMS_H
#define DATA_ITEMS_H

#include "core/types.h"
#include <string>
#include <vector>
using namespace std;

// ============================================================
// 物品结构体
// ============================================================
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

// ============================================================
// 初始化 / 清理
// ============================================================
void initItems();
void sanitizeAllRecipes();

// ============================================================
// 物品注册工具
// ============================================================
void regItem(int id, string name, string desc, ItemType type,
			 int price, int effVal,
			 ItemEffect eff = EFF_NONE, int quality = 0,
			 Element ele = ELE_NONE, int reqLv = 0);

void addRecipe(int target, int matId, int cnt);

#endif // DATA_ITEMS_H
