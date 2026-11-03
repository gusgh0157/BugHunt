#include "c1_auction.h"

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
    case Tier::Gold: return "골드";
    case Tier::Silver: return "실버";
    default: return "일반";
    }
}
}

void BidderPanel::refresh(AuctionHouse& house)
{
    lines_.clear();
    house.forEachByTier([this](const Bidder& b) {
        lines_.push_back(std::string("[") + tierName(b.tier) + "] " + b.name + " - 금화 " + std::to_string(b.gold));
    });
}

void StatusPanel::refresh(const AuctionHouse& house)
{
    if (!house.leader())
    {
        text_ = house.item() + ": 입찰 없음";
        return;
    }
    text_ = house.item() + ": 최고가 " + std::to_string(house.highestBid()) + " (" + house.bidder(*house.leader()).name + ")";
}
}
