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

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// ============================================================================

#pragma region 힌트 1 (먼저 호출 스택을 보고 펼치세요)
// damagePerSecond에서 seconds는 0입니다.
// 0으로 나누는 이 함수는 받은 값을 썼을 뿐입니다. 왜 0이 들어왔을까요?
#pragma endregion

#pragma region 힌트 2
// 한 칸 아래 printBattleSummary 프레임에서 battle을 펼쳐 보세요.
// 어떤 적이고, durationMs는 얼마인가요?
#pragma endregion

#pragma region 힌트 3
// 800 / 1000 은 정수 나눗셈으로 얼마인가요?
#pragma endregion
