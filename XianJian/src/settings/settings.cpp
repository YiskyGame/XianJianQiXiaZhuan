#include "settings/settings.h"
#include "core/globals.h"
#include <fstream>
#include <cstdlib>
using namespace std;

// ============================================================
// 全局设置对象定义
// ============================================================
GameSettings g_settings;

// ============================================================
// 读取设置
// 格式：
//   skipUpdateLog=0  或  1
// ============================================================
GameSettings loadSettings() {
    GameSettings s;
    ifstream f(SETTINGS_FILE);
    if (!f) return s;

    string line;
    while (getline(f, line)) {
        // 去掉首尾空白
        size_t b = line.find_first_not_of(" \t\r\n");
        size_t e = line.find_last_not_of(" \t\r\n");
        if (b == string::npos) continue;
        line = line.substr(b, e - b + 1);

        if (line.size() > 14 && line.substr(0, 14) == "skipUpdateLog=") {
            int v = atoi(line.substr(14).c_str());
            s.skipUpdateLog = (v != 0);
        }
    }
    f.close();
    return s;
}

// ============================================================
// 保存设置
// ============================================================
void saveSettings(const GameSettings& s) {
    ofstream f(SETTINGS_FILE);
    if (!f) return;
    f << "skipUpdateLog=" << (s.skipUpdateLog ? 1 : 0) << "\n";
    f.close();
}