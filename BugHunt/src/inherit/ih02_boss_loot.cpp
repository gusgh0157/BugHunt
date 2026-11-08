#include "../labs.h"
#include <string>
#include <vector>

// 웨이브 설정대로 몬스터를 만들고, 웨이브가 끝나면 모든 몬스터의 전리품을 모은다.
// 보스(Boss)는 일반 전리품에 더해 희귀 아이템을 하나 떨어뜨린다.

// [JA] ウェーブ設定どおりにモンスターを作り、ウェーブが終わったら全モンスターの戦利品を集めます。
// [JA] ボス(Boss)は通常の戦利品に加えて、レアアイテムを1つ落とします。
namespace inh2
{
class Monster
{
public:
    Monster(std::string name, int hp) : name_(std::move(name)), hp_(hp) {}
    virtual ~Monster() = default;

    const std::string& name() const { return name_; }
    int hp() const { return hp_; }
    virtual std::vector<std::string> loot() const { return { "뼈 조각" }; }

private:
    std::string name_;
    int hp_;
};

class Boss : public Monster
{
public:
    Boss(std::string name, int hp, std::string rare)
        : Monster(std::move(name), hp), rare_(std::move(rare)) {}

    std::vector<std::string> loot() const override { return { "뼈 조각", rare_ }; }

private:
    std::string rare_;
};

struct SpawnInfo
{
    std::string name;
    int hp = 0;
    std::string rareDrop;          // 비어 있으면 일반 몬스터, 있으면 보스
};

std::vector<Monster> spawnWave(const std::vector<SpawnInfo>& infos)
{
    std::vector<Monster> wave;
    for (const SpawnInfo& info : infos)
    {
        if (info.rareDrop.empty())
            wave.push_back(Monster(info.name, info.hp));
        else
            wave.push_back(Boss(info.name, info.hp, info.rareDrop));
    }
    return wave;
}

std::vector<std::string> collectLoot(const std::vector<Monster>& wave)
{
    std::vector<std::string> bag;
    for (const Monster& m : wave)
    {
        for (const std::string& item : m.loot())
            bag.push_back(item);
    }
    return bag;
}

bool contains(const std::vector<std::string>& bag, const std::string& item)
{
    for (const std::string& s : bag)
    {
        if (s == item)
            return true;
    }
    return false;
}
}

void ih02_boss_loot()
{
    using namespace inh2;

    const std::vector<std::string> bag = collectLoot(spawnWave({
        { "고블린", 30, "" },
        { "고블린", 30, "" },
        { "고블린 왕", 300, "황금 왕관" },
    }));

    std::cout << i18n::L("  전리품:", "  戦利品:");
    for (const std::string& item : bag)
        std::cout << " [" << item << "]";
    std::cout << '\n';

    expect("전리품 개수", static_cast<size_t>(4), bag.size());
    expect("황금 왕관이 나왔는가?", true, contains(bag, "황금 왕관"));
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(Ctrl+M, Ctrl+O = すべて折りたたむ)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// Boss::loot에 중단점을 걸어 보세요. 한 번이라도 멈추나요?
// 멈추지 않으면 collectLoot에서 wave를 펼쳐 세 번째 원소의 형식을 보세요. Boss인가요? rare_는 어디 있나요?

// [JA] Boss::loot にブレークポイントを置いてみてください。一度でも止まりますか?
// [JA] 止まらなければ、collectLoot で wave を展開して3番目の要素の型を見てください。Boss でしょうか? rare_ はどこにありますか?
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// std::vector<Monster>는 Monster "값"을 담습니다. Boss를 넣으면 Monster 부분만 복사되고 Boss 부분(rare_, 가상 함수 표)은 잘립니다.
// 이것을 객체 슬라이싱(slicing)이라고 합니다. Boss 생성자는 불렸지만, 담긴 것은 Monster입니다.

// [JA] std::vector<Monster> は Monster の「値」を格納します。Boss を入れると Monster の部分だけがコピーされ、Boss の部分(rare_、仮想関数テーブル)は切り落とされます。
// [JA] これをオブジェクトのスライシングと呼びます。Boss のコンストラクターは呼ばれましたが、格納されたのは Monster です。
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// 다형성이 필요한 객체는 포인터로 담으세요: std::vector<std::unique_ptr<Monster>>와 std::make_unique<Boss>(...).
// 기반 클래스의 복사를 막아 두면(복사 생성자 = delete) 같은 실수를 컴파일러가 잡아 줍니다.

// [JA] ポリモーフィズムが必要なオブジェクトはポインターで格納してください: std::vector<std::unique_ptr<Monster>> と std::make_unique<Boss>(...)。
// [JA] 基底クラスのコピーを禁止しておけば(コピーコンストラクター = delete)、同じミスをコンパイラーが捕まえてくれます。
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(grade.bat)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void ih02_boss_loot_hidden()
{
    using namespace inh2;

    const std::vector<std::string> bag = collectLoot(spawnWave({
        { "해골 기사", 500, "저주받은 검" },
        { "박쥐", 8, "" },
        { "리치", 900, "영혼석" },
    }));
    expect("숨은 웨이브: 전리품 개수", static_cast<size_t>(5), bag.size());
    expect("숨은 웨이브: 저주받은 검", true, contains(bag, "저주받은 검"));
    expect("숨은 웨이브: 영혼석", true, contains(bag, "영혼석"));
    expect("숨은 웨이브: 황금 왕관은 없어야", false, contains(bag, "황금 왕관"));
}
#pragma endregion
