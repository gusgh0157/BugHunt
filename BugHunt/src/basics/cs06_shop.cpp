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
