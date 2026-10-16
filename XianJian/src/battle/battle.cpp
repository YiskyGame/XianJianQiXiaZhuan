#include "battle/battle.h"
#include "core/globals.h"
#include "core/utils.h"
#include "data/items.h"
#include "data/classes.h"
#include "data/monsters.h"
#include "data/achievements.h"
#include "data/quests.h"
#include "entity/player.h"
#include "entity/stats.h"
#include "item/bag.h"

// ============================================================
// 玩家普通攻击
// F9: 非法输入 → 空过
// F1: 暴击使用 critDmg
// F2: firstAttack 在 battle() 开头重置
// ============================================================
void processPlayerAttack(BattleContext& ctx) {
    system("cls");
    printf("选择攻击方向: [J]上 [U]右 [I]下 [K]左\n");
    int dir = read();
    int pDir = (dir == 'j' || dir == 'J') ? 0
               : (dir == 'u' || dir == 'U') ? 1
               : (dir == 'i' || dir == 'I') ? 2
               : (dir == 'k' || dir == 'K') ? 3 : -1;

    if (pDir == -1) {
        printf("无效输入，本回合空过。\n");
        Sleep(600);
        ctx.state = BST_MONSTER_TURN;
        return;
    }

    int mBlock = rand() % 4;
    if (pDir == mBlock) {
        printf("怪物格挡了你的攻击！\n");
        Sleep(600);
        ctx.state = BST_MONSTER_TURN;
        return;
    }

    int dmg = getTotalAtk();
    bool isCrit = false;

    if (player.classId == 2 && player.firstAttack) {
        isCrit = true;
        player.firstAttack = false;
    } else if (rand() % 100 < getTotalCrit()) {
        isCrit = true;
    }

    if (isCrit) {
        float critMul = getTotalCritDmg();
        dmg = (int)(dmg * critMul);
        player.totalCritCount++;
        printf("暴击！(x%.2f) ", critMul);
        if (ctx.monsterId < 100) {
            int ns = rand() % 3 + 1;
            if (ctx.monsterStunTurns > 0)
                ctx.monsterStunTurns = max(ctx.monsterStunTurns, ns);
            else
                ctx.monsterStunTurns = ns;
            printf("怪物眩晕 %d 回合！\n", ns);
        }
    }

    dmg = max(1, dmg - ctx.mDef);
    ctx.mHp -= dmg;
    printf("造成 %d 点伤害\n", dmg);

    if (player.classId == 2 && player.skillLv.size() > 2 && player.skillLv[2] > 0) {
        ctx.poisonTurn = 3;
        ctx.poisonDmg = getTotalAtk() / 3;
        printf("淬毒：怪物中毒！\n");
    }
    if (!isCrit && rand() % 100 < 12 && ctx.monsterId < 100) {
        int ns = rand() % 3 + 1;
        if (ctx.monsterStunTurns > 0)
            ctx.monsterStunTurns = max(ctx.monsterStunTurns, ns);
        else
            ctx.monsterStunTurns = ns;
        printf("怪物被击晕！剩余 %d 回合\n", ctx.monsterStunTurns);
    }
    Sleep(600);
    ctx.state = BST_MONSTER_TURN;
}

