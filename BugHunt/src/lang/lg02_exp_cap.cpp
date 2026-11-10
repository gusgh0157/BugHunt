#include "../labs.h"

// 이벤트 기간에는 몬스터 경험치에 배율이 붙는다. 누적 경험치는 상한(20억)을 넘지 않게 자른다.

// [JA] イベント期間中はモンスターの経験値に倍率が付きます。累積経験値は上限(20億)を超えないように切り詰めます。
namespace lang2
{
constexpr int kMaxExp = 2'000'000'000;

// 실제 게임에서는 서버가 보내 준 값이다. 컴파일러가 값을 미리 계산해 두지 못하게 실행 중에 읽는다.
// [JA] 実際のゲームではサーバーが送ってきた値です。コンパイラーが値を前もって計算しておけないように、実行中に読み込みます。
int fromServer(int value)
{
    volatile int received = value;
    return received;
}

// 처치 보상을 더한 누적 경험치
// [JA] 討伐報酬を足した累積経験値
int addExp(int current, int baseExp, int multiplier)
{
    int total = current + baseExp * multiplier;
    if (total > kMaxExp)            // 상한 검사 [JA] 上限の検査
        total = kMaxExp;
    return total;
}
}

void lg02_exp_cap()
{
    using namespace lang2;

    const int current = fromServer(1'950'000'000);
    const int dragonExp = fromServer(100'000'000);
    const int eventMultiplier = fromServer(2);

    const int after = addExp(current, dragonExp, eventMultiplier);
    std::cout << i18n::L("  고대 용 처치: ", "  古代竜討伐: ") << current << " + " << dragonExp << " x " << eventMultiplier << " → " << after << '\n';

    const int small = addExp(fromServer(1'000), fromServer(250), fromServer(2));
    std::cout << i18n::L("  슬라임 처치: 1000 + 250 x 2 → ", "  スライム討伐: 1000 + 250 x 2 → ") << small << '\n';

    expect(i18n::L("슬라임 처치 뒤 경험치", "スライム討伐後の経験値"), 1'500, small);
    expect(i18n::L("고대 용 처치 뒤 경험치 (상한)", "古代竜討伐後の経験値(上限)"), kMaxExp, after);
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(Ctrl+M, Ctrl+O = すべて折りたたむ)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// addExp의 if 줄에 조건부 중단점(total < 0)을 걸어 보세요. 멈췄을 때 조사식에 current, baseExp * multiplier, total을 넣고 16진수 표시로도 보세요.
// 상한 검사는 실행되었나요? total이 이미 틀린 값이라면, 틀린 값은 어느 연산에서 처음 생겼나요?

// [JA] addExp の if の行に条件付きブレークポイント(total < 0)を置いてみてください。止まったら、ウォッチに current、baseExp * multiplier、total を入れて16進表示でも見てください。
// [JA] 上限チェックは実行されましたか? total がすでにおかしな値なら、おかしな値はどの演算で最初に生まれましたか?
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// int의 최댓값은 2,147,483,647입니다. 19억 5천만 + 2억은 int에 담을 수 없습니다.
// 부호 있는 정수의 오버플로는 정의되지 않은 동작입니다. 실제로는 대개 음수로 돌아가지만(랩어라운드), 컴파일러는 "넘치지 않는다"고 가정해도 됩니다.
// 그래서 넘친 "뒤에" 검사하는 코드는 틀립니다. 이 랩의 if는 이미 망가진 값을 보고, if (a + b < a) 같은 검사는 Release 최적화가 통째로 지워 버리기도 합니다.

// [JA] int の最大値は 2,147,483,647 です。19億5千万 + 2億は int に入りません。
// [JA] 符号付き整数のオーバーフローは未定義動作です。実際にはたいてい負の数に回り込みますが(ラップアラウンド)、コンパイラーは「オーバーフローしない」と仮定してかまいません。
// [JA] そのため、オーバーフローした「後で」チェックするコードは間違いです。このラボの if はすでに壊れた値を見ていますし、if (a + b < a) のようなチェックは Release の最適化が丸ごと消してしまうこともあります。
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// 넘치기 전에 막으세요. 더 넓은 형식으로 계산하고(long long total = static_cast<long long>(current) + static_cast<long long>(baseExp) * multiplier;)
// 상한으로 자른 뒤 int로 돌려줍니다. 곱셈도 넘칠 수 있으니 곱하기 전에 넓혀야 합니다. 또는 std::numeric_limits로 "더하기 전에" 남은 여유를 검사합니다.

// [JA] オーバーフローする前に防いでください。より広い型で計算し(long long total = static_cast<long long>(current) + static_cast<long long>(baseExp) * multiplier;)、
// [JA] 上限で切り詰めてから int で返します。掛け算もオーバーフローしうるので、掛ける前に広げる必要があります。または std::numeric_limits で「足す前に」残りの余裕をチェックします。
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(grade.bat)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
// [JA] 採点器(grade.bat)が、画面には表示されない別のデータで同じコードをもう一度検査します。
// [JA] 期待値をハードコードして [PASS] にしても、ここで引っかかります。
void lg02_exp_cap_hidden()
{
    using namespace lang2;
    expect(i18n::L("숨은 보상: 넘치지 않는 상한", "隠し報酬: オーバーフローしない上限"), kMaxExp, addExp(fromServer(1'990'000'000), fromServer(20'000'000), fromServer(1)));
    expect(i18n::L("숨은 보상: 곱셈이 넘쳐 양수로 돌아옴", "隠し報酬: 掛け算があふれて正の数に戻る"), kMaxExp, addExp(fromServer(100), fromServer(1'500'000'000), fromServer(3)));
    expect(i18n::L("숨은 보상: 덧셈이 넘침", "隠し報酬: 足し算があふれる"), kMaxExp, addExp(fromServer(2'000'000'000), fromServer(147'483'648), fromServer(1)));
    expect(i18n::L("숨은 보상: 보통 처치", "隠し報酬: 通常の討伐"), 1'234'567, addExp(fromServer(1'000'000), fromServer(78'189), fromServer(3)));
}
#pragma endregion
