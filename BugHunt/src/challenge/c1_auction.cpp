#include "c1_auction.h"
#include <algorithm>

namespace chal1
{
BidderId AuctionHouse::join(std::string name, int gold, Tier tier)
{
    bidders_.push_back({ std::move(name), gold, tier, 0 });
    return bidders_.size() - 1;
}

bool AuctionHouse::placeBid(BidderId who, int amount)
{
    if (closed_ || who >= bidders_.size() || amount <= highestBid_)
        return false;

    Bidder& b = bidders_[who];
    if (leader_ && *leader_ == who)
    {
        // 자기 입찰을 올리는 경우: 이미 맡긴 금액과의 차액만 더 맡긴다.
        // [JA] 自分の入札を引き上げる場合: すでに預けた金額との差額だけを追加で預けます。
        const int extra = amount - highestBid_;
        if (b.gold < extra)
            return false;
        b.gold -= extra;
        highestBid_ = amount;
        return true;
    }

    if (b.gold < amount)
        return false;
    b.gold -= amount;                              // 새 최고 입찰자의 금화를 맡아 두고 [JA] 新しい最高入札者の金貨を預かっておき
    if (leader_)
        bidders_[*leader_].gold += highestBid_;    // 이전 최고 입찰자에게 돌려준다 [JA] 前の最高入札者に返す
    leader_ = who;
    highestBid_ = amount;
    return true;
}

void AuctionHouse::close()
{
    if (closed_)
        return;
    closed_ = true;
    if (!leader_)
        return;
    ++bidders_[*leader_].itemsWon;
    const int fee = highestBid_ * kFeePercent / 100;   // 수수료는 내림 (TODO: 반올림이 맞는지 기획 확인) [JA] 手数料は切り捨て (TODO: 四捨五入が正しいか企画に確認)
    sellerProceeds_ = highestBid_ - fee;
}

void AuctionHouse::forEachByTier(const std::function<void(const Bidder&)>& visit)
{
    std::stable_sort(bidders_.begin(), bidders_.end(),
                     [](const Bidder& a, const Bidder& b) { return a.tier > b.tier; });
    for (const Bidder& b : bidders_)
        visit(b);
}
}
