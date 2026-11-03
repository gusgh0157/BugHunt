#include "../labs.h"
#include <string>

// 파티원의 공격력 = 기본 공격력 + 장착한 무기 보너스. 무기가 없으면 맨손(보너스 0).
namespace ptr5
{
struct Weapon
{
    std::string name;
    int bonus;
};

class Hero
{
public:
    Hero(const std::string& name, int baseAttack)
        : name_(name), baseAttack_(baseAttack)
    {
    }

    Hero(const std::string& name, int baseAttack, const Weapon* weapon)
        : name_(name), baseAttack_(baseAttack), weapon_(weapon)
    {
    }

    void equip(const Weapon* weapon) { weapon_ = weapon; }

    int attackPower() const
    {
        int power = baseAttack_;
        if (weapon_ != nullptr)
            power += weapon_->bonus;
        return power;
    }

    const std::string& name() const { return name_; }

private:
    std::string name_;
    int baseAttack_;
    const Weapon* weapon_;
};

int partyAttack(const Hero* heroes, int count)
{
    int total = 0;
    for (int i = 0; i < count; ++i)
    {
        const int power = heroes[i].attackPower();
        std::cout << "  " << heroes[i].name() << ": 공격력 " << power << '\n';
        total += power;
    }
    return total;
}
}

void pt05_equipment()
{
    using namespace ptr5;

    const Weapon flameSword = { "Flame Sword", 25 };
    const Weapon oakStaff = { "Oak Staff", 10 };

    const Hero party[] = {
        Hero("Kael", 30, &flameSword),
        Hero("Mira", 18, &oakStaff),
        Hero("Doran", 35),               // 맨손 격투가
    };

    expect("파티 총 공격력 (55 + 28 + 35)", 118, partyAttack(party, 3));
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// ============================================================================

#pragma region 힌트 1 (먼저 직접 추적해 보고 펼치세요)
// F5로 실행하면 "읽기 액세스 위반"에서 멈춥니다. 지역 창에서 this를 펼쳐 weapon_ 값을 읽어 보세요.
// 0x0000000000000000(nullptr)이 아니라면 어떤 값인가요? README의 매직 넘버 표를 보세요.
// (예외 메시지의 주소는 0xFFFFFFFFFFFFFFFF로 나올 수 있습니다. x64에서 "있을 수 없는 주소"를 읽으면 그렇게 표시됩니다.)
#pragma endregion

#pragma region 힌트 2
// weapon_의 값은 누가 넣었나요?
// Doran을 만든 생성자에 중단점을 걸고, 생성자가 끝난 직후 weapon_을 확인해 보세요.
#pragma endregion

#pragma region 힌트 3
// Release와 ASan 구성에서도 실행해 보세요. 같은 버그가 왜 매번 다르게 보일까요?
// "초기화하지 않은 포인터"는 nullptr 검사로 막을 수 없습니다. 검사하려면 먼저 nullptr로 만들어 둬야 합니다.
#pragma endregion

#pragma region 채점용 숨은 테스트 (풀이에는 필요 없습니다. 고치지 마세요)
// 채점기(grade.bat)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void pt05_equipment_hidden()
{
    using namespace ptr5;

    const Weapon bow = { "Long Bow", 14 };

    Hero rookie("Nell", 12);             // 맨손으로 시작했다가
    expect("Nell 맨손 공격력", 12, rookie.attackPower());
    rookie.equip(&bow);                  // 활을 든다
    expect("Nell 활 장착 공격력", 26, rookie.attackPower());

    const Hero duo[] = { Hero("Ivo", 40), Hero("Pia", 22, &bow) };
    expect("두 명 총 공격력", 76, partyAttack(duo, 2));
}
#pragma endregion
