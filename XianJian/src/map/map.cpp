#include "map/map.h"
#include "map/map_coords.h"
#include "core/globals.h"
#include "core/utils.h"
#include "data/monsters.h"
#include "data/items.h"
#include "entity/player.h"
#include "item/bag.h"
#include "data/quests.h"

// ============================================================
// 全局地图对象定义
// ============================================================
GameMap currentMap;

// ============================================================
// 加载地图
// ============================================================
void loadMap(int id) {
    currentMap.boxes.clear();
    currentMap.mobs.clear();
    currentMap.npcs.clear();
    currentMap.exitLocked = true;

    // 统一默认值（会被 validateMapCoords 修正）
    currentMap.px = 2;
    currentMap.py = 3;
    currentMap.ex = 7;
    currentMap.ey = 68;
    currentMap.themeColor = 10;

    switch (id) {
        case 1:
            currentMap.name = mapChineseNames[1];
            currentMap.data = {
                "*******************************************************",
                "*                                                     *",
                "*                                                     *",
                "*                                                     *",
                "*                                                     *",
                "*                                                     *",
                "*                                                     *",
                "*                                                     *",
                "*******************************************************"
            };
            currentMap.boxes.push_back({3, 28, {2, 102, 301}, false});
            currentMap.mobs.push_back({0, 5, 25, 35, true});
            currentMap.npcs.push_back({3, 35, 'N', 14, "村长",
                {"年轻人，请帮我把信交给铁匠。", "铁匠就在村子东边。"}, 1});
            currentMap.npcs.push_back({6, 10, 'N', 10, "猎人",
                {"最近森林里的灵兽皮毛很值钱。", "给我5张皮毛，我会给你报酬。"}, 2});
            break;
        case 2:
            currentMap.name = mapChineseNames[2];
            currentMap.data = {
                "**********************************************************************",
                "*         ~~~                      ######                            *",
                "*         ~ ~                      #                                 *",
                "*         ~~~                           #                            *",
                "*                                  ######                            *",
                "*                                                                    *",
                "*                                                                    *",
                "*                                                                    *",
                "*                                                                    *",
                "**********************************************************************"
            };
            currentMap.boxes.push_back({4, 55, {3, 401}, false});
            currentMap.boxes.push_back({6, 25, {103, 402}, false});
            currentMap.mobs.push_back({1, 3, 35, 50, true});
            currentMap.mobs.push_back({1, 5, 60, 50, true});
            currentMap.mobs.push_back({10, 6, 45, 90, true});
            currentMap.npcs.push_back({4, 30, 'N', 11, "采药人",
                {"前面的洞穴里有珍贵的灵芝。", "帮我采3株，我会给你报酬。"}, 3});
            break;
        case 3:
            currentMap.name = mapChineseNames[3];
            currentMap.data = {
                "*****************************************************************",
                "*     ~~~~~~               ~~~~~~~~~                            *",
                "*                                                               *",
                "*     ~    ~~~~            ~       ~~~~                         *",
                "*     ~~~~~~               ~~~~~~~~  ~                          *",
                "*            ~~~~                            X                 *",
                "*                                                               *",
                "*                                                               *",
                "*                                                               *",
                "*****************************************************************"
            };
            currentMap.boxes.push_back({2, 40, {4, 403}, false});
            currentMap.boxes.push_back({5, 52, {104, 404}, false});
            currentMap.mobs.push_back({2, 3, 35, 70, true});
            currentMap.mobs.push_back({11, 5, 50, 80, true});
            currentMap.mobs.push_back({100, 7, 58, 180, true});
            currentMap.npcs.push_back({5, 10, 'N', 13, "探险家",
                {"这洞穴深处有强大的怪物。", "打败黑山老妖，我会奖赏你。"}, 4});
            currentMap.themeColor = 8;
            break;
        case 4:
            currentMap.name = mapChineseNames[4];
            currentMap.data = {
                "**********************************************************************",
                "*         ####          ~~~~                   ~~~~                  *",
                "*                       ~                         ~                  *",
                "*         ####             ~                   ~                     *",
                "*                       ~~~~                   ~~~~                  *",
                "*                                                       ~~~~~~~~~~  *",
                "*                                                                     *",
                "*                                                                     *",
                "*                                                                     *",
                "**********************************************************************"
            };
            currentMap.boxes.push_back({3, 60, {8, 408}, false});
            currentMap.boxes.push_back({5, 30, {12, 409}, false});
            currentMap.mobs.push_back({12, 4, 55, 110, true});
            currentMap.mobs.push_back({13, 6, 50, 100, true});
            currentMap.mobs.push_back({101, 7, 35, 220, true});
            currentMap.npcs.push_back({5, 25, 'N', 14, "考古学家",
                {"找到完整的藏宝图，你将获得巨大财富。", "收集三块藏宝图碎片可以合成完整的藏宝图。"}, 5});
            currentMap.themeColor = 14;
            break;
        case 5:
            currentMap.name = mapChineseNames[5];
            currentMap.data = {
                "**********************************************************************",
                "*  ~~~~~        ##              ~~~~~~~~~        ##                 *",
                "*               ##                               ##                 *",
                "*        ~~     ##      X               ~~~~     ##        F        *",
                "*  ~~~~~        ##              ~~~~~~~~         ##                 *",
                "*        ~~~~   ##                      ~~~~    ##                 *",
                "*                                                                     *",
                "*                                                                     *",
                "*                                                                     *",
                "**********************************************************************"
            };
            currentMap.boxes.push_back({3, 50, {14, 411}, false});
            currentMap.boxes.push_back({5, 25, {107, 412}, false});
            currentMap.mobs.push_back({14, 3, 45, 150, true});
            currentMap.mobs.push_back({102, 5, 60, 280, true});
            currentMap.mobs.push_back({100, 6, 65, 180, true});
            currentMap.npcs.push_back({2, 20, 'N', 12, "幽灵商人",
                {"帮我消灭暗影刺客，我将赐你宝物。"}, 9});
            currentMap.themeColor = 5;
            break;
        case 6:
            currentMap.name = mapChineseNames[6];
            currentMap.data = {
                "***************************************************************************",
                "*  ~~~~~        #######            ~~~~~            #######               *",
                "*      ~              #            ~                #                     *",
                "*  ~   ~~~~     #     #    X       ~   ~~~~         #     #  ~~~~         *",
                "*  ~~~~~  ~     #                  ~~~~~  ~               #  ~  ~         *",
                "*        ~~~~   #######                  ~~~~       #######  ~~~~         *",
                "*                                                                           *",
                "*                                                                           *",
                "*                                                                           *",
                "***************************************************************************"
            };
            currentMap.boxes.push_back({3, 65, {16, 415}, false});
            currentMap.boxes.push_back({4, 45, {110, 416}, false});
            currentMap.mobs.push_back({15, 3, 50, 180, true});
            currentMap.mobs.push_back({103, 5, 60, 350, true});
            currentMap.mobs.push_back({104, 6, 40, 320, true});
            currentMap.mobs.push_back({109, 7, 30, 600, true});
            currentMap.npcs.push_back({4, 30, 'N', 11, "流浪剑客",
                {"击败冰霜巨龙，证明你的实力！"}, 6});
            currentMap.themeColor = 12;
            break;
        case 7:
            currentMap.name = mapChineseNames[7];
            currentMap.data = {
                "**********************************************************************",
                "*  ~~~~~~~~~~~~~           XXXX            ~~~~~~~~~~~~~             *",
                "*  ~                                                    ~             *",
                "*      ~~~~~   ~                            ~   ~~~~~                 *",
                "*  ~       ~   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~   ~   ~   ~             *",
                "*  ~~~~~   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~   ~~~~~             *",
                "*                                                                     *",
                "*                                                                     *",
                "*                                                                     *",
                "**********************************************************************"
            };
            currentMap.boxes.push_back({3, 60, {31, 413}, false});
            currentMap.boxes.push_back({5, 40, {111, 402}, false});
            currentMap.mobs.push_back({5, 1, 15, 65, true});
            currentMap.mobs.push_back({5, 2, 25, 65, true});
            currentMap.mobs.push_back({5, 3, 35, 65, true});
            currentMap.mobs.push_back({12, 4, 45, 110, true});
            currentMap.mobs.push_back({17, 6, 55, 300, true});
            currentMap.npcs.push_back({3, 25, 'N', 13, "铁匠",
                {
                    "年轻人！快救救我！我被这群冰霜怪物困在这里好久了！",
                    "你身上有村长的信？太好了！快给我看看！",
                    "什么？村长他...还活着？太好了！",
                    "这把玄铁剑...这是村长的佩剑啊！当年我们并肩作战时...",
                    "作为报答，我用这把玄铁剑给你锻造一件神兵！"
                }, 1});
            currentMap.themeColor = 11;
            break;
        case 8:
            currentMap.name = mapChineseNames[8];
            currentMap.data = {
                "**********************************************************************",
                "*  FFFFFF        ~~~~~~~~~           FFFFFF        ~~~~~~~~~         *",
                "*  F    F        ~       ~                F        ~       ~         *",
                "*  F    FFFF     ~       ~~~~        F    FFFF     ~       ~~~~      *",
                "*                ~       ~  ~        F             ~       ~  ~      *",
                "*  FFFFFFFFFF F  ~~~~   ~~~~         FFFFFFFFFF F  ~~~~   ~~~~      *",
                "*                                                                     *",
                "*                                                                     *",
                "*                                                                     *",
                "**********************************************************************"
            };
            currentMap.boxes.push_back({2, 55, {25, 414}, false});
            currentMap.boxes.push_back({5, 30, {112, 402}, false});
            currentMap.mobs.push_back({4, 3, 45, 60, true});
            currentMap.mobs.push_back({11, 5, 50, 80, true});
            currentMap.mobs.push_back({102, 6, 40, 280, true});
            currentMap.npcs.push_back({3, 20, 'N', 12, "丹药师",
                {"炼成5颗大还丹，我教你新配方。"}, 7});
            currentMap.themeColor = 12;
            break;
        case 9:
            currentMap.name = mapChineseNames[9];
            currentMap.data = {
                "**********************************************************************",
                "*  ~~~~~~~~~~           ********           ~~~~~~~~~~                *",
                "*                       *      *                    ~                *",
                "*  ~  * **  ~~~  ~~~~  ~*      ~~~~~~~~~~~~  ****  ~                *",
                "*  ~  *  *  ~                      ~           *  ~                *",
                "*  ~~~~  ~~~~                      ~~~~~~~~~  ~~~~                *",
                "*                                                                     *",
                "*                                                                     *",
                "*                                                                     *",
                "**********************************************************************"
            };
            currentMap.boxes.push_back({2, 60, {17, 409}, false});
            currentMap.mobs.push_back({6, 3, 45, 70, true});
            currentMap.mobs.push_back({13, 5, 55, 100, true});
            currentMap.mobs.push_back({104, 6, 65, 320, true});
            currentMap.npcs.push_back({4, 50, 'N', 14, "雷神使者",
                {"击败雷神，证明你有资格获得雷神锤。"}, -1});
            currentMap.themeColor = 14;
            break;
        case 10:
            currentMap.name = mapChineseNames[10];
            currentMap.data = {
                "**********************************************************************",
                "*  ########           ~~~~~~~~~~           ########                  *",
                "*         #           ~        ~                  #                  *",
                "*  #  **  ###########   ~  ~  ~   ########  **  #                  *",
                "*  #  **               ~  ~  ~               **                     *",
                "*  ########           ~~~~~~~~~~           ########                  *",
                "*                                                                     *",
                "*                                                                     *",
                "*                                                                     *",
                "**********************************************************************"
            };
            currentMap.boxes.push_back({2, 55, {21, 419}, false});
            currentMap.boxes.push_back({4, 40, {117, 402}, false});
            currentMap.mobs.push_back({8, 3, 45, 55, true});
            currentMap.mobs.push_back({108, 5, 60, 350, true});
            currentMap.mobs.push_back({15, 6, 50, 180, true});
            currentMap.npcs.push_back({5, 30, 'N', 15, "圣殿骑士",
                {"净化暗影深渊，你将获得圣光祝福。"}, -1});
            currentMap.themeColor = 15;
            break;
        case 11:
            currentMap.name = mapChineseNames[11];
            currentMap.data = {
                "**********************************************************************",
                "*  ~~~~~~~~~           **********           ~~~~~~~~~                *",
                "*                      *        *                   ~                *",
                "*  ~  ****  ~~~~~  ~~~~   **   *~~~~~~~~~~  *  *  ~                *",
                "*  ~  *  *  ~                               *  *                   *",
                "*  ~~~~  ~~~~                       ~~~~  ~~~  ~~~~                *",
                "*                                                                     *",
                "*                                                                     *",
                "*                                                                     *",
                "**********************************************************************"
            };
            currentMap.boxes.push_back({2, 60, {19, 418}, false});
            currentMap.mobs.push_back({7, 3, 45, 75, true});
            currentMap.mobs.push_back({107, 5, 60, 380, true});
            currentMap.mobs.push_back({109, 6, 65, 600, true});
            currentMap.npcs.push_back({4, 35, 'N', 12, "暗影刺客",
                {"击败我，你才有资格面对最终Boss。"}, -1});
            currentMap.themeColor = 5;
            break;
        case 12:
            currentMap.name = mapChineseNames[12];
            currentMap.data = {
                "***************************************************************************",
                "*  ~~~~~~            **********            ~~~~~~                        *",
                "*  ~                 *                     ~    ~                        *",
                "*  ~    ~~~~~~~~~~~~~*   **   *~~~~~~~~~~~~~    ~~~~~~~~~                *",
                "*  ~                     **   *                         ~                *",
                "*  ~~~~~~            **********            ~~~~~~       ~~~~              *",
                "*                                                                          *",
                "*                                                                          *",
                "*                                                                          *",
                "***************************************************************************"
            };
            currentMap.boxes.push_back({3, 65, {16, 415}, false});
            currentMap.boxes.push_back({5, 50, {17, 409}, false});
            currentMap.mobs.push_back({15, 3, 50, 180, true});
            currentMap.mobs.push_back({18, 5, 65, 300, true});
            currentMap.mobs.push_back({109, 6, 40, 600, true});
            currentMap.npcs.push_back({4, 30, 'N', 11, "虚空旅者",
                {"找到混沌碎片，我将给你终极力量。"}, 10});
            currentMap.themeColor = 5;
            break;
        case 13:
            currentMap.name = mapChineseNames[13];
            currentMap.data = {
                "********************************************************************************",
                "*  ########            ~~~~~~~~~~            ########                          *",
                "*  #      #            ~        ~            #      #                          *",
                "*  #  **  ##############   ~~~~   #############  **  ##########               *",
                "*     **                      ~  ##                          ##               *",
                "*  ########                ~~~               ########          ######          *",
                "*                                                                              *",
                "*                                                                              *",
                "*                                                                              *",
                "********************************************************************************"
            };
            currentMap.boxes.push_back({2, 70, {21, 419}, false});
            currentMap.boxes.push_back({4, 50, {110, 415}, false});
            currentMap.mobs.push_back({108, 3, 60, 350, true});
            currentMap.mobs.push_back({19, 5, 70, 450, true});
            currentMap.mobs.push_back({109, 6, 65, 600, true});
            currentMap.npcs.push_back({3, 40, 'N', 14, "元素长老",
                {"击败所有Boss，混沌天魔才会现身。"}, -1});
            currentMap.themeColor = 14;
            break;
        case 14:
            currentMap.name = mapChineseNames[14];
            currentMap.data = {
                "********************************************************************************",
                "*  ~~~~~~~             ********             ~~~~~~~                           *",
                "*  ~     ~             *      *             ~     ~                           *",
                "*        ~~~~~~~~~~~~~~*        ~~~~~~~~~~~~~     ~~~~~~~~~~~~~               *",
                "*                         **  *                            ~                 *",
                "*  ~~~~~~~             ********             ~ ~~~~~        ~~~~               *",
                "*                                                                              *",
                "*                                                                              *",
                "*                                                                              *",
                "********************************************************************************"
            };
            currentMap.boxes.push_back({2, 70, {110, 420}, false});
            currentMap.boxes.push_back({5, 50, {110, 415}, false});
            currentMap.mobs.push_back({19, 3, 65, 450, true});
            currentMap.mobs.push_back({107, 5, 70, 380, true});
            currentMap.mobs.push_back({109, 6, 50, 600, true});
            currentMap.npcs.push_back({4, 55, 'N', 15, "时间守护者",
                {"最终决战即将到来，你准备好了吗？"}, -1});
            currentMap.themeColor = 11;
            break;
        case 15:
            currentMap.name = mapChineseNames[15];
            currentMap.data = {
                "*************************************************************************************",
                "*  ~~~~~~           **********           ~~~~~~           **********               *",
                "*                                                                                  *",
                "*       ~  ~~~~~~~~~    **    ~~~~~  ~~~~~    ~~~~~~~~~~~~*   **   *  ~~~~         *",
                "*                       **                                *   **                   *",
                "*  ~~~~~~           **********           ~~~~~~           **********  ~~~~         *",
                "*        ~~~~~~~~~~                                ~~~~~~~~~~                      *",
                "*                                                                                   *",
                "*                                                                                   *",
                "*                                                                                   *",
                "*************************************************************************************"
            };
            currentMap.boxes.push_back({3, 75, {29, 420}, false});
            currentMap.boxes.push_back({6, 60, {110, 415}, false});
            currentMap.mobs.push_back({19, 3, 65, 500, true});
            currentMap.mobs.push_back({108, 5, 75, 450, true});
            currentMap.mobs.push_back({109, 8, 60, 600, true});
            currentMap.mobs.push_back({15, 9, 40, 400, true});
            currentMap.npcs.push_back({6, 70, 'N', 13, "先知",
                {"击败混沌天魔，拯救世界！"}, -1});
            currentMap.themeColor = 12;
            break;
        case 16:
            currentMap.name = mapChineseNames[16];
            currentMap.data = {
                "*************************************************************************************",
                "*  ######            ######            ######            ######                 *",
                "*  #    #            #    #            #    #            #    #                 *",
                "*  #    #   ######   #    #   ######   #    #   ######   #    #                 *",
                "*  #    #            #    #            #    #            #    #                 *",
                "*  ######            ######            ######            ######                 *",
                "*                                                                                *",
                "*                                                                                *",
                "*                                                                                *",
                "*                                                                                *",
                "*************************************************************************************"
            };
            currentMap.boxes.push_back({3, 70, {41, 421, 422}, false});
            currentMap.boxes.push_back({5, 50, {118, 426}, false});
            currentMap.mobs.push_back({20, 3, 50, 200, true});
            currentMap.mobs.push_back({110, 5, 60, 380, true});
            currentMap.mobs.push_back({15, 7, 45, 180, true});
            currentMap.npcs.push_back({4, 35, 'N', 14, "龙族长老",
                {"击败龙骨守卫，证明你的实力！"}, 11});
            break;
        case 17:
            currentMap.name = mapChineseNames[17];
            currentMap.data = {
                "*************************************************************************************",
                "*  ~~~~~~~~~~           ~~~~~~~~~~           ~~~~~~~~~~                    *",
                "*  ~        ~           ~        ~           ~        ~                    *",
                "*  ~        ~~~~~~~~~~~ ~        ~~~~~~~~~~~ ~        ~~~~~~~~~~~        *",
                "*  ~                     ~                     ~                           *",
                "*  ~~~~~~~~~~           ~~~~~~~~~~           ~~~~~~~~~~                    *",
                "*                                                                          *",
                "*                                                                          *",
                "*                                                                          *",
                "*                                                                          *",
                "*************************************************************************************"
            };
            currentMap.boxes.push_back({3, 65, {42, 430}, false});
            currentMap.boxes.push_back({5, 45, {211, 430}, false});
            currentMap.mobs.push_back({23, 3, 55, 200, true});
            currentMap.mobs.push_back({112, 5, 65, 400, true});
            currentMap.mobs.push_back({15, 7, 50, 180, true});
            currentMap.npcs.push_back({4, 30, 'N', 15, "仙灵使者",
                {"寻找仙灵草，炼制仙丹。"}, 12});
            currentMap.themeColor = 15;
            break;
        case 18:
            currentMap.name = mapChineseNames[18];
            currentMap.data = {
                "*************************************************************************************",
                "*  ~~~~~~~            #######            ~~~~~~~                        *",
                "*  ~     ~            #     #            ~     ~                        *",
                "*  ~     ~~~~~~~~~~~~~#     #~~~~~~~~~~~~~     ~~~~~~~~~~~~~            *",
                "*  ~                   #     #                   ~                        *",
                "*  ~~~~~~~            #######            ~~~~~~~                        *",
                "*                                                                          *",
                "*                                                                          *",
                "*                                                                          *",
                "*                                                                          *",
                "*************************************************************************************"
            };
            currentMap.boxes.push_back({3, 70, {45, 429, 420}, false});
            currentMap.boxes.push_back({5, 50, {120, 429}, false});
            currentMap.mobs.push_back({21, 3, 55, 200, true});
            currentMap.mobs.push_back({22, 5, 60, 200, true});
            currentMap.mobs.push_back({111, 7, 50, 400, true});
            currentMap.npcs.push_back({4, 40, 'N', 5, "魔界使者",
                {"击败魔界领主，获取魔界之钥。"}, 13});
            currentMap.themeColor = 5;
            break;
        case 19:
            currentMap.name = mapChineseNames[19];
            currentMap.data = {
                "*************************************************************************************",
                "*  ~~~~~~           **********           ~~~~~~           **********      *",
                "*  ~                 *        *           ~                 *        *      *",
                "*  ~    ~~~~~~~~~~~~~*        ~~~~~~~~~~~~~    ~~~~~~~~~~~~~*        ~~~~~~*",
                "*  ~                 *                      ~                 *              *",
                "*  ~~~~~~           **********           ~~~~~~           **********      *",
                "*        ~~~~~~~~~~                                ~~~~~~~~~~              *",
                "*                                                                          *",
                "*                                                                          *",
                "*                                                                          *",
                "*************************************************************************************"
            };
            currentMap.boxes.push_back({3, 75, {40, 120, 420}, false});
            currentMap.boxes.push_back({6, 60, {45, 120}, false});
            currentMap.mobs.push_back({19, 3, 65, 500, true});
            currentMap.mobs.push_back({109, 5, 75, 600, true});
            currentMap.mobs.push_back({111, 7, 60, 500, true});
            currentMap.mobs.push_back({112, 8, 50, 450, true});
            currentMap.npcs.push_back({6, 70, 'N', 13, "最终先知",
                {"击败所有强敌，完成终极试炼！"}, 14});
            currentMap.themeColor = 13;
            break;
        default:
            currentMap.name = "未知区域";
            currentMap.data = {"*****", "*   *", "*****"};
            currentMap.exitLocked = false;
            currentMap.themeColor = 7;
            break;
    }

    // 加载后自动修正出生点和出口
    validateMapCoords();
}

