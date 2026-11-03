#include "../labs.h"
#include <stdexcept>
#include <string>

// 전투 기록을 읽어 초당 피해량(DPS)을 보여 준다.

// [JA] 戦闘記録を読んで秒間ダメージ(DPS)を表示します。
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
    if (seconds <= 0)
        throw std::invalid_argument("damagePerSecond: 전투 시간이 0초 이하");
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
// 힌트: 막힐 때 하나씩만 펼치세요. (편집기의 접기 기능으로 모두 접어 두세요)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(エディターの折りたたみ機能ですべて折りたたんでおいてください)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 호출 스택을 보고 펼치세요 / まず呼び出し履歴を見てから開いてください)
// damagePerSecond에서 seconds는 0입니다.
// 0으로 나누는 이 함수는 받은 값을 썼을 뿐입니다. 왜 0이 들어왔을까요?

// [JA] damagePerSecond で seconds は 0 です。
// [JA] 0 で割っているこの関数は、受け取った値を使っただけです。なぜ 0 が入ってきたのでしょうか?
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// 한 칸 아래 printBattleSummary 프레임에서 battle을 펼쳐 보세요.
// 어떤 적이고, durationMs는 얼마인가요?

// [JA] 一つ下の printBattleSummary フレームで battle を展開してみてください。
// [JA] どの敵で、durationMs はいくつですか?
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// 800 / 1000 은 정수 나눗셈으로 얼마인가요?

// [JA] 800 / 1000 は整数の割り算でいくつですか?
#pragma endregion
