#include "../labs.h"
#include <algorithm>
#include <chrono>
#include <latch>
#include <mutex>
#include <string>
#include <thread>
#include <utility>
#include <vector>

// 플레이어 간 거래. 거래 서버는 거래마다 스레드 하나를 쓴다.
// 거래 중에 다른 거래가 같은 인벤토리를 건드리지 못하도록 인벤토리마다 mutex를 둔다.
namespace hang5
{
struct Inventory
{
    Inventory(std::string o, int g, std::vector<std::string> i) : owner(std::move(o)), gold(g), items(std::move(i)) {}

    std::string owner;
    int gold;
    std::vector<std::string> items;
    std::mutex lock;
};

// 위조 아이템인지 서버에 물어본다. (네트워크라 시간이 좀 걸린다)
bool verifyItem(const Inventory& inv, const std::string& item)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    return std::find(inv.items.begin(), inv.items.end(), item) != inv.items.end();
}

// seller가 item을 buyer에게 price골드에 판다.
void trade(Inventory& seller, Inventory& buyer, const std::string& item, int price)
{
    std::lock_guard<std::mutex> sellerLock(seller.lock);
    if (!verifyItem(seller, item))
        return;

    std::lock_guard<std::mutex> buyerLock(buyer.lock);
    if (buyer.gold < price)
        return;

    seller.items.erase(std::find(seller.items.begin(), seller.items.end(), item));
    buyer.items.push_back(item);
    buyer.gold -= price;
    seller.gold += price;
}

struct Order
{
    Inventory* seller;
    Inventory* buyer;
    std::string item;
    int price;
};

// 주문들을 동시에(각자 스레드에서) 처리한다.
void runTradeServer(const std::vector<Order>& orders)
{
    std::latch start(static_cast<std::ptrdiff_t>(orders.size()));
    std::vector<std::thread> workers;
    for (const Order& order : orders)
    {
        workers.emplace_back([&start, order] {
            start.arrive_and_wait();   // 모든 거래가 같은 순간에 시작한다
            trade(*order.seller, *order.buyer, order.item, order.price);
        });
    }
    for (std::thread& t : workers)
        t.join();
}
}

void hg05_trade()
{
    using namespace hang5;

    Inventory kael{ "Kael", 100, { "Flame Sword" } };
    Inventory mira{ "Mira", 800, { "Healing Potion", "Healing Potion" } };

    runWithWatchdog("동시 거래 처리", [&] {
        runTradeServer({
            { &kael, &mira, "Flame Sword", 500 },     // Kael → Mira: 검
            { &mira, &kael, "Healing Potion", 30 },   // Mira → Kael: 포션
        });
    });

    expect("Kael 골드 (100 + 500 - 30)", 570, kael.gold);
    expect("Mira 골드 (800 - 500 + 30)", 330, mira.gold);
    expect("Mira 아이템 수", 2, static_cast<int>(mira.items.size()));
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (편집기의 접기 기능으로 모두 접어 두세요)
// ============================================================================

#pragma region 힌트 1 (README의 "따라 해 보기"를 먼저 하세요)
// 멈추면 모두 중단(break all) → 모든 스레드의 호출 스택을 보세요. (VS: 병렬 스택, gdb: thread apply all bt)
// trade에서 멈춰 있는 스레드가 몇 개인가요? 각각 trade의 몇 번째 줄에서 기다리고 있나요?
#pragma endregion

#pragma region 힌트 2
// 스레드 목록에서 두 거래 스레드를 하나씩 더블클릭하고, 호출 스택에서 trade 프레임을 골라 seller.owner와 buyer.owner를 보세요.
// 각 스레드가 이미 잡은 잠금은 무엇이고, 기다리는 잠금은 무엇인가요? 표로 그려 보세요.
#pragma endregion

#pragma region 힌트 3
// 두 잠금을 "항상 같은 순서로" 잡거나, 표준 라이브러리에 두 mutex를 한꺼번에 안전하게 잡는 도구가 있습니다.
// 위조 검사를 잠금을 잡기 전으로 옮기는 것만으로는 왜 충분하지 않을까요?
#pragma endregion

#pragma region 채점용 숨은 테스트 (풀이에는 필요 없습니다. 고치지 마세요)
// 채점기(--grade)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void hg05_trade_hidden()
{
    using namespace hang5;

    // 세 명이 원을 이루며 동시에 판다: Ivo → Nell → Pia → Ivo
    Inventory ivo{ "Ivo", 300, { "Bow" } };
    Inventory nell{ "Nell", 300, { "Shield" } };
    Inventory pia{ "Pia", 300, { "Ring" } };

    runWithWatchdog("동시 거래 처리 (숨은 케이스)", [&] {
        runTradeServer({
            { &ivo, &nell, "Bow", 120 },
            { &nell, &pia, "Shield", 90 },
            { &pia, &ivo, "Ring", 200 },
        });
    });

    expect("Ivo 골드 (300 + 120 - 200)", 220, ivo.gold);
    expect("Nell 골드 (300 - 120 + 90)", 270, nell.gold);
    expect("Pia 골드 (300 - 90 + 200)", 410, pia.gold);
}
#pragma endregion
