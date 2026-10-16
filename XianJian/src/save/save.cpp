#include "save/save.h"
#include "core/globals.h"
#include "core/utils.h"
#include "data/items.h"
#include "data/classes.h"
#include "data/quests.h"
#include "data/achievements.h"
#include "entity/player.h"
#include "entity/stats.h"
#include "item/bag.h"

// ============================================================
// 存档路径
// ============================================================
string getSaveSlotPath(int slot) {
    return "save" + to_string(slot) + ".dat";
}

// ============================================================
// 读取存档元信息
// ============================================================
SaveMeta readSaveMeta(int slot) {
    SaveMeta meta;
    meta.exists = false;
    if (slot < 1 || slot > MAX_SLOTS) return meta;

    ifstream f(getSaveSlotPath(slot));
    if (!f) return meta;

    string line;
    if (!getline(f, line)) return meta;

    stringstream ss(line);
    long long ts = 0;
    if (!(ss >> meta.name >> meta.level >> meta.gold
          >> ts >> meta.progress)) {
        return meta;
    }
    meta.saveTime = (time_t)ts;
    meta.exists = true;
    return meta;
}

// ============================================================
// 旧存档迁移
// ============================================================
void migrateLegacySave() {
    ifstream old("save.dat");
    if (!old) return;

    ifstream check("save1.dat");
    if (check) return;
    check.close();

    stringstream buf;
    buf << old.rdbuf();
    old.close();

    ofstream out("save1.dat");
    if (!out) return;
    out << buf.str();
    out.close();

    remove("save.dat");
    printf("已将旧存档迁移到 存档1。\n");
    Sleep(1000);
}

// ============================================================
// 存档
// ============================================================
void saveGame(int slot) {
    if (slot < 1 || slot > MAX_SLOTS) return;
    ofstream f(getSaveSlotPath(slot));
    if (!f) {
        printf("存档失败！\n");
        Sleep(500);
        return;
    }

    time_t now = time(nullptr);
    f << player.name << " "
      << player.level << " "
      << player.gold << " "
      << (long long)now << " "
      << currentLevel << "\n";

    f << player.name << "\n";
    f << player.classId << " " << player.level << " "
      << player.exp << " " << player.maxExp << "\n";
    f << player.hp << " " << player.maxHp << " "
      << player.atk << " " << player.def << " " << player.crit << " "
      << player.critDmg << "\n";
    f << player.gold << " " << player.mp << " " << player.maxMp << "\n";
    f << player.weapon << " " << player.armor << " "
      << player.clothing << " " << player.accessory << "\n";
    for (int i = 0; i < 10; i++) f << player.statusDur[i] << " ";
    f << "\n";
    f << player.dungeonLevel << " " << player.passiveBonus << " "
      << player.firstAttack << "\n";
    f << (int)player.playerElement << " "
      << player.killCount << " " << player.bossKillCount << " "
      << player.spiritKillCount << " "
      << player.forgeCount << " " << player.purchaseCount << " "
      << player.goldEarned << " " << player.mapsCleared << "\n";
    f << player.skillLv.size() << "\n";
    for (int lv : player.skillLv) f << lv << " ";
    f << "\n" << player.bag.size() << "\n";
    for (auto& p : player.bag) f << p.first << " " << p.second << "\n";
    f << quests.size() << "\n";
    for (auto& q : quests) f << q.taken << " " << q.completed << "\n";
    f << achievements.size() << "\n";
    for (auto& a : achievements) f << a.unlocked << " ";
    f << "\n";
    f << currentLevel << "\n";
    f << player.treasurePieces << "\n";
    f << player.totalSpent << " " << player.skillUseCount << " "
      << player.totalCritCount << " " << player.trapDeathCount << "\n";
    f.close();

    if (!f) {
        printf("存档写入失败！\n");
        Sleep(500);
        return;
    }
    printf("已保存到 存档%d。\n", slot);
    Sleep(500);
}

