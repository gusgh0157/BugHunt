#include "../labs.h"
#include <algorithm>

// 10초(600틱) 동안 보호막을 두른 탱커가 공격을 받는다.
// 받은 피해는 반드시 "보호막이 흡수"하거나 "HP가 깎이거나" 둘 중 하나여야 한다.

// [JA] 10秒(600ティック)のあいだシールドをまとったタンクが攻撃を受けます。
// [JA] 受けたダメージは必ず「シールドが吸収する」か「HPが減る」かのどちらかでなければなりません。
namespace flow3
{
constexpr int kTicks = 600;          // 60틱 = 1초
constexpr int kMaxShield = 30;

struct Unit
{
    int hp = 200;
    int shield = kMaxShield;
};

void takeDamage(Unit& unit, int damage)
{
    if (unit.shield > 0)
    {
        unit.shield -= damage;
        if (unit.shield < 0)
            unit.shield = 0;
        return;
    }
    unit.hp -= damage;
}

void rechargeShield(Unit& unit, int amount)
{
    unit.shield = std::min(kMaxShield, unit.shield + amount);
}
}

void ft03_shield()
{
    using namespace flow3;

    Unit tank;
    int received = 0;   // 들어온 피해 합계
    int absorbed = 0;   // 보호막이 줄어든 양
    int hpLost = 0;     // HP가 줄어든 양

    for (int tick = 1; tick <= kTicks; ++tick)
    {
        int damage = 0;
        if (tick % 37 == 0) damage += 7;    // 오크의 도끼
        if (tick % 50 == 0) damage += 4;    // 독 장판

        if (damage > 0)
        {
            const Unit before = tank;
            takeDamage(tank, damage);
            received += damage;
            absorbed += before.shield - tank.shield;
            hpLost += before.hp - tank.hp;
        }

        if (tick % 120 == 0)
            rechargeShield(tank, 10);
    }

    std::cout << i18n::L("  받은 피해 ", "  受けたダメージ") << received << i18n::L(", 보호막 흡수 ", "、シールド吸収") << absorbed
              << i18n::L(", HP 감소 ", "、HP減少") << hpLost << i18n::L(", 남은 HP ", "、残りHP ") << tank.hp << '\n';
    expect("받은 피해 == 보호막 흡수 + HP 감소", received, absorbed + hpLost);
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (편집기의 접기 기능으로 모두 접어 두세요)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(エディターの折りたたみ機能ですべて折りたたんでおいてください)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// 600틱을 한 줄씩 따라갈 수는 없습니다. 피해가 "사라진" 순간에만 멈추게 하세요.
// ft03_shield의 hpLost += ... 줄에 조건부 중단점을 거세요. 조건:
//     (before.shield - tank.shield) + (before.hp - tank.hp) != damage
// 이 조건이 참일 때만 멈춥니다. 멈추면 지역 변수에서 tick, damage, before, tank 를 보세요.

// [JA] 600ティックを1行ずつ追いかけることはできません。ダメージが「消えた」瞬間にだけ止まるようにしてください。
// [JA] ft03_shield の hpLost += ... の行に条件付きブレークポイントを置いてください。条件:
// [JA]     (before.shield - tank.shield) + (before.hp - tank.hp) != damage
// [JA] この条件が真のときだけ止まります。止まったら、ローカル変数で tick, damage, before, tank を見てください。
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// 멈췄을 때 before.shield는 damage보다 작았나요, 컸나요?
// 조건을 tick == (그때의 tick) 으로 바꿔 takeDamage 호출 줄에 걸고 다시 실행한 뒤, 그 함수 안으로 들어가 보세요.

// [JA] 止まったとき、before.shield は damage より小さかったですか、大きかったですか?
// [JA] 条件を tick == (そのときの tick) に変えて takeDamage の呼び出し行に置き、もう一度実行してから、その関数の中に入ってみてください。
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// 보호막이 피해보다 적게 남아 있을 때, 보호막이 막지 못한 나머지 피해는 어디로 가야 할까요?

// [JA] シールドがダメージより少なく残っているとき、シールドが防げなかった残りのダメージはどこへ行くべきでしょうか?
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(--grade)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void ft03_shield_hidden()
{
    using namespace flow3;

    Unit full;                       // 보호막 30이 피해 7을 전부 막는다
    takeDamage(full, 7);
    expect("보호막 충분: 보호막", 23, full.shield);
    expect("보호막 충분: HP", 200, full.hp);

    Unit cracked;                    // 보호막 5가 피해 12 중 5만 막는다
    cracked.shield = 5;
    takeDamage(cracked, 12);
    expect("보호막 부족: 보호막", 0, cracked.shield);
    expect("보호막 부족: HP", 193, cracked.hp);

    Unit bare;                       // 보호막이 없으면 전부 HP로
    bare.shield = 0;
    takeDamage(bare, 9);
    expect("보호막 없음: HP", 191, bare.hp);
}
#pragma endregion
