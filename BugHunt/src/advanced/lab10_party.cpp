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

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// ============================================================================

#pragma region 힌트 1 (먼저 직접 추적해 보고 펼치세요)
// 첫 levelUpAll(party); 줄에 중단점 → 조사식에 &party[0].hp
// → 우클릭해서 "값이 변경되면 중단"(데이터 중단점) → F5
#pragma endregion

#pragma region 힌트 2
// 멈춘 곳의 호출 스택을 보세요. hp를 쓰는 코드가 아닌데 hp가 바뀌었다면?
#pragma endregion

#pragma region 힌트 3
// 메모리 창에 &party[0]을 넣고 name과 hp가 메모리에서 어떻게 붙어 있는지 보세요.
// "Sir Kael"은 몇 바이트인가요?
#pragma endregion

#pragma region 참고
// 이 버그는 ASan도 /RTC도 못 잡습니다(구조체 내부 오버플로). 데이터 중단점이 답입니다.
#pragma endregion
