#include "../labs.h"
#include <map>
#include <string>
#include <vector>

// 상점: 장바구니 합계를 계산한다.
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
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// ============================================================================

#pragma region 힌트 1 (먼저 호출 스택을 보고 펼치세요)
// 디버거가 "처리되지 않은 예외: std::out_of_range"로 멈춥니다.
// std::map::at 같은 외부 코드 아래에서 첫 lab6:: 줄을 찾으세요.
#pragma endregion

#pragma region 힌트 2
// priceOf 프레임의 itemId를 kPrices의 키와 비교해 보세요.
#pragma endregion

#pragma region 보너스
// Ctrl+Alt+E(예외 설정)에서 "C++ Exceptions"를 체크하면
// 예외를 잡든 안 잡든 던져지는 순간에 멈춥니다.
#pragma endregion
