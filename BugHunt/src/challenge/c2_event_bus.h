#pragma once
#include <algorithm>
#include <functional>
#include <string>
#include <vector>

// 챌린지 C-2: 게임 이벤트 버스.
// 시스템들은 관심 있는 이벤트를 구독하고, 전투 코드는 일어난 일을 publish한다.

// [JA] チャレンジ C-2: ゲームイベントバスです。
// [JA] システムは関心のあるイベントを購読し、戦闘コードは起きたことを publish します。
namespace chal2
{
enum class EventType
{
    MonsterKilled,
    PlayerHit,
    RaidEnd,
};

struct Event
{
    int seq = 0;                // 몇 번째 이벤트인가 (1부터) [JA] 何番目のイベントか(1から)
    EventType type = EventType::MonsterKilled;
    std::string monster;        // MonsterKilled일 때 몬스터 종류 [JA] MonsterKilled のときのモンスターの種類
    int damage = 0;             // PlayerHit일 때 받은 피해 (방어로 막으면 0) [JA] PlayerHit のときに受けたダメージ(防御で防いだら0)
};

using SubscriptionId = int;
using Handler = std::function<void(const Event&)>;

// 구독한 순서대로 핸들러를 부른다.
// 핸들러 안에서 unsubscribe를 불러도 된다. (퀘스트가 끝나면 스스로 구독을 끊는 식)

// [JA] 購読した順にハンドラーを呼びます。
// [JA] ハンドラーの中で unsubscribe を呼んでもかまいません。(クエストが終わったら自分で購読を解除する、といった使い方です)
class EventBus
{
public:
    SubscriptionId subscribe(std::string name, Handler handler)
    {
        const SubscriptionId id = nextId_++;
        subscribers_.push_back({ id, std::move(name), std::move(handler) });
        return id;
    }

    void unsubscribe(SubscriptionId id)
    {
        const auto it = std::find_if(subscribers_.begin(), subscribers_.end(),
                                     [id](const Subscriber& s) { return s.id == id; });
        if (it != subscribers_.end())
            subscribers_.erase(it);
    }

    void publish(const Event& e)
    {
        for (size_t i = 0; i < subscribers_.size(); ++i)
            subscribers_[i].handler(e);
    }

    size_t subscriberCount() const { return subscribers_.size(); }

    std::vector<std::string> subscriberNames() const
    {
        std::vector<std::string> names;
        for (const Subscriber& s : subscribers_)
            names.push_back(s.name);
        return names;
    }

private:
    struct Subscriber
    {
        SubscriptionId id;
        std::string name;   // 디버깅용 이름 [JA] デバッグ用の名前
        Handler handler;
    };

    std::vector<Subscriber> subscribers_;
    SubscriptionId nextId_ = 1;
};
}
