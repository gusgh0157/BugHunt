#include "../labs.h"
#include <string>
#include <vector>

// 챌린지 B-4

// [JA] チャレンジ B-4
namespace blind4
{
class Monster
{
public:
    Monster(std::string name, int maxHp, int armor)
        : name_(std::move(name))
    {
        maxHp = maxHp;
        hp_ = maxHp;
        armor_ = armor;
    }

    const std::string& name() const { return name_; }
    int hp() const { return hp_; }
    int maxHp() const { return maxHp_; }

    void takeDamage(int raw)
    {
        const int dealt = raw > armor_ ? raw - armor_ : 1;
        hp_ = hp_ > dealt ? hp_ - dealt : 0;
    }

    // 체력 바: 10칸 중 몇 칸이 차 있는가
    int barCells() const { return hp_ * 10 / maxHp_; }

private:
    std::string name_;
    int maxHp_;
    int hp_;
    int armor_;
};
}

void chB4()
{
    using namespace blind4;
    std::vector<Monster> wave;
    wave.emplace_back(i18n::L("고블린", "ゴブリン"), 60, 2);
    wave.emplace_back(i18n::L("오우거", "オーガ"), 400, 10);
    for (Monster& m : wave)
        m.takeDamage(32);
    for (const Monster& m : wave)
        std::cout << "  " << m.name() << " " << m.hp() << "/" << m.maxHp() << '\n';
    expect("고블린 체력 바 (30/60)", 5, wave[0].barCells());
    expect("오우거 체력 바 (378/400)", 9, wave[1].barCells());
}

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
void chB4_hidden()
{
    using namespace blind4;
    Monster slime(i18n::L("슬라임", "スライム"), 25, 0);
    expect("최대 체력", 25, slime.maxHp());
    slime.takeDamage(5);
    expect("체력 바 (20/25)", 8, slime.barCells());
    Monster golem(i18n::L("골렘", "ゴーレム"), 1000, 50);
    golem.takeDamage(40);
    expect("골렘 체력 (방어가 높아도 최소 1)", 999, golem.hp());
    expect("골렘 체력 바", 9, golem.barCells());
}
#pragma endregion
