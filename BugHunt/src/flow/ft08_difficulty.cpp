#include "../labs.h"
#include <sstream>
#include <string>

// 게임을 켜면 저장된 설정을 읽고, 볼륨을 적용하고, 첫 웨이브를 소환한다.
// 플레이어는 "어려움" 난이도와 볼륨 35로 저장해 두었다.
namespace flow8
{
enum class Difficulty { Easy, Normal, Hard };

struct Settings
{
    Difficulty difficulty = Difficulty::Normal;
    int masterVolume = 80;
};

Settings g_settings;

const char* kSaveFile =
    "volume=35\n"
    "difficulty=hard\n";

void loadSettings(Settings& settings, const std::string& text)
{
    std::istringstream in(text);
    std::string line;
    while (std::getline(in, line))
    {
        const size_t eq = line.find('=');
        if (eq == std::string::npos)
            continue;
        const std::string key = line.substr(0, eq);
        const std::string value = line.substr(eq + 1);

        if (key == "volume")
            settings.masterVolume = std::stoi(value);
        else if (key == "difficulty")
            settings.difficulty = value == "easy" ? Difficulty::Easy
                                : value == "hard" ? Difficulty::Hard
                                                  : Difficulty::Normal;
    }
}

class AudioSystem
{
public:
    explicit AudioSystem(const Settings& settings) : settings_(settings) {}

    int applyVolume() { return currentVolume_ = settings_.masterVolume; }

private:
    const Settings& settings_;
    int currentVolume_ = 0;
};

class World
{
public:
    explicit World(const Settings& settings) : settings_(settings) {}

    int spawnWave()
    {
        constexpr int kBaseHp = 100;
        switch (settings_.difficulty)
        {
        case Difficulty::Easy:   enemyHp_ = kBaseHp * 7 / 10; break;
        case Difficulty::Normal: enemyHp_ = kBaseHp;          break;
        case Difficulty::Hard:   enemyHp_ = kBaseHp * 3 / 2;  break;
        }
        return enemyHp_;
    }

private:
    Settings settings_;
    int enemyHp_ = 0;
};

class Game
{
public:
    Game() : audio_(g_settings), world_(g_settings) {}

    void boot()
    {
        loadSettings(g_settings, kSaveFile);
        volume_ = audio_.applyVolume();
        firstWaveHp_ = world_.spawnWave();
    }

    int volume() const { return volume_; }
    int firstWaveHp() const { return firstWaveHp_; }

private:
    AudioSystem audio_;
    World world_;
    int volume_ = 0;
    int firstWaveHp_ = 0;
};
}

void ft08_difficulty()
{
    flow8::Game game;
    game.boot();

    expect("볼륨 (저장값 35)", 35, game.volume());
    expect("첫 웨이브 적 HP (어려움 = 150)", 150, game.firstWaveHp());
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// ============================================================================

#pragma region 힌트 1 (먼저 직접 추적해 보고 펼치세요)
// 이번에는 수상한 반복문도, 실패하는 특정 입력도 없습니다. ft08_difficulty 첫 줄부터 F11로 따라가세요.
// 조사식에 두 개를 고정해 두고 한 단계마다 비교하면 됩니다:
//     flow8::g_settings.difficulty
//     game.world_.settings_.difficulty      (boot 안에서는 world_.settings_.difficulty)
#pragma endregion

#pragma region 힌트 2
// 볼륨은 제대로 들어가는데 난이도는 안 들어갑니다. AudioSystem과 World는 설정을 똑같이 "보관"하고 있나요?
// 멤버 선언을 비교해 보세요. 그리고 그 멤버가 만들어진 시점은 loadSettings보다 앞인가요, 뒤인가요?
#pragma endregion

#pragma region 힌트 3
// 고치는 방법은 여러 가지입니다: 참조로 보관하기, 설정을 다 읽은 뒤에 World를 만들기,
// spawnWave가 그때그때 설정을 인자로 받기. 각각의 장단점은 무엇일까요?
#pragma endregion
