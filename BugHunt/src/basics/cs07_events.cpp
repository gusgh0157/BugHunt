#include "../labs.h"
#include <functional>
#include <memory>
#include <vector>

// HP가 바뀌면 이벤트를 보내고, 구독자(로그, HP 바)가 반응한다.
namespace lab7
{
class HealthBar
{
public:
    void setValue(int hp)
    {
        value_ = hp;
        std::cout << "  [HP 바] " << value_ << '\n';
    }

private:
    int value_ = 0;
};

class EventBus
{
public:
    void subscribe(std::function<void(int)> listener)
    {
        listeners_.push_back(std::move(listener));
    }

    void publishHpChanged(int hp)
    {
        for (const auto& listener : listeners_)
            listener(hp);
    }

private:
    std::vector<std::function<void(int)>> listeners_;
};

struct Game
{
    EventBus events;
    std::unique_ptr<HealthBar> healthBar;
};

void connectLogger(Game& game)
{
    game.events.subscribe([](int hp) { std::cout << "  [로그] HP 변경: " << hp << '\n'; });
}

void connectHealthBar(Game& game)
{
    HealthBar* bar = game.healthBar.get();
    game.events.subscribe([bar](int hp) { bar->setValue(hp); });
}

void createUi(Game& game)
{
    game.healthBar = std::make_unique<HealthBar>();
}

void startGame()
{
    Game game;
    connectLogger(game);
    connectHealthBar(game);
    createUi(game);

    // 플레이어가 피해를 입었다.
    game.events.publishHpChanged(75);
}
}

void cs07_events()
{
    lab7::startGame();
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// ============================================================================

#pragma region 힌트 1 (먼저 호출 스택을 보고 펼치세요)
// HealthBar::setValue에서 지역 창의 this를 보세요.
// 객체가 없는데 멤버 함수가 불렸습니다.
#pragma endregion

#pragma region 힌트 2
// 한 칸 아래, <lambda_1>::operator()처럼 생긴 줄이 람다입니다.
// 거기서 캡처된 bar 값을 보세요.
#pragma endregion

#pragma region 힌트 3
// 호출 스택은 "지금"만 보여 줍니다. bar가 언제 nullptr로 저장됐는지는 스택에 없습니다.
// connectHealthBar에 중단점(F9)을 걸고 다시 실행해서, 그 순간 game.healthBar를 보세요.
#pragma endregion

#pragma region 힌트 4
// startGame에서 함수를 부르는 순서를 보세요.
#pragma endregion
