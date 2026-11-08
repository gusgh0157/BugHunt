#include "c1_auction.h"
#include "../i18n/i18n.h"

// 챌린지 C-1: 경매장 화면의 패널들. 게임 루프가 틈날 때마다 refresh를 부른다.

// [JA] チャレンジ C-1: オークション会場画面のパネルです。ゲームループが手の空いたときに refresh を呼びます。
namespace chal1
{
namespace
{
const char* tierName(Tier tier)
{
    switch (tier)
    {
    case Tier::Gold: return i18n::L("골드", "ゴールド");
    case Tier::Silver: return i18n::L("실버", "シルバー");
    default: return i18n::L("일반", "一般");
    }
}
}

void BidderPanel::refresh(AuctionHouse& house)
{
    lines_.clear();
    house.forEachByTier([this](const Bidder& b) {
        lines_.push_back(std::string("[") + tierName(b.tier) + "] " + b.name + i18n::L(" - 금화 ", " - 金貨 ") + std::to_string(b.gold));
    });
}

void StatusPanel::refresh(const AuctionHouse& house)
{
    if (!house.leader())
    {
        text_ = house.item() + i18n::L(": 입찰 없음", ": 入札なし");
        return;
    }
    text_ = house.item() + i18n::L(": 최고가 ", ": 最高額 ") + std::to_string(house.highestBid()) + " (" + house.bidder(*house.leader()).name + ")";
}
}
