#include "../labs.h"
#include <coroutine>
#include <exception>
#include <memory>
#include <string>
#include <utility>
#include <vector>

// 컷신 스크립트. 컷신 하나는 코루틴이고, 한 줄을 보여 준 뒤 다음 프레임까지 멈췄다가(co_await) 이어서 실행된다.
// 컷신 엔진은 매 프레임 멈춰 있는 컷신들을 차례로 다시 깨운다.

// [JA] カットシーンのスクリプトです。カットシーン1つが1つのコルーチンで、1行を表示したあと次のフレームまで中断し(co_await)、続きから実行されます。
// [JA] カットシーンエンジンは毎フレーム、中断しているカットシーンを順番に再開させます。
namespace async7
{
// 매 프레임 한 번 깨워 주는 아주 작은 코루틴 타입
// [JA] 毎フレーム1回起こしてくれる、ごく小さなコルーチン型
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
    // [JA] 次のフレームまで実行します。終わっていれば false。
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
    int affinity;   // 호감도 [JA] 好感度
};

struct DialogueLog
{
    std::vector<int> affinityShown;   // 대사 창에 보여 준 호감도 [JA] 会話ウィンドウに表示した好感度
};

// NPC와의 대화 컷신. 인사 → (다음 프레임) 선물 받기 → (다음 프레임) 호감도 보여 주기
// [JA] NPC との会話カットシーン。あいさつ → (次のフレーム) プレゼントを受け取る → (次のフレーム) 好感度を表示
Cutscene talkTo(const Npc& npc, DialogueLog& log)
{
    log.affinityShown.push_back(npc.affinity);
    co_await std::suspend_always{};
    co_await std::suspend_always{};
    log.affinityShown.push_back(npc.affinity + 10);   // 선물로 호감도 +10 [JA] プレゼントで好感度 +10
}

// 마을 장면: NPC를 불러와 대화 컷신을 예약한다. 컷신은 엔진이 다음 프레임부터 돌린다.
// [JA] 村のシーン: NPC を読み込み、会話カットシーンを予約します。カットシーンはエンジンが次のフレームから実行します。
Cutscene startVillageScene(int baseAffinity, DialogueLog& log)
{
    auto npc = std::make_unique<Npc>(Npc{ "Elder Bram", baseAffinity });
    return talkTo(*npc, log);
}   // 장면 준비 끝. 불러온 NPC 정리 [JA] シーンの準備完了。読み込んだ NPC を片付ける
}

void as07_cutscene()
{
    using namespace async7;
    DialogueLog log;
    Cutscene scene = startVillageScene(40, log);
    while (scene.step())
    {
    }

    expect(i18n::L("대사 창의 호감도 개수", "会話ウィンドウの好感度の個数"), 2, static_cast<int>(log.affinityShown.size()));
    expect(i18n::L("처음 보여 준 호감도", "最初に表示した好感度"), 40, log.affinityShown.at(0));
    expect(i18n::L("선물 뒤 호감도", "プレゼント後の好感度"), 50, log.affinityShown.at(1));
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (편집기의 접기 기능으로 모두 접어 두세요)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(エディターの折りたたみ機能ですべて折りたたんでおいてください)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// talkTo의 첫 줄에 중단점을 걸고 실행하세요. 호출 스택에 startVillageScene이 있나요? 누가 이 줄을 실행하고 있나요?
// npc.affinity를 보세요. 16진수로 보면?

// [JA] talkTo の最初の行にブレークポイントを置いて実行してください。呼び出し履歴に startVillageScene はありますか? 誰がこの行を実行していますか?
// [JA] npc.affinity を見てください。16進数で見ると?
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// 코루틴은 처음 불릴 때 바로 실행되지 않을 수 있습니다(initial_suspend). 그리고 co_await에서 멈췄다가 나중에 이어집니다.
// 코루틴 프레임에 저장되는 것은 매개변수 npc의 "참조"입니다. 참조가 가리키는 Npc는 언제 지워지나요?

// [JA] コルーチンは最初に呼ばれたときすぐには実行されないことがあります(initial_suspend)。そして co_await で中断し、あとで再開されます。
// [JA] コルーチンフレームに保存されるのは引数 npc の「参照」です。参照が指す Npc はいつ削除されますか?
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// 코루틴이 멈췄다 이어지는 동안 살아 있어야 하는 것은 코루틴 프레임 안으로 복사해 가져가세요(값으로 받기).
// 참조나 포인터, string_view 매개변수는 코루틴에서 특히 위험합니다. (9부 람다 캡처와 같은 종류의 실수)

// [JA] コルーチンが中断してから再開するまでの間生きていなければならないものは、コルーチンフレームの中にコピーして持っていってください(値で受け取る)。
// [JA] 参照やポインター、string_view の引数はコルーチンでは特に危険です。(第9部のラムダキャプチャと同じ種類のミス)
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(--grade)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
// [JA] 採点器(--grade)が、画面には表示されない別のデータで同じコードをもう一度検査します。
// [JA] 期待値をハードコードして [PASS] にしても、ここで引っかかります。
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
    expect(i18n::L("대사 창의 호감도 개수", "会話ウィンドウの好感度の個数"), 4, static_cast<int>(log.affinityShown.size()));
    expect(i18n::L("보여 준 호감도 합 (5 + 77 + 15 + 87)", "表示した好感度の合計(5 + 77 + 15 + 87)"), 184, sum);
}
#pragma endregion
