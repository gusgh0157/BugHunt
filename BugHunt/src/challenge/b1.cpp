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
        { i18n::L("대장장이", "鍛冶屋"), { { i18n::L("단검", "短剣"), 40 }, { i18n::L("장검", "長剣"), 120 }, { i18n::L("대검", "大剣"), 300 } } },
        { i18n::L("약초상", "薬草屋"), { { i18n::L("해독제", "解毒剤"), 30 }, { i18n::L("회복약", "回復薬"), 25 } } },
        { i18n::L("떠돌이 상인", "旅の商人"), {} },
        { i18n::L("보석상", "宝石商"), { { i18n::L("루비", "ルビー"), 500 } } },
    };
    std::cout << i18n::L("  진열대 ", "  陳列棚 ") << market.size() << i18n::L("곳 검사\n", "か所を検査\n");
    expect(i18n::L("가격 순으로 정리된 진열대 수", "価格順に並んだ陳列棚の数"), 3, countSortedShelves(market));
}

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
void chB1_hidden()
{
    using namespace blind1;
    expect(i18n::L("빈 진열대", "空の陳列棚"), true, isSortedByPrice({}));
    expect(i18n::L("하나", "1つだけ"), true, isSortedByPrice({ { "a", 9 } }));
    expect(i18n::L("같은 가격", "同じ価格"), true, isSortedByPrice({ { "a", 5 }, { "b", 5 } }));
    expect(i18n::L("마지막만 어긋남", "最後だけずれている"), false, isSortedByPrice({ { "a", 1 }, { "b", 2 }, { "c", 3 }, { "d", 2 } }));
    expect(i18n::L("진열대 수", "陳列棚の数"), 2, countSortedShelves({ { "x", {} }, { "y", { { "a", 3 }, { "b", 1 } } }, { "z", { { "a", 1 } } } }));
}
#pragma endregion