// ============================================================
// 地图是否清空
// ============================================================
bool isMapClear() {
    for (auto& m : currentMap.mobs) if (m.alive) return false;
    return true;
}

// ============================================================
// 绘制地图
// ============================================================
void drawMap() {
    int h = (int)currentMap.data.size();
    if (h == 0) return;
    for (int i = 0; i < h; i++) {
        int w = (int)currentMap.data[i].size();
        for (int j = 0; j < w; j++) {
            if (i == currentMap.px && j == currentMap.py) {
                setColor(14);
                cout << 'P';
                continue;
            }
            if (!currentMap.exitLocked && i == currentMap.ex && j == currentMap.ey) {
                setColor(10);
                cout << 'O';
                continue;
            }
            bool isBox = false;
            for (auto& b : currentMap.boxes) {
                if (!b.opened && b.x == i && b.y == j) {
                    setColor(6);
                    cout << 'B';
                    isBox = true;
                    break;
                }
            }
            if (isBox) continue;
            bool isMob = false;
            for (auto& m : currentMap.mobs) {
                if (m.alive && m.x == i && m.y == j) {
                    setColor(monsterDB[m.type].color);
                    cout << monsterDB[m.type].symbol;
                    isMob = true;
                    break;
                }
            }
            bool isNpc = false;
            for (auto& npc : currentMap.npcs) {
                if (npc.x == i && npc.y == j) {
                    setColor(npc.color);
                    cout << npc.symbol;
                    isNpc = true;
                    break;
                }
            }
            if (isNpc || isMob) continue;
            char c = currentMap.data[i][j];
            if (c == '*') setColor(7);
            else if (c == '~') setColor(9);
            else if (c == '#') setColor(2);
            else if (c == 'X') {
                setColor(12);
                cout << 'X';
                continue;
            } else if (c == 'F') {
                setColor(4);
                cout << 'F';
                continue;
            } else setColor(15);
            cout << c;
        }
        cout << endl;
    }
    resetColor();
}

