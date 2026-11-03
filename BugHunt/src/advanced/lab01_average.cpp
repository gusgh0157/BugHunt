#include "../labs.h"
#include <vector>

namespace
{
// 플레이어 점수 목록의 평균을 구한다.

// [JA] プレイヤーのスコア一覧の平均を求めます。
double average(const std::vector<int>& scores)
{
    int sum = 0;
    for (size_t i = 0; i <= scores.size(); ++i)
        sum += scores.at(i);
    return static_cast<double>(sum) / scores.size();
}
}

void lab01_average()
{
    const std::vector<int> scores = { 80, 92, 75, 60, 93 };
    expect("평균 점수", 80.0, average(scores));
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (편집기의 접기 기능으로 모두 접어 두세요)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(エディターの折りたたみ機能ですべて折りたたんでおいてください)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// 디버거는 처리되지 않은 예외(std::out_of_range)에서 멈춥니다. 호출 스택에서 내 코드가 나오는 첫 프레임을 고르세요.

// [JA] デバッガーはハンドルされない例外(std::out_of_range)で止まります。呼び出し履歴で、自分のコードが出てくる最初のフレームを選んでください。
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// 멈춘 순간 i와 scores.size()의 값은?

// [JA] 止まった瞬間の i と scores.size() の値は?
#pragma endregion
