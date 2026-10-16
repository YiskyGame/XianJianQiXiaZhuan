#include "ui/ui.h"
#include "core/globals.h"
#include "core/utils.h"
#include "data/items.h"
#include "data/achievements.h"
#include "entity/player.h"
#include "item/bag.h"
#include <random>

// ============================================================
// 竞拍者
// ============================================================
struct AuctionBidder {
    string name;
    int maxBid, currentBid, gold;
    bool isPlayer;
    int bidIncrement;
};
static vector<AuctionBidder> bidders;
static int auctionWinner = -1;
static int auctionFinalPrice = 0;

// ============================================================
// 初始化竞拍者
// ============================================================
static void initAuctionBidders() {
    bidders.clear();
    vector<pair<string, int>> npcData = {
        {"青云剑宗·李长老", 8000 + rand() % 5000},
        {"天机阁·赵掌柜",   6000 + rand() % 4000},
        {"万宝楼·钱庄主",  10000 + rand() % 8000},
        {"暗影殿·影杀",     5000 + rand() % 3000},
        {"仙灵岛·白仙子",  12000 + rand() % 5000},
        {"龙族·敖烈",      15000 + rand() % 8000},
        {"魔界·暗影王",     7000 + rand() % 4000},
        {"散修·云游子",     3000 + rand() % 2000},
    };
    bidders.push_back({player.name, player.gold, 0, player.gold, true, 100});
    for (auto& npc : npcData)
        bidders.push_back({npc.first, npc.second, 0, npc.second, false,
                           50 + rand() % 150});
    random_device rd;
    mt19937 gen(rd());
    shuffle(bidders.begin() + 1, bidders.end(), gen);
}

// ============================================================
// 随机拍品
// ============================================================
static int selectAuctionItem() {
    vector<int> items = {801, 802, 803, 804, 805, 806, 807, 808, 809, 810,
                         811, 812, 813, 814, 815, 816, 817, 818, 819, 820};
    return items[rand() % items.size()];
}

// ============================================================
// NPC 出价
// ============================================================
static bool npcBid(int bidderIdx, int currentPrice, int itemId) {
    AuctionBidder& bidder = bidders[bidderIdx];
    if (bidder.isPlayer) return false;
    if (currentPrice >= bidder.maxBid) return false;
    int priceRatio = (bidder.maxBid > 0) ? (currentPrice * 100) / bidder.maxBid : 100;
    int interest = 50 + rand() % 50;
    if (itemId >= 801 && itemId <= 805) interest += 20;
    if (itemId >= 806 && itemId <= 810) interest += 15;
    if (priceRatio > 80) interest -= 30;
    else if (priceRatio > 60) interest -= 15;
    else if (priceRatio > 40) interest -= 5;
    if (rand() % 100 < interest) {
        int increment = bidder.bidIncrement + rand() % 200;
        int newBid = min(currentPrice + increment, bidder.maxBid);
        if (newBid > currentPrice) {
            bidder.currentBid = newBid;
            return true;
        }
    }
    return false;
}

// ============================================================
// 一轮拍卖
// ============================================================
static void doAuctionRound(int itemId) {
    system("cls");
    setColor(14);
    cout << "==============================\n";
    cout << "          拍 卖 大 会\n";
    cout << "==============================\n\n";
    resetColor();
    string itemName = safeItemName(itemId);
    int basePrice = 1000 + rand() % 4000;

    cout << "【今日拍品】" << itemName << "\n";
    cout << "  起拍价：" << basePrice << " 金币\n\n";

    int currentPrice = basePrice;
    int lastBidder = -1;
    int consecutivePass = 0;
    bool playerStillActive = true;
    vector<bool> activeBidders(bidders.size(), true);

    while (consecutivePass < 3) {
        system("cls");
        cout << "【拍品】" << itemName
             << "  当前价格：" << currentPrice << " 金币\n\n";

        vector<int> activeList;
        for (size_t i = 0; i < bidders.size(); i++)
            if (activeBidders[i] && bidders[i].maxBid >= currentPrice + 100
                && !bidders[i].isPlayer)
                activeList.push_back(i);

        bool playerBid = false;
        if (activeBidders[0] && bidders[0].maxBid >= currentPrice + 100
            && playerStillActive) {
            cout << "主持人：当前 " << currentPrice
                 << " 金币，要加价吗？\n";
            cout << "你的预算：" << bidders[0].maxBid << "\n";
            cout << "1. 出价  2. 放弃：";
            char choice = read();
            if (choice == '1') {
                int inc = 100 + rand() % 300;
                int newBid = currentPrice + inc;
                if (newBid <= bidders[0].maxBid) {
                    currentPrice = newBid;
                    playerBid = true;
                    lastBidder = 0;
                    consecutivePass = 0;
                    cout << "你出价 " << currentPrice << " 金币！\n";
                    Sleep(500);
                } else {
                    cout << "预算不足！\n";
                    Sleep(500);
                }
            } else {
                playerStillActive = false;
                activeBidders[0] = false;
            }
        }

        if (!playerBid && !activeList.empty()) {
            int idx = activeList[rand() % activeList.size()];
            if (npcBid(idx, currentPrice, itemId)) {
                currentPrice = bidders[idx].currentBid;
                lastBidder = idx;
                consecutivePass = 0;
                cout << bidders[idx].name << " 出价 "
                     << currentPrice << " 金币！\n";
                Sleep(400);
            } else {
                activeBidders[idx] = false;
                cout << bidders[idx].name << " 放弃了。\n";
                Sleep(300);
            }
        } else if (!playerBid) {
            consecutivePass++;
            cout << "无人加价... (" << consecutivePass << "/3)\n";
            Sleep(500);
        }
    }

    system("cls");
    if (lastBidder >= 0) {
        auctionWinner = lastBidder;
        auctionFinalPrice = currentPrice;
        cout << "成交价：" << auctionFinalPrice << " 金币\n";
        cout << "得主：" << bidders[auctionWinner].name << "\n";
        if (bidders[auctionWinner].isPlayer) {
            if (player.gold >= auctionFinalPrice) {
                player.gold -= auctionFinalPrice;
                addBag(itemId);
                player.totalSpent += auctionFinalPrice;
                cout << "恭喜你拍得 " << itemName << "！\n";
                checkAchievements();
            } else {
                cout << "金币不足！拍卖无效。\n";
            }
        } else {
            cout << "很遗憾，你没有拍得此物。\n";
        }
    } else {
        cout << "流拍。\n";
    }
    cout << "\n按任意键返回...";
    read();
}

// ============================================================
// 拍卖行入口
// ============================================================
void auctionHouse() {
    if (player.gold < 1000) {
        system("cls");
        printf("你的金币不足1000，无法参加拍卖。\n");
        Sleep(1500);
        return;
    }
    system("cls");
    setColor(14);
    cout << "===== 拍 卖 行 =====\n\n";
    resetColor();
    cout << "[1] 参加拍卖\n[2] 查看规则\n[0] 离开\n请选择：";
    char choice = read();
    if (choice == '1') {
        initAuctionBidders();
        int itemId = selectAuctionItem();
        doAuctionRound(itemId);
        auctionWinner = -1;
        auctionFinalPrice = 0;
    } else if (choice == '2') {
        system("cls");
        printf("===== 拍卖规则 =====\n");
        printf("1. 随机出现一件稀世珍宝\n");
        printf("2. 所有竞拍者按预算出价\n");
        printf("3. 每次至少加价 100 金币\n");
        printf("4. 连续3次无人加价，拍卖结束\n");
        printf("5. 出价最高者获得\n\n");
        system("pause");
    }
}