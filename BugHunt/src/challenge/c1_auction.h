#pragma once
#include <functional>
#include <optional>
#include <string>
#include <vector>

// 챌린지 C-1: 경매장.
// 입찰하면 금화를 경매장이 맡아 두고(에스크로), 더 높은 입찰이 들어오면 이전 최고 입찰자에게 돌려준다.

// [JA] チャレンジ C-1: オークション会場です。
// [JA] 入札すると金貨をオークション会場が預かり(エスクロー)、より高い入札が入ると以前の最高入札者に返します。
namespace chal1
{
enum class Tier
{
    Normal = 0,
    Silver = 1,
    Gold = 2,
};

struct Bidder
{
    std::string name;
    int gold = 0;
    Tier tier = Tier::Normal;
    int itemsWon = 0;
};

using BidderId = size_t;   // 경매에 참가한 순서 (0부터) [JA] オークションに参加した順番(0から)

class AuctionHouse
{
public:
    explicit AuctionHouse(std::string item) : item_(std::move(item)) {}

    BidderId join(std::string name, int gold, Tier tier);

    // 최고가보다 높아야 하고, 금화가 충분해야 한다. 성공하면 true.
    // [JA] 最高額より高く、金貨が足りていなければなりません。成功したら true。
    bool placeBid(BidderId who, int amount);

    // 낙찰. 최고 입찰자가 물건을 받고, 판매자는 최고가에서 수수료를 뺀 금액을 받는다.
    // [JA] 落札です。最高入札者が品物を受け取り、出品者は最高額から手数料を引いた金額を受け取ります。
    void close();

    const Bidder& bidder(BidderId id) const { return bidders_.at(id); }
    size_t bidderCount() const { return bidders_.size(); }
    std::optional<BidderId> leader() const { return leader_; }
    int highestBid() const { return highestBid_; }
    int sellerProceeds() const { return sellerProceeds_; }
    bool closed() const { return closed_; }
    const std::string& item() const { return item_; }

    // 등급이 높은 참가자부터(같은 등급은 참가 순서대로) 하나씩 보여 준다. 화면 표시용.
    // [JA] 等級の高い参加者から(同じ等級は参加順に)1人ずつ見せます。画面表示用です。
    void forEachByTier(const std::function<void(const Bidder&)>& visit);

private:
    std::string item_;
    std::vector<Bidder> bidders_;
    std::optional<BidderId> leader_;
    int highestBid_ = 0;
    int sellerProceeds_ = 0;
    bool closed_ = false;
};

constexpr int kFeePercent = 5;   // 판매 수수료 5% [JA] 販売手数料5%

// 경매장 화면의 패널들 (c1_panels.cpp). 게임 루프가 틈날 때마다 refresh를 부른다.

// [JA] オークション会場画面のパネルです(c1_panels.cpp)。ゲームループが手の空いたときに refresh を呼びます。

// 참가자 목록 (등급 높은 순)

// [JA] 参加者一覧(等級の高い順)
class BidderPanel
{
public:
    void refresh(AuctionHouse& house);
    const std::vector<std::string>& lines() const { return lines_; }

private:
    std::vector<std::string> lines_;
};

// 현재 최고가 한 줄

// [JA] 現在の最高額1行
class StatusPanel
{
public:
    void refresh(const AuctionHouse& house);
    const std::string& text() const { return text_; }

private:
    std::string text_;
};
}