// ============================================================
// 读档
// ============================================================
void loadGame(int slot) {
    if (slot < 1 || slot > MAX_SLOTS) return;
    ifstream f(getSaveSlotPath(slot));
    if (!f) {
        printf("存档%d 不存在。\n", slot);
        Sleep(500);
        return;
    }

    string firstLine;
    if (!getline(f, firstLine)) {
        printf("存档损坏。\n");
        Sleep(500);
        return;
    }

    string name;
    {
        stringstream ss(firstLine);
        vector<string> tokens;
        string t;
        while (ss >> t) tokens.push_back(t);

        bool isMeta = false;
        if (tokens.size() == 5) {
            try {
                long long ts = stoll(tokens[3]);
                if (ts > 1000000) isMeta = true;
            } catch (...) {}
        }

        if (isMeta) {
            if (!getline(f, name)) {
                printf("存档损坏（无角色名）。\n");
                Sleep(500);
                return;
            }
        } else {
            name = firstLine;
        }
    }

    int classId, level, exp, maxExp;
    f >> classId >> level >> exp >> maxExp;
    int hp, maxHp, atk, def, crit;
    float critDmg;
    f >> hp >> maxHp >> atk >> def >> crit >> critDmg;
    int gold, mp, maxMp;
    f >> gold >> mp >> maxMp;
    int weapon, armor, clothing, accessory;
    f >> weapon >> armor >> clothing >> accessory;
    int statusDur[10];
    for (int i = 0; i < 10; i++) f >> statusDur[i];
    int dungeonLevel, passiveBonus;
    bool firstAttack;
    f >> dungeonLevel >> passiveBonus >> firstAttack;
    int elementInt;
    f >> elementInt;
    int killCount, bossKillCount, spiritKillCount, forgeCount, purchaseCount,
        goldEarned, mapsCleared;
    f >> killCount >> bossKillCount >> spiritKillCount
      >> forgeCount >> purchaseCount >> goldEarned >> mapsCleared;
    int savedSkillCount;
    f >> savedSkillCount;
    if (savedSkillCount < 0 || savedSkillCount > 100) {
        printf("存档损坏（技能数异常）。\n");
        Sleep(500);
        return;
    }
    vector<int> savedSkillLv(savedSkillCount);
    for (int i = 0; i < savedSkillCount; i++) f >> savedSkillLv[i];
    int bagSize;
    f >> bagSize;
    if (bagSize < 0 || bagSize > MAX_BAG * 2) {
        printf("存档损坏（背包数异常）。\n");
        Sleep(500);
        return;
    }
    vector<pair<int, int>> savedBag;
    for (int i = 0; i < bagSize; i++) {
        int id, cnt;
        f >> id >> cnt;
        if (id > 0 && id < (int)itemDB.size() && itemDB[id].id == id)
            savedBag.push_back({id, cnt});
    }
    int questCount;
    f >> questCount;
    if (questCount == (int)quests.size()) {
        for (int i = 0; i < questCount; i++) {
            bool taken, completed;
            f >> taken >> completed;
            quests[i].taken = taken;
            quests[i].completed = completed;
        }
    }
    int achCount;
    f >> achCount;
    vector<bool> achUnlocked(achCount > 0 ? achCount : 0);
    for (int i = 0; i < achCount && i < (int)achUnlocked.size(); i++) {
        int v;
        f >> v;
        achUnlocked[i] = (v != 0);
    }
    f >> currentLevel;
    if (currentLevel < 1) currentLevel = 1;
    if (currentLevel > MAX_MAPS) currentLevel = MAX_MAPS;
    int treasurePieces = 0;
    if (f.peek() != EOF) f >> treasurePieces;
    int totalSpent = 0, skillUseCount = 0, totalCritCount = 0, trapDeathCount = 0;
    if (f.peek() != EOF)
        f >> totalSpent >> skillUseCount >> totalCritCount >> trapDeathCount;
    f.close();

    if (classId < 0 || classId >= (int)classes.size()) {
        printf("存档损坏（职业ID异常）。\n");
        Sleep(500);
        return;
    }

    player.name = name;
    player.classId = classId;
    player.level = max(1, level);
    player.exp = exp;
    player.maxExp = maxExp > 0 ? maxExp : 50;
    player.maxHp = maxHp > 0 ? maxHp : 100;
    player.hp = min(hp, maxHp);
    if (player.hp <= 0) player.hp = player.maxHp;
    player.atk = atk;
    player.def = def;
    player.crit = crit;
    player.critDmg = critDmg;
    player.gold = gold;
    player.maxMp = maxMp > 0 ? maxMp : 50;
    player.mp = min(mp, maxMp);
    if (player.mp < 0) player.mp = 0;
    player.weapon = weapon;
    player.armor = armor;
    player.clothing = clothing;
    player.accessory = accessory;
    memcpy(player.statusDur, statusDur, sizeof(statusDur));
    player.dungeonLevel = dungeonLevel;
    player.passiveBonus = passiveBonus;
    player.firstAttack = firstAttack;
    player.playerElement = (Element)elementInt;
    player.killCount = killCount;
    player.bossKillCount = bossKillCount;
    player.spiritKillCount = spiritKillCount;
    player.forgeCount = forgeCount;
    player.purchaseCount = purchaseCount;
    player.goldEarned = goldEarned;
    player.mapsCleared = mapsCleared;
    player.treasurePieces = treasurePieces;
    player.totalSpent = totalSpent;
    player.skillUseCount = skillUseCount;
    player.totalCritCount = totalCritCount;
    player.trapDeathCount = trapDeathCount;
    player.skillLv = savedSkillLv;
    player.skillCd.assign(player.skillLv.size(), 0);
    player.bag = savedBag;
    for (size_t i = 0; i < achievements.size() && i < achUnlocked.size(); i++)
        achievements[i].unlocked = achUnlocked[i];
    player.hp = min(player.hp, getMaxHp());
    player.mp = min(player.mp, player.maxMp);

    currentSlot = slot;
    printf("已读取 存档%d。\n", slot);
    Sleep(500);
}

