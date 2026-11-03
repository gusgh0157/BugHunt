#include "../labs.h"

// 공격 한 번의 피해량을 단계별로 계산한다.
// 기본 피해 → 치명타 → 방어력 → 속성 약점
namespace flow2
{
struct Attacker
{
    int weaponPower;
    int strength;
    int critBonusPercent;   // 치명타 추가 피해 (%)
};

struct Target
{
    int defense;
    bool weakToFire;
};

int baseDamage(const Attacker& attacker)
{
    return attacker.weaponPower + attacker.strength * 2;
}

int applyCrit(int damage, bool isCrit, int critBonusPercent)
{
    int result = damage;
    if (isCrit)
        damage = damage * (100 + critBonusPercent) / 100;
    return result;
}

int applyDefense(int damage, int defense)
{
    return damage * 100 / (100 + defense);
}

int applyElement(int damage, bool weak)
{
    return weak ? damage * 3 / 2 : damage;
}

int computeDamage(const Attacker& attacker, const Target& target, bool isCrit)
{
    int damage = baseDamage(attacker);
    damage = applyCrit(damage, isCrit, attacker.critBonusPercent);
    damage = applyDefense(damage, target.defense);
    damage = applyElement(damage, target.weakToFire);
    return damage;
}
}

void ft02_critical()
{
    const flow2::Attacker mage = { 40, 15, 50 };
    const flow2::Target iceGolem = { 25, true };

    expect("일반 공격 피해", 84, flow2::computeDamage(mage, iceGolem, false));
    expect("치명타 피해", 126, flow2::computeDamage(mage, iceGolem, true));
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// ============================================================================

#pragma region 힌트 1 (먼저 직접 추적해 보고 펼치세요)
// computeDamage 첫 줄에 중단점을 걸면 일반 공격에서 먼저 멈춥니다. F5를 한 번 더 눌러 치명타 호출로 가세요.
// 그다음 F10으로 한 줄씩 넘기며 damage가 단계마다 몇이 되는지 적어 보세요. 기대값: 70 → 105 → 84 → 126
#pragma endregion

#pragma region 힌트 2
// 숫자가 기대와 처음 달라지는 단계에서 F11로 그 함수 안에 들어가세요.
// 안에서 F10으로 끝까지 가거나 Shift+F11로 나오면, 자동 창에 "applyXXX 반환" 줄이 보입니다.
// 함수 안에서 계산한 값과 반환한 값이 같나요?
#pragma endregion

#pragma region 채점용 숨은 테스트 (풀이에는 필요 없습니다. 고치지 마세요)
// 채점기(grade.bat)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void ft02_critical_hidden()
{
    const flow2::Attacker rogue = { 30, 20, 100 };
    const flow2::Target dummy = { 0, false };
    expect("도적 일반 공격", 70, flow2::computeDamage(rogue, dummy, false));
    expect("도적 치명타", 140, flow2::computeDamage(rogue, dummy, true));

    const flow2::Attacker archer = { 25, 10, 25 };
    const flow2::Target fireSpirit = { 50, true };
    expect("궁수 일반 공격", 45, flow2::computeDamage(archer, fireSpirit, false));
    expect("궁수 치명타", 55, flow2::computeDamage(archer, fireSpirit, true));
}
#pragma endregion
