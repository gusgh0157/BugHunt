#include "../labs.h"
#include <string>

// 전투 기록을 읽어 초당 피해량(DPS)을 보여 준다.
namespace lab3
{
struct BattleRecord
{
    std::string enemy;
    int totalDamage;
    int durationMs;   // 전투 시간 (밀리초)
};

int toSeconds(int ms)
{
    return ms / 1000;
}

int damagePerSecond(int totalDamage, int seconds)
{
    return totalDamage / seconds;
}

void printBattleSummary(const BattleRecord& battle)
{
    const int seconds = toSeconds(battle.durationMs);
    const int dps = damagePerSecond(battle.totalDamage, seconds);
    std::cout << "  " << battle.enemy << ": " << seconds << "초, DPS " << dps << '\n';
}

void printBattleLog()
{
    const BattleRecord log[] = {
        { "Wolf",  240, 12000 },
        { "Orc",   900, 30500 },
        { "Slime",  35,   800 },
        { "Troll", 1500, 41000 },
    };

    for (const BattleRecord& battle : log)
        printBattleSummary(battle);
}
}

void cs03_battle_log()
{
    lab3::printBattleLog();
}