// ============================================================
// 技能选择
// F1: 技能暴击使用 critDmg
// F4: 技能冷却生效
// ============================================================
void processSkillSelect(BattleContext& ctx) {
    system("cls");
    auto& skl = classes[player.classId].skills;
    printf("===== 技能列表 =====\n");
    for (size_t i = 0; i < skl.size(); i++) {
        int cd = (i < player.skillCd.size()) ? player.skillCd[i] : 0;
        int lv = (i < player.skillLv.size()) ? player.skillLv[i] : 0;
        printf("[%d] %s Lv.%d (MP:%d CD:%d) - %s\n",
               (int)i + 1, skl[i].name.c_str(), lv,
               skl[i].costMp, cd, skl[i].desc.c_str());
    }
    printf("[C] 返回\n");
    char ch = read();

    if (ch == 'c' || ch == 'C') {
        ctx.state = BST_PLAYER_TURN;
        return;
    }

    int idx = ch - '1';
    if (idx < 0 || idx >= (int)skl.size()) {
        printf("无效选择！\n");
        Sleep(500);
        ctx.state = BST_PLAYER_TURN;
        return;
    }
    if (idx >= (int)player.skillLv.size() || player.skillLv[idx] == 0) {
        printf("该技能尚未领悟，无法使用！\n");
        Sleep(500);
        ctx.state = BST_PLAYER_TURN;
        return;
    }
    if (idx < (int)player.skillCd.size() && player.skillCd[idx] > 0) {
        printf("技能冷却中（剩余 %d 回合）！\n", player.skillCd[idx]);
        Sleep(500);
        ctx.state = BST_PLAYER_TURN;
        return;
    }
    if (player.mp < skl[idx].costMp) {
        printf("灵力不足！\n");
        Sleep(500);
        ctx.state = BST_PLAYER_TURN;
        return;
    }

    player.mp -= skl[idx].costMp;
    player.skillUseCount++;

    if (idx < (int)player.skillCd.size()) {
        player.skillCd[idx] = skl[idx].cd;
    }

    if (!achievements[ACH_SKILL_MASTER].unlocked && player.skillUseCount >= 500)
        unlockAchievement(ACH_SKILL_MASTER);

    Skill& s = skl[idx];
    int lv = player.skillLv[idx];
    double scale = 1.0 + 0.2 * (lv - 1);
    int dmg = 0;
    bool skillCrit = false;

    if (s.type == 0) {
        if (player.classId == 3 && s.name == "舍身击") {
            int sc = max(1, player.hp / 10);
            player.hp -= sc;
            printf("舍身一击！消耗 %d 生命值。\n", sc);
        }

        if (s.special == 6) {
            dmg = (int)(getTotalAtk() * 2 * scale);
            skillCrit = true;
        } else if (s.special == 3) {
            int hits = 3, total = 0;
            for (int i = 0; i < hits; i++) {
                int hit = max(1, (int)(getTotalAtk() * 0.8 * scale) - ctx.mDef);
                ctx.mHp -= hit;
                total += hit;
            }
            printf("三连击造成 %d 点伤害！\n", total);
            Sleep(500);
            ctx.state = BST_MONSTER_TURN;
            return;
        } else if (s.special == 4) {
            dmg = (int)(getTotalAtk() * 1.5 * scale);
            ctx.poisonTurn = 3;
            ctx.poisonDmg = getTotalAtk() / 2;
        } else if (s.special == 1) {
            dmg = (int)(getTotalAtk() * 1.8 * scale);
            if (ctx.monsterId < 100)
                ctx.monsterStunTurns = max(ctx.monsterStunTurns, 2);
        } else {
            dmg = (int)(getTotalAtk() * (s.value / 100.0) * scale);
        }

        if (!skillCrit && s.special != 3 && s.special != 6) {
            if (rand() % 100 < getTotalCrit()) {
                dmg = (int)(dmg * getTotalCritDmg());
                skillCrit = true;
                player.totalCritCount++;
                printf("暴击！");
                if (ctx.monsterId < 100)
                    ctx.monsterStunTurns = max(ctx.monsterStunTurns, rand() % 3 + 1);
            }
        } else if (skillCrit) {
            player.totalCritCount++;
        }

        if (s.special != 3) {
            dmg = max(1, dmg - ctx.mDef);
            ctx.mHp -= dmg;
            printf("造成 %d 点伤害！\n", dmg);
        }
        if (s.special == 2) {
            int heal = dmg * 0.3;
            player.hp = min(getMaxHp(), player.hp + heal);
            printf("吸血恢复 %d 点生命值！\n", heal);
        }

    } else if (s.type == 1) {
        int heal = (int)(s.value * scale);
        if (player.classId == 1) heal = (int)(heal * 1.2);
        player.hp = min(getMaxHp(), player.hp + heal);
        printf("恢复 %d 点生命值！\n", heal);

    } else if (s.type == 2) {
        if (s.special == 8) {
            player.statusDur[2] = 3;
            printf("防御提升 3 回合！\n");
        } else if (s.special == 7) {
            player.statusDur[4] = 2;
            printf("反伤激活 2 回合！\n");
        } else if (s.special == 9) {
            player.statusDur[3] = 2;
            printf("闪避提升 2 回合！\n");
        } else if (s.special == 0) {
            player.statusDur[7] = 3;
            printf("攻击提升 3 回合！\n");
        }
    }

    if (!achievements[ACH_CRIT_LOVER].unlocked && player.totalCritCount >= 50)
        unlockAchievement(ACH_CRIT_LOVER);

    Sleep(600);
    ctx.state = BST_MONSTER_TURN;
}

