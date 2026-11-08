#include "../labs.h"
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <latch>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

// 한정 판매. 남은 수량이 있을 때만 결제하고 한 개를 판다.
// 구매 요청은 플레이어마다 다른 스레드에서 동시에 들어온다. 수량과 판매 수는 atomic이라 "안전하다".

// [JA] 限定販売です。残り数量があるときだけ決済して1個売ります。
// [JA] 購入リクエストはプレイヤーごとに別のスレッドから同時に届きます。数量と販売数は atomic なので「安全」です。
namespace async5
{
// 결제 서버. 요청을 모아서 한 번에 처리한다.
// 묶음이 다 차거나, 첫 요청 뒤 조금 기다려도 더 오지 않으면 모인 요청을 처리한다.
class PaymentServer
{
public:
    explicit PaymentServer(int batchSize) : batchSize_(batchSize) {}

    void charge(const std::string& /*player*/, int /*price*/)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        const int myBatch = batch_;
        if (++waiting_ >= batchSize_)
        {
            flush();
        }
        else if (!flushed_.wait_for(lock, std::chrono::milliseconds(200), [&] { return batch_ != myBatch; }))
        {
            flush();   // 더 오지 않는다. 모인 것만 처리
        }
    }

private:
    void flush()
    {
        waiting_ = 0;
        ++batch_;
        flushed_.notify_all();
    }

    std::mutex mutex_;
    std::condition_variable flushed_;
    int batchSize_;
    int waiting_ = 0;
    int batch_ = 0;
};

class LimitedShop
{
public:
    LimitedShop(int stock, int price, PaymentServer& payment) : stock_(stock), price_(price), payment_(payment) {}

    // 한 개 산다. 샀으면 true.
    bool buy(const std::string& player)
    {
        if (stock_.load() <= 0)
            return false;

        payment_.charge(player, price_);
        --stock_;
        ++sold_;
        return true;
    }

    int stock() const { return stock_.load(); }
    int sold() const { return sold_.load(); }

private:
    std::atomic<int> stock_;
    std::atomic<int> sold_{ 0 };
    int price_;
    PaymentServer& payment_;
};

// 플레이어들이 같은 순간에 구매 버튼을 누른다.
int rush(LimitedShop& shop, const std::vector<std::string>& players)
{
    std::latch start(static_cast<std::ptrdiff_t>(players.size()));
    std::atomic<int> bought{ 0 };
    std::vector<std::thread> clients;
    for (const std::string& player : players)
    {
        clients.emplace_back([&, player] {
            start.arrive_and_wait();
            if (shop.buy(player))
                ++bought;
        });
    }
    for (std::thread& t : clients)
        t.join();
    return bought.load();
}
}

void as05_limited_sale()
{
    using namespace async5;
    PaymentServer payment(8);
    LimitedShop shop(3, 990, payment);   // 한정 3개

    const int bought = rush(shop, { "Kael", "Mira", "Doran", "Selene", "Ivo" });
    std::cout << i18n::L("  구매 성공 ", "  購入成功 ") << bought << i18n::L("명, 남은 수량 ", "人、残り数量 ") << shop.stock() << i18n::L(", 판매 수 ", "、販売数 ") << shop.sold() << '\n';

    expect("판매 수 (한정 3개)", 3, shop.sold());
    expect("남은 수량", 0, shop.stock());
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (편집기의 접기 기능으로 모두 접어 두세요)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(エディターの折りたたみ機能ですべて折りたたんでおいてください)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// --stock_; 줄에 조건부 중단점 stock_ <= 0 을 걸어 보세요. 멈춘다면, 이 스레드는 어떻게 수량 검사를 통과했나요?
// 스레드 목록에서 buy를 실행 중인 다른 스레드들이 각각 어느 줄에 있는지 보세요.

// [JA] --stock_; の行に条件付きブレークポイント stock_ <= 0 を置いてみてください。止まったなら、このスレッドはどうやって数量チェックを通過したのでしょうか?
// [JA] スレッド ウィンドウで、buy を実行中のほかのスレッドがそれぞれどの行にいるか見てください。
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// stock_.load() <= 0 검사와 --stock_ 사이에는 무엇이 있나요? 그동안 다른 스레드는 무엇을 할 수 있나요?
// atomic은 읽기 하나, 빼기 하나를 각각 안전하게 해 줄 뿐입니다. "확인하고 빼기" 전체를 한 번에 해 주지는 않습니다.

// [JA] stock_.load() <= 0 のチェックと --stock_ の間には何がありますか? その間にほかのスレッドは何ができますか?
// [JA] atomic は読み取り1つ、減算1つをそれぞれ安全にしてくれるだけです。「確認してから減らす」全体を一度にやってくれるわけではありません。
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// 결제하기 전에 수량을 먼저 "한 번에" 확보하세요. 확보하는 연산 자체가 남은 수량을 확인해 주어야 합니다.
// (buy 전체를 mutex로 감싸도 맞게 동작하지만, 결제 서버가 요청을 모으지 못하고 한 명씩 기다리게 됩니다)

// [JA] 決済する前に、まず数量を「一度に」確保してください。確保する演算そのものが残り数量を確認してくれなければなりません。
// [JA] (buy 全体を mutex で囲んでも正しく動きますが、決済サーバーがリクエストをまとめられず、1人ずつ待たせることになります)
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(--grade)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void as05_limited_sale_hidden()
{
    using namespace async5;
    PaymentServer payment(16);
    LimitedShop shop(5, 50, payment);
    int bought = 0;
    runWithWatchdog("한정 판매 (숨은 케이스)", [&] {
        bought = rush(shop, { "A", "B", "C", "D", "E", "F", "G", "H", "I", "J" });
    });
    expect("구매 성공한 사람 수", 5, bought);
    expect("판매 수", 5, shop.sold());
    expect("남은 수량", 0, shop.stock());
}
#pragma endregion
