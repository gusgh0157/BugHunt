#pragma once
#include <memory>
#include <string>
#include <vector>

// 챌린지 C-3: 영웅 데이터.

// [JA] チャレンジ C-3: 英雄データです。
namespace chal3
{
struct Pet
{
    std::string name;
    int bonusAttack = 0;
};

struct Buff
{
    std::string name;
    int attackPercent = 0;   // 공격력 +N%
};

class Hero
{
public:
    Hero(std::string name, int level, int baseAttack);

    // 펫은 영웅마다 따로 가지므로 복사할 때 깊은 복사가 필요하다. 그래서 복사를 직접 만든다.
    Hero(const Hero& other);
    Hero& operator=(Hero other);
    friend void swap(Hero& a, Hero& b) noexcept;

    const std::string& name() const { return name_; }
    int level() const { return level_; }
    int hp() const { return hp_; }
    int maxHp() const { return maxHp_; }
    int baseAttack() const { return baseAttack_; }
    int prestige() const { return prestige_; }
    const Pet* pet() const { return pet_.get(); }
    const std::vector<Buff>& buffs() const { return buffs_; }

    void setHp(int hp) { hp_ = hp < 0 ? 0 : (hp > maxHp_ ? maxHp_ : hp); }
    void addBuff(Buff buff) { buffs_.push_back(std::move(buff)); }
    void clearBuffs() { buffs_.clear(); }
    void adoptPet(std::string name, int bonusAttack);
    void prestigeUp();   // 만렙에서 프레스티지: 레벨은 1로, 프레스티지 단계는 +1

private:
    std::string name_;
    int level_ = 1;
    int hp_ = 100;
    int maxHp_ = 100;
    int baseAttack_ = 10;
    std::unique_ptr<Pet> pet_;
    std::vector<Buff> buffs_;
    int prestige_ = 0;   // 시즌 2에서 추가: 프레스티지 단계마다 공격력 +5%
};
}
