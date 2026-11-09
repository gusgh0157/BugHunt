#include "../labs.h"
#include <algorithm>
#include <memory>
#include <string>
#include <vector>

// 휴식 지점에서 파티의 마법사들에게 마나 물약을 하나씩 먹인다. 전사는 마나가 없으니 건너뛴다.
// 마법사만 있는 파티에서는 잘 되는데, 전사가 끼면 프로그램이 죽는다.

// [JA] 休憩ポイントで、パーティーの魔法使いにマナポーションを1つずつ飲ませます。戦士はマナがないので飛ばします。
// [JA] 魔法使いだけのパーティーでは問題ないのに、戦士が混ざるとプログラムが落ちます。
namespace inh6
{
class Unit
{
public:
    explicit Unit(std::string name) : name_(std::move(name)) {}
    virtual ~Unit() = default;
    const std::string& name() const { return name_; }

private:
    std::string name_;
};

class Mage : public Unit
{
public:
    Mage(std::string name, int mana) : Unit(std::move(name)), mana_(mana) {}
    void restoreMana(int amount) { mana_ = std::min(kMaxMana, mana_ + amount); }
    int mana() const { return mana_; }

private:
    static constexpr int kMaxMana = 100;
    int mana_;
};

class Warrior : public Unit
{
public:
    using Unit::Unit;
};

using Party = std::vector<std::unique_ptr<Unit>>;

// 마법사에게만 물약을 먹인다. 먹인 수를 돌려준다.
int drinkManaPotions(Party& party, int amount)
{
    int used = 0;
    for (const auto& unit : party)
    {
        Mage* mage = dynamic_cast<Mage*>(unit.get());
        mage->restoreMana(amount);
        ++used;
    }
    return used;
}

int manaOf(const Party& party, const std::string& name)
{
    for (const auto& unit : party)
    {
        if (const Mage* m = dynamic_cast<const Mage*>(unit.get()); m && m->name() == name)
            return m->mana();
    }
    return -1;
}
}

void ih06_mana_potion()
{
    using namespace inh6;

    Party party;
    party.push_back(std::make_unique<Mage>(i18n::L("아린", "アリン"), 40));
    party.push_back(std::make_unique<Warrior>(i18n::L("도윤", "ドユン")));
    party.push_back(std::make_unique<Mage>(i18n::L("보람", "ボラム"), 85));

    const int used = drinkManaPotions(party, 30);
    std::cout << i18n::L("  물약 ", "  ポーション") << used << i18n::L("개 사용, 아린 마나 ", "個使用、アリンのマナ") << manaOf(party, i18n::L("아린", "アリン"))
              << i18n::L(", 보람 마나 ", "、ボラムのマナ") << manaOf(party, i18n::L("보람", "ボラム")) << '\n';

    expect(i18n::L("사용한 물약", "使ったポーション"), 2, used);
    expect(i18n::L("아린 마나", "アリンのマナ"), 70, manaOf(party, i18n::L("아린", "アリン")));
    expect(i18n::L("보람 마나 (최대 100)", "ボラムのマナ(最大100)"), 100, manaOf(party, i18n::L("보람", "ボラム")));
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(Ctrl+M, Ctrl+O = すべて折りたたむ)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// 디버거로 실행해 죽는 곳에서 멈추면 호출 스택을 보세요. 맨 위는 restoreMana입니다. this는 얼마인가요?
// 한 칸 위(drinkManaPotions)로 올라가 unit과 mage를 비교하세요.

// [JA] デバッガーで実行して落ちた場所で止まったら、呼び出し履歴を見てください。一番上は restoreMana です。this はいくつですか?
// [JA] 1つ上(drinkManaPotions)に上がって、unit と mage を比べてください。
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// 포인터에 대한 dynamic_cast는 실제 형식이 맞지 않으면 예외 대신 nullptr를 돌려줍니다.
// 전사(Warrior)를 Mage*로 바꾸려 하면 무엇이 나오나요?

// [JA] ポインターに対する dynamic_cast は、実際の型が合わなければ例外ではなく nullptr を返します。
// [JA] 戦士(Warrior)を Mage* に変換しようとすると何が返りますか?
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// dynamic_cast<T*>의 결과는 항상 확인하세요: if (Mage* mage = dynamic_cast<Mage*>(unit.get())) { ... }
// 확인 없이 쓸 거라면 형식이 확실하다는 뜻이니, 애초에 Mage만 담은 목록을 따로 두는 설계가 낫습니다.

// [JA] dynamic_cast<T*> の結果は必ず確認してください: if (Mage* mage = dynamic_cast<Mage*>(unit.get())) { ... }
// [JA] 確認せずに使うつもりなら型が確実だということなので、最初から Mage だけを入れたリストを別に持つ設計のほうがよいです。
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(grade.bat)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void ih06_mana_potion_hidden()
{
    using namespace inh6;

    Party warriors;
    warriors.push_back(std::make_unique<Warrior>(i18n::L("가", "ア")));
    warriors.push_back(std::make_unique<Warrior>(i18n::L("나", "イ")));
    expect(i18n::L("전사만: 사용한 물약", "戦士だけ: 使ったポーション"), 0, drinkManaPotions(warriors, 50));

    Party mixed;
    mixed.push_back(std::make_unique<Warrior>(i18n::L("다", "ウ")));
    mixed.push_back(std::make_unique<Mage>(i18n::L("라", "エ"), 10));
    expect(i18n::L("섞인 파티: 사용한 물약", "混成パーティー: 使ったポーション"), 1, drinkManaPotions(mixed, 45));
    expect(i18n::L("섞인 파티: 라 마나", "混成パーティー: エのマナ"), 55, manaOf(mixed, i18n::L("라", "エ")));
}
#pragma endregion
