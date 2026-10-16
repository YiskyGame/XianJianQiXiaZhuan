#include "ui/ui.h"
#include "core/globals.h"
#include "core/utils.h"
#include "data/items.h"
#include <string>

// ============================================================
// 物品类型名
// ============================================================
static string getItemTypeNameCN(int id) {
    if (id <= 0 || (size_t)id >= itemDB.size() || itemDB[id].id != id)
        return "未知";
    switch (itemDB[id].type) {
        case WEAPON:    return "武器";
        case ARMOR:     return "防具";
        case CLOTHING:  return "服装";
        case ACCESSORY: return "饰品";
        case POTION:    return "丹药";
        case THROWING:  return "暗器";
        case MATERIAL:  return "材料";
        case QUEST:     return "任务物品";
        case SPECIAL:   return "特殊物品";
        default:        return "其他";
    }
}

// ============================================================
// 品质名
// ============================================================
static string getQualityName(int q) {
    switch (q) {
        case 0: return "普通";
        case 1: return "精良";
        case 2: return "稀有";
        case 3: return "史诗";
        case 4: return "传说";
        default: return "未知";
    }
}

// ============================================================
// 元素名
// ============================================================
static string getElementNameCN(int ele) {
    static const char* names[] = {"无", "火", "冰", "雷", "毒", "圣", "暗", "风", "土"};
    if (ele >= 0 && ele <= 8) return names[ele];
    return "无";
}

// ============================================================
// 显示物品详情
// ============================================================
static void displayItemDetail(int id) {
    if (id <= 0 || (size_t)id >= itemDB.size() || itemDB[id].id != id) {
        printf("错误：物品ID无效！\n");
        return;
    }
    Item& it = itemDB[id];
    if (it.name.empty() || it.name == "[空]" || it.name == "[未知物品]") {
        printf("该物品槽位为空！\n");
        return;
    }
    setColor(14);
    printf("===== 物品详细信息 =====\n");
    resetColor();
    printf("\nID：%d\n名称：%s\n类型：%s\n描述：%s\n",
           id, it.name.c_str(),
           getItemTypeNameCN(id).c_str(), it.desc.c_str());
    if (it.quality > 0)
        printf("品质：%s\n", getQualityName(it.quality).c_str());
    if (it.element != ELE_NONE)
        printf("元素属性：%s\n", getElementNameCN(it.element).c_str());
    if (it.requiredLevel > 0)
        printf("需求等级：%d 级\n", it.requiredLevel);
    if (it.price > 0)
        printf("商店价格：%d 金币\n", it.price);
    string eff = getItemEffectDesc(id);
    if (!eff.empty()) printf("效果：%s\n", eff.c_str());
    if (!it.recipe.empty()) {
        printf("可锻造：是\n配方材料：\n");
        for (auto& r : it.recipe)
            printf("  %s x%d\n",
                   safeItemName(r.first).c_str(), r.second);
    } else {
        printf("可锻造：否\n");
    }
    printf("\n");
    system("pause");
}

// ============================================================
// 按名称搜索
// ============================================================
static void searchItemByName() {
    system("cls");
    printf("===== 按名称搜索 =====\n");
    printf("\n请输入关键词（输入0取消）: ");
    string keyword;
    cin >> keyword;
    if (keyword == "0") return;
    vector<int> results;
    for (size_t i = 0; i < itemDB.size(); i++)
        if (itemDB[i].id == (int)i
            && itemDB[i].name != "[空]"
            && itemDB[i].name != "[未知物品]"
            && itemDB[i].name.find(keyword) != string::npos)
            results.push_back(i);
    if (results.empty()) {
        printf("未找到包含 \"%s\" 的物品。\n", keyword.c_str());
        system("pause");
        return;
    }
    printf("\n找到 %d 个匹配物品：\n\n", (int)results.size());
    printf("ID    名称                    类型      品质\n");
    printf("================================================\n");
    for (int id : results)
        printf("%4d  %-20s  %-6s  %s\n",
               id, itemDB[id].name.c_str(),
               getItemTypeNameCN(id).c_str(),
               getQualityName(itemDB[id].quality).c_str());
    printf("================================================\n");
    printf("输入物品ID查看详情（输入0返回）: ");
    int choice;
    cin >> choice;
    if (choice > 0) displayItemDetail(choice);
}

// ============================================================
// 按类型浏览
// ============================================================
static void searchItemByType() {
    system("cls");
    printf("===== 按类型浏览 =====\n");
    printf("[1]武器 [2]防具 [3]服装 [4]饰品 [5]丹药 [6]暗器 [7]材料 [8]任务 [0]返回\n");
    char choice = read();
    if (choice == '0') return;
    ItemType targetType;
    string typeName;
    switch (choice) {
        case '1': targetType = WEAPON;    typeName = "武器";     break;
        case '2': targetType = ARMOR;     typeName = "防具";     break;
        case '3': targetType = CLOTHING;  typeName = "服装";     break;
        case '4': targetType = ACCESSORY; typeName = "饰品";     break;
        case '5': targetType = POTION;    typeName = "丹药";     break;
        case '6': targetType = THROWING;  typeName = "暗器";     break;
        case '7': targetType = MATERIAL;  typeName = "材料";     break;
        case '8': targetType = QUEST;     typeName = "任务物品"; break;
        default: return;
    }
    system("cls");
    printf("===== %s 列表 =====\n\n", typeName.c_str());
    printf("ID    名称                    价格      效果\n");
    printf("================================================\n");
    vector<int> results;
    for (size_t i = 0; i < itemDB.size(); i++)
        if (itemDB[i].id == (int)i
            && itemDB[i].name != "[空]"
            && itemDB[i].name != "[未知物品]"
            && itemDB[i].type == targetType)
            results.push_back(i);
    for (int id : results) {
        string eff = getItemEffectDesc(id);
        if (eff.length() > 20) eff = eff.substr(0, 17) + "...";
        printf("%4d  %-20s  %6d  %s\n",
               id, itemDB[id].name.c_str(),
               itemDB[id].price, eff.c_str());
    }
    printf("================================================\n");
    printf("输入物品ID查看详情（输入0返回）: ");
    int cid;
    cin >> cid;
    if (cid > 0) displayItemDetail(cid);
}

// ============================================================
// 物品查询主菜单
// ============================================================
void itemQueryUI() {
    while (true) {
        system("cls");
        printf("===== 物品查询系统 =====\n");
        printf("[1] 按ID查询\n[2] 按名称搜索\n[3] 按类型浏览\n[0] 返回\n");
        printf("\n请选择: ");
        char choice = read();
        switch (choice) {
            case '1': {
                system("cls");
                printf("请输入物品ID（输入0返回）: ");
                int id;
                cin >> id;
                if (id > 0) displayItemDetail(id);
                break;
            }
            case '2': searchItemByName(); break;
            case '3': searchItemByType(); break;
            case '0': return;
        }
    }
}
