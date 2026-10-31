#include "../labs.h"
#include <thread>
#include <vector>

namespace
{
class GuildBank
{
public:
    void deposit(int amount)
    {
        const int current = gold_;
        gold_ = current + amount;
    }

    int gold() const { return gold_; }

private:
    int gold_ = 0;
};
}

void lab09_guild_bank()
{
    constexpr int kMembers = 4;
    constexpr int kDepositsPerMember = 100000;

    GuildBank bank;

    // 길드원 4명이 동시에 1골드씩 10만 번 입금한다.
    std::vector<std::thread> members;
    for (int m = 0; m < kMembers; ++m)
    {
        members.emplace_back([&bank] {
            for (int i = 0; i < kDepositsPerMember; ++i)
                bank.deposit(1);
        });
    }
    for (std::thread& t : members)
        t.join();

    expect("길드 금고 골드", kMembers * kDepositsPerMember, bank.gold());
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// ============================================================================

#pragma region 힌트 1 (먼저 직접 추적해 보고 펼치세요)
// 여러 번 실행해 보세요. 결과가 매번 다르면 무엇을 의심해야 할까요?
#pragma endregion

#pragma region 힌트 2
// deposit 안에 중단점을 걸고 스레드 창을 보세요.
// 두 스레드가 같은 current 값을 읽는 상황을 상상해 보세요.
#pragma endregion

#pragma region 참고
// Release에서는 최적화 때문에 우연히 맞게 나올 수도 있습니다. 그래도 버그입니다.
#pragma endregion
