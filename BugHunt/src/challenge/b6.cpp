#include "../labs.h"
#include <string>
#include <vector>

// 챌린지 B-6

// [JA] チャレンジ B-6
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

// [JA] 今日ログインしたプレイヤーの記録から、互いに異なるプレイヤーだけを取り出します。最初に現れた順序を守ります。
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
    std::cout << i18n::L("  접속 기록 ", "  ログイン記録 ") << logins.size() << i18n::L("건, 서로 다른 플레이어 ", "件、異なるプレイヤー ")
              << unique.size() << i18n::L("명, 비교 ", "人、比較 ") << g_compares << i18n::L("번\n", "回\n");
    expect("서로 다른 플레이어 수", static_cast<size_t>(6000), unique.size());
    expect("PlayerId 비교가 기록 수의 4배 이하인가?", true, g_compares <= 4LL * static_cast<long long>(logins.size()));
}

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
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
