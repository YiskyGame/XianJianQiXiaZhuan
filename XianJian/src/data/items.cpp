#include "data/items.h"
#include "core/globals.h"
#include "core/utils.h"

// ============================================================
// 物品注册工具
// ============================================================
void regItem(int id, string name, string desc, ItemType type,
			 int price, int effVal,
			 ItemEffect eff, int quality,
			 Element ele, int reqLv) {
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
// 初始化物品
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
// 清理非法配方
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
