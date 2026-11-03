#include "../labs.h"
#include <string>
#include <vector>

namespace
{
struct Enemy
{
    std::string name;
    int hp;
};

// HP가 0 이하인 적을 목록에서 제거한다.
void removeDead(std::vector<Enemy>& enemies)
{
    for (auto it = enemies.begin(); it != enemies.end(); ++it)
    {
        if (it->hp <= 0)
            enemies.erase(it);
    }
}
}

void lab05_enemies()
{
    std::vector<Enemy> enemies = {
        { "Goblin",   0 },
        { "Orc",     35 },
        { "Slime",   -4 },
        { "Skeleton", 0 },
        { "Troll",   80 },
    };

    removeDead(enemies);

    expect("남은 적 수", size_t{ 2 }, enemies.size());
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (편집기의 접기 기능으로 모두 접어 두세요)
// ============================================================================

#pragma region 힌트 1 (먼저 직접 추적해 보고 펼치세요)
// erase 다음 줄에서 it가 가리키는 원소를 조사식으로 보세요. 지운 원소를 가리키던 이터레이터는 어떻게 되나요?
#pragma endregion

#pragma region 힌트 2
// erase의 리턴값은 무엇인가요? (cppreference에서 확인)
#pragma endregion
