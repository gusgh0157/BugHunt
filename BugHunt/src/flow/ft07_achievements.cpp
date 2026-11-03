#include "../labs.h"
#include <string>
#include <vector>

// 전투가 끝나면 업적 조건을 검사해 새로 달성한 업적을 해금한다.
namespace flow7
{
struct Player
{
    int wolfKills = 0;
    int gold = 0;
    int level = 1;
    std::vector<std::string> achievements;

    bool has(const std::string& id) const
    {
        for (const std::string& a : achievements)
        {
            if (a == id)
                return true;
        }
        return false;
    }
};

// 새로 해금했으면 true. 이미 가진 업적이면 false.
bool unlock(Player& p, const std::string& id)
{
    if (p.has(id))
        return false;
    p.achievements.push_back(id);
    std::cout << "  ★ 업적 달성: " << id << '\n';
    return true;
}

bool checkKillAchievements(Player& p)
{
    return p.wolfKills >= 100 && unlock(p, "늑대 사냥꾼");
}

bool checkGoldAchievements(Player& p)
{
    return p.gold >= 10000 && unlock(p, "부자");
}

bool checkLevelAchievements(Player& p)
{
    return p.level >= 10 && unlock(p, "베테랑");
}

// 업적을 모두 검사하고, 하나라도 새로 열렸으면 축하 효과를 보여 준다.
void onBattleEnd(Player& p)
{
    const bool anyUnlocked = checkKillAchievements(p)
                          || checkGoldAchievements(p)
                          || checkLevelAchievements(p);
    if (anyUnlocked)
        std::cout << "  (축하 폭죽!)\n";
}
}

void ft07_achievements()
{
    flow7::Player player;
    player.wolfKills = 99;
    player.gold = 3200;
    player.level = 9;

    // 이번 전투에서 100번째 늑대를 잡고, 경험치를 받아 10레벨이 되었다.
    player.wolfKills += 1;
    player.level += 1;
    flow7::onBattleEnd(player);

    expect("해금된 업적 수", 2, static_cast<int>(player.achievements.size()));
    expect("'베테랑' 업적", true, player.has("베테랑"));
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// ============================================================================

#pragma region 힌트 1 (먼저 직접 추적해 보고 펼치세요)
// checkKillAchievements, checkGoldAchievements, checkLevelAchievements 첫 줄에 각각 중단점을 거세요.
// F5로 실행하면 어느 중단점이 걸리나요? "걸리지 않는 중단점"도 중요한 정보입니다.
#pragma endregion

#pragma region 힌트 2
// onBattleEnd의 const bool anyUnlocked 줄에서 F11을 여러 번 눌러 보세요.
// 첫 번째 검사에서 돌아온 직후, 노란 화살표는 어디로 가나요?
// (팁: 그 줄에서 우클릭 → "한 단계씩 코드 실행 - 특정"으로 들어갈 함수를 고를 수 있습니다.)
#pragma endregion

#pragma region 힌트 3
// A || B 에서 A가 true이면, C++은 B를 계산할까요?
#pragma endregion

#pragma region 채점용 숨은 테스트 (풀이에는 필요 없습니다. 고치지 마세요)
// 채점기(grade.bat)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void ft07_achievements_hidden()
{
    flow7::Player player;            // 늑대는 조금, 골드와 레벨은 충분
    player.wolfKills = 5;
    player.gold = 12000;
    player.level = 10;
    flow7::onBattleEnd(player);

    expect("해금된 업적 수", 2, static_cast<int>(player.achievements.size()));
    expect("'부자' 업적", true, player.has("부자"));
    expect("'베테랑' 업적", true, player.has("베테랑"));
}
#pragma endregion
