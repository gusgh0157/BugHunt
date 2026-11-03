#include "../labs.h"
#include <cstdio>
#include <string>
#include <vector>

// 시즌 랭킹 보드. 상위 플레이어를 "1위 Kael (3200점)" 같은 줄로 만들어 모아 둔다.
namespace ptr1
{
struct Player
{
    std::string name;
    int score;
};

const char* formatEntry(int rank, const Player& player)
{
    char line[64];
    std::snprintf(line, sizeof(line), "%d위 %s (%d점)", rank, player.name.c_str(), player.score);
    return line;
}

std::vector<const char*> buildBoard(const std::vector<Player>& ranking)
{
    std::vector<const char*> board;
    for (size_t i = 0; i < ranking.size(); ++i)
        board.push_back(formatEntry(static_cast<int>(i) + 1, ranking[i]));
    return board;
}
}

void pt01_ranking()
{
    const std::vector<ptr1::Player> ranking = {
        { "Kael", 3200 },
        { "Mira", 2950 },
        { "Doran", 2400 },
    };

    const auto board = ptr1::buildBoard(ranking);
    for (const auto& line : board)
        std::cout << "  " << line << '\n';

    expect("1위 줄", std::string("1위 Kael (3200점)"), std::string(board[0]));
    expect("3위 줄", std::string("3위 Doran (2400점)"), std::string(board[2]));
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (편집기의 접기 기능으로 모두 접어 두세요)
// ============================================================================

#pragma region 힌트 1 (먼저 직접 추적해 보고 펼치세요)
// 빌드할 때 나온 이 파일의 컴파일러 경고를 먼저 읽어 보세요.
// 그다음 buildBoard의 return 줄에 중단점을 걸고 조사식에 board[0], board[1], board[2]를 넣으세요.
// 세 포인터의 주소(16진수)를 비교해 보세요.
#pragma endregion

#pragma region 힌트 2
// formatEntry가 돌려준 주소는 formatEntry의 어떤 변수를 가리키나요?
// 그 변수는 함수가 끝난 뒤에도 살아 있나요? formatEntry에 두 번 들어가(step into) &line을 비교해 보세요.
#pragma endregion

#pragma region 힌트 3
// 함수가 끝나도 살아 있는 것을 돌려주세요. C++에서는 문자열을 값으로 돌려주는 것이 가장 간단합니다.
#pragma endregion

#pragma region 채점용 숨은 테스트 (풀이에는 필요 없습니다. 고치지 마세요)
// 채점기(--grade)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void pt01_ranking_hidden()
{
    const std::vector<ptr1::Player> ranking = {
        { "Selene", 5100 },
        { "Ivo", 4870 },
        { "Nell", 4020 },
        { "Bram", 990 },
    };

    const auto board = ptr1::buildBoard(ranking);
    expect("1위 줄", std::string("1위 Selene (5100점)"), std::string(board[0]));
    expect("2위 줄", std::string("2위 Ivo (4870점)"), std::string(board[1]));
    expect("4위 줄", std::string("4위 Bram (990점)"), std::string(board[3]));
}
#pragma endregion
