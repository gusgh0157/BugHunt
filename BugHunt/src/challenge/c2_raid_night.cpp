#include "../labs.h"
#include "c2_event_bus.h"
#include "c2_systems.h"
#include <algorithm>

// 챌린지 C-2
// 레이드 한 판을 이벤트로 재생한다. 전투 코드는 일어난 일을 이벤트 버스에 publish하고,
// 기록/의뢰/업적/전리품/칭호 시스템이 각자 구독해서 처리한다.

// [JA] チャレンジ C-2
// [JA] レイド1回分をイベントで再生します。戦闘コードは起きたことをイベントバスに publish し、
// [JA] 記録/依頼/実績/戦利品/称号システムがそれぞれ購読して処理します。
namespace chal2
{
namespace
{
struct Raid
{
    EventBus bus;
    int seq = 0;

    void kill(const std::string& monster)
    {
        Event e;
        e.seq = ++seq;
        e.type = EventType::MonsterKilled;
        e.monster = monster;
        bus.publish(e);
    }

    void hit(int damage)
    {
        Event e;
        e.seq = ++seq;
        e.type = EventType::PlayerHit;
        e.damage = damage;
        bus.publish(e);
    }

    void end()
    {
        Event e;
        e.seq = ++seq;
        e.type = EventType::RaidEnd;
        bus.publish(e);
    }
};

bool hasTitle(const TitleSystem& titles, const std::string& name)
{
    const auto& t = titles.titles();
    return std::find(t.begin(), t.end(), name) != t.end();
}
}
}

void chC2_raid_night()
{
    using namespace chal2;

    Raid raid;
    CombatLog log(raid.bus);
    HuntQuest quest(raid.bus, i18n::L("늑대 무리 소탕", "オオカミの群れ討伐"), i18n::L("늑대", "オオカミ"), 3);
    AchievementTracker tracker(raid.bus);
    LootSystem loot(raid.bus);
    TitleSystem titles(raid.bus, tracker);

    raid.hit(7);          // 입구에서 한 대 맞고 시작
    raid.kill(i18n::L("늑대", "オオカミ"));
    raid.kill(i18n::L("늑대", "オオカミ"));
    raid.kill(i18n::L("늑대", "オオカミ"));
    raid.kill(i18n::L("멧돼지", "イノシシ"));
    raid.kill(i18n::L("멧돼지", "イノシシ"));
    raid.kill(i18n::L("늑대", "オオカミ"));
    raid.hit(0);          // 방패로 막음
    raid.kill(i18n::L("멧돼지", "イノシシ"));
    raid.kill(i18n::L("늑대", "オオカミ"));
    raid.end();

    for (const std::string& line : log.lines())
        std::cout << "  " << line << '\n';
    std::cout << i18n::L("  의뢰 \"", "  依頼 \"") << quest.title() << "\": " << (quest.done() ? i18n::L("완료", "完了") : i18n::L("진행 중", "進行中")) << '\n';
    std::cout << i18n::L("  전리품 상자 ", "  戦利品の箱 ") << loot.chests() << i18n::L("개, 최고 무피격 연속 처치 ", "個、最高ノーダメージ連続撃破 ") << tracker.bestStreak() << '\n';
    std::cout << i18n::L("  칭호:", "  称号:");
    for (const std::string& t : titles.titles())
        std::cout << ' ' << t;
    std::cout << '\n';

    expect("의뢰 완료", true, quest.done());
    expect("전리품 상자", 8, loot.chests());
    expect("칭호 \"그림자\"를 받았는가? (무피격 연속 처치 6 이상)", true, hasTitle(titles, i18n::L("그림자", "影")));
}

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(--grade)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.

// [JA] 採点器(--grade)が、画面には表示されない別のデータで同じコードをもう一度検査します。
void chC2_raid_night_hidden()
{
    using namespace chal2;

    {
        Raid raid;
        CombatLog log(raid.bus);
        HuntQuest quest(raid.bus, i18n::L("늑대 두 마리", "オオカミ2匹"), i18n::L("늑대", "オオカミ"), 2);
        AchievementTracker tracker(raid.bus);
        TitleSystem titles(raid.bus, tracker);
        for (int i = 0; i < 6; ++i)
            raid.kill(i18n::L("늑대", "オオカミ"));
        raid.end();
        expect("업적의 늑대 처치 수 = 기록의 처치 수", log.kills(), tracker.killsOf(i18n::L("늑대", "オオカミ")));
        expect("칭호 \"늑대 사냥꾼\"", true, hasTitle(titles, i18n::L("늑대 사냥꾼", "オオカミハンター")));
    }
    {
        Raid raid;
        AchievementTracker tracker(raid.bus);
        TitleSystem titles(raid.bus, tracker);
        for (int i = 0; i < 5; ++i)
            raid.kill(i18n::L("박쥐", "コウモリ"));
        raid.hit(0);   // 막아도 연속 처치는 끊긴다 (기획서 4.2)
        for (int i = 0; i < 3; ++i)
            raid.kill(i18n::L("박쥐", "コウモリ"));
        raid.end();
        expect("막아 낸 공격 뒤의 최고 연속 처치", 5, tracker.bestStreak());
        expect("칭호 \"그림자\"를 받지 않아야 한다", false, hasTitle(titles, i18n::L("그림자", "影")));
    }
    {
        Raid raid;
        CombatLog log(raid.bus);
        HuntQuest first(raid.bus, i18n::L("박쥐 하나", "コウモリ1匹"), i18n::L("박쥐", "コウモリ"), 1);
        HuntQuest second(raid.bus, i18n::L("박쥐 둘", "コウモリ2匹"), i18n::L("박쥐", "コウモリ"), 2);
        LootSystem loot(raid.bus);
        AchievementTracker tracker(raid.bus);
        raid.kill(i18n::L("박쥐", "コウモリ"));
        raid.kill(i18n::L("박쥐", "コウモリ"));
        raid.kill(i18n::L("거미", "クモ"));
        expect("두 의뢰 모두 완료", true, first.done() && second.done());
        expect("전리품 상자 = 처치 수", log.kills(), loot.chests());
        expect("업적 처치 수 = 처치 수", log.kills(), tracker.totalKills());
        expect("끝난 의뢰는 구독이 끊겨야 한다 (남은 구독자 수)", static_cast<size_t>(3), raid.bus.subscriberCount());
    }
}
#pragma endregion
