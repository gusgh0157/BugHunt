#include "../labs.h"
#include <coroutine>
#include <exception>
#include <memory>
#include <string>
#include <utility>
#include <vector>

// 컷신 스크립트. 컷신 하나는 코루틴이고, 한 줄을 보여 준 뒤 다음 프레임까지 멈췄다가(co_await) 이어서 실행된다.
// 컷신 엔진은 매 프레임 멈춰 있는 컷신들을 차례로 다시 깨운다.
namespace async7
{
// 매 프레임 한 번 깨워 주는 아주 작은 코루틴 타입
struct Cutscene
{
    struct promise_type
    {
        Cutscene get_return_object() { return Cutscene{ std::coroutine_handle<promise_type>::from_promise(*this) }; }
        std::suspend_always initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void return_void() {}
        void unhandled_exception() { std::terminate(); }
    };

    explicit Cutscene(std::coroutine_handle<promise_type> h) : handle(h) {}
    Cutscene(Cutscene&& other) noexcept : handle(std::exchange(other.handle, {})) {}
    Cutscene(const Cutscene&) = delete;
    Cutscene& operator=(const Cutscene&) = delete;
    Cutscene& operator=(Cutscene&&) = delete;
    ~Cutscene()
    {
        if (handle)
            handle.destroy();
    }

    // 다음 프레임까지 실행한다. 끝났으면 false.
    bool step()
    {
        if (!handle || handle.done())
            return false;
        handle.resume();
        return !handle.done();
    }

    std::coroutine_handle<promise_type> handle;
};

struct Npc
{
    std::string name;
    int affinity;   // 호감도
};

struct DialogueLog
{
    std::vector<int> affinityShown;   // 대사 창에 보여 준 호감도
};

// NPC와의 대화 컷신. 인사 → (다음 프레임) 선물 받기 → (다음 프레임) 호감도 보여 주기
Cutscene talkTo(const Npc& npc, DialogueLog& log)
{
    log.affinityShown.push_back(npc.affinity);
    co_await std::suspend_always{};
    co_await std::suspend_always{};
    log.affinityShown.push_back(npc.affinity + 10);   // 선물로 호감도 +10
}

// 마을 장면: NPC를 불러와 대화 컷신을 예약한다. 컷신은 엔진이 다음 프레임부터 돌린다.
Cutscene startVillageScene(int baseAffinity, DialogueLog& log)
{
    auto npc = std::make_unique<Npc>(Npc{ "Elder Bram", baseAffinity });
    return talkTo(*npc, log);
}   // 장면 준비 끝. 불러온 NPC 정리
}

void as07_cutscene()
{
    using namespace async7;
    DialogueLog log;
    Cutscene scene = startVillageScene(40, log);
    while (scene.step())
    {
    }

    expect("대사 창의 호감도 개수", 2, static_cast<int>(log.affinityShown.size()));
    expect("처음 보여 준 호감도", 40, log.affinityShown.at(0));
    expect("선물 뒤 호감도", 50, log.affinityShown.at(1));
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (편집기의 접기 기능으로 모두 접어 두세요)
// ============================================================================

#pragma region 힌트 1 (먼저 직접 추적해 보고 펼치세요)
// talkTo의 첫 줄에 중단점을 걸고 실행하세요. 호출 스택에 startVillageScene이 있나요? 누가 이 줄을 실행하고 있나요?
// npc.affinity를 보세요. 16진수로 보면?
#pragma endregion

#pragma region 힌트 2
// 코루틴은 처음 불릴 때 바로 실행되지 않을 수 있습니다(initial_suspend). 그리고 co_await에서 멈췄다가 나중에 이어집니다.
// 코루틴 프레임에 저장되는 것은 매개변수 npc의 "참조"입니다. 참조가 가리키는 Npc는 언제 지워지나요?
#pragma endregion

#pragma region 힌트 3
// 코루틴이 멈췄다 이어지는 동안 살아 있어야 하는 것은 코루틴 프레임 안으로 복사해 가져가세요(값으로 받기).
// 참조나 포인터, string_view 매개변수는 코루틴에서 특히 위험합니다. (9부 람다 캡처와 같은 종류의 실수)
#pragma endregion

#pragma region 채점용 숨은 테스트 (풀이에는 필요 없습니다. 고치지 마세요)
// 채점기(--grade)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void as07_cutscene_hidden()
{
    using namespace async7;
    DialogueLog log;
    Cutscene a = startVillageScene(5, log);
    Cutscene b = startVillageScene(77, log);
    bool running = true;
    while (running)
    {
        const bool ra = a.step();
        const bool rb = b.step();
        running = ra || rb;
    }
    int sum = 0;
    for (int v : log.affinityShown)
        sum += v;
    expect("대사 창의 호감도 개수", 4, static_cast<int>(log.affinityShown.size()));
    expect("보여 준 호감도 합 (5 + 77 + 15 + 87)", 184, sum);
}
#pragma endregion
