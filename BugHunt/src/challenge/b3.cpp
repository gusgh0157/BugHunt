#include "../labs.h"
#include <cstdint>
#include <vector>

// 챌린지 B-3

// [JA] チャレンジ B-3
namespace blind3
{
struct Tile
{
    int height;
};

// 지형 한 줄의 높이 변화 합 (오르막만)

// [JA] 地形1列分の高さの変化の合計です(上り坂のみ)。
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
    runWithWatchdog(i18n::L("지형 계산", "地形計算"), [] {
        expect(i18n::L("짧은 길 (폭 40)", "短い道(幅40)"), 105, climbOf(makeRow(40)));
        expect(i18n::L("긴 길 (폭 300)", "長い道(幅300)"), 763, climbOf(makeRow(300)));
    });
}

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
void chB3_hidden()
{
    using namespace blind3;
    runWithWatchdog(i18n::L("지형 계산 (숨은 케이스)", "地形計算(隠しケース)"), [] {
        expect(i18n::L("폭 0", "幅0"), 0, climbOf({}));
        expect(i18n::L("폭 256", "幅256"), 651, climbOf(makeRow(256)));
        expect(i18n::L("폭 1000", "幅1000"), 2548, climbOf(makeRow(1000)));
    });
}
#pragma endregion
