#include "../labs.h"
#include <string>

// 스킬은 모두 Skill을 상속하고, 전투 코드는 Skill&로만 스킬을 쓴다.
// 백스탭은 등 뒤에서 찌르면 피해가 세 배여야 한다.

// [JA] スキルはすべて Skill を継承し、戦闘コードは Skill& 経由でだけスキルを使います。
// [JA] バックスタブは背後から刺すとダメージが3倍になるはずです。
namespace inh1
{
struct Target
{
    int hp = 0;
    bool facingAway = false;      // 등을 보이고 있는가 [JA] 背中を向けているか
};

class Skill
{
public:
    Skill(std::string name, int power) : name_(std::move(name)), power_(power) {}
    virtual ~Skill() = default;

    const std::string& name() const { return name_; }
    virtual int damage(const Target&) const { return power_; }

protected:
    std::string name_;
    int power_;
};

class Fireball : public Skill
{
public:
    Fireball() : Skill(i18n::L("화염구", "ファイアボール"), 35) {}
    int damage(const Target& t) const override { return power_ + t.hp / 20; }
};

class Backstab : public Skill
{
public:
    Backstab() : Skill(i18n::L("백스탭", "バックスタブ"), 20) {}
    // 등 뒤에서 찌르면 세 배
    // [JA] 背後から刺すと 3 倍
    virtual int damage(const Target& t) { return t.facingAway ? power_ * 3 : power_; }
};

// 전투 코드: 어떤 스킬이든 Skill&로 받아 쓴다
// [JA] 戦闘コード: どのスキルも Skill& で受け取って使います
int useSkill(const Skill& skill, Target& target)
{
    const int dealt = skill.damage(target);
    target.hp -= dealt;
    return dealt;
}
}

void ih01_backstab()
{
    using namespace inh1;

    Target ogre{ 200, true };          // 등을 보인 오우거 [JA] 背中を向けたオーガ
    const Fireball fireball;
    const Backstab backstab;

    const int a = useSkill(fireball, ogre);
    const int b = useSkill(backstab, ogre);
    std::cout << "  " << fireball.name() << " " << a << ", " << backstab.name() << " " << b
              << i18n::L(" → 남은 HP ", " → 残りHP ") << ogre.hp << '\n';

    expect(i18n::L("화염구 피해", "ファイアボールのダメージ"), 45, a);
    expect(i18n::L("등 뒤 백스탭 피해", "背後からのバックスタブのダメージ"), 60, b);
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(Ctrl+M, Ctrl+O = すべて折りたたむ)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// useSkill의 skill.damage(target) 줄에 조건부 중단점을 거세요. 조건: skill.name() == "백스탭" (계산이 안 되면 적중 횟수 2)
// 함수 안으로 한 단계씩 들어가면 어느 클래스의 damage로 들어가나요?

// [JA] useSkill の skill.damage(target) の行に条件付きブレークポイントを設定してください。条件: skill.name() == "バックスタブ"(`--lang ko` では "백스탭"。評価できなければヒットカウント2)
// [JA] ステップインすると、どのクラスの damage に入りますか?
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// Skill::damage와 Backstab::damage의 선언을 글자 하나하나 비교해 보세요. 끝에 무엇이 다른가요?
// 시그니처가 조금이라도 다르면 재정의(override)가 아니라 같은 이름의 새 함수가 되어 기반 함수를 "가립니다".

// [JA] Skill::damage と Backstab::damage の宣言を1文字ずつ比べてみてください。末尾の何が違いますか?
// [JA] シグネチャーが少しでも違うと、オーバーライドではなく同じ名前の新しい関数になり、基底の関数を「隠し」ます。
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// 재정의하려는 함수에는 항상 override를 붙이세요. 붙이는 순간 컴파일러가 "재정의할 함수가 없다"고 알려 줍니다.
// (clang과 GCC 13 이상은 -Wall에서 "hides overloaded virtual function" 경고도 냅니다. MSVC는 기본 경고 수준에서 조용합니다.)

// [JA] オーバーライドするつもりの関数には必ず override を付けてください。付けた瞬間、コンパイラーが「オーバーライドする関数がない」と教えてくれます。
// [JA] (clang と GCC 13 以降は -Wall で "hides overloaded virtual function" の警告も出します。MSVC は既定の警告レベルでは何も言いません。)
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(grade.bat)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
// [JA] 採点器(grade.bat)が、画面には表示されない別のデータで同じコードをもう一度検査します。
// [JA] 期待値をハードコードして [PASS] にしても、ここで引っかかります。
void ih01_backstab_hidden()
{
    using namespace inh1;

    const Backstab backstab;
    Target guard{ 90, false };
    expect(i18n::L("정면 백스탭", "正面からのバックスタブ"), 20, useSkill(backstab, guard));
    guard.facingAway = true;
    expect(i18n::L("등 뒤 백스탭", "背後からのバックスタブ"), 60, useSkill(backstab, guard));
    expect(i18n::L("남은 HP", "残りHP"), 10, guard.hp);

    Target boss{ 400, true };
    expect(i18n::L("화염구 (HP 400)", "ファイアボール(HP 400)"), 55, useSkill(Fireball(), boss));
}
#pragma endregion
