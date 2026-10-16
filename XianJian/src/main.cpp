#include "core/globals.h"
#include "core/types.h"
#include "core/utils.h"
#include "data/items.h"
#include "data/classes.h"
#include "data/monsters.h"
#include "data/achievements.h"
#include "data/quests.h"
#include "entity/player.h"
#include "save/save.h"
#include "settings/settings.h"
#include "ui/ui.h"

// ============================================================
// 主函数
// ============================================================
int main() {
	SetConsoleOutputCP(65001);
	SetConsoleCP(65001);
	srand((unsigned)time(nullptr));
	
	// 1. 加载设置
	g_settings = loadSettings();
	
	// 2. 初始化游戏数据
	initItems();
	initClasses();
	initMonsters();
	sanitizeAllRecipes();
	initAchievements();
	initQuests();
	
	// 3. 启动时显示更新内容（受 settings 控制）
	showUpdateLogAtStartup();
	
	// 4. 标题 / 选档
	titleScreen();
	
	// 5. 进入游戏
	if (needLoadSave) {
		loadGame(currentSlot);
	} else {
		chooseClass();
	}
	mainMenu();
	return 0;
}
