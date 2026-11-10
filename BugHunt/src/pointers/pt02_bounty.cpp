#include "../labs.h"
#include <string>
#include <vector>

// 현상금 사냥. 플레이어는 몬스터 하나를 타깃으로 잡고 공격한다.
// 몬스터가 쓰러지면 전장에서 치우고, 타깃의 현상금을 골드로 받는다.

// [JA] 賞金稼ぎです。プレイヤーはモンスターを一体ターゲットにして攻撃します。
// [JA] モンスターが倒れると戦場から片付け、ターゲットの賞金をゴールドで受け取ります。
namespace ptr2
{
struct Monster
{
    std::string name;
    int hp;
    int bounty;   // 처치 시 받는 골드 [JA] 倒したときにもらえるゴールド
};

class Battlefield
{
public:
    ~Battlefield()
    {
        for (Monster* m : monsters_)
            delete m;
    }

    void spawn(const std::string& name, int hp, int bounty) { monsters_.push_back(new Monster{ name, hp, bounty }); }

    Monster* find(const std::string& name)
    {
        for (Monster* m : monsters_)
        {
            if (m->name == name)
                return m;
        }
        return nullptr;
    }

    // 쓰러진 몬스터를 전장에서 치운다.
    // [JA] 倒れたモンスターを戦場から取り除きます。
    void removeDefeated()
    {
        for (size_t i = 0; i < monsters_.size();)
        {
            if (monsters_[i]->hp <= 0)
            {
                delete monsters_[i];
                monsters_.erase(monsters_.begin() + i);
            }
            else
            {
                ++i;
            }
        }
    }

private:
    std::vector<Monster*> monsters_;
};

struct Hunter
{
    int gold = 0;
    Monster* target = nullptr;
};

// 타깃을 공격한다. 쓰러뜨렸으면 true.
// [JA] ターゲットを攻撃します。倒したら true。
bool attack(Hunter& hunter, int damage)
{
    hunter.target->hp -= damage;
    return hunter.target->hp <= 0;
}

void hunt(Battlefield& field, Hunter& hunter, const std::string& targetName, int damagePerHit)
{
    hunter.target = field.find(targetName);
    while (!attack(hunter, damagePerHit))
    {
    }

    field.removeDefeated();          // 전장 정리 [JA] 戦場の片付け
    hunter.gold += hunter.target->bounty;
    hunter.target = nullptr;
}
}

void pt02_bounty()
{
    ptr2::Battlefield field;
    field.spawn("Goblin", 60, 150);
    field.spawn("Troll", 300, 900);
    field.spawn("Wolf", 45, 80);

    ptr2::Hunter hunter;
    ptr2::hunt(field, hunter, "Goblin", 25);
    std::cout << i18n::L("  고블린 처치 후 골드: ", "  ゴブリン討伐後のゴールド: ") << hunter.gold << '\n';
    ptr2::hunt(field, hunter, "Wolf", 25);
    std::cout << i18n::L("  늑대 처치 후 골드: ", "  オオカミ討伐後のゴールド: ") << hunter.gold << '\n';

    expect(i18n::L("현상금 합계 (150 + 80)", "賞金の合計(150 + 80)"), 230, hunter.gold);
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (편집기의 접기 기능으로 모두 접어 두세요)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(エディターの折りたたみ機能ですべて折りたたんでおいてください)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// hunter.gold += ... 줄에 중단점을 걸고 지역 변수에서 hunter.target을 펼쳐 보세요.
// name, hp, bounty가 각각 어떻게 보이나요? bounty를 16진수로 보면(디버거의 16진수 표시, gdb: p/x)?
// 0xEEEEEEEE는 BugHunt 메모리 검사기가 해제된 메모리에 덮어 두는 값입니다. (README의 "메모리 검사기")

// [JA] hunter.gold += ... の行にブレークポイントを置き、ローカル変数で hunter.target を展開してみてください。
// [JA] name, hp, bounty はそれぞれどう見えますか? bounty を16進数で見ると(デバッガーの16進数表示、gdb: p/x)?
// [JA] 0xEEEEEEEE は、BugHunt のメモリ検査器が解放されたメモリに上書きしておく値です。(README の「メモリ検査器」)
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// target이 가리키는 몬스터는 바로 윗줄에서 어떻게 되었나요?
// field.removeDefeated() 안으로 들어가서 delete 줄을 지나갈 때 target 주소와 비교해 보세요.

// [JA] target が指しているモンスターは、すぐ上の行でどうなりましたか?
// [JA] field.removeDefeated() の中に入り、delete の行を通るときに target のアドレスと比べてみてください。
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// ASan 구성으로 빌드해서 실행해 보세요. 어떤 이름의 오류가, 어느 줄에서 해제된 메모리를, 어느 줄에서 읽었다고 하나요?
// 고치는 방법은 두 가지 방향이 있습니다: 순서를 바꾸거나, 지워질 객체를 붙잡고 있지 않거나.

// [JA] ASan 構成でビルドして実行してみてください。どんな名前のエラーが、どの行で解放されたメモリを、どの行で読んだと言っていますか?
// [JA] 直し方には二つの方向があります: 順番を入れ替えるか、消されるオブジェクトをつかんだままにしないか。
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(--grade)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
// [JA] 採点器(--grade)が、画面には表示されない別のデータで同じコードをもう一度検査します。
// [JA] 期待値をハードコードして [PASS] にしても、ここで引っかかります。
void pt02_bounty_hidden()
{
    ptr2::Battlefield field;
    field.spawn("Bandit", 90, 320);
    field.spawn("Ogre", 200, 1200);
    field.spawn("Bat", 10, 15);

    ptr2::Hunter hunter;
    ptr2::hunt(field, hunter, "Ogre", 70);
    ptr2::hunt(field, hunter, "Bat", 70);
    expect(i18n::L("현상금 합계 (1200 + 15)", "賞金の合計(1200 + 15)"), 1215, hunter.gold);
    expect(i18n::L("남은 Bandit HP", "残りの Bandit HP"), 90, field.find("Bandit") ? field.find("Bandit")->hp : -1);
}
#pragma endregion
