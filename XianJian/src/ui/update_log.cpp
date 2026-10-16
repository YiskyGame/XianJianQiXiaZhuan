#include "ui/ui.h"
#include "core/globals.h"
#include "core/utils.h"
#include "settings/settings.h"

// ============================================================
// 显示更新内容
// 返回：
//   0 = 关闭（下次还会弹）
//   1 = 永久关闭（写入 settings.dat）
//   -1 = 取消
// ============================================================
int showUpdateLog() {
    system("cls");
    setColor(14);cout << "========================================================\n";
    cout << "                《 鲜 贱 骑 虾 转 》\n";
    cout << "              重置版 ver 1.0 测试版一\n";
    cout << "========================================================\n\n";
    resetColor();

    setColor(11);
    cout << "【本次更新要点】\n";
    resetColor();
	
	cout << "  版本：重置版 ver 1.0 测试版一\n\n";
	cout << "  一、多存档系统\n";
    cout << "    - 支持 3 个独立存档槽位\n";
    cout << "    - 启动时可视化选档（显示角色名/等级/金币/进度/时间）\n";
    cout << "    - 支持新建、覆盖、删除\n";
    cout << "    - 旧版 save.dat 自动迁移到 存档1\n\n";

    cout << "  二、地图坐标自动修复（V1~V9）\n";
    cout << "    - 出生点自动扫描，不再卡墙\n";
    cout << "    - 出口自动扫描，不再在墙里或地图外\n";
    cout << "    - 新增 isWalkable / findRandomWalkable\n";
    cout << "       / findFarthestWalkable / validateMapCoords\n";
    cout << "    - drawMap / explore / monsterMove 全部加越界保护\n\n";

    cout << "  三、战斗系统修复（F1~F11）\n";
    cout << "    - 暴击伤害使用 critDmg（不再硬编码 x2）\n";
    cout << "    - 暗影职业首次攻击每场战斗都生效\n";
    cout << "    - 技能冷却 cd 正式生效\n";
    cout << "    - 状态持续时间不再双重递减\n";
    cout << "    - 击杀计数不再重复\n";
    cout << "    - 符修眩晕时也能回蓝\n";
    cout << "    - 临时丹药（神力丹/护体丹）改为真临时\n\n";

    cout << "  四、成就系统统一\n";
    cout << "    - 新增 unlockAchievement() 统一发放\n";
    cout << "    - 隐藏成就解锁后才显示\n";
    cout << "    - 炼丹成就改为综合合成/购买判定\n\n";

    cout << "  五、其他\n";
    cout << "    - 移除所有死代码与冗余变量\n";
    cout << "    - 存档加读写失败检查\n";
    cout << "    - NPC/怪物与出生点重叠时自动重定位\n\n";

    setColor(10);
    cout << "========================================================\n";
    resetColor();
    cout << "  [1] 关闭（下次启动仍会显示）\n";
    cout << "  [2] 永久关闭（可在主菜单重新开启）\n";
    cout << "  [0] 返回\n";
    cout << "\n请选择: ";

    int choice = read();
    if (choice == '1') return 0;
    if (choice == '2') return 1;
    return -1;
}

// ============================================================
// 启动时显示（受 settings 控制）
// ============================================================
void showUpdateLogAtStartup() {
    if (g_settings.skipUpdateLog) return;

    int r = showUpdateLog();
    if (r == 1) {
        g_settings.skipUpdateLog = true;
        saveSettings(g_settings);
        printf("\n已永久关闭更新内容显示。\n");
        printf("如需重新开启，可在主菜单 → 更新内容 中恢复。\n");
        Sleep(1500);
    }
}
