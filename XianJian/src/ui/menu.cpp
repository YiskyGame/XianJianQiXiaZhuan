#include "ui/ui.h"
#include "core/globals.h"
#include "core/utils.h"
#include "data/items.h"
#include "data/classes.h"
#include "data/achievements.h"
#include "data/quests.h"
#include "entity/player.h"
#include "item/bag.h"
#include "save/save.h"
#include "settings/settings.h"
#include "map/map.h"

// ============================================================
// 选择职业
// ============================================================
void chooseClass() {
    system("cls");
    printf("===== 选择你的修道之路 =====\n");
    for (int i = 0; i < (int)classes.size(); i++) {
        printf("%d. %s (%s)\n", i + 1,
               classes[i].name.c_str(), classes[i].title.c_str());
        printf("   生命:%d 攻击:%d 防御:%d 暴击:%d%% 技能: ",
               classes[i].baseHp, classes[i].baseAtk,
               classes[i].baseDef, classes[i].baseCrit);
        for (int j = 0; j < (int)classes[i].skills.size(); j++)
            printf("%s ", classes[i].skills[j].name.c_str());
        printf("\n   被动: %s - %s\n",
               classes[i].passiveName.c_str(),
               classes[i].passiveDesc.c_str());
    }
    char ch = read();
    int id = ch - '1';
    if (id < 0 || id > 7) id = 0;
    cout << "请输入你的名字: ";
    string name;
    cin >> name;
    initPlayer(id, name);
    printf("欢迎，%s %s！\n",
           classes[id].title.c_str(), name.c_str());
    Sleep(1500);
}

// ============================================================
// 标题 / 选档界面
// ============================================================
void titleScreen() {
    migrateLegacySave();

    while (1) {
        system("cls");
        setColor(14);
        cout << "      ____  _                     _          ____        _                 \n";
        cout << "     / ___|(_)  ____ _ _ __    __| | ___    / ___| _   _| |__   ___  _ __  \n";
        cout << "     \\___ \\| | / _` | '_ \\  / _` |/ _ \\   \\___ \\| | | | '_ \\ / _ \\| '_ \\ \n";
        cout << "      ___) | || (_| | | | || (_| |  __/    ___) | |_| | |_) | (_) | | | |\n";
        cout << "     |____/|_| \\__,_|_| |_| \\__,_|\\___|   |____/ \\__,_|_.__/ \\___/|_| |_|\n";
        setColor(11);
        cout << "                       《 鲜 贱 骑 虾 转 》\n";
        setColor(10);
        cout << "                      重置版 ver 1.0 测试版一\n";
        setColor(15);
        cout << "            作者：Liuhang, guojunyan2031, 2030chenxilin\n";
        cout << "      ====================================================\n\n";
        setColor(7);

        printf("===== 选择存档槽位 =====\n\n");
        for (int i = 1; i <= MAX_SLOTS; i++) showSlotInfo(i);
        printf("\n");
        printf("  [1~%d] 选择槽位\n", MAX_SLOTS);
        printf("  [N] 新建游戏\n");
        printf("  [D] 删除存档\n");
        printf("  [0] 退出\n");
        printf("请选择: ");

        int choice = read();
        if (choice == '0') {
            exit(0);
        } else if (choice == 'N' || choice == 'n') {
            int slot = chooseSlot(1);
            if (slot == 0) continue;
            remove(getSaveSlotPath(slot).c_str());
            currentSlot = slot;
            needLoadSave = false;
            player = Player();
            return;
        } else if (choice == 'D' || choice == 'd') {
            printf("要删除哪个槽位？(1~%d, 0取消): ", MAX_SLOTS);
            int slot = read() - '0';
            if (slot >= 1 && slot <= MAX_SLOTS) {
                SaveMeta meta = readSaveMeta(slot);
                if (!meta.exists) {
                    printf("该槽位为空。\n");
                } else {
                    printf("确认删除 存档%d（%s Lv.%d）？(y/n) ",
                           slot, meta.name.c_str(), meta.level);
                    char ans = read();
                    if (ans == 'y' || ans == 'Y') {
                        remove(getSaveSlotPath(slot).c_str());
                        printf("已删除。\n");
                    }
                }
                Sleep(800);
            }
        } else if (choice >= '1' && choice <= '0' + MAX_SLOTS) {
            int slot = choice - '0';
            SaveMeta meta = readSaveMeta(slot);
            if (!meta.exists) {
                currentSlot = slot;
                needLoadSave = false;
                player = Player();
                return;
            } else {
                currentSlot = slot;
                needLoadSave = true;
                return;
            }
        }
    }
}

