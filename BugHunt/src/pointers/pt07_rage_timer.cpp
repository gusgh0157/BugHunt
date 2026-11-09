#include "../labs.h"
#include <functional>
#include <string>
#include <vector>

// 턴제 전투의 버프. "분노"를 쓰면 공격력이 절반만큼 오르고, 3턴 뒤에 원래대로 돌아온다.
// 3턴 뒤에 할 일은 람다로 만들어 타이머에 맡겨 둔다.

// [JA] ターン制バトルのバフです。「怒り」を使うと攻撃力が半分だけ上がり、3ターン後に元に戻ります。
// [JA] 3ターン後にやることはラムダにして、タイマーに預けておきます。
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
    std::cout << "  " << hero.name << i18n::L(" 분노! 공격력 ", " 怒り! 攻撃力 ") << hero.attack << " (+" << bonus << ")\n";

    timers.after(3, [&] {
        hero.attack -= bonus;
        std::cout << "  " << hero.name << i18n::L(" 분노 끝. 공격력 ", " の怒りが切れた。攻撃力 ") << hero.attack << '\n';
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

    expect(i18n::L("분노가 끝난 Kael 공격력", "怒りが切れた Kael の攻撃力"), 60, kael.attack);
    expect(i18n::L("분노가 끝난 Doran 공격력", "怒りが切れた Doran の攻撃力"), 90, doran.attack);
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (편집기의 접기 기능으로 모두 접어 두세요)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(エディターの折りたたみ機能ですべて折りたたんでおいてください)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// 람다 안의 hero.attack -= bonus; 줄에 중단점을 걸고 디버거로 실행. 멈추면 지역 변수에서 bonus를 보세요.
// 분노를 걸 때 출력된 (+30)과 같은 값인가요? 호출 스택을 보면 지금 castRage는 실행 중인가요?

// [JA] ラムダの中の hero.attack -= bonus; の行にブレークポイントを置き、デバッガーで実行してください。止まったら、ローカル変数で bonus を見てください。
// [JA] 怒りをかけたときに出力された (+30) と同じ値ですか? 呼び出し履歴を見ると、今 castRage は実行中でしょうか?
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// 람다를 펼치면 캡처한 것들이 보입니다. bonus는 값으로 들어 있나요, 아니면 어딘가를 가리키나요?
// 조사식에 &bonus 를 넣고, castRage 안에서 본 &bonus와 비교해 보세요. 그 주소는 지금 누구의 스택인가요?

// [JA] ラムダを展開すると、キャプチャしたものが見えます。bonus は値として入っていますか、それともどこかを指していますか?
// [JA] ウォッチに &bonus を入れ、castRage の中で見た &bonus と比べてみてください。そのアドレスは今、誰のスタックでしょうか?
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// [&]는 "보이는 것을 모두 참조로" 캡처합니다. 람다가 함수보다 오래 산다면 어떤 것은 값으로 가져가야 합니다.
// ASan 구성으로도 실행해 보세요. 잡히나요? 왜 그런지는 README의 "람다 캡처와 ASan"에 있습니다.

// [JA] [&] は「見えるものをすべて参照で」キャプチャします。ラムダが関数より長く生きるなら、値で持っていくべきものがあります。
// [JA] ASan 構成でも実行してみてください。検出されますか? なぜそうなるのかは README の「ラムダキャプチャと ASan」にあります。
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(--grade)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
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
    expect(i18n::L("분노 중인 Pia 공격력 (7 + 3)", "怒り中の Pia の攻撃力(7 + 3)"), 10, heroes[2].attack);
    timers.advance(10);
    expect(i18n::L("Nell 공격력", "Nell の攻撃力"), 41, heroes[0].attack);
    expect(i18n::L("Ivo 공격력", "Ivo の攻撃力"), 120, heroes[1].attack);
    expect(i18n::L("Pia 공격력", "Pia の攻撃力"), 7, heroes[2].attack);
}
#pragma endregion
