#include "item/bag.h"
#include "entity/player.h"
#include "core/globals.h"
#include "data/items.h"

// ============================================================
// 加入背包
// ============================================================
void addBag(int id, int cnt) {
    if (id <= 0) return;
    if ((size_t)id >= itemDB.size()) return;
    if (itemDB[id].id != id) return;
    if (cnt <= 0) return;

    int currentCount = 0;
    for (auto& p : player.bag) currentCount += p.second;
    if (currentCount + cnt > MAX_BAG) {
        printf("背包已满！无法添加更多物品。\n");
        Sleep(500);
        return;
    }

    for (auto &p : player.bag) {
        if (p.first == id) {
            p.second += cnt;
            return;
        }
    }
    player.bag.push_back({id, cnt});
}

// ============================================================
// 移除背包
// ============================================================
bool removeBag(int id, int cnt) {
    if (id <= 0) return false;
    if ((size_t)id >= itemDB.size()) return false;
    if (itemDB[id].id != id) return false;
    if (cnt <= 0) return false;
    for (auto it = player.bag.begin(); it != player.bag.end(); ++it) {
        if (it->first == id) {
            if (it->second >= cnt) {
                it->second -= cnt;
                if (it->second == 0) player.bag.erase(it);
                return true;
            }
            return false;
        }
    }
    return false;
}

// ============================================================
// 统计背包数量
// ============================================================
int countBag(int id) {
    if (id <= 0) return 0;
    if ((size_t)id >= itemDB.size()) return 0;
    if (itemDB[id].id != id) return 0;
    for (auto &p : player.bag)
        if (p.first == id) return p.second;
    return 0;
}