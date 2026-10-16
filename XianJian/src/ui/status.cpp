#include "ui/ui.h"
#include "core/globals.h"
#include "core/utils.h"
#include "data/items.h"
#include "data/classes.h"
#include "entity/player.h"
#include "entity/stats.h"

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
    string en[] = {"无", "火", "冰", "雷", "毒", "圣", "暗", "风", "土"};
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
    for (size_t i = 0; i < classes[player.classId].skills.size(); i++) {
        int lv = (i < player.skillLv.size()) ? player.skillLv[i] : 0;
        printf("  %s Lv.%d - %s\n",
               classes[player.classId].skills[i].name.c_str(),
               lv,
               classes[player.classId].skills[i].desc.c_str());
    }
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