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
