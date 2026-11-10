#include "../labs.h"
#include <cstdio>
#include <cstring>
#include <vector>

namespace
{
struct Character
{
    char name[8];
    int hp;
    int level;
};

Character makeCharacter(const char* name, int hp, int level)
{
    Character c{};
    std::snprintf(c.name, sizeof(c.name), "%s", name);
    c.hp = hp;
    c.level = level;
    return c;
}

void setName(Character& c, const char* newName)
{
    std::memcpy(c.name, newName, std::strlen(newName) + 1);
}

// 레벨 10 이상인 캐릭터에게 기사 작위를 준다.

// [JA] レベル10以上のキャラクターに騎士の称号を与えます。
void grantKnighthood(std::vector<Character>& party)
{
    for (Character& c : party)
    {
        if (c.level >= 10)
        {
            char titled[32];
            std::snprintf(titled, sizeof(titled), "Sir %s", c.name);
            setName(c, titled);
        }
    }
}

void levelUpAll(std::vector<Character>& party)
{
    for (Character& c : party)
        ++c.level;
}
}

void lab10_party()
{
    std::vector<Character> party = {
        makeCharacter("Kael",  100, 12),
        makeCharacter("Mira",   80,  5),
        makeCharacter("Doran", 120, 15),
    };

    levelUpAll(party);
    grantKnighthood(party);
    levelUpAll(party);

    // 전투는 한 번도 없었으므로 HP는 처음 그대로여야 한다.
    // [JA] 戦闘は一度もなかったので、HPは最初のままのはずです。
    expect(i18n::L("Kael HP", "Kael HP"),  100, party[0].hp);
    expect(i18n::L("Mira HP", "Mira HP"),   80, party[1].hp);
    expect(i18n::L("Doran HP", "Doran HP"), 120, party[2].hp);
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (편집기의 접기 기능으로 모두 접어 두세요)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(エディターの折りたたみ機能ですべて折りたたんでおいてください)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// 첫 levelUpAll(party); 줄에 중단점 → 조사식에 &party[0].hp
// → 그 주소에 데이터 중단점(watchpoint, "값이 바뀌면 멈춤")을 걸고 계속 실행

// [JA] 最初の levelUpAll(party); の行にブレークポイント → ウォッチに &party[0].hp
// [JA] → そのアドレスにデータブレークポイント(watchpoint、「値が変わったら中断」)を置いて続行
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// 멈춘 곳의 호출 스택을 보세요. hp를 쓰는 코드가 아닌데 hp가 바뀌었다면?

// [JA] 止まった場所の呼び出し履歴を見てください。hp に書き込むコードではないのに hp が変わったとしたら?
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// 메모리 보기에 &party[0]을 넣고 name과 hp가 메모리에서 어떻게 붙어 있는지 보세요.
// "Sir Kael"은 몇 바이트인가요?

// [JA] メモリ ウィンドウに &party[0] を入れて、name と hp がメモリ上でどう並んでいるかを見てください。
// [JA] "Sir Kael" は何バイトですか?
#pragma endregion

#pragma region 참고 / 参考
// 이 버그는 ASan도 못 잡습니다(구조체 안에서 넘친 것이라 할당 범위 밖이 아님). 데이터 중단점이 답입니다.

// [JA] このバグは ASan でも捕まえられません(構造体の中であふれたので、割り当て範囲の外ではありません)。データブレークポイントが答えです。
#pragma endregion
