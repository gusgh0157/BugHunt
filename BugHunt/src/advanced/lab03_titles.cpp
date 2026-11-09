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

// [JA] 呼び出すたびに文字列をコピーしないよう、string_view で返します。(最適化)
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

    expect(i18n::L("첫 번째 칭호", "1番目の称号"), std::string("[Northwind Rangers] Kael the Wanderer"), std::string(titles[0]));
    expect(i18n::L("세 번째 칭호", "3番目の称号"), std::string("[Deepforge Clan] Doran Ironhand"), std::string(titles[2]));
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (편집기의 접기 기능으로 모두 접어 두세요)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(エディターの折りたたみ機能ですべて折りたたんでおいてください)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// titleOf가 돌려주는 string_view가 가리키는 문자열은 누가 소유하나요?

// [JA] titleOf が返す string_view が指している文字列は、誰が所有していますか?
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// 그 문자열의 수명은 어디서 끝나나요? ASan으로 확인해 보세요.

// [JA] その文字列の寿命はどこで終わりますか? ASan で確かめてみてください。
#pragma endregion
