#include "labs.h"

#include "platform/platform.h"

#include <chrono>
#include <condition_variable>
#include <cstdlib>
#include <memory>
#include <mutex>
#include <thread>

// 10부, 11부 랩이 진짜로 영원히 멈추지 않게 하는 감시 타이머.
// 메인 스레드는 기다리기만 하고, 랩 코드는 "랩 작업 스레드"에서 돈다.
// 시간이 넘으면 작업 스레드는 멈춘 채로 두고 프로세스를 통째로 끝낸다.
namespace
{
struct WatchState
{
    std::mutex mutex;
    std::condition_variable finished;
    bool done = false;
};
}

void runWithWatchdog(const char* what, const std::function<void()>& work, int timeoutMs)
{
    auto state = std::make_shared<WatchState>();

    std::thread worker([state, &work] {
        // 디버거의 스레드 목록에 이 이름으로 보인다.
        platform::nameThisThread("lab-worker");
        work();
        {
            std::lock_guard<std::mutex> lock(state->mutex);
            state->done = true;
        }
        state->finished.notify_one();
    });

    std::unique_lock<std::mutex> lock(state->mutex);
    if (platform::debuggerAttached())
    {
        std::cout << i18n::L("  (디버거가 붙어 있어 감시 타이머를 껐습니다. 멈춘 것 같으면 디버거에서 모두 중단(break all)하세요)\n",
                             "  (デバッガーが接続されているので監視タイマーを止めました。止まったようならデバッガーで「すべて中断」(break all)してください)\n");
        state->finished.wait(lock, [&] { return state->done; });
    }
    else if (!state->finished.wait_for(lock, std::chrono::milliseconds(timeoutMs), [&] { return state->done; }))
    {
        std::cout << "  [FAIL] " << i18n::tr(what) << i18n::L("  응답 없음 (", "  応答なし (") << timeoutMs
                  << i18n::L(" ms 초과)\n", " ms 超過)\n") << std::flush;
        std::_Exit(kExitTimeout);   // 멈춘 작업 스레드는 프로세스와 함께 정리된다
    }
    lock.unlock();
    worker.join();
}
