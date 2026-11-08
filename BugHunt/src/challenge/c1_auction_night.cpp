#include "../labs.h"
#include "c1_auction.h"

// 챌린지 C-1
// 경매 한 건을 처음부터 끝까지 재생한다. 입찰 사이사이에 게임 루프가 화면 패널을 새로 그린다.

// [JA] チャレンジ C-1
// [JA] オークション1件を最初から最後まで再生します。入札の合間に、ゲームループが画面のパネルを描き直します。
namespace chal1
{
namespace
{
const Bidder* findByName(const AuctionHouse& house, const std::string& name)
{
    for (BidderId id = 0; id < house.bidderCount(); ++id)
    {
        if (house.bidder(id).name == name)
            return &house.bidder(id);
    }
    return nullptr;
}

int goldOf(const AuctionHouse& house, const std::string& name)
{
    const Bidder* b = findByName(house, name);
    return b ? b->gold : -1;
}

std::string winnerOf(const AuctionHouse& house)
{
    for (BidderId id = 0; id < house.bidderCount(); ++id)
    {
        if (house.bidder(id).itemsWon > 0)
            return house.bidder(id).name;
    }
    return i18n::L("(없음)", "(なし)");
}

// 게임 루프의 한 프레임: 화면을 다시 그린다.

// [JA] ゲームループの1フレーム: 画面を描き直します。
void drawFrame(AuctionHouse& house, BidderPanel& bidders, StatusPanel& status)
{
    bidders.refresh(house);
    status.refresh(house);
}
}
}

void chC1_auction_night()
{
    using namespace chal1;

    AuctionHouse house("용의 비늘 갑옷");
    BidderPanel bidderPanel;
    StatusPanel statusPanel;

    const BidderId doran = house.join("Doran", 500, Tier::Normal);
    const BidderId mira = house.join("Mira", 800, Tier::Gold);
    const BidderId kael = house.join("Kael", 600, Tier::Silver);
    drawFrame(house, bidderPanel, statusPanel);

    house.placeBid(doran, 200);
    drawFrame(house, bidderPanel, statusPanel);
    std::cout << "  " << statusPanel.text() << '\n';

    house.placeBid(kael, 300);
    drawFrame(house, bidderPanel, statusPanel);
    std::cout << "  " << statusPanel.text() << '\n';

    house.placeBid(mira, 450);
    drawFrame(house, bidderPanel, statusPanel);
    std::cout << "  " << statusPanel.text() << '\n';

    house.close();
    drawFrame(house, bidderPanel, statusPanel);
    for (const std::string& line : bidderPanel.lines())
        std::cout << "  " << line << '\n';
    std::cout << i18n::L("  낙찰: ", "  落札: ") << winnerOf(house) << i18n::L(", 판매자 수령액 ", "、出品者の受取額 ") << house.sellerProceeds() << '\n';

    expect("낙찰자 (가장 높게 부른 사람)", std::string("Mira"), winnerOf(house));
    expect("Mira의 금화 (800 - 450)", 350, goldOf(house, "Mira"));
    expect("Kael의 금화 (밀려나서 돌려받음)", 600, goldOf(house, "Kael"));
    expect("Doran의 금화 (밀려나서 돌려받음)", 500, goldOf(house, "Doran"));
    expect("판매자 수령액 (450 - 수수료 5%)", 428, house.sellerProceeds());
}

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(--grade)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.

// [JA] 採点器(--grade)が、画面には表示されない別のデータで同じコードをもう一度検査します。
void chC1_auction_night_hidden()
{
    using namespace chal1;

    AuctionHouse house("서리 지팡이");
    BidderPanel panel;
    const BidderId a = house.join("Ivo", 300, Tier::Normal);
    const BidderId b = house.join("Nell", 900, Tier::Silver);
    const BidderId c = house.join("Pia", 700, Tier::Gold);
    const BidderId d = house.join("Rook", 400, Tier::Silver);

    panel.refresh(house);
    expect("목록 첫 줄은 골드 등급", true, panel.lines().size() == 4 && panel.lines()[0].find("Pia") != std::string::npos);
    expect("목록 둘째 줄은 먼저 온 실버", true, panel.lines()[1].find("Nell") != std::string::npos);
    expect("화면을 그린 뒤에도 참가 번호는 그대로", std::string("Ivo"), house.bidder(a).name);

    house.placeBid(a, 250);
    panel.refresh(house);
    house.placeBid(d, 260);
    panel.refresh(house);
    house.placeBid(b, 610);
    panel.refresh(house);
    house.placeBid(b, 650);   // 자기 입찰 올리기
    panel.refresh(house);
    house.placeBid(c, 640);   // 최고가보다 낮아서 실패
    house.close();

    expect("낙찰자 번호", b, house.leader().value_or(99));
    expect("Nell의 금화", 250, house.bidder(b).gold);
    expect("Ivo의 금화", 300, house.bidder(a).gold);
    expect("Rook의 금화", 400, house.bidder(d).gold);
    expect("Pia의 금화", 700, house.bidder(c).gold);
    expect("판매자 수령액", 618, house.sellerProceeds());
}
#pragma endregion
