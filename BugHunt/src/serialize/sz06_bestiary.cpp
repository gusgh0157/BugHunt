#include "../labs.h"
#include <pugixml.hpp>
#include <string>
#include <vector>

// 몬스터 도감(XML)을 읽는다. 이번 패치에서 데이터 담당이 형식을 바꿨다:
//     예전: <Monster name="고블린"><HP>45</HP><Attack>7</Attack></Monster>
//     지금: <Monster name="고블린" hp="45"><Attack>7</Attack></Monster>

// [JA] モンスター図鑑(XML)を読み込みます。今回のパッチでデータ担当が形式を変えました:
// [JA]     以前: <Monster name="ゴブリン"><HP>45</HP><Attack>7</Attack></Monster>
// [JA]     現在: <Monster name="ゴブリン" hp="45"><Attack>7</Attack></Monster>
namespace sz6
{
struct Monster
{
    std::string name;
    int hp = 0;
    int attack = 0;
};

std::vector<Monster> loadBestiary(const char* xml)
{
    std::vector<Monster> list;
    pugi::xml_document doc;
    if (!doc.load_string(xml))
        return list;
    for (pugi::xml_node node : doc.child("Bestiary").children("Monster"))
    {
        Monster m;
        m.name = node.attribute("name").as_string();
        m.hp = node.child("hp").text().as_int();
        m.attack = node.child("Attack").text().as_int();
        list.push_back(m);
    }
    return list;
}
}

void sz06_bestiary()
{
    using namespace sz6;

    const char* xml = R"(
<Bestiary>
  <Monster name="고블린" hp="45"><Attack>7</Attack></Monster>
  <Monster name="오우거" hp="320"><Attack>25</Attack></Monster>
  <Monster name="박쥐" hp="12"><Attack>3</Attack></Monster>
</Bestiary>)";

    const std::vector<Monster> list = loadBestiary(xml);
    for (const Monster& m : list)
        std::cout << "  " << m.name << ": HP " << m.hp << ", 공격력 " << m.attack << '\n';

    expect("도감 몬스터 수", static_cast<size_t>(3), list.size());
    expect("오우거 HP", 320, list.size() > 1 ? list[1].hp : -1);
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(Ctrl+M, Ctrl+O = すべて折りたたむ)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// m.hp = ... 줄에 중단점을 거세요. 조사식에 node.child("hp").empty() 와 node.attribute("hp").value() 를 넣어 보세요.

// [JA] m.hp = ... の行にブレークポイントを置いてください。ウォッチに node.child("hp").empty() と node.attribute("hp").value() を入れてみてください。
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// pugixml은 없는 자식/속성을 찾으면 예외 대신 "빈 노드"를 돌려주고, 빈 노드의 as_int()는 0입니다.
// 그래서 이름을 잘못 쓰면(대소문자 "Attack"/"attack" 포함) 아무 경고 없이 0이 들어갑니다.
// hp는 지금 자식 요소인가요, 속성인가요?

// [JA] pugixml はない子要素/属性を探すと、例外の代わりに「空のノード」を返し、空のノードの as_int() は0です。
// [JA] そのため名前を間違えると(大文字小文字 "Attack"/"attack" も含む)、何の警告もなく0が入ります。
// [JA] hp は今、子要素でしょうか、属性でしょうか?
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// 속성은 attribute("hp").as_int()로 읽습니다. 꼭 있어야 하는 값이면 빈 노드/속성인지 먼저 확인해서
// 데이터가 잘못되었다고 알리세요(0을 조용히 쓰지 말고).

// [JA] 属性は attribute("hp").as_int() で読みます。必ずあるべき値なら、まず空のノード/属性かどうかを確認して、
// [JA] データが間違っていると知らせてください(黙って0を使わずに)。
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(grade.bat)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void sz06_bestiary_hidden()
{
    using namespace sz6;

    const std::vector<Monster> list = loadBestiary(R"(
<Bestiary>
  <Monster hp="900" name="드래곤"><Attack>80</Attack></Monster>
  <Monster name="슬라임" hp="5"><Attack>1</Attack></Monster>
</Bestiary>)");
    expect("숨은 도감: 몬스터 수", static_cast<size_t>(2), list.size());
    if (list.size() == 2)
    {
        expect("숨은 도감: 드래곤 HP", 900, list[0].hp);
        expect("숨은 도감: 슬라임 HP", 5, list[1].hp);
        expect("숨은 도감: 드래곤 공격력", 80, list[0].attack);
    }
}
#pragma endregion