// ============================================================
// 战斗中使用丹药
// ============================================================
void processItemSelect(BattleContext& ctx) {
    system("cls");
    printf("===== 可用丹药 =====\n");
    vector<int> pots;
    for (auto& p : player.bag) {
        if (p.first > 0 && p.first < (int)itemDB.size()
            && itemDB[p.first].id == p.first
            && itemDB[p.first].type == POTION) {
            pots.push_back(p.first);
            printf("[%d] %s x%d\n", (int)pots.size(),
                   safeItemName(p.first).c_str(), p.second);
        }
    }
    if (pots.empty()) {
        printf("没有可用丹药！\n");
        Sleep(500);
        ctx.state = BST_PLAYER_TURN;
        return;
    }
    printf("[C] 返回\n");
    char ch = read();
    if (ch == 'c' || ch == 'C') {
        ctx.state = BST_PLAYER_TURN;
        return;
    }
    int i = ch - '1';
    if (i >= 0 && i < (int)pots.size()) {
        int id = pots[i];
        removeBag(id, 1);
        usePotion(id, true);
        printf("使用 %s\n", safeItemName(id).c_str());
        Sleep(500);
        ctx.state = (player.classId == 1
                     && player.skillLv.size() > 2
                     && player.skillLv[2] > 0)
                    ? BST_PLAYER_TURN : BST_MONSTER_TURN;
    }
}

// ============================================================
// 暗器
// ============================================================
void processThrowSelect(BattleContext& ctx) {
    system("cls");
    printf("===== 暗器 =====\n");
    vector<int> thr;
    for (auto& p : player.bag) {
        if (p.first > 0 && p.first < (int)itemDB.size()
            && itemDB[p.first].id == p.first
            && itemDB[p.first].type == THROWING) {
            thr.push_back(p.first);
            printf("[%d] %s x%d\n", (int)thr.size(),
                   safeItemName(p.first).c_str(), p.second);
        }
    }
    if (thr.empty()) {
        printf("没有暗器！\n");
        Sleep(500);
        ctx.state = BST_PLAYER_TURN;
        return;
    }
    printf("[C] 返回\n");
    char ch = read();
    if (ch == 'c' || ch == 'C') {
        ctx.state = BST_PLAYER_TURN;
        return;
    }
    int i = ch - '1';
    if (i >= 0 && i < (int)thr.size()) {
        int id = thr[i];
        removeBag(id, 1);
        ctx.mHp -= itemDB[id].effectVal;
        printf("使用 %s，造成 %d 伤害\n",
               safeItemName(id).c_str(), itemDB[id].effectVal);
        Sleep(500);
        ctx.state = BST_MONSTER_TURN;
    }
}

// ============================================================
// 怪物技能
// ============================================================
void monsterSkill(BattleContext& ctx) {
    int id = ctx.monsterId;
    if (id >= 10 && id < 100) {
        int dmg = (int)(ctx.mAtk * 1.5);
        printf("%s 使用强力撕咬！无法格挡！\n", ctx.mName.c_str());
        int real = max(1, dmg - getTotalDef());
        player.hp -= real;
        printf("受到 %d 伤害\n", real);
    } else if (id >= 100 && id < 200) {
        if (rand() % 2 == 0) {
            int dmg = (int)(ctx.mAtk * 2.0);
            printf("%s 使用暗影冲击！无法格挡！\n", ctx.mName.c_str());
            int real = max(1, dmg - getTotalDef());
            player.hp -= real;
            printf("受到 %d 伤害\n", real);
        } else {
            ctx.mDef = (int)(ctx.mDef * 1.05);
            printf("%s 使用黑暗护盾，防御提升！\n", ctx.mName.c_str());
        }
    }
}

