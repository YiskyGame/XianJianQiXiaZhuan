#ifndef SAVE_SAVE_H
#define SAVE_SAVE_H

#include "core/types.h"
#include <string>
#include <ctime>
using namespace std;

// ============================================================
// 存档元信息（读取存档第一行，用于选档界面显示）
// ============================================================
struct SaveMeta {
    string name;
    int level;
    int gold;
    time_t saveTime;
    int progress;
    bool exists;
};

// ============================================================
// 路径 / 元信息
// ============================================================
string   getSaveSlotPath(int slot);
SaveMeta readSaveMeta(int slot);

// ============================================================
// 旧存档迁移（save.dat → save1.dat）
// ============================================================
void migrateLegacySave();

// ============================================================
// 存档 / 读档
// ============================================================
void saveGame(int slot);
void loadGame(int slot);

// ============================================================
// 槽位显示 / 选择
// ============================================================
void showSlotInfo(int slot);
int  chooseSlot(int mode);   // mode: 0=读档，1=存档

#endif // SAVE_SAVE_H