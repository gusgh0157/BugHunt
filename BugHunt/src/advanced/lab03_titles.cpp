#include "../labs.h"
#include <string>
#include <string_view>
#include <vector>

namespace
{
struct Player
{
    std::string name;
    std::string guild;
};

std::string makeTitle(const Player& p)
{
    return "[" + p.guild + "] " + p.name;
}

// 호출할 때마다 문자열을 복사하지 않도록 string_view로 돌려준다. (최적화)
std::string_view titleOf(const Player& p)
{
    return makeTitle(p);
}
}

void lab03_titles()
{
    const std::vector<Player> players = {
        { "Kael the Wanderer", "Northwind Rangers" },
        { "Mira Stormcaller",  "Order of the Ember" },
        { "Doran Ironhand",    "Deepforge Clan" },
    };

    std::vector<std::string_view> titles;
    for (const Player& p : players)
        titles.push_back(titleOf(p));

    expect("첫 번째 칭호", std::string("[Northwind Rangers] Kael the Wanderer"), std::string(titles[0]));
    expect("세 번째 칭호", std::string("[Deepforge Clan] Doran Ironhand"), std::string(titles[2]));
}
