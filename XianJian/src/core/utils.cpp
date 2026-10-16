#include "core/utils.h"
#include "core/globals.h"
#include "data/items.h"

// ============================================================
// 控制台颜色
// ============================================================
void setColor(int c) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

void resetColor() {
	setColor(7);
}

// ============================================================
// 输入（处理方向键）
// ============================================================
int read() {
	int c = _getch();
	if (c == 0 || c == 224) {
		_getch();
		return -1;
	}
	return c;
}

// ============================================================
// 物品工具
// ============================================================
string safeItemName(int id) {
	if (id <= 0) return "[未知物品]";
	if ((size_t)id >= itemDB.size()) return "[未知物品]";
	if (itemDB[id].id != id) return "[未知物品]";
	if (itemDB[id].name.empty() || itemDB[id].name == "[空]") return "[空物品]";
	return itemDB[id].name;
}

string getItemEffectDesc(int id) {
	if (id <= 0) return "";
	if ((size_t)id >= itemDB.size()) return "";
	if (itemDB[id].id != id) return "";
	
	Item &it = itemDB[id];
	string desc;
	switch (it.effect) {
	case ADD_ATK:
		desc = "攻击+" + to_string(it.effectVal);
		break;
	case ADD_DEF:
		desc = "防御+" + to_string(it.effectVal);
		break;
	case ADD_HP:
		desc = "生命+" + to_string(it.effectVal);
		break;
	case ADD_CRIT:
		desc = "暴击率+" + to_string(it.effectVal) + "%";
		break;
	case ADD_CRITDMG:
		desc = "暴伤+" + to_string(it.effectVal) + "%";
		break;
	case HEAL:
		if (id == 325) desc = "恢复全部生命和灵力";
		else desc = "恢复" + to_string(it.effectVal) + "生命";
		break;
	case CURE:
		desc = "解除中毒";
		break;
	case ADD_MP:
		desc = "灵力+" + to_string(it.effectVal);
		break;
	case REVIVE:
		if (it.effectVal > 0)
			desc = "复活并恢复" + to_string(it.effectVal) + "%生命";
		else
			desc = "复活并恢复全部生命";
		break;
	case TEMP_ATK:
		desc = "临时攻击+20% 3回合";
		break;
	case TEMP_DEF:
		desc = "临时防御+20% 3回合";
		break;
	default:
		break;
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
