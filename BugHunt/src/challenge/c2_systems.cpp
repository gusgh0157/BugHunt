#include "c2_systems.h"

namespace chal2
{
CombatLog::CombatLog(EventBus& bus)
{
    bus.subscribe("전투 기록", [this](const Event& e) {
        switch (e.type)
        {
        case EventType::MonsterKilled:
            ++kills_;
            lines_.push_back("#" + std::to_string(e.seq) + " " + e.monster + " 처치");
            break;
        case EventType::PlayerHit:
            lines_.push_back("#" + std::to_string(e.seq) + " 피격 " + std::to_string(e.damage));
            break;
        case EventType::RaidEnd:
            lines_.push_back("#" + std::to_string(e.seq) + " 레이드 종료");
            break;
        }
    });
}

HuntQuest::HuntQuest(EventBus& bus, std::string title, std::string target, int count)
    : bus_(bus), title_(std::move(title)), target_(std::move(target)), count_(count)
{
    sub_ = bus_.subscribe("의뢰: " + title_, [this](const Event& e) {
        if (e.type != EventType::MonsterKilled || e.monster != target_ || done_)
            return;
        ++progress_;
        if (progress_ >= count_)
        {
            done_ = true;
            bus_.unsubscribe(sub_);   // 끝난 의뢰는 더 듣지 않는다
        }
    });
}

AchievementTracker::AchievementTracker(EventBus& bus)
{
    bus.subscribe("업적 추적", [this](const Event& e) {
        if (e.type == EventType::MonsterKilled)
        {
            ++kills_[e.monster];
            ++current_;
        }
        else if (e.type == EventType::PlayerHit)
        {
            // 막아 낸 공격(피해 0)도 피격이다. (기획서 4.2)
            if (current_ > best_)
                best_ = current_;
            current_ = 0;
        }
    });
}

int AchievementTracker::killsOf(const std::string& monster) const
{
    const auto it = kills_.find(monster);
    return it == kills_.end() ? 0 : it->second;
}

int AchievementTracker::totalKills() const
{
    int total = 0;
    for (const auto& [monster, count] : kills_)
        total += count;
    return total;
}

LootSystem::LootSystem(EventBus& bus)
{
    bus.subscribe("전리품", [this](const Event& e) {
        if (e.type == EventType::MonsterKilled)
            ++chests_;
    });
}

TitleSystem::TitleSystem(EventBus& bus, const AchievementTracker& tracker) : tracker_(tracker)
{
    bus.subscribe("칭호", [this](const Event& e) {
        if (e.type == EventType::RaidEnd)
            award();
    });
}

void TitleSystem::award()
{
    if (tracker_.bestStreak() >= kShadowStreak)
        titles_.push_back("그림자");
    if (tracker_.killsOf("늑대") >= kWolfSlayer)
        titles_.push_back("늑대 사냥꾼");
}
}
