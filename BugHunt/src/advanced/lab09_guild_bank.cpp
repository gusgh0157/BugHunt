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
