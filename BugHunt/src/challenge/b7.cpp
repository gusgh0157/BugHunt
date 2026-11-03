#include "../labs.h"
#include <numeric>
#include <string>
#include <vector>

// 챌린지 B-7
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
double withShipping(const std::vector<Order>& cart)
{
    const double total = cartTotal(cart);
    return total >= 10.0 ? total : total + 1.5;
}
}

void chB7()
{
    using namespace blind7;
    const std::vector<Order> cart = { { "회복약", 2.5 }, { "화살 묶음", 3.75 }, { "횃불", 1.25 }, { "밧줄", 2.5 } };
    std::cout << "  장바구니 " << cart.size() << "개\n";
    expect("결제 금액 (10금화, 무료 배송)", 10.0, withShipping(cart));
}

#pragma region 채점용 숨은 테스트 (풀이에는 필요 없습니다. 고치지 마세요)
void chB7_hidden()
{
    using namespace blind7;
    expect("합계", 4.5, cartTotal({ { "a", 1.5 }, { "b", 1.5 }, { "c", 1.5 } }));
    expect("빈 장바구니 배송비", 1.5, withShipping({}));
    expect("9.9금화는 배송비 붙음", 11.4, withShipping({ { "a", 9.9 } }));
    expect("정수 금액", 12.0, withShipping({ { "a", 7.0 }, { "b", 5.0 } }));
}
#pragma endregion
