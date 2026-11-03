#include "../labs.h"
#include <string>

// 자동 이동 경로. 웨이포인트를 연결 리스트로 잇고, 중간에 들를 곳을 끼워 넣는다.
// minutes = 앞 지점에서 이 지점까지 걸리는 시간(분)

// [JA] 自動移動ルートです。ウェイポイントを連結リストでつなぎ、途中で立ち寄る場所を挿入します。
// [JA] minutes = 前の地点からこの地点までにかかる時間(分)
namespace hang4
{
struct Waypoint
{
    std::string name;
    int minutes;
    Waypoint* next = nullptr;
};

// pos 바로 뒤에 wp를 끼워 넣는다.
void insertAfter(Waypoint* pos, Waypoint* wp)
{
    pos->next = wp;
    wp->next = pos->next;
}

int countStops(const Waypoint* head)
{
    int stops = 0;
    for (const Waypoint* p = head; p != nullptr; p = p->next)
        ++stops;
    return stops;
}

int totalMinutes(const Waypoint* head)
{
    int total = 0;
    for (const Waypoint* p = head; p != nullptr; p = p->next)
        total += p->minutes;
    return total;
}
}

void hg04_route()
{
    using namespace hang4;

    Waypoint town = { "Town", 0 };
    Waypoint bridge = { "Bridge", 25 };
    Waypoint castle = { "Castle", 40 };
    town.next = &bridge;
    bridge.next = &castle;

    // 다리로 가는 길에 숲에 들러 약초를 캔다.
    Waypoint forest = { "Forest", 15 };
    insertAfter(&town, &forest);

    int stops = 0;
    int minutes = 0;
    runWithWatchdog("경로 계산", [&] {
        stops = countStops(&town);
        minutes = totalMinutes(&town);
    });

    expect("들르는 곳 (Town, Forest, Bridge, Castle)", 4, stops);
    expect("걸리는 시간 (0 + 15 + 25 + 40)", 80, minutes);
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (편집기의 접기 기능으로 모두 접어 두세요)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(エディターの折りたたみ機能ですべて折りたたんでおいてください)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// 멈추면 모두 중단(break all) → 스레드 목록에서 lab-worker(랩 작업 스레드). 지역 변수에서 p와 stops를 보세요.
// stops는 4보다 훨씬 큰데 p->name은 무엇인가요?

// [JA] 止まったらすべて中断(break all) → スレッド ウィンドウで lab-worker(ラボの作業スレッド)へ。ローカル変数で p と stops を見てください。
// [JA] stops は4よりはるかに大きいのに、p->name は何ですか?
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// 조사식에 p, p->next, p->next->next를 넣어 주소를 비교해 보세요.
// 처음부터 보려면 hg04_route의 insertAfter 줄에 중단점을 걸고 town을 펼쳐 next를 따라가 보세요. 끼워 넣기 전과 후가 어떻게 다른가요?

// [JA] ウォッチに p, p->next, p->next->next を入れてアドレスを比べてみてください。
// [JA] 最初から見るには、hg04_route の insertAfter の行にブレークポイントを置き、town を展開して next をたどってみてください。挿入の前と後で何が違いますか?
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// insertAfter를 한 줄씩 지나가며 pos->next와 wp->next를 보세요. 두 줄의 순서가 중요합니다.

// [JA] insertAfter を1行ずつ進めながら pos->next と wp->next を見てください。2行の順序が重要です。
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(--grade)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void hg04_route_hidden()
{
    using namespace hang4;

    Waypoint camp = { "Camp", 0 };
    Waypoint peak = { "Peak", 90 };
    camp.next = &peak;

    Waypoint river = { "River", 20 };
    Waypoint cave = { "Cave", 35 };
    Waypoint shrine = { "Shrine", 10 };
    insertAfter(&camp, &river);       // Camp, River, Peak
    insertAfter(&river, &cave);       // Camp, River, Cave, Peak
    insertAfter(&peak, &shrine);      // Camp, River, Cave, Peak, Shrine (맨 끝)

    int stops = 0;
    int minutes = 0;
    runWithWatchdog("경로 계산 (숨은 케이스)", [&] {
        stops = countStops(&camp);
        minutes = totalMinutes(&camp);
    });
    expect("들르는 곳", 5, stops);
    expect("걸리는 시간", 155, minutes);
}
#pragma endregion
