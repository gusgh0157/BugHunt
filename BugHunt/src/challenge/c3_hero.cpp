#include "c3_hero.h"
#include <utility>

namespace chal3
{
Hero::Hero(std::string name, int level, int baseAttack)
    : name_(std::move(name)), level_(level), hp_(80 + level * 20), maxHp_(80 + level * 20), baseAttack_(baseAttack)
{
}

Hero::Hero(const Hero& other)
    : name_(other.name_),
      level_(other.level_),
      hp_(other.hp_),
      maxHp_(other.maxHp_),
      baseAttack_(other.baseAttack_),
      pet_(other.pet_ ? std::make_unique<Pet>(*other.pet_) : nullptr),
      buffs_(other.buffs_)
{
}

Hero& Hero::operator=(Hero other)
{
    swap(*this, other);
    return *this;
}

void swap(Hero& a, Hero& b) noexcept
{
    using std::swap;
    swap(a.name_, b.name_);
    swap(a.level_, b.level_);
    swap(a.hp_, b.hp_);
    swap(a.maxHp_, b.maxHp_);
    swap(a.baseAttack_, b.baseAttack_);
    swap(a.pet_, b.pet_);
    swap(a.buffs_, b.buffs_);
    swap(a.prestige_, b.prestige_);
}

void Hero::adoptPet(std::string name, int bonusAttack)
{
    pet_ = std::make_unique<Pet>(Pet{ std::move(name), bonusAttack });
}

void Hero::prestigeUp()
{
    level_ = 1;
    ++prestige_;
    maxHp_ = 80 + level_ * 20;
    hp_ = maxHp_;
}
}
