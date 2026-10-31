#include "../labs.h"
#include <string>
#include <vector>

// 친구 목록에서 플레이어를 골라 프로필을 보여 준다.
namespace lab1
{
struct Player
{
    std::string name;
    int level;
    int hp;
};

std::vector<Player> g_players = {
    { "Kael",  12, 100 },
    { "Mira",   5,  80 },
    { "Doran", 15, 120 },
};

Player* findPlayer(const std::string& name)
{
    for (Player& p : g_players)
    {
        if (p.name == name)
            return &p;
    }
    return nullptr;
}

void printPlayer(const Player* player)
{
    std::cout << "  Lv." << player->level << " " << player->name
              << " (HP " << player->hp << ")\n";
}

void showProfile(const std::string& name)
{
    Player* player = findPlayer(name);
    printPlayer(player);
}

void openFriendList()
{
    showProfile("Kael");
    showProfile("Mira");
    showProfile("Dorna");
}
}

void cs01_profile()
{
    lab1::openFriendList();
}
