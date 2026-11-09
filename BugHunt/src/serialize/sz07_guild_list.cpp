#include "../labs.h"
#include <pugixml.hpp>
#include <string>
#include <vector>

// 길드 목록을 문자열을 이어 붙여 XML로 저장하고, pugixml로 다시 읽는다.

// [JA] ギルド一覧を文字列の連結でXMLとして保存し、pugixml で読み直します。
namespace sz7
{
struct Guild
{
    std::string name;
    int members = 0;
};

std::string saveGuilds(const std::vector<Guild>& guilds)
{
    std::string xml = "<Guilds>\n";
    for (const Guild& g : guilds)
    {
        xml += "  <Guild members=\"" + std::to_string(g.members) + "\">"
             + "<Name>" + g.name + "</Name></Guild>\n";
    }
    xml += "</Guilds>\n";
    return xml;
}

std::vector<Guild> loadGuilds(const std::string& xml)
{
    std::vector<Guild> guilds;
    pugi::xml_document doc;
    const pugi::xml_parse_result result = doc.load_string(xml.c_str());
    if (!result)
    {
        std::cout << i18n::L("  길드 목록 읽기 실패: ", "  ギルド一覧の読み込み失敗: ") << result.description() << i18n::L(" (위치 ", " (位置 ")
                  << result.offset << ")\n";
        return guilds;
    }
    for (pugi::xml_node node : doc.child("Guilds").children("Guild"))
        guilds.push_back({ node.child("Name").text().as_string(), node.attribute("members").as_int() });
    return guilds;
}
}

void sz07_guild_list()
{
    using namespace sz7;

    const std::vector<Guild> guilds = { { i18n::L("새벽의 검", "暁の剣"), 24 }, { i18n::L("<검과 방패>", "<剣と盾 騎士団>"), 9 }, { i18n::L("달빛", "月光"), 41 } };
    const std::string xml = saveGuilds(guilds);
    const std::vector<Guild> loaded = loadGuilds(xml);

    for (const Guild& g : loaded)
        std::cout << "  " << g.name << " (" << g.members << i18n::L("명)\n", "人)\n");

    expect(i18n::L("불러온 길드 수", "読み込んだギルド数"), static_cast<size_t>(3), loaded.size());
    expect(i18n::L("둘째 길드 이름", "2番目のギルド名"), std::string(i18n::L("<검과 방패>", "<剣と盾 騎士団>")), loaded.size() > 1 ? loaded[1].name : std::string());
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(Ctrl+M, Ctrl+O = すべて折りたたむ)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// loadGuilds의 return guilds; (실패 쪽) 줄에 중단점을 걸고 result.offset을 보세요.
// 그다음 조사식에 xml.c_str() + result.offset 을 넣으면 파서가 막힌 곳부터 글자가 보입니다.

// [JA] loadGuilds の return guilds;(失敗側)の行にブレークポイントを置き、result.offset を見てください。
// [JA] 次にウォッチに xml.c_str() + result.offset を入れると、パーサーが詰まった場所からの文字が見えます。
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// XML에서 <, &, 따옴표는 특별한 글자입니다. 이름 속의 <는 파서에게 "새 태그 시작"으로 보입니다.
// "I &lt;3 PvP" 같은 이름은 오류도 없이 다른 이름("I <3 PvP")으로 바뀌어 읽힙니다.

// [JA] XMLでは <、&、引用符は特別な文字です。名前の中の < は、パーサーには「新しいタグの始まり」に見えます。
// [JA] "I &lt;3 PvP" のような名前は、エラーもなく別の名前("I <3 PvP")に変わって読まれます。
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// 문자열을 이어 붙여 XML을 만들 때는 &를 가장 먼저 &amp;로, 그다음 <, >, ", '를 &lt; &gt; &quot; &apos;로 바꾸세요.
// 더 안전한 방법은 pugixml로 문서를 만들고(append_child, text().set) doc.save()로 쓰는 것입니다.

// [JA] 文字列を連結してXMLを作るときは、まず & を &amp; に、次に <、>、"、' を &lt; &gt; &quot; &apos; に置き換えてください。
// [JA] より安全な方法は、pugixml で文書を組み立て(append_child、text().set)、doc.save() で書き出すことです。
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(grade.bat)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void sz07_guild_list_hidden()
{
    using namespace sz7;

    const std::vector<Guild> guilds = {
        { "I &lt;3 PvP", 3 },          // 글자 그대로 &lt; 가 들어간 이름
        { "Tom & Jerry", 2 },
        { "a<b>c", 5 },
        { i18n::L("\"따옴표\" 'Q'", "\"引用符\" 'Q'"), 7 },
    };
    const std::vector<Guild> loaded = loadGuilds(saveGuilds(guilds));
    expect(i18n::L("숨은 목록: 길드 수", "隠し一覧: ギルド数"), guilds.size(), loaded.size());
    for (size_t i = 0; i < guilds.size() && i < loaded.size(); ++i)
        expect(i18n::L("숨은 목록: 이름 왕복", "隠し一覧: 名前の往復"), guilds[i].name, loaded[i].name);
}
#pragma endregion
