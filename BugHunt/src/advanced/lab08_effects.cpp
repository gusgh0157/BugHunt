#include "../labs.h"
#include <memory>
#include <vector>

namespace
{
// 화상의 턴별 피해표. 효과 하나마다 하나씩 만들어지고, 효과가 사라질 때 함께 사라져야 한다.
// 몇 개가 살아 있는지 세어 두었다가 누수를 확인한다.

// [JA] やけどのターンごとのダメージ表です。効果一つごとに一つずつ作られ、効果が消えるときに一緒に消えなければなりません。
// [JA] いくつ生きているかを数えておき、リークを確認します。
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

    expect(i18n::L("전투 1000번 후 메모리에 남은 화상 피해표", "戦闘1000回後にメモリに残ったやけどダメージ表"), 0, DamageSchedule::alive());
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (편집기의 접기 기능으로 모두 접어 두세요)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(エディターの折りたたみ機能ですべて折りたたんでおいてください)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// DamageSchedule의 소멸자에 중단점을 걸어 보세요. 불리나요?

// [JA] DamageSchedule のデストラクターにブレークポイントを置いてみてください。呼ばれますか?
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// unique_ptr<StatusEffect>는 어떤 타입의 포인터로 delete를 할까요?
// 빌드 경고도 다시 읽어 보세요. (MSVC /W4의 C5205, Clang -Wall의 -Wdelete-abstract-non-virtual-dtor)
// (경고는 표준 헤더 안의 std::default_delete에서 납니다. MSVC는 이 저장소의 CMake가 켜 둔 /external:templates-가 있어야 보이고,
//  GCC는 시스템 헤더 안이라 보이지 않을 수 있습니다.)

// [JA] unique_ptr<StatusEffect> はどの型のポインターとして delete するでしょうか?
// [JA] ビルド警告ももう一度読んでみてください。(MSVC /W4 の C5205、Clang -Wall の -Wdelete-abstract-non-virtual-dtor)
// [JA] (警告は標準ヘッダー内の std::default_delete で出ます。MSVC はこのリポジトリの CMake が有効にしている /external:templates- があるときに見え、
// [JA]  GCC はシステムヘッダーの中なので見えないことがあります。)
#pragma endregion
