#include "../labs.h"
#include <string>

// 캐릭터 머리 위 이름표. 이름표는 매 프레임 그려지므로 문자열을 복사하지 않고 "가리키기만" 한다.
// 칭호를 얻으면 이름 뒤에 칭호가 붙고, 이름표도 새 이름을 보여 줘야 한다.
namespace ct6
{
struct Hero
{
    std::string name;
};

class Nameplate
{
public:
    explicit Nameplate(const Hero& hero) : text_(hero.name.c_str()) {}

    const char* text() const { return text_; }

private:
    const char* text_;
};

void grantTitle(Hero& hero, const std::string& title)
{
    hero.name += " " + title;
}
}

void ct06_nameplates()
{
    using namespace ct6;

    Hero heroes[3] = { { "Kael" }, { "Doran" }, { "Selene Stormcaller" } };
    const Nameplate plates[3] = { Nameplate(heroes[0]), Nameplate(heroes[1]), Nameplate(heroes[2]) };

    grantTitle(heroes[0], "the Brave");
    grantTitle(heroes[1], "the Unbreakable");
    grantTitle(heroes[2], "of the Ember Order");

    for (int i = 0; i < 3; ++i)
        std::cout << "  이름표: " << plates[i].text() << '\n';

    expect("Kael 이름표", std::string("Kael the Brave"), std::string(plates[0].text()));
    expect("Doran 이름표", std::string("Doran the Unbreakable"), std::string(plates[1].text()));
    expect("Selene 이름표", std::string("Selene Stormcaller of the Ember Order"), std::string(plates[2].text()));
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (편집기의 접기 기능으로 모두 접어 두세요)
// ============================================================================

#pragma region 힌트 1 (먼저 직접 추적해 보고 펼치세요)
// 세 이름표 중 맞게 나오는 것이 있나요? 틀린 것들은 어떻게 틀렸나요? 틀린 모양도 서로 다를 수 있습니다.
// grantTitle 호출 세 줄 앞뒤로 조사식에 (void*)heroes[i].name.c_str() 와 (void*)plates[i].text() 를 넣고 비교해 보세요.
#pragma endregion

#pragma region 힌트 2
// std::string은 짧은 문자열을 객체 안에 바로 담고, 길면 힙에 따로 할당합니다(SSO, 짧은 문자열 최적화).
// 몇 글자까지 안에 담는지는 표준 라이브러리마다 다릅니다(MSVC STL과 libstdc++는 15바이트, libc++는 22바이트).
// 칭호가 붙은 뒤 각 이름의 길이는? 그 한도를 넘었나요? 원래부터 힙에 있었나요?
// ASan 구성으로도 실행해 보세요. 셋 중 어느 것을 잡나요? 못 잡는 것은 왜 못 잡을까요?
#pragma endregion

#pragma region 힌트 3
// c_str()이 돌려준 포인터는 그 문자열이 "바뀌기 전까지"만 유효합니다. 이름표가 가리켜야 할 것은 무엇일까요?
#pragma endregion

#pragma region 채점용 숨은 테스트 (풀이에는 필요 없습니다. 고치지 마세요)
// 채점기(--grade)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void ct06_nameplates_hidden()
{
    using namespace ct6;

    Hero rin{ "Rin" };
    Hero bram{ "Bram Longshadow of Deepforge" };
    const Nameplate a(rin);
    const Nameplate b(bram);
    grantTitle(rin, "Archmage of the Northern Tower");
    grantTitle(bram, "the Elder Smith and Keeper of Flames");

    expect("Rin 이름표", std::string("Rin Archmage of the Northern Tower"), std::string(a.text()));
    expect("Bram 이름표", std::string("Bram Longshadow of Deepforge the Elder Smith and Keeper of Flames"), std::string(b.text()));
}
#pragma endregion
