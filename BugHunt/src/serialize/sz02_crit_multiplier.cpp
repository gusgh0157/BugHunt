#include "../labs.h"
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

// 무기 표(JSON)를 읽는다. "crit"(치명타 배율)이 없는 무기는 배율 1을 쓴다.
// 이번 패치에서 기획자가 단검의 배율을 2 → 1.5로 바꿨다.

// [JA] 武器表(JSON)を読み込みます。"crit"(クリティカル倍率)がない武器は倍率1を使います。
// [JA] 今回のパッチでプランナーが短剣の倍率を2 → 1.5に変えました。
namespace sz2
{
using nlohmann::json;

struct Weapon
{
    std::string name;
    int attack = 0;
    double crit = 1.0;     // 치명타 배율
};

Weapon loadWeapon(const json& j)
{
    Weapon w;
    w.name = j.at("name").get<std::string>();
    w.attack = j.at("attack").get<int>();
    w.crit = j.value("crit", 1);
    return w;
}

int critDamage(const Weapon& w)
{
    return static_cast<int>(w.attack * w.crit);
}
}

void sz02_crit_multiplier()
{
    using namespace sz2;

    const json table = json::parse(i18n::L(R"([
        {"name":"단검", "attack":30, "crit":1.5},
        {"name":"도끼", "attack":50, "crit":2},
        {"name":"지팡이", "attack":20}
    ])", R"([
        {"name":"短剣", "attack":30, "crit":1.5},
        {"name":"斧", "attack":50, "crit":2},
        {"name":"杖", "attack":20}
    ])"));

    std::vector<Weapon> weapons;
    for (const json& j : table)
        weapons.push_back(loadWeapon(j));
    for (const Weapon& w : weapons)
        std::cout << "  " << w.name << i18n::L(": 공격력 ", ": 攻撃力 ") << w.attack << i18n::L(", 배율 ", "、倍率 ") << w.crit
                  << i18n::L(", 치명타 피해 ", "、クリティカルダメージ ") << critDamage(w) << '\n';

    expect("단검 치명타 피해", 45, critDamage(weapons[0]));
    expect("도끼 치명타 피해", 100, critDamage(weapons[1]));
    expect("지팡이 치명타 피해", 20, critDamage(weapons[2]));
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(Ctrl+M, Ctrl+O = すべて折りたたむ)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// loadWeapon에 조건부 중단점을 거세요. 조건: w.name == "단검" 을 return 줄에. 이때 w.crit은 얼마인가요?
// 조사식에 j["crit"] (또는 j.at("crit"))도 넣어 JSON에 들어 있는 값과 비교하세요.

// [JA] loadWeapon の return 行に条件付きブレークポイントを設定してください。条件: w.name == "短剣"(`--lang ko` では "단검")。このとき w.crit はいくつですか?
// [JA] ウォッチに j["crit"](または j.at("crit"))も入れて、JSONに入っている値と比べてください。
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// j.value(키, 기본값)이 돌려주는 값의 형식은 무엇이 정하나요? 기본값 1은 int입니다.
// value<int>로 읽으면 1.5는 예외 없이 조용히 잘려서 1이 됩니다. 그다음에 double에 대입해도 이미 늦었습니다.

// [JA] j.value(キー, 既定値) が返す値の型は何で決まるでしょうか? 既定値の1は int です。
// [JA] value<int> で読むと、1.5は例外もなく黙って切り捨てられて1になります。その後 double に代入しても、もう手遅れです。
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// 기본값을 원하는 형식으로 쓰세요(1.0). 또는 j.value<double>("crit", 1) 처럼 형식을 직접 적으세요.
// 반올림으로 맞추면 1.75나 2.25 같은 배율에서 다시 틀립니다.

// [JA] 既定値を欲しい型で書いてください(1.0)。または j.value<double>("crit", 1) のように型を直接書いてください。
// [JA] 四捨五入で合わせると、1.75や2.25のような倍率でまた間違えます。
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(grade.bat)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void sz02_crit_multiplier_hidden()
{
    using namespace sz2;

    expect("배율 2.25", 225, critDamage(loadWeapon(json::parse(i18n::L(R"({"name":"활","attack":100,"crit":2.25})", R"({"name":"弓","attack":100,"crit":2.25})")))));
    expect("배율 1.75", 70, critDamage(loadWeapon(json::parse(i18n::L(R"({"name":"창","attack":40,"crit":1.75})", R"({"name":"槍","attack":40,"crit":1.75})")))));
    expect("배율 3 (정수)", 36, critDamage(loadWeapon(json::parse(i18n::L(R"({"name":"망치","attack":12,"crit":3})", R"({"name":"ハンマー","attack":12,"crit":3})")))));
    expect("배율 없음", 64, critDamage(loadWeapon(json::parse(i18n::L(R"({"name":"방패","attack":64})", R"({"name":"盾","attack":64})")))));
}
#pragma endregion
