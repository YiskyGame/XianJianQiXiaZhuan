#ifndef BATTLE_BATTLE_H
#define BATTLE_BATTLE_H

#include "core/types.h"
#include "data/monsters.h"
#include <string>
using namespace std;

// ============================================================
// 战斗上下文
// ============================================================
struct BattleContext {
    Monster* mon;
    int mHp, mMaxHp, mAtk, mDef;
    string mName;
    int charge;
    int poisonDmg, poisonTurn;
    BattleState state;
    int monsterStunTurns, monsterId;
};

// ============================================================
// 战斗主函数
// 返回 true = 胜利，false = 战败
// ============================================================
bool battle(Monster& mon, int& mx, int& my);

// ============================================================
// 战斗子流程（供 battle.cpp 内部使用，也可由外部扩展）
// ============================================================
void processPlayerAttack(BattleContext& ctx);
void processSkillSelect(BattleContext& ctx);
void processItemSelect(BattleContext& ctx);
void processThrowSelect(BattleContext& ctx);
void processMonsterTurn(BattleContext& ctx);
void monsterSkill(BattleContext& ctx);

#endif // BATTLE_BATTLE_H