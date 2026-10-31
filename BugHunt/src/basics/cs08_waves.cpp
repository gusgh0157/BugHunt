#include "../labs.h"
#include <array>
#include <string>
#include <vector>

// 웨이브마다 몬스터를 소환한다. 3의 배수 웨이브에는 보스가 나온다.
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
    return kLevelTable[level];
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
