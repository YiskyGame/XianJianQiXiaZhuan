#include "ui/ui.h"
#include "core/globals.h"
#include "core/utils.h"
#include "data/items.h"
#include "entity/player.h"
#include "entity/stats.h"
#include "item/bag.h"

// ============================================================
// 背包
// ============================================================
void bagUI() {
    while (1) {
        player.bag.erase(remove_if(player.bag.begin(), player.bag.end(),
            [](pair<int, int>& p) {
                return p.first <= 0
                    || p.first >= (int)itemDB.size()
                    || itemDB[p.first].id != p.first;
            }), player.bag.end());
        if (player.hp > getMaxHp()) player.hp = getMaxHp();
        if (player.mp > player.maxMp) player.mp = player.maxMp;

        system("cls");
        printf("===== 背包 (%d 件) =====\n", (int)player.bag.size());
        for (size_t i = 0; i < player.bag.size(); i++) {
            int id = player.bag[i].first;
            if (id <= 0 || id >= (int)itemDB.size()
                || itemDB[id].id != id) continue;
            string eff = getItemEffectDesc(id);
            string typeIcon;
            switch (itemDB[id].type) {
                case WEAPON:    typeIcon = "[武]"; break;
                case ARMOR:     typeIcon = "[防]"; break;
                case CLOTHING:  typeIcon = "[衣]"; break;
                case ACCESSORY: typeIcon = "[饰]"; break;
                case POTION:    typeIcon = "[丹]"; break;
                case THROWING:  typeIcon = "[暗]"; break;
                case MATERIAL:  typeIcon = "[材]"; break;
                case QUEST:     typeIcon = "[任]"; break;
                default:        typeIcon = "[ ]"; break;
            }
            string equipped = "";
            if (id == player.weapon || id == player.armor
                || id == player.clothing || id == player.accessory)
                equipped = " ?装备中";
            printf("%2d. %s %s x%d%s\n", (int)i + 1, typeIcon.c_str(),
                   safeItemName(id).c_str(),
                   player.bag[i].second, equipped.c_str());
            if (!eff.empty()) printf("    └ %s\n", eff.c_str());
        }
        printf("\n输入编号使用/装备，[D]丢弃，[C]关闭\n");
        string input;
        cin >> input;
        if (input == "C" || input == "c") break;
        if (input == "D" || input == "d") {
            printf("输入要丢弃的编号：");
            cin >> input;
            int idx = atoi(input.c_str()) - 1;
            if (idx >= 0 && idx < (int)player.bag.size()) {
                int id = player.bag[idx].first;
                if (id <= 0 || id >= (int)itemDB.size()
                    || itemDB[id].id != id) {
                    printf("该物品无效，自动清理。\n");
                    player.bag.erase(player.bag.begin() + idx);
                } else {
                    printf("丢弃 %s ？(y/n)", safeItemName(id).c_str());
                    char ans = read();
                    if (ans == 'y' || ans == 'Y') {
                        removeBag(id, 1);
                        printf("已丢弃\n");
                    }
                }
            }
            Sleep(400);
            continue;
        }
        int idx = atoi(input.c_str()) - 1;
        if (idx >= 0 && idx < (int)player.bag.size()) {
            int id = player.bag[idx].first;
            if (id <= 0 || id >= (int)itemDB.size()
                || itemDB[id].id != id) {
                printf("物品数据错误，已自动移除。\n");
                player.bag.erase(player.bag.begin() + idx);
                Sleep(500);
                continue;
            }
            Item& it = itemDB[id];
            if (it.type == WEAPON) {
                if (player.weapon) addBag(player.weapon);
                player.weapon = id;
                removeBag(id);
                printf("已装备武器：%s (%s)\n",
                       safeItemName(id).c_str(),
                       getItemEffectDesc(id).c_str());
                Sleep(500);
            } else if (it.type == ARMOR) {
                if (player.armor) addBag(player.armor);
                player.armor = id;
                removeBag(id);
                printf("已装备防具：%s (%s)\n",
                       safeItemName(id).c_str(),
                       getItemEffectDesc(id).c_str());
                Sleep(500);
            } else if (it.type == CLOTHING) {
                if (player.clothing) addBag(player.clothing);
                player.clothing = id;
                removeBag(id);
                printf("已装备服装：%s (%s)\n",
                       safeItemName(id).c_str(),
                       getItemEffectDesc(id).c_str());
                Sleep(500);
            } else if (it.type == ACCESSORY) {
                if (player.accessory) addBag(player.accessory);
                player.accessory = id;
                removeBag(id);
                printf("已装备饰品：%s (%s)\n",
                       safeItemName(id).c_str(),
                       getItemEffectDesc(id).c_str());
                Sleep(500);
            } else if (it.type == POTION) {
                removeBag(id);
                usePotion(id, false);
                Sleep(500);
            } else {
                printf("该物品无法直接使用。\n");
                Sleep(500);
            }
        } else {
            printf("无效编号。\n");
            Sleep(400);
        }
    }
}