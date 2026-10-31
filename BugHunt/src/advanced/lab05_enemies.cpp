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
