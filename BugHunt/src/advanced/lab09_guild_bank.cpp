#include "../labs.h"
#include <chrono>
#include <latch>
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
        std::this_thread::sleep_for(std::chrono::milliseconds(1));   // 장부를 확인하는 동안 다른 길드원도 장부를 본다
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
    constexpr int kDepositsPerMember = 50;

    GuildBank bank;

    // 길드원 4명이 동시에 1골드씩 50번 입금한다.
    std::latch start(kMembers);   // 4명이 모두 모이면 함께 시작한다
    std::vector<std::thread> members;
    for (int m = 0; m < kMembers; ++m)
    {
        members.emplace_back([&bank, &start] {
            start.arrive_and_wait();
            for (int i = 0; i < kDepositsPerMember; ++i)
                bank.deposit(1);
        });
    }
    for (std::thread& t : members)
        t.join();

    expect("길드 금고 골드", kMembers * kDepositsPerMember, bank.gold());
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (편집기의 접기 기능으로 모두 접어 두세요)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(エディターの折りたたみ機能ですべて折りたたんでおいてください)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// 4명이 50번씩 넣었는데 금고에는 50골드 남짓뿐입니다. 사라진 입금은 어디로 갔을까요? 여러 스레드가 같은 값을 고칠 때 무엇을 의심해야 할까요?

// [JA] 4人が50回ずつ入れたのに、金庫には50ゴールドほどしかありません。消えた入金はどこへ行ったのでしょうか? 複数のスレッドが同じ値を書き換えるとき、何を疑うべきでしょうか?
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// deposit 안에 중단점을 걸고 스레드 목록을 보세요.
// 두 스레드가 같은 current 값을 읽는 상황을 상상해 보세요.

// [JA] deposit の中にブレークポイントを置いて、スレッド一覧を見てください。
// [JA] 二つのスレッドが同じ current の値を読む状況を想像してみてください。
#pragma endregion

#pragma region 참고 / 参考
// 데이터 경쟁(data race)은 정의되지 않은 동작입니다. sleep_for가 없으면 최적화 컴파일러가 반복을 합쳐
// 우연히 맞게 나올 수도 있지만, 그래도 버그입니다.

// [JA] データ競合(data race)は未定義動作です。sleep_for がないと、最適化コンパイラーがループをまとめて
// [JA] たまたま正しい結果が出ることもありますが、それでもバグです。
#pragma endregion
