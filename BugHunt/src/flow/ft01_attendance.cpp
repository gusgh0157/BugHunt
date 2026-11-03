#include "../labs.h"
#include <vector>

// 출석 기록으로 보상을 계산한다.
// 매일 출석하면 100골드, 7일 연속 출석을 처음 달성하면 보너스 1000골드.
namespace flow1
{
constexpr int kDailyGold = 100;
constexpr int kStreakGoal = 7;
constexpr int kStreakBonus = 1000;

struct RewardResult
{
    int bestStreak = 0;
    int gold = 0;
};

RewardResult calculateRewards(const std::vector<bool>& loginLog)
{
    RewardResult result;
    bool bonusGiven = false;

    for (bool loggedIn : loginLog)
    {
        int streak = 0;
        if (loggedIn)
        {
            result.gold += kDailyGold;
            ++streak;
            if (streak > result.bestStreak)
                result.bestStreak = streak;
            if (streak == kStreakGoal && !bonusGiven)
            {
                result.gold += kStreakBonus;
                bonusGiven = true;
            }
        }
        else
        {
            streak = 0;
        }
    }
    return result;
}
}

void ft01_attendance()
{
    // 1~7일 출석, 8일 결석, 9~10일 출석
    const std::vector<bool> loginLog = { true, true, true, true, true, true, true, false, true, true };

    const flow1::RewardResult result = flow1::calculateRewards(loginLog);
    expect("최장 연속 출석", 7, result.bestStreak);
    expect("받은 골드 (9일 x 100 + 보너스 1000)", 1900, result.gold);
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// ============================================================================

#pragma region 힌트 1 (README의 "따라 해 보기"를 먼저 하세요)
// for 문 첫 줄에 F9로 중단점을 걸고 F10으로 한 줄씩 진행하세요.
// 지역 창에서 streak가 어떻게 변하는지만 보세요. 둘째 날에 2가 되나요?
#pragma endregion

#pragma region 힌트 2
// streak가 1이 되었다가 다음 바퀴에서 다시 0이 되는 순간, 노란 화살표는 어느 줄에 있나요?
#pragma endregion
