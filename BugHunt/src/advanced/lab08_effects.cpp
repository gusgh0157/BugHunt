#include "../labs.h"
#include <memory>
#include <vector>

namespace
{
// 화상의 턴별 피해표. 효과 하나마다 하나씩 만들어지고, 효과가 사라질 때 함께 사라져야 한다.
// 몇 개가 살아 있는지 세어 두었다가 누수를 확인한다.
class DamageSchedule
{
public:
    DamageSchedule() { ++s_alive; }
    DamageSchedule(const DamageSchedule&) = delete;
    DamageSchedule& operator=(const DamageSchedule&) = delete;
    ~DamageSchedule() { --s_alive; }

    int at(size_t turn) const { return turn < perTurn_.size() ? perTurn_[turn] : 0; }
    size_t turns() const { return perTurn_.size(); }

    static int alive() { return s_alive; }

private:
    std::vector<int> perTurn_ = { 8, 6, 4, 2 };
    static inline int s_alive = 0;
};

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
        if (turn_ < schedule_.turns())
            hp -= schedule_.at(turn_++);
    }

private:
    DamageSchedule schedule_;
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
    simulateBattles(1000);

    expect("전투 1000번 후 메모리에 남은 화상 피해표", 0, DamageSchedule::alive());
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (편집기의 접기 기능으로 모두 접어 두세요)
// ============================================================================

#pragma region 힌트 1 (먼저 직접 추적해 보고 펼치세요)
// DamageSchedule의 소멸자에 중단점을 걸어 보세요. 불리나요?
#pragma endregion

#pragma region 힌트 2
// unique_ptr<StatusEffect>는 어떤 타입의 포인터로 delete를 할까요?
// 빌드 경고도 다시 읽어 보세요. (MSVC /W4의 C5205, GCC·Clang -Wall의 -Wdelete-non-virtual-dtor)
#pragma endregion
