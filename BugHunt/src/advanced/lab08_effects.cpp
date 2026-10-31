#include "../labs.h"
#include <memory>
#include <vector>

#ifdef _DEBUG
#include <crtdbg.h>
#endif

namespace
{
class StatusEffect
{
public:
    ~StatusEffect() = default;
    virtual void tick(int& hp) = 0;
};

class Burn : public StatusEffect
{
public:
    void tick(int& hp) override
    {
        if (turn_ < damagePerTurn_.size())
            hp -= damagePerTurn_[turn_++];
    }

private:
    std::vector<int> damagePerTurn_ = { 8, 6, 4, 2 };
    size_t turn_ = 0;
};

void simulateBattles(int battleCount)
{
    for (int i = 0; i < battleCount; ++i)
    {
        int hp = 100;
        std::unique_ptr<StatusEffect> effect = std::make_unique<Burn>();
        for (int t = 0; t < 4; ++t)
            effect->tick(hp);
    }
}
}

void lab08_effects()
{
#if defined(_DEBUG) && !defined(__SANITIZE_ADDRESS__)
    _CrtMemState before, after, diff;
    _CrtMemCheckpoint(&before);

    simulateBattles(1000);

    _CrtMemCheckpoint(&after);
    long long leakedBytes = 0;
    if (_CrtMemDifference(&diff, &before, &after))
        leakedBytes = static_cast<long long>(diff.lSizes[_NORMAL_BLOCK]);

    expect("전투 1000번 후 새어 나간 바이트", 0LL, leakedBytes);
#else
    simulateBattles(1000);
    std::cout << "  이 랩의 누수 측정은 Debug 구성에서만 동작한다. Debug로 다시 빌드하세요.\n";
#endif
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// ============================================================================

#pragma region 힌트 1 (먼저 직접 추적해 보고 펼치세요)
// Burn에 소멸자를 직접 만들고 중단점을 걸어 보세요. 불리나요?
#pragma endregion

#pragma region 힌트 2
// unique_ptr<StatusEffect>는 어떤 타입의 포인터로 delete를 할까요?
// 빌드 경고도 다시 읽어 보세요.
#pragma endregion
