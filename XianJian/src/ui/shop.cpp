#include "ui/ui.h"
#include "core/globals.h"
#include "core/utils.h"
#include "data/items.h"
#include "data/achievements.h"
#include "entity/player.h"
#include "item/bag.h"

// ============================================================
// 物品是否有效
// ============================================================
static bool isValidItem(int id) {
    if (id <= 0) return false;
    if ((size_t)id >= itemDB.size()) return false;
    if (itemDB[id].id != id) return false;
    if (itemDB[id].name.empty() || itemDB[id].name == "[空]"
        || itemDB[id].name == "[未知物品]") return false;
    if (itemDB[id].price <= 0
        && itemDB[id].type != QUEST
        && itemDB[id].type != SPECIAL) return false;
    return true;
}

// ============================================================
// 商店
// ============================================================
void shopCategory() {
    while (1) {
        system("cls");
        printf("===== 商店 =====\n");
        printf("[1] 武器\n[2] 防具\n[3] 服装\n[4] 丹药\n");
        printf("[5] 暗器\n[6] 饰品\n[7] 材料\n[0] 返回\n");
        char choice = read();
        if (choice == '0') break;

        vector<int> idList;
        switch (choice) {
            case '1': {
                int arr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
                             21, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
                             41, 42, 43, 44, 45};
                idList.assign(arr, arr + sizeof(arr) / sizeof(arr[0]));
                break;
            }
            case '2': {
                int arr[] = {101, 102, 103, 104, 105, 106, 107, 108, 109, 110,
                             111, 112, 113, 114, 115, 116, 117, 118, 119, 120};
                idList.assign(arr, arr + sizeof(arr) / sizeof(arr[0]));
                break;
            }
            case '3': {
                int arr[] = {201, 202, 203, 204, 205, 206, 207, 208, 209, 210,
                             211, 212, 213, 214, 215};
                idList.assign(arr, arr + sizeof(arr) / sizeof(arr[0]));
                break;
            }
            case '4': {
                int arr[] = {301, 302, 303, 304, 305, 306, 308, 309, 310, 311, 312, 313,
                             314, 315, 316, 317, 318, 319, 320, 321, 322, 323, 324, 325};
                idList.assign(arr, arr + sizeof(arr) / sizeof(arr[0]));
                break;
            }
            case '5': {
                int arr[] = {501, 502, 503, 505, 506, 507, 510, 511, 512, 513, 514, 515};
                idList.assign(arr, arr + sizeof(arr) / sizeof(arr[0]));
                break;
            }
            case '6': {
                int arr[] = {601, 602, 603, 604, 605, 606, 607, 610, 611, 612, 614, 615,
                             616, 617, 618, 619, 620, 621, 622, 623, 624, 625};
                idList.assign(arr, arr + sizeof(arr) / sizeof(arr[0]));
                break;
            }
            case '7': {
                int arr[] = {401, 402, 403, 404, 405, 406, 407, 408, 409, 410, 411, 412, 413,
                             414, 415, 416, 417, 418, 419, 420, 421, 422, 423, 424, 425, 426,
                             427, 428, 429, 430};
                idList.assign(arr, arr + sizeof(arr) / sizeof(arr[0]));
                break;
            }
            default:
                continue;
        }

        vector<int> validList;
        for (int id : idList) {
            if (!isValidItem(id)) continue;
            if (itemDB[id].price <= 0) continue;
            if (itemDB[id].requiredLevel > player.level) continue;
            validList.push_back(id);
        }
        if (validList.empty()) {
            printf("当前分类无可用商品！\n");
            Sleep(1500);
            continue;
        }

        const char* catName = choice == '1' ? "武器"
                              : choice == '2' ? "防具"
                              : choice == '3' ? "服装"
                              : choice == '4' ? "丹药"
                              : choice == '5' ? "暗器"
                              : choice == '6' ? "饰品" : "材料";

        while (1) {
            system("cls");
            printf("===== 购买 %s ===== (金币:%d  等级:%d)\n",
                   catName, player.gold, player.level);
            for (size_t i = 0; i < validList.size(); i++) {
                int id = validList[i];
                string eff = getItemEffectDesc(id);
                printf("%2d. %-20s %5d金币", (int)i + 1,
                       itemDB[id].name.c_str(), itemDB[id].price);
                if (!eff.empty()) printf("  [%s]", eff.c_str());
                if (itemDB[id].requiredLevel > 0)
                    printf(" (需%d级)", itemDB[id].requiredLevel);
                printf("\n");
            }
            printf("\n[B]批量购买  [S]批量出售  [C]返回上一级\n");

            string input;
            cin >> input;

            // 批量购买
            if (input == "b" || input == "B") {
                printf("请输入物品编号：");
                cin >> input;
                int idxRaw = atoi(input.c_str()) - 1;
                if (idxRaw < 0 || (size_t)idxRaw >= validList.size()) {
                    printf("编号不存在！\n");
                    Sleep(500);
                    continue;
                }
                int id = validList[idxRaw];
                printf("请输入购买数量：");
                cin >> input;
                int amount = atoi(input.c_str());
                if (amount <= 0) {
                    printf("数量必须大于0！\n");
                    Sleep(500);
                    continue;
                }
                int totalCost = itemDB[id].price * amount;
                if (player.gold < totalCost) {
                    printf("金币不足！需要 %d，当前 %d\n",
                           totalCost, player.gold);
                    Sleep(500);
                    continue;
                }
                player.gold -= totalCost;
                addBag(id, amount);
                player.purchaseCount += amount;
                player.totalSpent += totalCost;
                printf("成功购买 %d 个 %s\n", amount,
                       itemDB[id].name.c_str());
                checkAchievements();
                Sleep(800);
                continue;
            }

            // 批量出售
            if (input == "s" || input == "S") {
                system("cls");
                printf("===== 当前分类批量出售 =====\n");
                vector<pair<int, int>> sellList;
                int showNo = 1;
                for (auto& bagItem : player.bag) {
                    int itemId = bagItem.first;
                    if (!isValidItem(itemId)) continue;
                    bool match = false;
                    for (int cid : validList)
                        if (itemId == cid) {
                            match = true;
                            break;
                        }
                    if (match && bagItem.second > 0) {
                        sellList.push_back({showNo, itemId});
                        printf("%d. %s 持有:%d\n", showNo,
                               itemDB[itemId].name.c_str(),
                               bagItem.second);
                        showNo++;
                    }
                }
                if (sellList.empty()) {
                    printf("背包无该分类可出售物品\n");
                    Sleep(1000);
                    continue;
                }
                printf("\n输入物品编号：");
                cin >> input;
                int sellNo = atoi(input.c_str());
                int targetId = -1;
                for (auto& p : sellList)
                    if (p.first == sellNo) {
                        targetId = p.second;
                        break;
                    }
                if (targetId == -1) {
                    printf("不存在该编号物品！\n");
                    Sleep(800);
                    continue;
                }
                int ownNum = countBag(targetId);
                if (ownNum <= 0) {
                    printf("该物品已不在背包中！\n");
                    Sleep(800);
                    continue;
                }
                printf("当前持有%d，请输入出售数量：", ownNum);
                cin >> input;
                int sellAmt = atoi(input.c_str());
                if (sellAmt <= 0 || sellAmt > ownNum) {
                    printf("数量无效！\n");
                    Sleep(600);
                    continue;
                }
                int earnGold = (itemDB[targetId].price / 2) * sellAmt;
                if (removeBag(targetId, sellAmt)) {
                    player.gold += earnGold;
                    printf("出售成功，获得%d金币\n", earnGold);
                }
                Sleep(1000);
                continue;
            }

            if (input == "c" || input == "C") break;

            // 单件购买
            int idxRaw = atoi(input.c_str()) - 1;
            if (idxRaw < 0 || (size_t)idxRaw >= validList.size()) {
                printf("无效物品编号！\n");
                Sleep(500);
                continue;
            }
            int id = validList[idxRaw];
            if (player.gold >= itemDB[id].price) {
                player.gold -= itemDB[id].price;
                addBag(id);
                player.purchaseCount++;
                player.totalSpent += itemDB[id].price;
                printf("购买成功！获得 %s\n", itemDB[id].name.c_str());
                checkAchievements();
            } else {
                printf("金币不足！\n");
            }
            Sleep(500);
        }
    }
}

void shop() {
    shopCategory();
}