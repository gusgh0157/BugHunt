#include "../labs.h"
#include <cstdint>
#include <vector>

// 챌린지 B-3
namespace blind3
{
struct Tile
{
    int height;
};

// 지형 한 줄의 높이 변화 합 (오르막만)
int climbOf(const std::vector<Tile>& row)
{
    int climb = 0;
    const int count = static_cast<int>(row.size());
    for (std::uint8_t x = 0; x + 1 < count; ++x)
    {
        const int diff = row[x + 1].height - row[x].height;
        if (diff > 0)
            climb += diff;
    }
    return climb;
}

std::vector<Tile> makeRow(int width)
{
    std::vector<Tile> row;
    for (int x = 0; x < width; ++x)
        row.push_back({ (x * 7) % 11 });
    return row;
}
}

void chB3()
{
    using namespace blind3;
    runWithWatchdog("지형 계산", [] {
        expect("짧은 길 (폭 40)", 105, climbOf(makeRow(40)));
        expect("긴 길 (폭 300)", 763, climbOf(makeRow(300)));
    });
}

#pragma region 채점용 숨은 테스트 (풀이에는 필요 없습니다. 고치지 마세요)
void chB3_hidden()
{
    using namespace blind3;
    runWithWatchdog("지형 계산 (숨은 케이스)", [] {
        expect("폭 0", 0, climbOf({}));
        expect("폭 256", 651, climbOf(makeRow(256)));
        expect("폭 1000", 2548, climbOf(makeRow(1000)));
    });
}
#pragma endregion
