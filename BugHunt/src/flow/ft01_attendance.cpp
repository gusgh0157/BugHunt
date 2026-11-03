#include "../labs.h"
#include <vector>

// 출석 기록으로 보상을 계산한다.
// 매일 출석하면 100골드, 7일 연속 출석을 처음 달성하면 보너스 1000골드.

// [JA] 出席記録から報酬を計算します。
// [JA] 毎日出席すると100ゴールド、7日連続出席を初めて達成するとボーナス1000ゴールドです。
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
// 힌트: 막힐 때 하나씩만 펼치세요. (편집기의 접기 기능으로 모두 접어 두세요)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(エディターの折りたたみ機能ですべて折りたたんでおいてください)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (README의 "따라 해 보기"를 먼저 하세요 / READMEの"やってみよう"を先に行ってください)
// for 문 첫 줄에 중단점을 걸고 한 줄씩 실행(step over)하세요.
// 지역 변수에서 streak가 어떻게 변하는지만 보세요. 둘째 날에 2가 되나요?

// [JA] for 文の最初の行にブレークポイントを置き、1行ずつ実行(ステップオーバー)してください。
// [JA] ローカル変数で streak がどう変わるかだけを見てください。2日目に2になりますか?
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// streak가 1이 되었다가 다음 바퀴에서 다시 0이 되는 순간, 현재 줄은 어디인가요?

// [JA] streak が1になったあと、次の周回で再び0になる瞬間、現在の行はどこでしょうか?
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(--grade)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void ft01_attendance_hidden()
{
    // 1~2일 출석, 3일 결석, 4~11일 출석(8일 연속), 12일 결석, 13일 출석
    const std::vector<bool> loginLog = { true, true, false, true, true, true, true, true, true, true, true, false, true };

    const flow1::RewardResult result = flow1::calculateRewards(loginLog);
    expect("최장 연속 출석", 8, result.bestStreak);
    expect("받은 골드 (11일 x 100 + 보너스 1000)", 2100, result.gold);
}
#pragma endregion