// ============================================================
// 怪物移动
// ============================================================
void monsterMove(int &mx, int &my, int px, int py, vector<string>& map) {
    if (mx == 0 && my == 0) return;
    int h = (int)map.size();
    if (h == 0) return;
    int w = (int)map[0].size();
    if (mx <= 0 || mx >= h - 1 || my <= 0 || my >= w - 1) return;

    vector<int> valid;
    for (int i = 0; i < 4; i++) {
        int nx = mx + dx[i], ny = my + dy[i];
        if (nx > 0 && nx < h - 1 && ny > 0 && ny < w - 1
            && map[nx][ny] != '*' && map[nx][ny] != '#'
            && map[nx][ny] != '~')
            valid.push_back(i);
    }
    if (valid.empty()) return;

    if (abs(mx - px) + abs(my - py) <= 10) {
        vector<pair<int, int>> cand;
        for (int d : valid) {
            int nx = mx + dx[d], ny = my + dy[d];
            cand.push_back({abs(nx - px) + abs(ny - py), d});
        }
        sort(cand.begin(), cand.end());
        int best = cand[0].second;
        mx += dx[best];
        my += dy[best];
        return;
    }
    vector<int> safe;
    for (int d : valid) {
        int nx = mx + dx[d], ny = my + dy[d];
        int cnt = 0;
        for (int j = 0; j < 4; j++) {
            int nnx = nx + dx[j], nny = ny + dy[j];
            if (nnx > 0 && nnx < h - 1 && nny > 0 && nny < w - 1
                && map[nnx][nny] != '*' && map[nnx][nny] != '#'
                && map[nnx][nny] != '~')
                cnt++;
        }
        if (cnt > 0) safe.push_back(d);
    }
    int d = safe.empty() ? valid[rand() % valid.size()]
                         : safe[rand() % safe.size()];
    mx += dx[d];
    my += dy[d];
}