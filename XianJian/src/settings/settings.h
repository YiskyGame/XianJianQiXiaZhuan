#ifndef SETTINGS_SETTINGS_H
#define SETTINGS_SETTINGS_H

#include "core/types.h"
#include <string>
using namespace std;

// ============================================================
// 玩家设置
// ============================================================
struct GameSettings {
    bool skipUpdateLog;   // true = 永久关闭更新内容弹窗

    GameSettings() : skipUpdateLog(false) {}
};

// ============================================================
// 全局设置对象（extern）
// ============================================================
extern GameSettings g_settings;

// ============================================================
// 读写设置
// ============================================================
GameSettings loadSettings();
void         saveSettings(const GameSettings& s);

#endif // SETTINGS_SETTINGS_H