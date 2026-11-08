#include "../labs.h"
#include <functional>
#include <memory>
#include <vector>

// HP가 바뀌면 이벤트를 보내고, 구독자(로그, HP 바)가 반응한다.

// [JA] HP が変わるとイベントを送り、購読者(ログ、HP バー)が反応します。
namespace lab7
{
class HealthBar
{
public:
    void setValue(int hp)
    {
        value_ = hp;
        std::cout << i18n::L("  [HP 바] ", "  [HPバー] ") << value_ << '\n';
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
    game.events.subscribe([](int hp) { std::cout << i18n::L("  [로그] HP 변경: ", "  [ログ] HP変更: ") << hp << '\n'; });
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
// 힌트: 막힐 때 하나씩만 펼치세요. (편집기의 접기 기능으로 모두 접어 두세요)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(エディターの折りたたみ機能ですべて折りたたんでおいてください)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 호출 스택을 보고 펼치세요 / まず呼び出し履歴を見てから開いてください)
// HealthBar::setValue에서 지역 변수 this를 보세요.
// 객체가 없는데 멤버 함수가 불렸습니다.

// [JA] HealthBar::setValue でローカル変数 this を見てください。
// [JA] オブジェクトがないのにメンバー関数が呼ばれました。
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// 한 칸 아래, <lambda_1>::operator()처럼 생긴 줄이 람다입니다.
// 거기서 캡처된 bar 값을 보세요.

// [JA] 一つ下の、<lambda_1>::operator() のような形の行がラムダです。
// [JA] そこでキャプチャされた bar の値を見てください。
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// 호출 스택은 "지금"만 보여 줍니다. bar가 언제 nullptr로 저장됐는지는 스택에 없습니다.
// connectHealthBar에 중단점을 걸고 다시 실행해서, 그 순간 game.healthBar를 보세요.

// [JA] 呼び出し履歴は「今」しか見せてくれません。bar がいつ nullptr として保存されたかはスタックにはありません。
// [JA] connectHealthBar にブレークポイントを置いてもう一度実行し、その瞬間の game.healthBar を見てください。
#pragma endregion

#pragma region 힌트 4 / ヒント 4
// startGame에서 함수를 부르는 순서를 보세요.

// [JA] startGame で関数を呼ぶ順番を見てください。
#pragma endregion
