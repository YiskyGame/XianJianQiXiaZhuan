#include "event/random_event.h"
#include "core/globals.h"
#include "core/utils.h"
#include "data/items.h"
#include "data/achievements.h"
#include "entity/player.h"
#include "entity/stats.h"
#include "item/bag.h"

// ============================================================
// 随机事件
// ============================================================
void randomEventExtended() {
    int r = rand() % 20;
    if (r == 0) {
        system("cls");
        printf("一位神秘商人出现在你面前，准备用材料换取装备。\n");
        Sleep(800);
        struct TradeItem {
            int itemId;
            vector<pair<int, int>> cost;
        };
        vector<TradeItem> tradeList = {
            {3,   {{401, 3}, {404, 2}}},
            {103, {{401, 2}, {403, 2}}},
            {303, {{402, 2}, {404, 3}}},
            {105, {{401, 4}, {403, 3}, {404, 2}}},
            {302, {{402, 3}, {401, 2}}},
            {15,  {{401, 5}, {403, 4}, {404, 4}}},
            {201, {{401, 3}, {403, 1}}},
            {202, {{401, 2}, {406, 1}}},
            {203, {{401, 4}, {404, 3}}},
            {204, {{401, 3}, {406, 2}}},
            {205, {{401, 6}, {407, 2}}},
            {206, {{401, 8}, {410, 1}}},
            {41,  {{408, 3}, {421, 2}}},
            {42,  {{413, 3}, {416, 2}}},
            {43,  {{414, 3}, {405, 2}}},
            {44,  {{416, 3}, {407, 2}}}
        };
        vector<TradeItem> validTrade;
        for (auto& t : tradeList) {
            int tid = t.itemId;
            if (tid <= 0 || (size_t)tid >= itemDB.size()) continue;
            if (itemDB[tid].id != tid) continue;
            if ((tid >= 801 && tid <= 820) || (tid >= 701 && tid <= 712)) continue;
            if (itemDB[tid].recipe.empty()) continue;
            validTrade.push_back(t);
        }
        if (validTrade.empty()) {
            printf("商人无可交易物品。\n");
            Sleep(1000);
            return;
        }
        TradeItem trade = validTrade[rand() % validTrade.size()];
        printf("他想用 [%s] 交换以下材料：\n", safeItemName(trade.itemId).c_str());
        for (auto& c : trade.cost)
            printf("  %s x%d (你拥有: %d)\n",
                   safeItemName(c.first).c_str(), c.second,
                   countBag(c.first));
        printf("\n你愿意交换吗？(y/n) ");
        char ans = read();
        if (ans == 'y' || ans == 'Y') {
            bool enough = true;
            for (auto& c : trade.cost)
                if (countBag(c.first) < c.second) {
                    enough = false;
                    break;
                }
            if (enough) {
                for (auto& c : trade.cost) removeBag(c.first, c.second);
                addBag(trade.itemId);
                printf("交换成功！你获得了 %s。\n",
                       safeItemName(trade.itemId).c_str());
            } else {
                printf("材料不足，交易失败。\n");
            }
        } else {
            printf("你拒绝了交易。\n");
        }
        Sleep(1000);
    } else if (r == 1) {
        printf("你发现了一个隐藏的宝箱！\n");
        addBag(rand() % 2 ? 408 : 409, 1);
        Sleep(500);
    } else if (r == 2) {
        printf("触发陷阱！损失10生命。\n");
        player.hp = max(1, player.hp - 10);
        Sleep(500);
    } else if (r == 3) {
        printf("你找到了一处疗伤温泉，生命恢复20点。\n");
        player.hp = min(getMaxHp(), player.hp + 20);
        Sleep(500);
    } else if (r == 4) {
        printf("一位流浪汉送给你一颗丹药。\n");
        addBag(301, 1);
        Sleep(500);
    } else if (r == 5) {
        printf("一群强盗袭击了你，你损失了20金币。\n");
        player.gold = max(0, player.gold - 20);
        Sleep(500);
    } else if (r == 6) {
        printf("你捡到了一块藏宝图碎片！\n");
        if (player.treasurePieces < 3) {
            int piece = 705 + player.treasurePieces;
            addBag(piece);
            player.treasurePieces++;
            if (player.treasurePieces >= 3) {
                addBag(708);
                printf("三块藏宝图碎片合成了完整的藏宝图！\n");
                if (!achievements[ACH_TREASURE_HUNTER].unlocked)
                    unlockAchievement(ACH_TREASURE_HUNTER);
            }
        }
        Sleep(500);
    } else if (r == 7) {
        printf("你遇到了一位云游仙人，他传授了你一些经验。\n");
        gainExp(50);
        Sleep(500);
    }
}