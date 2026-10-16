#include "entity/stats.h"
#include "entity/player.h"
#include "core/globals.h"
#include "data/items.h"

// ============================================================
// 总攻击
// ============================================================
int getTotalAtk() {
    int sum = player.atk;
    if (player.weapon > 0) {
        if ((size_t)player.weapon < itemDB.size()
            && itemDB[player.weapon].id == player.weapon
            && itemDB[player.weapon].effect == ADD_ATK) {
            int bonus = itemDB[player.weapon].effectVal;
            if (itemDB[player.weapon].element == player.playerElement)
                bonus = (int)(bonus * 1.3);
            sum += bonus;
        }
    }
    if (player.accessory > 0) {
        if ((size_t)player.accessory < itemDB.size()
            && itemDB[player.accessory].id == player.accessory
            && itemDB[player.accessory].effect == ADD_ATK)
            sum += itemDB[player.accessory].effectVal;
    }
    if (player.statusDur[7]) sum = (int)(sum * 1.2);
    return sum;
}

// ============================================================
// 总防御
// ============================================================
int getTotalDef() {
    int sum = player.def;
    if (player.armor > 0) {
        if ((size_t)player.armor < itemDB.size()
            && itemDB[player.armor].id == player.armor
            && itemDB[player.armor].effect == ADD_DEF)
            sum += itemDB[player.armor].effectVal;
    }
    if (player.accessory > 0) {
        if ((size_t)player.accessory < itemDB.size()
            && itemDB[player.accessory].id == player.accessory
            && itemDB[player.accessory].effect == ADD_DEF)
            sum += itemDB[player.accessory].effectVal;
    }
    if (player.statusDur[2])
        sum = (int)(sum * (player.classId == 3 ? 2.0 : 1.5));
    if (player.classId == 3 && player.hp < getMaxHp() * 0.3)
        sum *= 2;
    return sum;
}

// ============================================================
// 总暴击
// ============================================================
int getTotalCrit() {
    int sum = player.crit + player.passiveBonus;
    if (player.weapon > 0
        && (size_t)player.weapon < itemDB.size()
        && itemDB[player.weapon].effect == ADD_CRIT)
        sum += itemDB[player.weapon].effectVal;
    if (player.clothing > 0
        && (size_t)player.clothing < itemDB.size()
        && itemDB[player.clothing].effect == ADD_CRIT)
        sum += itemDB[player.clothing].effectVal;
    if (player.accessory > 0
        && (size_t)player.accessory < itemDB.size()
        && itemDB[player.accessory].effect == ADD_CRIT)
        sum += itemDB[player.accessory].effectVal;
    if (player.statusDur[8]) sum += 20;
    return min(100, sum);
}

// ============================================================
// 总暴伤
// ============================================================
float getTotalCritDmg() {
    float sum = player.critDmg;
    if (player.accessory > 0
        && (size_t)player.accessory < itemDB.size()
        && itemDB[player.accessory].effect == ADD_CRITDMG)
        sum += itemDB[player.accessory].effectVal / 100.0f;
    return sum;
}

// ============================================================
// 总生命上限
// ============================================================
int getMaxHp() {
    int sum = player.maxHp;
    if (player.armor > 0
        && (size_t)player.armor < itemDB.size()
        && itemDB[player.armor].id == player.armor
        && itemDB[player.armor].effect == ADD_HP)
        sum += itemDB[player.armor].effectVal;
    if (player.clothing > 0
        && (size_t)player.clothing < itemDB.size()
        && itemDB[player.clothing].id == player.clothing
        && itemDB[player.clothing].effect == ADD_HP)
        sum += itemDB[player.clothing].effectVal;
    if (player.accessory > 0
        && (size_t)player.accessory < itemDB.size()
        && itemDB[player.accessory].id == player.accessory
        && itemDB[player.accessory].effect == ADD_HP)
        sum += itemDB[player.accessory].effectVal;
    if (player.classId == 3 && player.skillLv.size() > 7 && player.skillLv[7] > 0)
        sum = (int)(sum * 1.2);
    return sum;
}