#include "../labs.h"
#include <chrono>
#include <condition_variable>
#include <latch>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

// 레이드 입장 대기. 공대장은 공대원이 모두 "준비 완료"를 누를 때까지 기다렸다가 입장한다.
// 공대원은 각자의 스레드(클라이언트 연결)에서 준비 완료를 보낸다.

// [JA] レイド入場待ちです。レイドリーダーはメンバー全員が「準備完了」を押すまで待ってから入場します。
// [JA] メンバーはそれぞれのスレッド(クライアント接続)から準備完了を送ります。
namespace async3
{
class ReadyCheck
{
public:
    explicit ReadyCheck(int memberCount) : memberCount_(memberCount) {}

    // 공대원 스레드: 준비 완료
    void markReady()
    {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            ++readyCount_;
        }
        allReady_.notify_one();
    }

    // 공대장 스레드: 모두 준비될 때까지 기다린다.
    void waitForAll()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        allReady_.wait(lock);
    }

    int readyCount()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return readyCount_;
    }

    int memberCount() const { return memberCount_; }

private:
    std::mutex mutex_;
    std::condition_variable allReady_;
    int memberCount_;
    int readyCount_ = 0;
};

// 공대장이 입장 공지를 쓰는 동안 공대원들이 먼저 준비를 마치는 상황.
// (이 랩은 이 순서를 매번 똑같이 재현하려고, 공지를 다 쓰는 시점을 "공대원이 모두 버튼을 누른 뒤"로 고정했다)
int enterRaid(const std::vector<std::string>& members)
{
    ReadyCheck check(static_cast<int>(members.size()));
    std::latch buttonsPressed(static_cast<std::ptrdiff_t>(members.size()));

    std::vector<std::thread> clients;
    for (const std::string& name : members)
    {
        clients.emplace_back([&check, &buttonsPressed, name] {
            check.markReady();
            buttonsPressed.count_down();
        });
    }

    buttonsPressed.wait();   // 공대장: 입장 공지를 다 썼다
    check.waitForAll();      // 공대장: 이제 모두를 기다린다

    for (std::thread& t : clients)
        t.join();
    return check.readyCount();
}
}

void as03_raid_ready()
{
    using namespace async3;
    int entered = 0;
    runWithWatchdog("레이드 입장", [&] {
        entered = enterRaid({ "Kael", "Mira", "Doran", "Selene" });
    });
    expect("입장한 공대원 수", 4, entered);
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (편집기의 접기 기능으로 모두 접어 두세요)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(エディターの折りたたみ機能ですべて折りたたんでおいてください)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// 디버거로 실행해서 굳으면 모두 중단(break all)하고 스레드 목록을 보세요.
// 공대원 스레드는 몇 개 남아 있나요? 공대장(lab-worker) 스레드는 어디에서 기다리고 있나요?

// [JA] デバッガーで実行して固まったら、すべて中断(break all)してスレッド ウィンドウを見てください。
// [JA] メンバーのスレッドはいくつ残っていますか? リーダー(lab-worker)のスレッドはどこで待っていますか?
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// 공대장 스레드의 호출 스택에서 waitForAll 프레임을 골라 readyCount_와 memberCount_를 보세요.
// 기다릴 필요가 있나요? notify_one은 언제 불렸고, 그때 wait 중인 스레드가 있었나요?

// [JA] リーダーのスレッドの呼び出し履歴で waitForAll のフレームを選び、readyCount_ と memberCount_ を見てください。
// [JA] 待つ必要はありますか? notify_one はいつ呼ばれ、そのとき wait 中のスレッドはありましたか?
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// condition_variable의 알림은 저장되지 않습니다. 아무도 기다리지 않을 때 보낸 알림은 사라집니다.
// 또 wait는 알림 없이도 깨어날 수 있습니다(가짜 깨어남). 그래서 wait는 "무엇을 기다리는지" 조건과 함께 씁니다.
// 알림을 보내는 쪽도 한 명이 아니라 기다리는 사람 모두를 깨워야 하는지 생각해 보세요.

// [JA] condition_variable の通知は保存されません。誰も待っていないときに送った通知は消えてしまいます。
// [JA] また、wait は通知がなくても起きることがあります(見せかけの起床、spurious wakeup)。そのため wait は「何を待っているのか」という条件と一緒に使います。
// [JA] 通知を送る側も、1人ではなく待っている全員を起こすべきかどうか考えてみてください。
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(--grade)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void as03_raid_ready_hidden()
{
    using namespace async3;

    // 1) 공대원 여섯 명
    int entered = 0;
    runWithWatchdog("레이드 입장 (숨은 케이스)", [&] {
        entered = enterRaid({ "Ivo", "Nell", "Pia", "Rook", "Sana", "Tom" });
    });
    expect("입장한 공대원 수", 6, entered);

    // 2) 공대장이 먼저 기다리고, 공대원이 한 명씩 늦게 준비하는 경우
    ReadyCheck check(3);
    int seen = 0;
    runWithWatchdog("늦게 준비하는 공대원 (숨은 케이스)", [&] {
        std::thread leader([&] {
            check.waitForAll();
            seen = check.readyCount();
        });
        for (int i = 0; i < 3; ++i)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            check.markReady();
        }
        leader.join();
    });
    expect("공대장이 입장할 때 준비한 공대원 수", 3, seen);
}
#pragma endregion
