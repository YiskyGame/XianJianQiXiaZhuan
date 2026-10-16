#include "ui/ui.h"
#include "core/globals.h"
#include "core/utils.h"
#include "data/items.h"
#include "data/quests.h"
#include "data/achievements.h"
#include "entity/player.h"
#include "item/bag.h"

// ============================================================
// 锻造
// ============================================================
void forgingUI() {
    while (1) {
        system("cls");
        printf("===== 锻造系统 =====\n");
        printf("[1] 查看可锻造物品\n");
        printf("[2] 锻造物品\n");
        printf("[3] 查看锻造配方\n");
        printf("[0] 返回\n");
        char choice = read();
        if (choice == '0') break;
        if (choice == '1') {
            system("cls");
            printf("===== 可锻造物品 =====\n");
            int count = 0;
            for (size_t i = 0; i < itemDB.size(); i++) {
                if (i > 0 && !itemDB[i].recipe.empty()) {
                    printf("%d. %s - %s\n", (int)i,
                           safeItemName(i).c_str(),
                           itemDB[i].desc.c_str());
                    count++;
                    if (count % 10 == 0) {
                        system("pause");
                        system("cls");
                        printf("===== 可锻造物品 =====\n");
                    }
                }
            }
            printf("共 %d 种可锻造物品。\n", count);
            system("pause");
            continue;
        }
        if (choice == '2') {
            system("cls");
            printf("===== 锻造 =====\n");
            printf("请输入要锻造的物品ID（输入0取消）：");
            int id;
            cin >> id;
            if (id <= 0) continue;
            if (id >= (int)itemDB.size() || itemDB[id].recipe.empty()) {
                printf("该物品无法锻造。\n");
                Sleep(500);
                continue;
            }
            Item& it = itemDB[id];
            printf("锻造 %s 需要以下材料：\n", safeItemName(id).c_str());
            for (auto& r : it.recipe)
                printf("  %s x%d (当前拥有: %d)\n",
                       safeItemName(r.first).c_str(),
                       r.second, countBag(r.first));
            printf("数量：");
            int amount;
            cin >> amount;
            if (amount <= 0) continue;
            bool enough = true;
            for (auto& r : it.recipe) {
                if (countBag(r.first) < r.second * amount) {
                    printf("材料不足！需要 %s x%d，当前只有 %d\n",
                           safeItemName(r.first).c_str(),
                           r.second * amount, countBag(r.first));
                    enough = false;
                }
            }
            if (enough) {
                for (auto& r : it.recipe)
                    removeBag(r.first, r.second * amount);
                addBag(id, amount);
                player.forgeCount += amount;
                printf("锻造成功！获得 %d 个 %s\n", amount,
                       safeItemName(id).c_str());
                checkAchievements();
                if (id == 105 && quests[7].taken && !quests[7].completed) {
                    if (countBag(105) >= 1) {
                        quests[7].completed = true;
                        player.gold += quests[7].rewardGold;
                        if (quests[7].rewardItemId > 0)
                            addBag(quests[7].rewardItemId);
                        printf("任务'装备升级'已完成！\n");
                    }
                }
            }
            Sleep(800);
            continue;
        }
        if (choice == '3') {
            system("cls");
            printf("===== 锻造配方 =====\n");
            printf("输入物品ID查看配方（输入0返回）：");
            int id;
            cin >> id;
            if (id <= 0) continue;
            if (id >= (int)itemDB.size() || itemDB[id].recipe.empty()) {
                printf("该物品没有锻造配方。\n");
                Sleep(500);
                continue;
            }
            printf("%s 的锻造配方：\n", safeItemName(id).c_str());
            for (auto& r : itemDB[id].recipe)
                printf("  %s x%d\n",
                       safeItemName(r.first).c_str(), r.second);
            system("pause");
        }
    }
}