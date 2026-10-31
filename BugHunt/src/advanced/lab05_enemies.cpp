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
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// ============================================================================

#pragma region 힌트 1 (먼저 직접 추적해 보고 펼치세요)
// Assertion 메시지를 그대로 읽어 보세요. "invalidated"는 무슨 뜻일까요?
#pragma endregion

#pragma region 힌트 2
// erase의 리턴값은 무엇인가요? (cppreference에서 확인)
#pragma endregion
