#include "../labs.h"
#include <string>
#include <vector>

// 챌린지 B-6
namespace blind6
{
long long g_compares = 0;

struct PlayerId
{
    int server;
    int number;
    bool operator==(const PlayerId& o) const
    {
        ++g_compares;
        return server == o.server && number == o.number;
    }
};

// 오늘 접속한 플레이어 기록에서 서로 다른 플레이어만 뽑는다. 처음 나온 순서를 지킨다.
std::vector<PlayerId> uniquePlayers(const std::vector<PlayerId>& logins)
{
    std::vector<PlayerId> unique;
    for (const PlayerId& p : logins)
    {
        bool seen = false;
        for (const PlayerId& u : unique)
        {
            if (u == p)
            {
                seen = true;
                break;
            }
        }
        if (!seen)
            unique.push_back(p);
    }
    return unique;
}

std::vector<PlayerId> makeLogins(int count, int distinct)
{
    std::vector<PlayerId> logins;
    for (int i = 0; i < count; ++i)
        logins.push_back({ (i * 13) % 3, (i * 7919) % distinct });
    return logins;
}
}

void chB6()
{
    using namespace blind6;
    const auto logins = blind6::makeLogins(20000, 6000);
    g_compares = 0;
    const auto unique = uniquePlayers(logins);
    std::cout << "  접속 기록 " << logins.size() << "건, 서로 다른 플레이어 " << unique.size() << "명, 비교 " << g_compares
              << "번\n";
    expect("서로 다른 플레이어 수", static_cast<size_t>(6000), unique.size());
    expect("PlayerId 비교가 기록 수의 4배 이하인가?", true, g_compares <= 4LL * static_cast<long long>(logins.size()));
}

#pragma region 채점용 숨은 테스트 (풀이에는 필요 없습니다. 고치지 마세요)
void chB6_hidden()
{
    using namespace blind6;
    const auto logins = makeLogins(5000, 700);
    g_compares = 0;
    const auto unique = uniquePlayers(logins);
    expect("서로 다른 플레이어 수", static_cast<size_t>(2100), unique.size());
    expect("첫 플레이어", true, unique.at(0).server == 0 && unique.at(0).number == 0);
    expect("둘째 플레이어", true, unique.at(1).server == 1 && unique.at(1).number == 219);
    expect("비교 횟수", true, g_compares <= 4LL * static_cast<long long>(logins.size()));
}
#pragma endregion
