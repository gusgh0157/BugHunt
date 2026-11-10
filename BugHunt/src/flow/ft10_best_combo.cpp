#include "../labs.h"
#include <vector>

// 공격이 맞으면 콤보가 1 오르고, 빗나가면 0으로 돌아간다.
// best에는 이번 전투의 최고 콤보를 남긴다.

// [JA] 攻撃が当たるとコンボが1上がり、外れると0に戻ります。
// [JA] bestにはこの戦闘の最高コンボを記録します。
namespace flow10
{
#define BUGHUNT_MAX(a, b) ((a) > (b) ? (a) : (b))

struct Combo
{
    int current = 0;
    int best = 0;
};

void onAttack(Combo& c, bool hit)
{
    if (!hit)
    {
        c.current = 0;
        return;
    }
    // 콤보를 올리면서 최고 기록과 비교한다.
    // [JA] コンボを増やしながら最高記録と比べます。
    c.best = BUGHUNT_MAX(c.best, ++c.current);
}
}

void ft10_best_combo()
{
    using namespace flow10;

    // 맞음 x3, 빗나감, 맞음 x2
    // [JA] 命中 x3、ミス、命中 x2
    const std::vector<bool> attacks = { true, true, true, false, true, true };

    Combo combo;
    std::vector<int> shown;           // 매 공격 뒤 화면에 띄운 콤보 숫자 [JA] 攻撃のたびに画面に表示したコンボ数
    for (bool hit : attacks)
    {
        onAttack(combo, hit);
        shown.push_back(combo.current);
    }

    std::cout << i18n::L("  화면에 뜬 콤보:", "  画面のコンボ:");
    for (int n : shown)
        std::cout << ' ' << n;
    std::cout << '\n';

    expect(i18n::L("세 번째 공격 뒤 콤보", "3回目の攻撃後のコンボ"), 3, shown[2]);
    expect(i18n::L("최고 콤보", "最高コンボ"), 3, combo.best);
    expect(i18n::L("마지막 콤보", "最後のコンボ"), 2, combo.current);
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(Ctrl+M, Ctrl+O = すべて折りたたむ)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// c.current에 데이터 중단점(값이 바뀌면 멈추는 중단점, watchpoint)을 걸어 보세요.
// 공격 한 번에 몇 번 멈추나요? 멈출 때마다 호출 스택의 맨 위 줄이 어디인지 보세요.

// [JA] c.currentにデータ ブレークポイント(値が変わると止まるブレークポイント、ウォッチポイント)を設定してください。
// [JA] 攻撃1回で何回止まりますか? 止まるたびに呼び出し履歴(コール スタック)の一番上の行がどこかを見てください。
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// BUGHUNT_MAX는 함수가 아니라 매크로입니다. 인자 자리에 ++c.current를 그대로 붙여 넣어 펼쳐 보세요.
// 조건 식이 참일 때와 거짓일 때 ++c.current는 각각 몇 번 계산되나요?

// [JA] BUGHUNT_MAXは関数ではなくマクロです。引数の位置に++c.currentをそのまま貼り付けて展開してみてください。
// [JA] 条件式が真のときと偽のとき、++c.currentはそれぞれ何回評価されますか?
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// 부작용(++, 함수 호출)이 있는 식은 매크로 인자로 넘기지 마세요.
// 값을 먼저 올리고 비교만 따로 하거나, 인자를 한 번만 계산하는 std::max를 쓰세요.

// [JA] 副作用(++、関数呼び出し)のある式をマクロの引数に渡さないでください。
// [JA] 先に値を増やしてから比較だけを別に行うか、引数を一度だけ評価するstd::maxを使ってください。
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(grade.bat)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
// [JA] 採点器(grade.bat)が、画面には表示されない別のデータで同じコードをもう一度検査します。
// [JA] 期待値をハードコードして [PASS] にしても、ここで引っかかります。
void ft10_best_combo_hidden()
{
    using namespace flow10;

    Combo c;
    c.best = 4;                       // 이전 구간에서 4콤보를 이미 찍었다 [JA] 前の区間ですでに4コンボを記録している
    const bool pattern[] = { true, true, false, true, true, true, true, true, true };
    const int expected[] = { 1, 2, 0, 1, 2, 3, 4, 5, 6 };
    for (int i = 0; i < 9; ++i)
    {
        onAttack(c, pattern[i]);
        expect(i18n::L("공격 뒤 콤보", "攻撃後のコンボ"), expected[i], c.current);
    }
    expect(i18n::L("숨은 전투: 최고 콤보", "隠し戦闘: 最高コンボ"), 6, c.best);
}
#pragma endregion