// ============================================================
// 槽位显示
// ============================================================
void showSlotInfo(int slot) {
    SaveMeta meta = readSaveMeta(slot);
    if (!meta.exists) {
        printf("  [%d] 空存档\n", slot);
        return;
    }
    char timeBuf[64] = "";
    struct tm* t = localtime(&meta.saveTime);
    if (t) strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M", t);
    printf("  [%d] %-10s Lv.%-3d %d金币  进度:%d  (%s)\n",
           slot, meta.name.c_str(), meta.level,
           meta.gold, meta.progress, timeBuf);
}

// ============================================================
// 槽位选择
// mode = 0: 读档，必须存在
// mode = 1: 存档，可覆盖
// ============================================================
int chooseSlot(int mode) {
    while (1) {
        system("cls");
        if (mode == 0) printf("===== 读取存档 =====\n\n");
        else           printf("===== 保存到槽位 =====\n\n");

        for (int i = 1; i <= MAX_SLOTS; i++) showSlotInfo(i);
        printf("\n  [0] 返回\n");
        printf("请选择槽位 (1~%d): ", MAX_SLOTS);

        int slot = read();
        slot = slot - '0';
        if (slot == 0) return 0;
        if (slot < 1 || slot > MAX_SLOTS) continue;

        if (mode == 0) {
            SaveMeta meta = readSaveMeta(slot);
            if (!meta.exists) {
                printf("该槽位为空！\n");
                Sleep(800);
                continue;
            }
            return slot;
        } else {
            SaveMeta meta = readSaveMeta(slot);
            if (meta.exists) {
                printf("该槽位已有存档（%s Lv.%d），覆盖吗？(y/n) ",
                       meta.name.c_str(), meta.level);
                char ans = read();
                if (ans != 'y' && ans != 'Y') continue;
            }
            return slot;
        }
    }
}