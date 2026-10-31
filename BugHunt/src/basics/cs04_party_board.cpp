#include "../labs.h"
#include <string>
#include <vector>

// 파티 게시판과 레이드 알림.
namespace lab4
{
struct Party
{
    std::string name;
    std::vector<std::string> members;   // 첫 번째 멤버가 파티장
};

const std::string& leaderOf(const Party& party)
{
    return party.members[0];
}

void printPartyBoard(const std::vector<Party>& parties)
{
    std::cout << "  [파티 게시판]\n";
    for (const Party& party : parties)
    {
        if (party.members.empty())
        {
            std::cout << "    " << party.name << " (빈 파티)\n";
            continue;
        }
        std::cout << "    " << party.name << " - 파티장 " << leaderOf(party) << '\n';
    }
}

void sendRaidReminders(const std::vector<Party>& parties)
{
    std::cout << "  [레이드 알림]\n";
    for (const Party& party : parties)
        std::cout << "    " << leaderOf(party) << "님, 20시 레이드를 잊지 마세요!\n";
}

void showWelcome(const Party& myParty)
{
    std::cout << "  환영합니다! 우리 파티장: " << leaderOf(myParty) << '\n';
}

void runPartyScreen()
{
    const std::vector<Party> parties = {
        { "Northwind",     { "Kael", "Mira" } },
        { "Silent Hollow", {} },                       // 멤버가 모두 탈퇴함
        { "Deepforge",     { "Doran", "Brakka", "Ulf" } },
    };

    showWelcome(parties[0]);
    printPartyBoard(parties);
    sendRaidReminders(parties);
}
}

void cs04_party_board()
{
    lab4::runPartyScreen();
}
