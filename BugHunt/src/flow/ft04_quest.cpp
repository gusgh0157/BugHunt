#include "../labs.h"
#include <functional>
#include <string>
#include <vector>

// "늑대 5마리 처치" 퀘스트. 적이 죽으면 이벤트가 구독자들에게 차례로 전달된다.
namespace flow4
{
class EventBus
{
public:
    using Handler = std::function<void(const std::string& enemy)>;

    void subscribe(Handler handler) { handlers_.push_back(std::move(handler)); }

    void publishKill(const std::string& enemy)
    {
        for (const Handler& handler : handlers_)
            handler(enemy);
    }

private:
    std::vector<Handler> handlers_;
};

class KillCounter
{
public:
    void onKill(const std::string& enemy)
    {
        if (enemy == "Wolf")
            ++wolves_;
    }

    int wolves() const { return wolves_; }

private:
    int wolves_ = 0;
};

class WolfQuest
{
public:
    explicit WolfQuest(const KillCounter& counter) : counter_(counter) {}

    void onKill(const std::string& enemy)
    {
        if (completed_ || enemy != "Wolf")
            return;
        ++wolfKillsSeen_;
        if (counter_.wolves() >= kRequired)
        {
            completed_ = true;
            completedAtKill_ = wolfKillsSeen_;
            std::cout << "  ** 퀘스트 완료: 늑대 사냥 **\n";
        }
    }

    int completedAtKill() const { return completedAtKill_; }

private:
    static constexpr int kRequired = 5;
    const KillCounter& counter_;
    bool completed_ = false;
    int wolfKillsSeen_ = 0;
    int completedAtKill_ = 0;
};

struct Game
{
    EventBus bus;
    KillCounter counter;
    WolfQuest quest{ counter };

    void setup()
    {
        // 퀘스트 창이 먼저 반응하도록 퀘스트를 먼저 붙이고, 통계는 그다음에 붙인다.
        bus.subscribe([this](const std::string& e) { quest.onKill(e); });
        bus.subscribe([this](const std::string& e) { counter.onKill(e); });
    }
};
}

void ft04_quest()
{
    flow4::Game game;
    game.setup();

    const char* kills[] = { "Wolf", "Boar", "Wolf", "Wolf", "Wolf", "Wolf", "Wolf", "Wolf" };
    int wolfCount = 0;
    for (const char* enemy : kills)
    {
        if (std::string(enemy) == "Wolf")
            std::cout << "  늑대 처치 (" << ++wolfCount << ")\n";
        else
            std::cout << "  " << enemy << " 처치\n";
        game.bus.publishKill(enemy);
    }

    expect("퀘스트가 완료된 늑대 처치 번호", 5, game.quest.completedAtKill());
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// ============================================================================

#pragma region 힌트 1 (먼저 직접 추적해 보고 펼치세요)
// 멈추지 않는 중단점(추적점)을 두 개 거세요. 중단점 우클릭 → 작업 → 메시지 입력, "실행 계속" 체크.
//   KillCounter::onKill의 if 줄:            카운터: {enemy}, 증가 전 {wolves_}
//   WolfQuest::onKill의 if (counter_... 줄: 퀘스트 검사: 카운터 값 {counter_.wolves()}
// F5로 실행하고 출력 창(Ctrl+Alt+O)에서 두 메시지가 어떤 순서로 찍히는지 보세요.
#pragma endregion

#pragma region 힌트 2
// 다섯 번째 늑대를 잡았을 때, 퀘스트가 본 카운터 값은 몇이었나요?
// 그 순서는 누가 정했나요? EventBus::publishKill이 handlers_를 도는 순서를 거슬러 올라가 보세요.
#pragma endregion
