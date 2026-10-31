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
