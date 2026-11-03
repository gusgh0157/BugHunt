#pragma once
#include "c2_event_bus.h"
#include <map>
#include <string>
#include <vector>

// 챌린지 C-2: 이벤트 버스를 구독하는 게임 시스템들.
namespace chal2
{
// 전투 기록: 모든 이벤트를 문자열로 남긴다.
class CombatLog
{
public:
    explicit CombatLog(EventBus& bus);
    const std::vector<std::string>& lines() const { return lines_; }
    int kills() const { return kills_; }

private:
    std::vector<std::string> lines_;
    int kills_ = 0;
};

// 사냥 의뢰: target을 count마리 잡으면 끝난다. 끝나면 더 들을 필요가 없으니 스스로 구독을 끊는다.
class HuntQuest
{
public:
    HuntQuest(EventBus& bus, std::string title, std::string target, int count);
    bool done() const { return done_; }
    int progress() const { return progress_; }
    const std::string& title() const { return title_; }

private:
    EventBus& bus_;
    SubscriptionId sub_ = 0;
    std::string title_;
    std::string target_;
    int count_;
    int progress_ = 0;
    bool done_ = false;
};

// 업적 추적: 종류별 처치 수와 "무피격 연속 처치"를 센다.
// 기획서 4.2: 공격을 받으면 연속 처치가 끊긴다. 방어로 막아 피해가 0이어도 "공격을 받은" 것이다.
class AchievementTracker
{
public:
    explicit AchievementTracker(EventBus& bus);
    int killsOf(const std::string& monster) const;
    int totalKills() const;
    int currentStreak() const { return current_; }
    int bestStreak() const { return best_ > current_ ? best_ : current_; }

private:
    std::map<std::string, int> kills_;
    int current_ = 0;
    int best_ = 0;
};

// 전리품: 처치마다 전리품 상자 하나.
class LootSystem
{
public:
    explicit LootSystem(EventBus& bus);
    int chests() const { return chests_; }

private:
    int chests_ = 0;
};

// 레이드가 끝나면 업적 추적 결과로 칭호를 준다.
class TitleSystem
{
public:
    TitleSystem(EventBus& bus, const AchievementTracker& tracker);
    const std::vector<std::string>& titles() const { return titles_; }

private:
    void award();

    const AchievementTracker& tracker_;
    std::vector<std::string> titles_;
};

constexpr int kShadowStreak = 6;   // 무피격 연속 처치 6 이상이면 "그림자"
constexpr int kWolfSlayer = 5;     // 늑대 5마리 이상이면 "늑대 사냥꾼"
}
