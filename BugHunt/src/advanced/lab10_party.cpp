#include "../labs.h"
#include <cstdio>
#include <cstring>
#include <vector>

namespace
{
struct Character
{
    char name[8];
    int hp;
    int level;
};

Character makeCharacter(const char* name, int hp, int level)
{
    Character c{};
    std::snprintf(c.name, sizeof(c.name), "%s", name);
    c.hp = hp;
    c.level = level;
    return c;
}

void setName(Character& c, const char* newName)
{
    std::memcpy(c.name, newName, std::strlen(newName) + 1);
}

// 레벨 10 이상인 캐릭터에게 기사 작위를 준다.
void grantKnighthood(std::vector<Character>& party)
{
    for (Character& c : party)
    {
        if (c.level >= 10)
        {
            char titled[32];
            std::snprintf(titled, sizeof(titled), "Sir %s", c.name);
            setName(c, titled);
        }
    }
}

void levelUpAll(std::vector<Character>& party)
{
    for (Character& c : party)
        ++c.level;
}
}

void lab10_party()
{
    std::vector<Character> party = {
        makeCharacter("Kael",  100, 12),
        makeCharacter("Mira",   80,  5),
        makeCharacter("Doran", 120, 15),
    };

    levelUpAll(party);
    grantKnighthood(party);
    levelUpAll(party);

    // 전투는 한 번도 없었으므로 HP는 처음 그대로여야 한다.
    expect("Kael HP",  100, party[0].hp);
    expect("Mira HP",   80, party[1].hp);
    expect("Doran HP", 120, party[2].hp);
}
