#include "map/map.h"
#include "map/map_coords.h"
#include "core/globals.h"
#include "core/utils.h"
#include "data/items.h"
#include "data/monsters.h"
#include "data/achievements.h"
#include "data/quests.h"
#include "entity/player.h"
#include "entity/stats.h"
#include "item/bag.h"
#include "battle/battle.h"
#include "event/random_event.h"
#include "ui/ui.h"

// ============================================================
// 统一通关判定
// ============================================================
void tryExitMap(bool& exited) {
    exited = false;
    if (currentMap.px != currentMap.ex || currentMap.py != currentMap.ey) return;
    if (currentMap.exitLocked) return;

    printf("通过 %s！\n", currentMap.name.c_str());
    Sleep(1500);
    player.mapsCleared++;

    if (currentMap.name != mapChineseNames[1]) {
        if (!player.usedPotionInCurrentMap
            && !achievements[ACH_INVINCIBLE].unlocked)
            unlockAchievement(ACH_INVINCIBLE);
    }
    checkAchievements();
    exited = true;
}

// ============================================================
// 探索
// ============================================================
void explore() {
    player.usedPotionInCurrentMap = false;
    while (1) {
        system("cls");
        printf("地图：%s  生命：%d/%d  金币：%d\n",
               currentMap.name.c_str(), player.hp, getMaxHp(), player.gold);
        drawMap();
        printf("[W]上 [S]下 [A]左 [D]右  [E]背包\n");
        char cmd = read();
        if (cmd == 'e' || cmd == 'E') {
            bagUI();
            continue;
        }

        int dir = -1;
        if (cmd == 'w' || cmd == 'W') dir = 0;
        else if (cmd == 's' || cmd == 'S') dir = 1;
        else if (cmd == 'a' || cmd == 'A') dir = 2;
        else if (cmd == 'd' || cmd == 'D') dir = 3;

        if (dir != -1) {
            int nx = currentMap.px + dx[dir];
            int ny = currentMap.py + dy[dir];
            int h = (int)currentMap.data.size();

            // 边界检查
            if (nx < 0 || nx >= h) continue;
            if (ny < 0 || ny >= (int)currentMap.data[nx].size()) continue;

            char tile = currentMap.data[nx][ny];
            if (tile == '*' || tile == '#' || tile == '~') continue;

            // 陷阱/熔岩
            if (tile == 'X' || tile == 'F') {
                player.hp = max(1, player.hp - 10);
                printf("你踩到了%s！扣除 10 点生命。\n",
                       tile == 'X' ? "陷阱" : "熔岩");
                Sleep(500);
                if (player.hp <= 0) {
                    printf("你因伤重而倒下……\n");
                    player.trapDeathCount++;
                    if (!achievements[ACH_SLIP].unlocked)
                        unlockAchievement(ACH_SLIP);
                    if (!achievements[ACH_UNDYING].unlocked)
                        unlockAchievement(ACH_UNDYING);
                    Sleep(1000);
                    player.hp = getMaxHp();
                    player.mp = player.maxMp;
                    return;
                }
            }

            // 宝箱
            bool boxOpened = false;
            for (auto& b : currentMap.boxes) {
                if (!b.opened && b.x == nx && b.y == ny) {
                    b.opened = true;
                    printf("打开宝箱！获得：\n");
                    for (int itemId : b.items) {
                        addBag(itemId);
                        printf("  %s\n", safeItemName(itemId).c_str());
                        if (itemId >= 705 && itemId <= 707) {
                            player.treasurePieces++;
                            if (player.treasurePieces >= 3) {
                                addBag(708);
                                printf("三块藏宝图碎片自动合成了完整的藏宝图！\n");
                                player.treasurePieces = 0;
                                if (!achievements[ACH_TREASURE_HUNTER].unlocked)
                                    unlockAchievement(ACH_TREASURE_HUNTER);
                            }
                        }
                    }
                    Sleep(1000);
                    boxOpened = true;
                    break;
                }
            }
            if (boxOpened) {
                currentMap.px = nx;
                currentMap.py = ny;
                if (currentMap.exitLocked && isMapClear()) {
                    currentMap.exitLocked = false;
                    printf("所有怪物已消灭，出口解锁！\n");
                    Sleep(800);
                }
                bool exited = false;
                tryExitMap(exited);
                if (exited) return;
                if (rand() % 100 < 15) randomEventExtended();
                continue;
            }

            // NPC
            bool npcFound = false;
            for (auto& npc : currentMap.npcs) {
                if (npc.x == nx && npc.y == ny) {
                    npcFound = true;
                    system("cls");
                    printf("你遇到了 %s。\n", npc.name.c_str());
                    Sleep(300);

                    // 铁匠
                    if (npc.name == "铁匠") {
                        bool allDead = true;
                        for (auto& m : currentMap.mobs)
                            if (m.alive) {
                                allDead = false;
                                break;
                            }
                        if (!allDead) {
                            printf("铁匠：'年轻人，我被困在这里太久了！请帮我消灭周围的冰霜怪物！'\n");
                            Sleep(1000);
                            break;
                        }
                        printf("铁匠：'谢谢你！我终于自由了！'\n");
                        Sleep(500);

                        Quest& q = quests[0];
                        if (q.taken && !q.completed) {
                            if (countBag(711) == 0) {
                                addBag(711, 1);
                                player.gold += 200;
                                player.goldEarned += 200;
                                printf("铁匠：'这是村长年轻时用过的佩剑，你替我还给他吧。'\n");
                                printf("获得 [玄铁剑] x1！\n");
                                printf("获得 200 金币！\n");
                            } else {
                                printf("铁匠：'你已经有玄铁剑了，快回去找村长吧。'\n");
                            }
                        } else if (q.completed) {
                            printf("铁匠：'你已经把剑还给村长了？那就好。'\n");
                        } else {
                            printf("铁匠：'你是新来的冒险者吧？'\n");
                            printf("铁匠：'你可以先去新手村找村长了解一下情况。'\n");
                        }
                        Sleep(500);
                        break;
                    }

                    // 村长
                    if (npc.name == "村长") {
                        for (auto& line : npc.dialogs) {
                            printf("%s：%s\n", npc.name.c_str(), line.c_str());
                            Sleep(800);
                        }
                        Quest& q = quests[0];
                        if (!q.taken) {
                            printf("接受任务 [%s] 吗？(y/n)\n", q.name.c_str());
                            char ans = read();
                            if (ans == 'y' || ans == 'Y') {
                                q.taken = true;
                                if (countBag(701) == 0) addBag(701, 1);
                                printf("任务已接受！获得 [村长手信] x1。\n");
                            }
                        } else if (!q.completed) {
                            if (countBag(711) >= 1) {
                                removeBag(711, 1);
                                q.completed = true;
                                player.gold += q.rewardGold;
                                player.goldEarned += q.rewardGold;
                                if (q.rewardItemId > 0) addBag(q.rewardItemId);
                                printf("村长：'这是当年我与铁匠并肩作战时的佩剑啊！'\n");
                                printf("任务完成！获得 %d 金币和 %s。\n",
                                       q.rewardGold,
                                       safeItemName(q.rewardItemId).c_str());
                                if (!achievements[ACH_BLACKSMITH_HEIR].unlocked)
                                    unlockAchievement(ACH_BLACKSMITH_HEIR);
                            } else {
                                printf("村长：'你找到铁匠了吗？他应该还在冰霜雪原。'\n");
                            }
                        } else {
                            printf("村长：'谢谢你完成了我交代的事。'\n");
                        }
                        Sleep(500);
                        break;
                    }

                    // 普通 NPC
                    for (auto& line : npc.dialogs) {
                        printf("%s：%s\n", npc.name.c_str(), line.c_str());
                        Sleep(800);
                    }
                    if (npc.questId != -1 && npc.questId <= 15) {
                        if (npc.name != "村长" && npc.name != "铁匠") {
                            Quest& q = quests[npc.questId - 1];
                            if (!q.taken) {
                                printf("接受任务 [%s] 吗？(y/n)\n", q.name.c_str());
                                char ans = read();
                                if (ans == 'y' || ans == 'Y') {
                                    q.taken = true;
                                    printf("任务已接受！\n");
                                }
                            } else if (!q.completed) {
                                bool can = false;
                                if (q.reqItemId > 0) {
                                    if (countBag(q.reqItemId) >= q.reqItemCount) {
                                        removeBag(q.reqItemId, q.reqItemCount);
                                        can = true;
                                    } else {
                                        printf("任务还需要 %d 个 %s。\n",
                                               q.reqItemCount - countBag(q.reqItemId),
                                               safeItemName(q.reqItemId).c_str());
                                    }
                                } else if (q.reqItemId == -1) {
                                    if (player.killCount > 0) can = true;
                                } else if (q.reqItemId == -2) {
                                    if (player.bossKillCount >= 1) can = true;
                                } else if (q.reqItemId == -3) {
                                    if (player.bossKillCount >= 2) can = true;
                                } else if (q.reqItemId == -4) {
                                    if (player.bossKillCount >= 3) can = true;
                                }
                                if (can) {
                                    q.completed = true;
                                    player.gold += q.rewardGold;
                                    player.goldEarned += q.rewardGold;
                                    if (q.rewardItemId > 0)
                                        addBag(q.rewardItemId);
                                    printf("任务完成！获得 %d 金币和 %s。\n",
                                           q.rewardGold,
                                           safeItemName(q.rewardItemId).c_str());
                                    checkAchievements();
                                }
                            } else {
                                printf("你已经完成了这个任务。\n");
                            }
                        }
                    }
                    Sleep(500);
                    break;
                }
            }

            if (npcFound) {
                currentMap.px = nx;
                currentMap.py = ny;
                if (currentMap.exitLocked && isMapClear()) {
                    currentMap.exitLocked = false;
                    printf("所有怪物已消灭，出口解锁！\n");
                    Sleep(800);
                }
                bool exited = false;
                tryExitMap(exited);
                if (exited) return;
                if (rand() % 100 < 15) randomEventExtended();
                continue;
            }

            // 怪物
            bool monsterFound = false, monsterDefeated = false;
            for (auto& m : currentMap.mobs) {
                if (!m.alive) continue;
                int dist = abs(m.x - nx) + abs(m.y - ny);
                if (dist <= 1) {
                    monsterFound = true;
                    currentMap.px = nx;
                    currentMap.py = ny;
                    printf("遭遇 %s！\n", monsterDB[m.type].name.c_str());
                    Sleep(500);
                    bool result = battle(monsterDB[m.type], m.x, m.y);
                    if (result) {
                        m.alive = false;
                        m.x = m.y = 0;
                        monsterDefeated = true;
                        printf("%s 已被击败！\n",
                               monsterDB[m.type].name.c_str());
                        Sleep(800);
                        if (currentMap.name == mapChineseNames[7] && isMapClear()) {
                            printf("\n=== 所有冰霜怪物已被消灭！===\n");
                            Sleep(500);
                            printf("远处传来铁匠的声音：'年轻人！你做到了！快过来！'\n");
                            Sleep(800);
                        }
                    } else {
                        printf("你战败了，返回主菜单...\n");
                        Sleep(1500);
                        if (!achievements[ACH_UNDYING].unlocked)
                            unlockAchievement(ACH_UNDYING);
                        return;
                    }
                    break;
                }
            }
            if (monsterFound) {
                if (monsterDefeated) {
                    if (currentMap.exitLocked && isMapClear()) {
                        currentMap.exitLocked = false;
                        printf("所有怪物已消灭，出口解锁！\n");
                        Sleep(800);
                    }
                    bool exited = false;
                    tryExitMap(exited);
                    if (exited) return;
                    if (rand() % 100 < 15) randomEventExtended();
                }
                continue;
            }

            // 普通移动
            currentMap.px = nx;
            currentMap.py = ny;
            if (currentMap.exitLocked && isMapClear()) {
                currentMap.exitLocked = false;
                printf("所有怪物已消灭，出口解锁！\n");
                Sleep(800);
            }
            bool exited = false;
            tryExitMap(exited);
            if (exited) return;
            if (rand() % 100 < 15) randomEventExtended();
        }
        for (auto& m : currentMap.mobs)
            if (m.alive)
                monsterMove(m.x, m.y, currentMap.px, currentMap.py, currentMap.data);
    }
}