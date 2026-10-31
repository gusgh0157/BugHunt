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

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// ============================================================================

#pragma region 힌트 1 (README의 "따라 해 보기"를 먼저 하세요)
// showProfile 프레임을 더블클릭하고, 지역 창에서 name 값을 보세요.
#pragma endregion

#pragma region 힌트 2
// 그 name을 g_players에 있는 이름들과 한 글자씩 비교해 보세요.
// 그리고 openFriendList 프레임에서 초록 화살표가 가리키는 줄을 보세요.
#pragma endregion
