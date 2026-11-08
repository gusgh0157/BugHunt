#include "../labs.h"
#include <nlohmann/json.hpp>
#include <string>
#include <utility>
#include <vector>

// 스킬 바 설정 파일: { "스킬 이름": "단축키", ... }를 파일에 적힌 순서대로 1번 칸부터 채운다.
// 플레이어가 칸을 바꾸면 저장하고, 다음 접속 때 다시 읽는다.

// [JA] スキルバー設定ファイル: { "スキル名": "ショートカットキー", ... } を、ファイルに書かれた順に1番スロットから埋めます。
// [JA] プレイヤーがスロットを入れ替えたら保存し、次のログイン時に読み直します。
namespace sz3
{
using Json = nlohmann::json;

struct Slot
{
    std::string skill;
    std::string hotkey;
};

std::vector<Slot> loadSkillBar(const std::string& text)
{
    std::vector<Slot> bar;
    const Json j = Json::parse(text);
    for (const auto& [skill, hotkey] : j.items())
        bar.push_back({ skill, hotkey.get<std::string>() });
    return bar;
}

std::string saveSkillBar(const std::vector<Slot>& bar)
{
    Json j = Json::object();
    for (const Slot& slot : bar)
        j[slot.skill] = slot.hotkey;
    return j.dump();
}

std::string order(const std::vector<Slot>& bar)
{
    std::string s;
    for (const Slot& slot : bar)
        s += (s.empty() ? "" : " ") + slot.skill;
    return s;
}
}

void sz03_skill_bar()
{
    using namespace sz3;

    const std::string file = i18n::L(R"({"화염구":"1","얼음창":"2","번개":"3","치유":"4"})", R"({"ファイアボール":"1","アイススピア":"2","サンダー":"3","ヒール":"4"})");
    std::vector<Slot> bar = loadSkillBar(file);
    std::cout << i18n::L("  불러온 스킬 바: ", "  読み込んだスキルバー: ") << order(bar) << '\n';
    expect("불러온 순서", std::string(i18n::L("화염구 얼음창 번개 치유", "ファイアボール アイススピア サンダー ヒール")), order(bar));

    // 플레이어가 치유를 1번 칸으로 끌어다 놓았다
    bar = { { i18n::L("치유", "ヒール"), "1" }, { i18n::L("화염구", "ファイアボール"), "2" }, { i18n::L("얼음창", "アイススピア"), "3" }, { i18n::L("번개", "サンダー"), "4" } };
    const std::vector<Slot> reloaded = loadSkillBar(saveSkillBar(bar));
    std::cout << i18n::L("  저장 후 다시 불러온 스킬 바: ", "  保存後に読み込み直したスキルバー: ") << order(reloaded) << '\n';
    expect("저장 후 순서", std::string(i18n::L("치유 화염구 얼음창 번개", "ヒール ファイアボール アイススピア サンダー")), order(reloaded));
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(Ctrl+M, Ctrl+O = すべて折りたたむ)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// loadSkillBar의 for 줄에 중단점을 걸고 j를 펼쳐 보세요. 키는 파일에 적힌 순서로 들어 있나요?
// 한 바퀴씩 넘기며 skill을 보면 어떤 순서로 나오는지 알 수 있습니다.

// [JA] loadSkillBar の for の行にブレークポイントを置き、j を展開してみてください。キーはファイルに書かれた順に入っていますか?
// [JA] 1周ずつ進めながら skill を見ると、どの順番で出てくるかが分かります。
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// 순서가 가나다순(정확히는 UTF-8 바이트순)인가요? nlohmann::json의 객체는 std::map에 키를 담습니다.
// JSON 표준도 객체 안 키의 순서는 의미가 없다고 봅니다.

// [JA] 順番は辞書順(正確にはUTF-8のバイト順)になっていませんか? nlohmann::json のオブジェクトはキーを std::map に格納します。
// [JA] JSONの規格も、オブジェクト内のキーの順番には意味がないとしています。
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// 넣은 순서를 지키는 nlohmann::ordered_json이 있습니다. Json 별칭 한 줄만 바꾸면 읽기와 저장이 모두 바뀝니다.
// 더 좋은 설계는 순서가 의미 있는 데이터를 배열([{"skill":..,"hotkey":..}, ...])로 저장하는 것입니다.

// [JA] 挿入順を守る nlohmann::ordered_json があります。Json の別名1行を変えるだけで、読み込みと保存の両方が変わります。
// [JA] よりよい設計は、順番に意味のあるデータを配列([{"skill":..,"hotkey":..}, ...])で保存することです。
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(grade.bat)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void sz03_skill_bar_hidden()
{
    using namespace sz3;

    const std::vector<Slot> a = loadSkillBar(i18n::L(R"({"zeta":"Q","alpha":"W","방패 막기":"E","Mid":"R"})", R"({"zeta":"Q","alpha":"W","シールド ブロック":"E","Mid":"R"})"));
    expect("숨은 바: 불러온 순서", std::string(i18n::L("zeta alpha 방패 막기 Mid", "zeta alpha シールド ブロック Mid")), order(a));

    const std::vector<Slot> b = { { i18n::L("회복", "リカバリー"), "F1" }, { "Dash", "F2" }, { i18n::L("가속", "ヘイスト"), "F3" }, { "blink", "F4" }, { "Aura", "F5" } };
    expect("숨은 바: 저장 후 순서", std::string(i18n::L("회복 Dash 가속 blink Aura", "リカバリー Dash ヘイスト blink Aura")), order(loadSkillBar(saveSkillBar(b))));
}
#pragma endregion
