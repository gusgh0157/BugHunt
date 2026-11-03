#include "../labs.h"
#include <algorithm>
#include <string>
#include <vector>

// 아레나 순위표. 점수가 높은 순서로 정렬한다. 점수가 같으면 누가 위든 상관없다.
namespace ct3
{
struct Entry
{
    std::string name;
    int score;
};

void sortByScore(std::vector<Entry>& entries)
{
    std::sort(entries.begin(), entries.end(), [](const Entry& a, const Entry& b) {
        return a.score >= b.score;   // 높은 점수가 앞으로
    });
}

int sumOfTop(const std::vector<Entry>& sorted, size_t n)
{
    int sum = 0;
    for (size_t i = 0; i < n && i < sorted.size(); ++i)
        sum += sorted[i].score;
    return sum;
}
}

void ct03_leaderboard()
{
    std::vector<ct3::Entry> arena = {
        { "Kael", 1500 }, { "Mira", 1720 }, { "Doran", 1500 }, { "Selene", 1980 },
        { "Ivo", 1500 }, { "Nell", 1340 }, { "Pia", 1720 }, { "Oren", 1100 },
    };

    ct3::sortByScore(arena);
    for (size_t i = 0; i < 3; ++i)
        std::cout << "  " << (i + 1) << "위 " << arena[i].name << " " << arena[i].score << '\n';

    expect("1위 점수", 1980, arena[0].score);
    expect("상위 3명 점수 합 (1980 + 1720 + 1720)", 5420, ct3::sumOfTop(arena, 3));
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// ============================================================================

#pragma region 힌트 1 (먼저 직접 추적해 보고 펼치세요)
// Debug에서 F5로 실행하면 어설션 창이 뜹니다. 메시지를 끝까지 읽어 보세요. 무엇이 "invalid"라고 하나요?
// 다시 시도 → 호출 스택에서 [외부 코드]를 지나 처음 나오는 내 코드는 어디인가요?
#pragma endregion

#pragma region 힌트 2
// 비교 함수에 같은 점수의 두 사람을 넣어 보세요. 조사식에서 직접 계산해도 됩니다.
// comp(Kael, Doran)과 comp(Doran, Kael)이 둘 다 true라면, 정렬 알고리즘은 누가 앞이라고 믿어야 하나요?
#pragma endregion

#pragma region 힌트 3
// std::sort의 비교 함수는 "엄격하게 앞서는가"를 답해야 합니다(strict weak ordering). 같으면 false여야 합니다.
// Release에서는 어설션이 없습니다. 같은 점수가 아주 많으면 어떤 일이 생길 수 있을까요?
#pragma endregion

#pragma region 채점용 숨은 테스트 (풀이에는 필요 없습니다. 고치지 마세요)
// 채점기(grade.bat)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void ct03_leaderboard_hidden()
{
    std::vector<ct3::Entry> league;
    for (int i = 0; i < 64; ++i)
        league.push_back({ "Guild" + std::to_string(i), (i % 3 == 0) ? 900 : 400 + i });

    ct3::sortByScore(league);
    expect("1위 점수", 900, league[0].score);
    expect("정렬되어 있나?", true, std::is_sorted(league.begin(), league.end(),
        [](const ct3::Entry& a, const ct3::Entry& b) { return a.score > b.score; }));
    expect("상위 5명 점수 합", 4500, ct3::sumOfTop(league, 5));
}
#pragma endregion
