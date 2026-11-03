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
    std::sort(entries.begin(), entries.end(), bughunt::checkedOrder([](const Entry& a, const Entry& b) {
        return a.score >= b.score;   // 높은 점수가 앞으로
    }));
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
// 힌트: 막힐 때 하나씩만 펼치세요. (편집기의 접기 기능으로 모두 접어 두세요)
// ============================================================================

#pragma region 힌트 1 (먼저 직접 추적해 보고 펼치세요)
// 디버거로 실행하면 abort()에서 멈춥니다. 콘솔의 [CRASH] 메시지를 끝까지 읽어 보세요. 무엇을 어겼다고 하나요?
// 호출 스택에서 표준 라이브러리 줄을 지나 처음 나오는 내 코드는 어디인가요?
// (bughunt::checkedOrder는 표준 라이브러리 디버그 모드가 하는 검사를 어느 구성에서나 하도록 감싼 것입니다)
#pragma endregion

#pragma region 힌트 2
// 비교 함수에 같은 점수의 두 사람을 넣어 보세요. 조사식에서 직접 계산해도 됩니다.
// comp(Kael, Doran)과 comp(Doran, Kael)이 둘 다 true라면, 정렬 알고리즘은 누가 앞이라고 믿어야 하나요?
#pragma endregion

#pragma region 힌트 3
// std::sort의 비교 함수는 "엄격하게 앞서는가"를 답해야 합니다(strict weak ordering). 같으면 false여야 합니다.
// 검사기가 없다면 이 코드는 원소가 적을 때 우연히 맞게 정렬되기도 합니다. 정의되지 않은 동작이라
// 같은 점수가 아주 많으면 배열 밖을 읽다가 죽을 수도 있습니다.
#pragma endregion

#pragma region 채점용 숨은 테스트 (풀이에는 필요 없습니다. 고치지 마세요)
// 채점기(--grade)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
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