// ============================================================
// 怪物回合
// F5: 状态持续时间已改在 battle() 主循环减一次
// F11: 符修回蓝已改在 battle() 开头
// ============================================================
void processMonsterTurn(BattleContext& ctx) {
    if (ctx.monsterStunTurns > 0) {
        ctx.monsterStunTurns--;
        if (ctx.monsterStunTurns == 0) {
            printf("怪物从眩晕中恢复！\n");
            if (ctx.charge > 0) {
                int dmg = getTotalAtk() * (1 + ctx.charge / 2);
                dmg = max(1, dmg - ctx.mDef);
                ctx.mHp -= dmg;
                printf("蓄力爆发！造成 %d 点伤害！\n", dmg);
                ctx.charge = 0;
                Sleep(500);
                if (ctx.mHp <= 0) {
                    ctx.state = BST_VICTORY;
                    return;
                }
            }
        } else {
            printf("怪物处于眩晕状态，无法行动（剩余 %d 回合）。\n",
                   ctx.monsterStunTurns);
            Sleep(600);
        }
        ctx.state = BST_PLAYER_TURN;
        return;
    }

    if (player.statusDur[3] > 0) {
        player.statusDur[3]--;
        printf("影遁触发！闪避并反击\n");
        int dmg = max(1, getTotalAtk() - ctx.mDef);
        ctx.mHp -= dmg;
        printf("反击造成 %d 伤害\n", dmg);
        Sleep(500);
        ctx.state = (ctx.mHp <= 0) ? BST_VICTORY : BST_PLAYER_TURN;
        return;
    }

    int skillChance = (ctx.monsterId >= 200) ? 50
                      : (ctx.monsterId >= 100) ? 40
                      : (ctx.monsterId >= 10)  ? 30 : 0;
    if (skillChance && rand() % 100 < skillChance) {
        monsterSkill(ctx);
    } else {
        printf("怪物攻击！请格挡 [J]上 [U]右 [I]下 [K]左\n");
        char def = read();
        int pDir = (def == 'j' || def == 'J') ? 0
                   : (def == 'u' || def == 'U') ? 1
                   : (def == 'i' || def == 'I') ? 2
                   : (def == 'k' || def == 'K') ? 3 : -1;
        int mDir = rand() % 4;
        if (pDir == mDir) {
            printf("格挡成功！\n");
        } else {
            int dmg = max(1, ctx.mAtk - getTotalDef());
            player.hp -= dmg;
            printf("受到 %d 伤害\n", dmg);
            if (player.statusDur[4] > 0) {
                int reb = dmg / 2;
                ctx.mHp -= reb;
                printf("反伤 %d 伤害\n", reb);
                player.statusDur[4]--;
            }
        }
    }
    Sleep(600);
    ctx.state = BST_PLAYER_TURN;
}

