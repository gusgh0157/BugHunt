#include "../labs.h"
#include <string>
#include <vector>

// 챌린지 B-1

// [JA] チャレンジ B-1
namespace blind1
{
struct ShopItem
{
    std::string name;
    int price;
};

// 진열대의 물건이 가격 오름차순으로 놓여 있는가?

// [JA] 陳列棚の品物が価格の昇順に並んでいますか?
bool isSortedByPrice(const std::vector<ShopItem>& shelf)
{
    for (size_t i = 0; i < shelf.size() - 1; ++i)
    {
        if (shelf[i].price > shelf[i + 1].price)
            return false;
    }
    return true;
}

struct Shelf
{
    std::string owner;
    std::vector<ShopItem> items;
};

int countSortedShelves(const std::vector<Shelf>& shelves)
{
    int sorted = 0;
    for (const Shelf& s : shelves)
    {
        if (isSortedByPrice(s.items))
            ++sorted;
    }
    return sorted;
}
}

void chB1()
{
    using namespace blind1;
    const std::vector<Shelf> market = {
        { "대장장이", { { "단검", 40 }, { "장검", 120 }, { "대검", 300 } } },
        { "약초상", { { "해독제", 30 }, { "회복약", 25 } } },
        { "떠돌이 상인", {} },
        { "보석상", { { "루비", 500 } } },
    };
    std::cout << "  진열대 " << market.size() << "곳 검사\n";
    expect("가격 순으로 정리된 진열대 수", 3, countSortedShelves(market));
}

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
void chB1_hidden()
{
    using namespace blind1;
    expect("빈 진열대", true, isSortedByPrice({}));
    expect("하나", true, isSortedByPrice({ { "a", 9 } }));
    expect("같은 가격", true, isSortedByPrice({ { "a", 5 }, { "b", 5 } }));
    expect("마지막만 어긋남", false, isSortedByPrice({ { "a", 1 }, { "b", 2 }, { "c", 3 }, { "d", 2 } }));
    expect("진열대 수", 2, countSortedShelves({ { "x", {} }, { "y", { { "a", 3 }, { "b", 1 } } }, { "z", { { "a", 1 } } } }));
}
#pragma endregion
