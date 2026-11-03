#include "../labs.h"
#include <array>
#include <string>
#include <vector>

// 웨이브마다 몬스터를 소환한다. 3의 배수 웨이브에는 보스가 나온다.

// [JA] ウェーブごとにモンスターを召喚します。3の倍数のウェーブにはボスが出ます。
namespace lab8
{
struct LevelInfo
{
    int maxHp;
    int attack;
};

constexpr int kMaxLevel = 50;

std::array<LevelInfo, kMaxLevel + 1> buildLevelTable()
{
    std::array<LevelInfo, kMaxLevel + 1> table{};
    for (int level = 0; level <= kMaxLevel; ++level)
        table[level] = { 50 + level * 20, 5 + level * 2 };
    return table;
}

const std::array<LevelInfo, kMaxLevel + 1> kLevelTable = buildLevelTable();

struct Monster
{
    std::string name;
    int level;
    int hp;
    int attack;
};

LevelInfo statsFor(int level)
{
    return kLevelTable.at(level);
}

Monster createMonster(const std::string& name, int level)
{
    const LevelInfo info = statsFor(level);
    return { name, level, info.maxHp, info.attack };
}

Monster spawnMonster(const std::string& name, int level, int bonusHp)
{
    Monster monster = createMonster(name, level);
    monster.hp += bonusHp;
    return monster;
}

void spawnWave(int wave, std::vector<Monster>& field)
{
    field.push_back(spawnMonster("Goblin", 3 + wave, 0));
    field.push_back(spawnMonster("Orc", 5 + wave, 20));

    if (wave % 3 == 0)
        field.push_back(spawnMonster("Ogre Boss", 200, 10 + wave));
}

void runWaves()
{
    std::vector<Monster> field;
    for (int wave = 1; wave <= 6; ++wave)
    {
        spawnWave(wave, field);
        std::cout << "  웨이브 " << wave << " 소환 완료 (필드 몬스터 " << field.size() << "마리)\n";
    }
}
}

void cs08_waves()
{
    lab8::runWaves();
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (편집기의 접기 기능으로 모두 접어 두세요)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(エディターの折りたたみ機能ですべて折りたたんでおいてください)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 호출 스택을 보고 펼치세요 / まず呼び出し履歴を見てから開いてください)
// 디버거는 처리되지 않은 예외(std::out_of_range)에서 멈춥니다.
// statsFor 프레임의 level은 얼마인가요? kLevelTable의 크기는요?

// [JA] デバッガーはハンドルされない例外(std::out_of_range)で止まります。
// [JA] statsFor フレームの level はいくつですか? kLevelTable のサイズは?
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// 한 칸씩 아래로 내려가면서 level이 어디서 왔는지 보세요.
// createMonster → spawnMonster → spawnWave

// [JA] 一つずつ下に降りながら、level がどこから来たのかを見てください。
// [JA] createMonster → spawnMonster → spawnWave
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// spawnMonster의 매개변수 순서와, spawnWave에서 넘긴 인자 순서를 비교해 보세요.

// [JA] spawnMonster の仮引数の順番と、spawnWave から渡した実引数の順番を比べてみてください。
#pragma endregion
