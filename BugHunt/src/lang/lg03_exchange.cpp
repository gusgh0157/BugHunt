#include "../labs.h"
#include <cmath>
#include <string>

// 상점 가격표는 골드(소수 둘째 자리까지)로 되어 있고, 거래소 정산은 은화로 한다. 1골드 = 은화 100개.
// 묶음 판매 기록(SaleRecord)은 옛날 패킷 형식을 그대로 쓴다.

// [JA] ショップの価格表はゴールド(小数第2位まで)で書かれていて、取引所の精算は銀貨で行います。1ゴールド = 銀貨100枚です。
// [JA] まとめ売りの記録(SaleRecord)は昔のパケット形式をそのまま使っています。
namespace lang3
{
struct SaleRecord
{
    int count;
    short totalSilver;   // 판매 총액 (은화) [JA] 販売総額 (銀貨)
};

// 골드 가격을 은화로
// [JA] ゴールドの価格を銀貨に
int toSilver(double gold)
{
    return gold * 100;
}

SaleRecord sellBundle(double unitGold, int count)
{
    SaleRecord record{};
    record.count = count;
    record.totalSilver = toSilver(unitGold) * count;
    return record;
}
}

void lg03_exchange()
{
    using namespace lang3;

    const double potionGold = 1.15;
    const int potionSilver = toSilver(potionGold);
    std::cout << i18n::L("  치유 물약 1개: ", "  ポーション1個: ") << potionGold << i18n::L(" 골드 → 은화 ", " ゴールド → 銀貨") << potionSilver << i18n::L("개\n", "枚\n");

    const SaleRecord bundle = sellBundle(potionGold, 300);
    std::cout << i18n::L("  치유 물약 ", "  ポーション") << bundle.count << i18n::L("개 묶음 판매: 은화 ", "個まとめ売り: 銀貨") << bundle.totalSilver << i18n::L("개\n", "枚\n");

    expect(i18n::L("치유 물약 1개 (은화)", "回復ポーション1個(銀貨)"), 115, potionSilver);
    expect(i18n::L("물약 300개 묶음 총액 (은화)", "ポーション300個まとめ売りの総額(銀貨)"), 34'500, static_cast<int>(bundle.totalSilver));
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(Ctrl+M, Ctrl+O = すべて折りたたむ)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// toSilver의 return 줄에서 멈추고 조사식에 gold * 100을 넣어 보세요. 소수점 아래까지 보이나요? 그 값이 int로 바뀌면 몇이 되나요?
// sellBundle에서는 조사식에 toSilver(unitGold) * count와 record.totalSilver를 나란히 넣어 보세요. 두 값이 언제 달라지나요?

// [JA] toSilver の return の行で止めて、ウォッチに gold * 100 を入れてみてください。小数点以下まで見えますか? その値が int に変わるといくつになりますか?
// [JA] sellBundle では、ウォッチに toSilver(unitGold) * count と record.totalSilver を並べて入れてみてください。二つの値はいつ食い違いますか?
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// 1.15는 2진수 double로 정확히 나타낼 수 없어 1.1499999999999999...로 저장되고, 100을 곱하면 114.99999999999999가 됩니다.
// double → int 변환은 반올림이 아니라 소수점 아래를 버립니다. 그래서 114.
// int → short 변환은 short(최대 32,767)에 들어가지 않는 값의 위쪽 비트를 잘라 버립니다. 34,500은 음수가 됩니다.
// 두 곳 모두 "축소 변환(narrowing)"이고, = 로 대입하거나 return할 때는 컴파일러가 아무 말 없이(또는 경고만 하고) 바꿔 버립니다.

// [JA] 1.15 は2進数の double で正確に表せないため 1.1499999999999999... として保存され、100を掛けると 114.99999999999999 になります。
// [JA] double → int の変換は四捨五入ではなく、小数点以下を切り捨てます。そのため 114 です。
// [JA] int → short の変換は、short(最大 32,767)に入らない値の上位ビットを切り落とします。34,500 は負の数になります。
// [JA] どちらも「縮小変換(narrowing)」で、= で代入したり return したりするときは、コンパイラーが何も言わずに(または警告だけ出して)変換してしまいます。
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// 중괄호 초기화는 축소 변환을 막아 줍니다. 예: int silver{ gold * 100 }; short total{ silver * count };  → MSVC·Clang에서는 두 곳이 모두 컴파일 오류로 드러납니다.
// (GCC는 상수가 아닌 값의 축소 변환에 경고만 냅니다. -Werror=narrowing을 켜면 오류가 됩니다.)
// 고치기: 은화는 반올림해서 정수로(static_cast<int>(std::lround(gold * 100))), 총액 필드는 int로 넓힙니다.
// 돈은 처음부터 정수(가장 작은 단위)로 들고 있는 것이 가장 안전합니다.

// [JA] 波かっこによる初期化は、縮小変換を防いでくれます。例: int silver{ gold * 100 }; short total{ silver * count };  → MSVC・Clang では二か所ともコンパイルエラーとして表に出ます。
// [JA] (GCC は定数でない値の縮小変換に警告だけを出します。-Werror=narrowing を付けるとエラーになります。)
// [JA] 直し方: 銀貨は四捨五入して整数にし(static_cast<int>(std::lround(gold * 100)))、総額のフィールドは int に広げます。
// [JA] お金は最初から整数(最小の単位)で持っておくのがいちばん安全です。
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(grade.bat)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
// [JA] 採点器(grade.bat)が、画面には表示されない別のデータで同じコードをもう一度検査します。
// [JA] 期待値をハードコードして [PASS] にしても、ここで引っかかります。
void lg03_exchange_hidden()
{
    using namespace lang3;
    expect(i18n::L("숨은 가격: 0.29 골드", "隠し価格: 0.29 ゴールド"), 29, toSilver(0.29));
    expect(i18n::L("숨은 가격: 4.35 골드", "隠し価格: 4.35 ゴールド"), 435, toSilver(4.35));
    expect(i18n::L("숨은 가격: 2.5 골드", "隠し価格: 2.5 ゴールド"), 250, toSilver(2.5));
    expect(i18n::L("숨은 묶음: 화살 1000개", "隠しまとめ売り: 矢1000本"), 29'000, static_cast<int>(sellBundle(0.29, 1000).totalSilver));
    expect(i18n::L("숨은 묶음: 마나 물약 500개", "隠しまとめ売り: マナポーション500個"), 217'500, static_cast<int>(sellBundle(4.35, 500).totalSilver));
}
#pragma endregion
