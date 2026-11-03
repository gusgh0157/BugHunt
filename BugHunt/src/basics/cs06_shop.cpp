#include "../labs.h"
#include <map>
#include <string>
#include <vector>

// 상점: 장바구니 합계를 계산한다.

// [JA] ショップ: カートの合計を計算します。
namespace lab6
{
const std::map<std::string, int> kPrices = {
    { "healing_potion",  50 },
    { "mana_potion",     70 },
    { "iron_sword",     300 },
    { "oak_shield",     180 },
};

int priceOf(const std::string& itemId)
{
    return kPrices.at(itemId);
}

int cartTotal(const std::vector<std::string>& cart)
{
    int total = 0;
    for (const std::string& itemId : cart)
        total += priceOf(itemId);
    return total;
}

void checkout()
{
    const std::vector<std::string> cart = {
        "iron_sword",
        "healing_potion",
        "healing_potion",
        "mana_poiton",
        "oak_shield",
    };
    std::cout << "  합계: " << cartTotal(cart) << " 골드\n";
}
}

void cs06_shop()
{
    lab6::checkout();
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (편집기의 접기 기능으로 모두 접어 두세요)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(エディターの折りたたみ機能ですべて折りたたんでおいてください)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 호출 스택을 보고 펼치세요 / まず呼び出し履歴を見てから開いてください)
// 디버거가 "처리되지 않은 예외: std::out_of_range"로 멈춥니다.
// std::map::at 같은 표준 라이브러리 줄 아래에서 첫 lab6:: 줄을 찾으세요.

// [JA] デバッガーが「ハンドルされない例外: std::out_of_range」で止まります。
// [JA] std::map::at のような標準ライブラリの行の下から、最初の lab6:: の行を探してください。
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// priceOf 프레임의 itemId를 kPrices의 키와 비교해 보세요.

// [JA] priceOf フレームの itemId を kPrices のキーと比べてみてください。
#pragma endregion

#pragma region 보너스 / ボーナス
// 디버거에서 "C++ 예외가 던져질 때 멈춤"(catch throw)을 켜면
// 예외를 잡든 안 잡든 던져지는 순간에 멈춥니다.

// [JA] デバッガーで「C++ 例外がスローされたときに中断」(catch throw)をオンにすると、
// [JA] 例外をキャッチするかどうかにかかわらず、スローされた瞬間に止まります。
#pragma endregion
