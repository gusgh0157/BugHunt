#include "../labs.h"
#include <vector>

// 파티의 치유사들이 "기본 치유"를 한 번씩 쓴다. 치유량을 정하지 않으면 직업마다 정한 기본값을 쓴다.
// 사제의 기본 치유량은 25 (+신성 보너스 5)여야 하는데, 훨씬 적게 회복된다.

// [JA] パーティーのヒーラーたちが「基本回復」を1回ずつ使います。回復量を指定しなければ、職業ごとに決めた既定値を使います。
// [JA] プリーストの基本回復量は25(+神聖ボーナス5)のはずなのに、ずっと少なく回復します。
namespace inh4
{
class Healer
{
public:
    virtual ~Healer() = default;
    virtual int heal(int amount = 10) const { return amount; }
};

class Priest : public Healer
{
public:
    int heal(int amount = 25) const override { return amount + 5; }    // 신성 보너스
};

class Druid : public Healer
{
public:
    int heal(int amount = 18) const override { return amount + 2; }    // 자연 보너스
};

// 전투 코드: 치유사 종류를 몰라도 기본 치유를 쓴다
int castDefaultHeal(const Healer& healer)
{
    return healer.heal();
}
}

void ih04_priest_heal()
{
    using namespace inh4;

    const Healer novice;
    const Priest priest;
    const std::vector<const Healer*> party = { &novice, &priest };

    int total = 0;
    for (const Healer* h : party)
        total += castDefaultHeal(*h);
    std::cout << i18n::L("  견습 치유사 ", "  見習いヒーラー") << castDefaultHeal(novice) << i18n::L(", 사제 ", "、プリースト") << castDefaultHeal(priest)
              << i18n::L(" (직접 부르면 ", "(直接呼ぶと") << priest.heal() << ")\n";

    expect("사제의 기본 치유", 30, castDefaultHeal(priest));
    expect("파티 총 치유량", 40, total);
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(Ctrl+M, Ctrl+O = すべて折りたたむ)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// Priest::heal에 중단점을 걸고 castDefaultHeal(priest)에서 멈추면 amount를 보세요.
// 출력의 "직접 부르면" 값과 왜 다를까요? 두 호출은 같은 함수에 들어가나요?

// [JA] Priest::heal にブレークポイントを置き、castDefaultHeal(priest) で止まったら amount を見てください。
// [JA] 出力の「直接呼ぶと」の値となぜ違うのでしょうか? 2つの呼び出しは同じ関数に入っていますか?
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// 어느 함수가 불릴지는 실행 중에(객체의 실제 형식으로) 정해지지만, 기본 인자는 컴파일할 때(식의 정적 형식으로) 정해집니다.
// castDefaultHeal 안에서 healer의 정적 형식은 Healer입니다.

// [JA] どの関数が呼ばれるかは実行時に(オブジェクトの実際の型で)決まりますが、デフォルト引数はコンパイル時に(式の静的な型で)決まります。
// [JA] castDefaultHeal の中で healer の静的な型は Healer です。
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// 가상 함수에 기본 인자를 두지 마세요. 기본값도 가상 함수로 물어보게 하세요. 예:
//     int heal() const { return heal(defaultAmount()); }   // 가상이 아닌 함수
//     virtual int defaultAmount() const { return 10; }    // 직업마다 재정의
// 파생 클래스에서 heal(int)을 재정의하면 같은 이름의 기반 heal()이 가려지므로(이름 숨김), Priest와 Druid에 using Healer::heal;을 넣으세요.
// 그래야 본문의 priest.heal()이 컴파일됩니다.
// 기반의 기본값만 25로 바꾸는 땜질은 드루이드(18)에서 틀립니다.

// [JA] 仮想関数にデフォルト引数を付けないでください。既定値も仮想関数で問い合わせるようにします。例:
// [JA]     int heal() const { return heal(defaultAmount()); }   // 非仮想関数
// [JA]     virtual int defaultAmount() const { return 10; }    // 職業ごとにオーバーライド
// [JA] 派生クラスで heal(int) をオーバーライドすると、同じ名前の基底の heal() が隠れる(名前の隠蔽)ので、Priest と Druid に using Healer::heal; を入れてください。
// [JA] そうしないと本文の priest.heal() がコンパイルできません。
// [JA] 基底の既定値だけを25に変えるその場しのぎは、ドルイド(18)で間違えます。
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(grade.bat)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void ih04_priest_heal_hidden()
{
    using namespace inh4;

    expect("드루이드의 기본 치유", 20, castDefaultHeal(Druid()));
    expect("견습 치유사의 기본 치유", 10, castDefaultHeal(Healer()));
    expect("사제의 기본 치유 (숨은)", 30, castDefaultHeal(Priest()));
    expect("드루이드 40 치유", 42, Druid().heal(40));
}
#pragma endregion
