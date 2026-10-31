#include "../labs.h"
#include <vector>

namespace
{
// 플레이어 점수 목록의 평균을 구한다.
double average(const std::vector<int>& scores)
{
    int sum = 0;
    for (size_t i = 0; i <= scores.size(); ++i)
        sum += scores[i];
    return static_cast<double>(sum) / scores.size();
}
}

void lab01_average()
{
    const std::vector<int> scores = { 80, 92, 75, 60, 93 };
    expect("평균 점수", 80.0, average(scores));
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// ============================================================================

#pragma region 힌트 1 (먼저 직접 추적해 보고 펼치세요)
// Assertion 창에서 다시 시도 → 호출 스택에서 내 코드가 나오는 첫 프레임을 더블클릭하세요.
#pragma endregion

#pragma region 힌트 2
// 멈춘 순간 i와 scores.size()의 값은?
#pragma endregion
