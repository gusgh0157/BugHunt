#include "../labs.h"
#include <array>

// 스킬 트리: 영웅은 스킬 4개(1번 ~ 4번)를 가지고, 골드 100을 내고 스킬 레벨을 하나씩 올린다.
// 화면의 스킬 번호는 1번부터 센다. 없는 번호는 거절한다.

// [JA] スキルツリー: 英雄はスキルを4つ(1番 ~ 4番)持ち、ゴールド100を払ってスキルレベルを一つずつ上げます。
// [JA] 画面のスキル番号は1番から数えます。存在しない番号は拒否します。
namespace ovf5
{
constexpr int kSkillCost = 100;

struct Hero
{
    std::array<int, 4> skillLevels{};   // 스킬 1번 ~ 4번의 레벨
    int gold = 0;
};

// 스킬 skillId(1 ~ 4)의 레벨을 올린다. 성공하면 true
bool learnSkill(Hero& hero, int skillId)
{
    if (skillId < 1 || skillId > 4 || hero.gold < kSkillCost)
        return false;
    hero.skillLevels[skillId] += 1;
    hero.gold -= kSkillCost;
    return true;
}

void print(const Hero& hero)
{
    std::cout << i18n::L("  스킬 레벨:", "  スキルレベル:");
    for (int level : hero.skillLevels)
        std::cout << ' ' << level;
    std::cout << i18n::L("  / 골드 ", "  / ゴールド ") << hero.gold << '\n';
}
}

void of05_skill_tree()
{
    using namespace ovf5;

    Hero hero;
    hero.gold = 1'000;
    std::cout << i18n::L("  2번 스킬(방패 막기) 배우기\n", "  2番スキル(シールドブロック)を覚える\n");
    learnSkill(hero, 2);
    print(hero);
    std::cout << i18n::L("  4번 스킬(번개 강타) 배우기\n", "  4番スキル(サンダーストライク)を覚える\n") << std::flush;
    learnSkill(hero, 4);
    print(hero);
    const bool rejected = !learnSkill(hero, 5);

    expect("없는 5번 스킬은 거절", true, rejected);
    expect("2번 스킬 레벨", 1, hero.skillLevels[1]);
    expect("4번 스킬 레벨", 1, hero.skillLevels[3]);
    expect("남은 골드", 800, hero.gold);
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(Ctrl+M, Ctrl+O = すべて折りたたむ)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// Debug와 Release로 각각 실행해 보세요. 한쪽은 죽고 한쪽은 틀린 값만 나옵니다.
// Release(또는 죽지 않는 쪽)에서: 아무도 골드를 늘리지 않는데 골드가 바뀝니다. hero.gold에 데이터 중단점을 걸어 보세요. 멈춘 줄은 골드를 다루는 줄인가요?

// [JA] Debug と Release でそれぞれ実行してみてください。片方は落ち、片方はおかしな値が出るだけです。
// [JA] Release(または落ちない方)で: 誰もゴールドを増やしていないのにゴールドが変わります。hero.gold にデータ ブレークポイントを置いてみてください。止まった行はゴールドを扱う行ですか?
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// 화면 번호는 1 ~ 4인데 배열 인덱스는 0 ~ 3입니다. skillLevels[4]는 배열 바로 뒤, 같은 구조체의 다음 멤버인 gold 자리입니다.
// operator[]는 범위를 검사하지 않습니다. Debug 구성의 표준 라이브러리는 "array subscript out of range" 같은 검사를 덧붙여 그 줄에서 멈춰 주지만, Release에서는 조용히 옆 변수를 덮습니다.
// (GCC·Clang에서는 이 저장소의 CMake가 Debug에 켜는 _GLIBCXX_ASSERTIONS, libc++ 하드닝이 그 검사를 합니다.)
// 같은 객체 안의 넘침이라 ASan도 대개 잡지 못합니다. at()은 어느 구성에서나 std::out_of_range를 던집니다.

// [JA] 画面の番号は 1 ~ 4 ですが、配列のインデックスは 0 ~ 3 です。skillLevels[4] は配列の直後、同じ構造体の次のメンバーである gold の場所です。
// [JA] operator[] は範囲をチェックしません。Debug 構成の標準ライブラリは「array subscript out of range」のようなチェックを付け加えてその行で止めてくれますが、Release では黙って隣の変数を上書きします。
// [JA] (GCC・Clang では、このリポジトリの CMake が Debug でオンにする _GLIBCXX_ASSERTIONS、libc++ のハードニングがそのチェックをします。)
// [JA] 同じオブジェクトの中でのはみ出しなので、ASan もたいてい捕まえられません。at() はどの構成でも std::out_of_range を投げます。
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// 화면 번호를 인덱스로 바꾸세요: hero.skillLevels[skillId - 1]. 번호 검사(1 ~ 4)와 인덱스 계산을 한곳에 두면 이런 어긋남이 줄어듭니다.
// 확신이 없는 인덱스에는 at()을 쓰면, 넘친 순간 어느 구성에서나 예외로 드러납니다.

// [JA] 画面の番号をインデックスに変えてください: hero.skillLevels[skillId - 1]。番号のチェック(1 ~ 4)とインデックスの計算を一か所に置くと、このようなずれが減ります。
// [JA] 確信のないインデックスには at() を使えば、はみ出した瞬間にどの構成でも例外として表に出ます。
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(grade.bat)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void of05_skill_tree_hidden()
{
    using namespace ovf5;
    Hero hero;
    hero.gold = 450;
    for (int id : { 4, 4, 1, 3, 0, 2 })
        learnSkill(hero, id);
    expect("숨은 스킬: 1번", 1, hero.skillLevels[0]);
    expect("숨은 스킬: 2번 (골드 부족)", 0, hero.skillLevels[1]);
    expect("숨은 스킬: 3번", 1, hero.skillLevels[2]);
    expect("숨은 스킬: 4번", 2, hero.skillLevels[3]);
    expect("숨은 스킬: 남은 골드", 50, hero.gold);
}
#pragma endregion