// ============================================================
// 主菜单
// ============================================================
void mainMenu() {
    while (1) {
        system("cls");
        printf("===== 鲜贱骑虾转 =====\n");
        printf("当前已通关最大关卡：%d\n", currentLevel);
        printf("[1] 游玩关卡\n");
        printf("[2] 商店\n");
        printf("[3] 锻造\n");
        printf("[4] 角色状态\n");
        printf("[5] 背包\n");
        printf("[6] 存档/读档\n");
        printf("[7] 成就\n");
        printf("[8] 任务\n");
        printf("[9] 更新内容\n");
        printf("[A] 拍卖行\n");
        printf("[B] 物品查询\n");
        printf("[0] 退出\n");
        char ch = read();

        if (ch == '1') {
            printf("请输入要进入的关卡编号（1~%d，0退出）: ", currentLevel);
            int mapId;
            cin >> mapId;
            if (mapId >= 1 && mapId <= currentLevel) {
                loadMap(mapId);
                explore();
                if (isMapClear() && !currentMap.exitLocked) {
                    if (mapId == currentLevel && currentLevel < MAX_MAPS) {
                        currentLevel++;
                        player.dungeonLevel = currentLevel;
                    }
                }
            } else if (mapId != 0) {
                printf("无效关卡！\n");
                Sleep(800);
            }
        } else if (ch == '2') {
            shop();
        } else if (ch == '3') {
            forgingUI();
        } else if (ch == '4') {
            status();
        } else if (ch == '5') {
            bagUI();
        } else if (ch == '6') {
            system("cls");
            printf("===== 存档/读档 =====\n");
            printf("当前槽位：存档%d\n\n", currentSlot);
            printf("[1] 保存到当前槽位\n");
            printf("[2] 另存为其他槽位\n");
            printf("[3] 读取存档\n");
            printf("[0] 返回\n");
            char subCh = read();
            if (subCh == '1') {
                saveGame(currentSlot);
            } else if (subCh == '2') {
                int slot = chooseSlot(1);
                if (slot != 0) {
                    saveGame(slot);
                    currentSlot = slot;
                }
            } else if (subCh == '3') {
                int slot = chooseSlot(0);
                if (slot != 0) loadGame(slot);
            }
        } else if (ch == '7') {
            showAchievements();
        } else if (ch == '8') {
            showQuests();
        } else if (ch == '9') {
            int r = showUpdateLog();
            if (r == 1) {
                g_settings.skipUpdateLog = true;
                saveSettings(g_settings);
                printf("\n已永久关闭更新内容显示。\n");
                Sleep(1000);
            } else if (r == 0) {
                printf("\n是否要重新开启启动时显示更新内容？(y/n) ");
                char ans = read();
                if (ans == 'y' || ans == 'Y') {
                    g_settings.skipUpdateLog = false;
                    saveSettings(g_settings);
                    printf("已重新开启。下次启动将显示更新内容。\n");
                    Sleep(1000);
                }
            }
        } else if (ch == 'A' || ch == 'a') {
            auctionHouse();
        } else if (ch == 'B' || ch == 'b') {
            itemQueryUI();
        } else if (ch == '0') {
            exit(0);
        }
    }
}
