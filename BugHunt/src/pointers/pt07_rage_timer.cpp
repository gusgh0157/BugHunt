#include "../labs.h"
#include <functional>
#include <string>
#include <vector>

// 턴제 전투의 버프. "분노"를 쓰면 공격력이 절반만큼 오르고, 3턴 뒤에 원래대로 돌아온다.
// 3턴 뒤에 할 일은 람다로 만들어 타이머에 맡겨 둔다.
namespace ptr7
{
struct Hero
{
    std::string name;
    int attack;
};

class TurnTimers
{
public:
    void after(int turns, std::function<void()> action) { pending_.push_back({ now_ + turns, std::move(action) }); }

    void advance(int turns)
    {
        for (int t = 0; t < turns; ++t)
        {
            ++now_;
            for (Pending& p : pending_)
            {
                if (p.dueTurn == now_ && p.action)
                {
                    p.action();
                    p.action = nullptr;
                }
            }
        }
    }

private:
    struct Pending
    {
        int dueTurn;
        std::function<void()> action;
    };
    std::vector<Pending> pending_;
    int now_ = 0;
};

int rageBonus(const Hero& hero)
{
    return hero.attack / 2;
}

void castRage(TurnTimers& timers, Hero& hero)
{
    const int bonus = rageBonus(hero);
    hero.attack += bonus;
    std::cout << "  " << hero.name << " 분노! 공격력 " << hero.attack << " (+" << bonus << ")\n";

    timers.after(3, [&] {
        hero.attack -= bonus;
        std::cout << "  " << hero.name << " 분노 끝. 공격력 " << hero.attack << '\n';
    });
}
}

void pt07_rage_timer()
{
    using namespace ptr7;

    Hero kael{ "Kael", 60 };
    Hero doran{ "Doran", 90 };
    TurnTimers timers;

    castRage(timers, kael);
    timers.advance(1);
    castRage(timers, doran);
    timers.advance(5);

    expect("분노가 끝난 Kael 공격력", 60, kael.attack);
    expect("분노가 끝난 Doran 공격력", 90, doran.attack);
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// ============================================================================

#pragma region 힌트 1 (먼저 직접 추적해 보고 펼치세요)
// 람다 안의 hero.attack -= bonus; 줄에 중단점을 걸고 F5. 멈추면 지역 창에서 bonus를 보세요.
// 분노를 걸 때 출력된 (+30)과 같은 값인가요? 호출 스택을 보면 지금 castRage는 실행 중인가요?
#pragma endregion

#pragma region 힌트 2
// 람다를 펼치면 캡처한 것들이 보입니다. bonus는 값으로 들어 있나요, 아니면 어딘가를 가리키나요?
// 조사식에 &bonus 를 넣고, castRage 안에서 본 &bonus와 비교해 보세요. 그 주소는 지금 누구의 스택인가요?
#pragma endregion

#pragma region 힌트 3
// [&]는 "보이는 것을 모두 참조로" 캡처합니다. 람다가 함수보다 오래 산다면 어떤 것은 값으로 가져가야 합니다.
// ASan 구성으로도 실행해 보세요. 잡히나요? 왜 그런지는 README의 "람다 캡처와 ASan"에 있습니다.
#pragma endregion

#pragma region 채점용 숨은 테스트 (풀이에는 필요 없습니다. 고치지 마세요)
// 채점기(grade.bat)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void pt07_rage_timer_hidden()
{
    using namespace ptr7;

    Hero heroes[3] = { { "Nell", 41 }, { "Ivo", 120 }, { "Pia", 7 } };
    TurnTimers timers;
    for (Hero& h : heroes)
    {
        castRage(timers, h);
        timers.advance(2);
    }
    expect("분노 중인 Pia 공격력 (7 + 3)", 10, heroes[2].attack);
    timers.advance(10);
    expect("Nell 공격력", 41, heroes[0].attack);
    expect("Ivo 공격력", 120, heroes[1].attack);
    expect("Pia 공격력", 7, heroes[2].attack);
}
#pragma endregion