// ============================================================
// 主战斗函数
// F2: firstAttack 每场战斗重置
// F5: 状态持续时间只减一次
// F7: 击杀计数只在 battle 里加一次
// F11: 符修每回合回蓝
// ============================================================
bool battle(Monster& mon, int& mx, int& my) {
    BattleContext ctx;
    ctx.mon = &mon;
    ctx.mHp = mon.hp;
    ctx.mMaxHp = mon.hp;
    ctx.mAtk = mon.atk;
    ctx.mDef = mon.def;
    ctx.mName = mon.name;
    ctx.charge = 0;
    ctx.poisonDmg = ctx.poisonTurn = 0;
    ctx.state = BST_PLAYER_TURN;
    ctx.monsterStunTurns = 0;
    ctx.monsterId = mon.id;

    player.firstAttack = true;
    player.usedPotionInBattle = false;

    bool isBoss = (mon.id >= 100 && mon.id < 200);

    if (player.classId == 5) {
        int r = rand() % 3;
        if (r == 0) player.statusDur[2] = 3;
        else if (r == 1) player.statusDur[7] = 3;
        else player.statusDur[4] = 2;
        printf("阵基触发，获得随机增益！\n");
    }

    while (ctx.mHp > 0 && player.hp > 0) {
        // F5: 状态持续时间只在这里减一次
        for (int i = 0; i < 10; i++) {
            if (player.statusDur[i] > 0) player.statusDur[i]--;
        }
        // F4: 技能冷却递减
        for (size_t i = 0; i < player.skillCd.size(); i++) {
            if (player.skillCd[i] > 0) player.skillCd[i]--;
        }
        // F11: 符修每回合回蓝
        if (player.classId == 4) {
            player.mp = min(player.maxMp, player.mp + 2);
        }

        if (ctx.poisonTurn > 0) {
            ctx.mHp -= ctx.poisonDmg;
            ctx.poisonTurn--;
        }
        if (ctx.mHp <= 0) {
            ctx.state = BST_VICTORY;
            break;
        }

        bool playerStun   = (player.statusDur[1] > 0);
        bool playerFrozen = (player.statusDur[5] > 0);

        system("cls");
        setColor(15);
        printf("===== 战斗 ===== %s  VS  %s\n",
               player.name.c_str(), ctx.mName.c_str());
        printf("生命: %d/%d  灵力: %d/%d  攻击: %d  防御: %d  暴击: %d%%  暴伤: %.2f\n",
               player.hp, getMaxHp(), player.mp, player.maxMp,
               getTotalAtk(), getTotalDef(),
               getTotalCrit(), getTotalCritDmg());
        printf("怪物生命: %d/%d  眩晕剩余: %d 回合\n",
               ctx.mHp, ctx.mMaxHp, ctx.monsterStunTurns);
        if (ctx.charge) printf("蓄力层数: %d\n", ctx.charge);

        switch (ctx.state) {
            case BST_PLAYER_TURN:
                if (playerStun || playerFrozen) {
                    printf("你无法行动！\n");
                    Sleep(800);
                    ctx.state = BST_MONSTER_TURN;
                    break;
                }
                printf("[1]攻击  [2]技能  [3]丹药  [4]暗器");
                if (ctx.monsterStunTurns > 0)
                    printf("  [5]蓄力  [0]结束回合");
                printf("\n");
                {
                    char cmd = read();
                    if (cmd == '1') {
                        processPlayerAttack(ctx);
                    } else if (cmd == '2') {
                        ctx.state = BST_PLAYER_SKILL_SELECT;
                    } else if (cmd == '3') {
                        ctx.state = BST_PLAYER_ITEM_SELECT;
                    } else if (cmd == '4') {
                        ctx.state = BST_PLAYER_THROW_SELECT;
                    } else if (cmd == '5' && ctx.monsterStunTurns > 0) {
                        if (ctx.charge >= 3) {
                            printf("蓄力已达最大层数！\n");
                            Sleep(400);
                        } else {
                            ctx.charge++;
                            printf("蓄力 +1，当前层数: %d\n", ctx.charge);
                            Sleep(400);
                        }
                    } else if (cmd == '0' && ctx.monsterStunTurns > 0) {
                        ctx.state = BST_MONSTER_TURN;
                    }
                }
                break;
            case BST_PLAYER_SKILL_SELECT:
                processSkillSelect(ctx);
                break;
            case BST_PLAYER_ITEM_SELECT:
                processItemSelect(ctx);
                break;
            case BST_PLAYER_THROW_SELECT:
                processThrowSelect(ctx);
                break;
            case BST_MONSTER_TURN:
                processMonsterTurn(ctx);
                break;
            default:
                break;
        }
        if (ctx.mHp <= 0) ctx.state = BST_VICTORY;
        if (player.hp <= 0) {
            if (player.statusDur[6] > 0) {
                player.hp = 1;
                player.statusDur[6] = 0;
                printf("不灭金身触发！\n");
                Sleep(500);
            } else {
                ctx.state = BST_DEFEAT;
            }
        }
    }

    if (player.hp <= 0) {
        system("cls");
        printf("你被击败了……\n");
        int penalty = player.gold / 10;
        player.gold -= penalty;
        printf("损失 %d 金币。\n", penalty);
        player.hp = getMaxHp();
        player.mp = player.maxMp;
        Sleep(2000);
        return false;
    }

    system("cls");
    printf("击败 %s！获得 %d 经验，%d 金币\n",
           ctx.mName.c_str(), mon.exp, mon.gold);
    gainExp(mon.exp);
    player.gold += mon.gold;

    for (auto& l : mon.loot) {
        if (rand() % 100 < l.second) {
            addBag(l.first);
            printf("获得物品: %s\n", safeItemName(l.first).c_str());
        }
    }

    // F7: 击杀计数只在这里加一次
    player.killCount++;
    player.goldEarned += mon.gold;
    if (mon.isSpirit) player.spiritKillCount++;
    if (mon.name.find("Boss") != string::npos) player.bossKillCount++;
    checkAchievements();
    checkQuestProgress(mon.id);

    if (isBoss) {
        if (player.hp <= 5 && !achievements[ACH_LIMIT_REVERSAL].unlocked)
            unlockAchievement(ACH_LIMIT_REVERSAL);
        if (!player.usedPotionInBattle && !achievements[ACH_PERSISTENT].unlocked)
            unlockAchievement(ACH_PERSISTENT);
        bool noArmor = (player.armor == 0
                        && player.clothing == 0
                        && player.accessory == 0);
        if (player.weapon == 1 && noArmor
            && !achievements[ACH_ARROGANT].unlocked)
            unlockAchievement(ACH_ARROGANT);
    }

    Sleep(1500);
    mx = my = 0;
    return true;
}