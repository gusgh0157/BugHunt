#include "../labs.h"
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

// 경매장 낙찰 공지. 경매 창에서 "마감 알림 받기"를 누르면, 마감 종이 울릴 때 낙찰가를 공지 게시판에 올린다.
// 마감까지 기다리는 일은 창을 붙잡지 않도록 따로 떼어 낸(detach) 스레드가 한다.

// [JA] オークションハウスの落札告知です。オークション画面で「締め切り通知を受け取る」を押すと、締め切りの鐘が鳴ったときに落札価格を告知掲示板に載せます。
// [JA] 締め切りまで待つ処理は、画面を引き止めないように切り離した(detach)スレッドが行います。
namespace async4
{
// 경매 마감 종. ring()이 불릴 때까지 기다리는 스레드를 깨운다.
class ClosingBell
{
public:
    void waitForRing()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        rang_.wait(lock, [&] { return ringing_; });
    }

    void ring()
    {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            ringing_ = true;
        }
        rang_.notify_all();
    }

private:
    std::mutex mutex_;
    std::condition_variable rang_;
    bool ringing_ = false;
};

// 공지 게시판. 올라온 낙찰가를 모은다.
class NoticeBoard
{
public:
    void post(int price)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        prices_.push_back(price);
        posted_.notify_all();
    }

    // 공지가 count개 올라올 때까지 기다린다.
    std::vector<int> waitFor(size_t count)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        posted_.wait(lock, [&] { return prices_.size() >= count; });
        return prices_;
    }

private:
    std::mutex mutex_;
    std::condition_variable posted_;
    std::vector<int> prices_;
};

class AuctionWindow
{
public:
    explicit AuctionWindow(int startPrice) : bid_(startPrice) {}

    void placeBid(int price)
    {
        if (price > bid_)
            bid_ = price;
    }

    // "마감 알림 받기"
    void subscribeClosingNotice(ClosingBell& bell, NoticeBoard& board)
    {
        std::thread([this, &bell, &board] {
            bell.waitForRing();
            board.post(bid_);
        }).detach();
    }

private:
    int bid_;
};

// 플레이어가 입찰하고 알림을 신청한 뒤, 마감 전에 경매 창을 닫는다.
void bidAndCloseWindow(int startPrice, const std::vector<int>& bids, ClosingBell& bell, NoticeBoard& board)
{
    auto window = std::make_unique<AuctionWindow>(startPrice);
    for (int price : bids)
        window->placeBid(price);
    window->subscribeClosingNotice(bell, board);
}   // 경매 창을 닫는다
}

void as04_auction_notice()
{
    using namespace async4;
    // 종과 게시판은 서버가 켜져 있는 동안 계속 있는 객체다.
    static ClosingBell bell;
    static NoticeBoard board;

    bidAndCloseWindow(1000, { 1200, 1500, 1350 }, bell, board);
    bell.ring();

    std::vector<int> prices;
    runWithWatchdog(i18n::L("낙찰 공지 기다리기", "落札告知を待つ"), [&] { prices = board.waitFor(1); });
    expect(i18n::L("공지된 낙찰가", "告知された落札価格"), 1500, prices.at(0));
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (편집기의 접기 기능으로 모두 접어 두세요)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(エディターの折りたたみ機能ですべて折りたたんでおいてください)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// board.post(bid_); 줄에 중단점을 걸고 실행하세요. 어느 스레드에서 멈췄나요? 호출 스택에 bidAndCloseWindow가 있나요?
// 그 프레임의 this와 bid_를 보세요. bid_를 16진수로 보면?

// [JA] board.post(bid_); の行にブレークポイントを置いて実行してください。どのスレッドで止まりましたか? 呼び出し履歴に bidAndCloseWindow はありますか?
// [JA] そのフレームの this と bid_ を見てください。bid_ を16進数で見ると?
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// 0xEEEEEEEE는 BugHunt 메모리 검사기가 해제된 메모리에 덮어 두는 값입니다.
// this가 가리키는 AuctionWindow는 언제 지워졌나요? 람다가 실행되는 시점과 비교해 보세요.
// ASan 구성으로 실행하면 어떤 오류가, 어느 스레드에서 나나요?

// [JA] 0xEEEEEEEE は、BugHunt のメモリチェッカーが解放済みのメモリに上書きしておく値です。
// [JA] this が指す AuctionWindow はいつ削除されましたか? ラムダが実行されるタイミングと比べてみてください。
// [JA] ASan 構成で実行すると、どんなエラーがどのスレッドで出ますか?
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// 떼어 낸 스레드와 나중에 실행되는 람다는 그것을 만든 객체나 함수보다 오래 살 수 있습니다.
// 람다가 [this]나 [&]로 무엇을 붙잡고 가는지, 그 대상이 람다보다 오래 사는지 하나씩 확인하세요.
// 필요한 값만 복사해 가세요. (스레드를 객체가 소유하고 소멸자에서 기다리게 하면, 이 랩처럼 종이 울리기 전에 창이 닫힐 때 소멸자가 멈춥니다.)

// [JA] 切り離したスレッドやあとで実行されるラムダは、それを作ったオブジェクトや関数より長く生きることがあります。
// [JA] ラムダが [this] や [&] で何をつかんで持っていくのか、その対象がラムダより長く生きるのかを1つずつ確認してください。
// [JA] 必要な値だけをコピーして持っていってください。(スレッドをオブジェクトに所有させてデストラクターで待たせると、このラボのようにベルが鳴る前に画面が閉じられたとき、デストラクターが止まります。)
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(--grade)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void as04_auction_notice_hidden()
{
    using namespace async4;
    static ClosingBell bell;
    static NoticeBoard board;

    bidAndCloseWindow(300, { 280, 450 }, bell, board);
    bidAndCloseWindow(7000, {}, bell, board);
    bell.ring();

    std::vector<int> prices;
    runWithWatchdog(i18n::L("낙찰 공지 기다리기 (숨은 케이스)", "落札告知を待つ(隠しケース)"), [&] { prices = board.waitFor(2); });
    const int sum = prices.at(0) + prices.at(1);
    expect(i18n::L("공지된 낙찰가 합 (450 + 7000)", "告知された落札価格の合計(450 + 7000)"), 7450, sum);
}
#pragma endregion
