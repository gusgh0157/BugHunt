#pragma once
#include "c3_hero.h"
#include <map>
#include <string>
#include <vector>

// 챌린지 C-3: 영웅을 다루는 게임 시스템들.
namespace chal3
{
// 공격력 = (기본 공격력 + 레벨 x 2 + 펫 보너스) x (100 + 프레스티지 x 5 + 버프%) / 100
class StatSystem
{
public:
    static int attackOf(const Hero& hero)
    {
        const int flat = hero.baseAttack() + hero.level() * 2 + (hero.pet() ? hero.pet()->bonusAttack : 0);
        int percent = 100 + hero.prestige() * 5;
        for (const Buff& b : hero.buffs())
            percent += b.attackPercent;
        return flat * percent / 100;
    }
};

// 성소에서 영웅 상태를 저장해 두고, 죽으면 그 상태로 되돌린다.
class CheckpointSystem
{
public:
    void save(const Hero& hero) { snapshots_.insert_or_assign(hero.name(), hero); }
    bool has(const std::string& name) const { return snapshots_.count(name) != 0; }
    const Hero& snapshotOf(const std::string& name) const { return snapshots_.at(name); }

private:
    std::map<std::string, Hero> snapshots_;
};

class RespawnSystem
{
public:
    explicit RespawnSystem(const CheckpointSystem& checkpoints) : checkpoints_(checkpoints) {}

    // 마지막 성소 상태로 되살린다. 체력은 절반.
    void respawn(Hero& hero) const
    {
        if (checkpoints_.has(hero.name()))
            hero = checkpoints_.snapshotOf(hero.name());
        hero.clearBuffs();               // 죽으면 버프가 모두 사라진다 (기획서 7.1)
        hero.setHp(hero.maxHp() / 2);
    }

private:
    const CheckpointSystem& checkpoints_;
};

// 전투 피해 기록
class DamageMeter
{
public:
    void hit(const Hero& hero, const std::string& target)
    {
        const int damage = StatSystem::attackOf(hero);
        log_.push_back(hero.name() + " → " + target + " " + std::to_string(damage));
        last_ = damage;
    }
    int lastHit() const { return last_; }
    const std::vector<std::string>& log() const { return log_; }

private:
    std::vector<std::string> log_;
    int last_ = 0;
};
}
