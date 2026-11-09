#include "../labs.h"
#include <numeric>
#include <string>
#include <vector>

// 챌린지 B-7

// [JA] チャレンジ B-7
namespace blind7
{
struct Order
{
    std::string item;
    double price;   // 금화 (소수점은 은화)
};

double cartTotal(const std::vector<Order>& cart)
{
    std::vector<double> prices;
    for (const Order& o : cart)
        prices.push_back(o.price);
    return std::accumulate(prices.begin(), prices.end(), 0);
}

// 10금화 이상이면 무료 배송, 아니면 배송비 1.5금화

// [JA] 10金貨以上なら送料無料、そうでなければ送料は1.5金貨です。
double withShipping(const std::vector<Order>& cart)
{
    const double total = cartTotal(cart);
    return total >= 10.0 ? total : total + 1.5;
}
}

void chB7()
{
    using namespace blind7;
    const std::vector<Order> cart = { { i18n::L("회복약", "回復薬"), 2.5 }, { i18n::L("화살 묶음", "矢の束"), 3.75 }, { i18n::L("횃불", "たいまつ"), 1.25 }, { i18n::L("밧줄", "ロープ"), 2.5 } };
    std::cout << i18n::L("  장바구니 ", "  カート ") << cart.size() << i18n::L("개\n", "個\n");
    expect(i18n::L("결제 금액 (10금화, 무료 배송)", "支払額(10金貨、送料無料)"), 10.0, withShipping(cart));
}

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
void chB7_hidden()
{
    using namespace blind7;
    expect(i18n::L("합계", "合計"), 4.5, cartTotal({ { "a", 1.5 }, { "b", 1.5 }, { "c", 1.5 } }));
    expect(i18n::L("빈 장바구니 배송비", "空のカートの送料"), 1.5, withShipping({}));
    expect(i18n::L("9.9금화는 배송비 붙음", "9.9金貨は送料がかかる"), 11.4, withShipping({ { "a", 9.9 } }));
    expect(i18n::L("정수 금액", "整数の金額"), 12.0, withShipping({ { "a", 7.0 }, { "b", 5.0 } }));
}
#pragma endregion
