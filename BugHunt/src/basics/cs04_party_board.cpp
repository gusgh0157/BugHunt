#include "../labs.h"
#include <string>
#include <vector>

// 파티 게시판과 레이드 알림.

// [JA] パーティー掲示板とレイド通知。
namespace lab4
{
struct Party
{
    std::string name;
    std::vector<std::string> members;   // 첫 번째 멤버가 파티장 [JA] 最初のメンバーがパーティーリーダー
};

const std::string& leaderOf(const Party& party)
{
    return party.members.at(0);
}

void printPartyBoard(const std::vector<Party>& parties)
{
    std::cout << i18n::L("  [파티 게시판]\n", "  [パーティー掲示板]\n");
    for (const Party& party : parties)
    {
        if (party.members.empty())
        {
            std::cout << "    " << party.name << i18n::L(" (빈 파티)\n", " (空のパーティー)\n");
            continue;
        }
        std::cout << "    " << party.name << i18n::L(" - 파티장 ", " - リーダー ") << leaderOf(party) << '\n';
    }
}

void sendRaidReminders(const std::vector<Party>& parties)
{
    std::cout << i18n::L("  [레이드 알림]\n", "  [レイド通知]\n");
    for (const Party& party : parties)
        std::cout << "    " << leaderOf(party) << i18n::L("님, 20시 레이드를 잊지 마세요!\n", "さん、20時のレイドを忘れないでください!\n");
}

void showWelcome(const Party& myParty)
{
    std::cout << i18n::L("  환영합니다! 우리 파티장: ", "  ようこそ! うちのパーティーのリーダー: ") << leaderOf(myParty) << '\n';
}

void runPartyScreen()
{
    const std::vector<Party> parties = {
        { "Northwind",     { "Kael", "Mira" } },
        { "Silent Hollow", {} },                       // 멤버가 모두 탈퇴함 [JA] メンバーが全員脱退した
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

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (편집기의 접기 기능으로 모두 접어 두세요)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(エディターの折りたたみ機能ですべて折りたたんでおいてください)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 호출 스택을 보고 펼치세요 / まず呼び出し履歴を見てから開いてください)
// leaderOf를 부르는 곳은 세 군데입니다.
// 이번에는 그중 어디서 불렀는지 호출 스택이 정확히 알려 줍니다.

// [JA] leaderOf を呼んでいる場所は3か所です。
// [JA] 今回は、そのうちどこから呼んだのかを呼び出し履歴が正確に教えてくれます。
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// 빈 파티 검사는 printPartyBoard에 있습니다.
// 호출 스택에 printPartyBoard가 있나요?

// [JA] 空のパーティーのチェックは printPartyBoard にあります。
// [JA] 呼び出し履歴に printPartyBoard はありますか?
#pragma endregion
