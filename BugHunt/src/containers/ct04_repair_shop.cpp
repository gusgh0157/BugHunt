#include "../labs.h"
#include <algorithm>
#include <string>
#include <vector>

// 장비 정리. 내구도가 0이 된(부서진) 장비를 가방에서 버린다.
namespace ct4
{
struct Gear
{
    std::string name;
    int durability;
};

void discardBroken(std::vector<Gear>& bag)
{
    std::remove_if(bag.begin(), bag.end(), [](const Gear& g) { return g.durability == 0; });
}

int countNamed(const std::vector<Gear>& bag, const std::string& name)
{
    return static_cast<int>(std::count_if(bag.begin(), bag.end(), [&](const Gear& g) { return g.name == name; }));
}
}

void ct04_repair_shop()
{
    std::vector<ct4::Gear> bag = {
        { "Iron Sword", 0 }, { "Oak Shield", 35 }, { "Leather Cap", 0 },
        { "Torch", 10 }, { "Bone Bow", 0 }, { "Silver Ring", 100 },
    };

    ct4::discardBroken(bag);

    std::cout << "  가방:";
    for (const ct4::Gear& g : bag)
        std::cout << ' ' << g.name << '(' << g.durability << ')';
    std::cout << '\n';

    expect("남은 장비 수", static_cast<size_t>(3), bag.size());
    expect("Silver Ring 개수", 1, ct4::countNamed(bag, "Silver Ring"));
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (편집기의 접기 기능으로 모두 접어 두세요)
// ============================================================================

#pragma region 힌트 1 (먼저 직접 추적해 보고 펼치세요)
// 빌드할 때 나온 이 파일의 컴파일러 경고를 읽어 보세요.
// discardBroken 호출 전후로 지역 변수 bag을 펼쳐 비교해 보세요. 크기와 내용이 각각 어떻게 바뀌었나요?
#pragma endregion

#pragma region 힌트 2
// remove_if의 반환값은 무엇을 가리키나요? 함수 이름은 remove지만, vector의 크기를 바꿀 수 있을까요?
// (remove_if는 이터레이터만 받습니다. vector 객체 자체를 모릅니다.)
#pragma endregion

#pragma region 채점용 숨은 테스트 (풀이에는 필요 없습니다. 고치지 마세요)
// 채점기(--grade)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void ct04_repair_shop_hidden()
{
    std::vector<ct4::Gear> bag = {
        { "Rope", 5 }, { "Lantern", 0 }, { "Lantern", 0 }, { "Map", 1 }, { "Lantern", 7 }, { "Pick", 0 }, { "Rope", 0 },
    };
    ct4::discardBroken(bag);
    expect("남은 장비 수", static_cast<size_t>(3), bag.size());
    expect("Lantern 개수", 1, ct4::countNamed(bag, "Lantern"));
    expect("Rope 개수", 1, ct4::countNamed(bag, "Rope"));

    std::vector<ct4::Gear> fine = { { "Helm", 9 } };
    ct4::discardBroken(fine);
    expect("버릴 게 없을 때", static_cast<size_t>(1), fine.size());
}
#pragma endregion
