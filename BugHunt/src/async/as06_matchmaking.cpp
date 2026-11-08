#include "../labs.h"
#include <atomic>
#include <chrono>
#include <string>
#include <thread>
#include <vector>

// 매치메이킹. 대기열을 받으면 매칭 스레드를 하나 띄워 4명씩 묶는 동안, 이 스레드는 대기열을 검사한다.
// 대기열이 비어 있거나 인원이 4의 배수가 아니면 매칭을 취소한다.

// [JA] マッチメイキングです。待ち行列を受け取るとマッチングスレッドを1つ起動して4人ずつまとめさせ、その間にこのスレッドは待ち行列をチェックします。
// [JA] 待ち行列が空か、人数が4の倍数でなければマッチングを取り消します。
namespace async6
{
struct MatchResult
{
    int matches = 0;
    bool cancelled = false;
};

void groupPlayers(const std::vector<std::string>& queue, const std::atomic<bool>& cancel, int& matches)
{
    for (size_t i = 0; i + 4 <= queue.size(); i += 4)
    {
        if (cancel.load())
            return;
        std::this_thread::sleep_for(std::chrono::milliseconds(5));   // 서버에 방을 만든다
        ++matches;
    }
}

MatchResult runMatchmaking(const std::vector<std::string>& queue)
{
    MatchResult result;
    std::atomic<bool> cancel{ false };
    std::thread matcher(groupPlayers, std::cref(queue), std::cref(cancel), std::ref(result.matches));

    if (queue.empty() || queue.size() % 4 != 0)
    {
        cancel = true;
        result.cancelled = true;
        result.matches = 0;
        return result;
    }

    matcher.join();
    return result;
}
}

void as06_matchmaking()
{
    using namespace async6;
    const MatchResult first = runMatchmaking({ "Kael", "Mira", "Doran", "Selene", "Ivo", "Nell", "Pia", "Rook" });
    std::cout << i18n::L("  첫 대기열: 매치 ", "  1 つ目のキュー: マッチ ") << first.matches << i18n::L("개\n", "件\n");

    const MatchResult second = runMatchmaking({ "Sana", "Tom", "Uma" });
    std::cout << i18n::L("  둘째 대기열: ", "  2 つ目のキュー: ") << (second.cancelled ? i18n::L("취소됨", "キャンセル") : i18n::L("매칭됨", "マッチング成立")) << '\n';

    expect("첫 대기열 매치 수", 2, first.matches);
    expect("둘째 대기열은 취소되었나?", true, second.cancelled);
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (편집기의 접기 기능으로 모두 접어 두세요)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(エディターの折りたたみ機能ですべて折りたたんでおいてください)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// 디버거로 실행하면 abort로 멈춥니다. 호출 스택을 아래에서 위로 읽어 보세요.
// 내 코드의 마지막 프레임은 어느 줄인가요? 그 줄에서 파괴되는 객체는 무엇인가요?

// [JA] デバッガーで実行すると abort で止まります。呼び出し履歴を下から上へ読んでみてください。
// [JA] 自分のコードの最後のフレームはどの行ですか? その行で破棄されるオブジェクトは何ですか?
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// std::terminate를 부른 것은 std::thread의 소멸자입니다.
// joinable한(join도 detach도 하지 않은) std::thread가 파괴되면 표준은 프로그램을 끝내라고 정해 두었습니다.
// runMatchmaking에서 matcher.join()을 지나지 않고 함수를 빠져나가는 길은 어디인가요?

// [JA] std::terminate を呼んだのは std::thread のデストラクターです。
// [JA] joinable な(join も detach もしていない)std::thread が破棄されたら、プログラムを終了するよう規格で定められています。
// [JA] runMatchmaking で matcher.join() を通らずに関数を抜ける道はどこですか?
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// 모든 빠져나가는 길(return, 예외)에서 스레드를 정리해야 합니다. 사람이 빠뜨리지 않게 하려면 RAII를 쓰세요.
// C++20의 std::jthread는 파괴될 때 멈춰 달라고 요청하고(stop_token) join합니다.
// 다만 취소할 때는 먼저 멈추게 하고 join한 뒤에 결과(matches, cancelled)를 고치세요. jthread에 맡겨 return 뒤에 join되면,
// 그 사이 매칭 스레드가 아직 result.matches를 올리고 있을 수 있습니다(돌려준 결과와 경쟁).

// [JA] すべての抜け道(return、例外)でスレッドを後始末しなければなりません。人がうっかり漏らさないようにするには RAII を使ってください。
// [JA] C++20 の std::jthread は、破棄されるときに停止を要求し(stop_token)、join します。
// [JA] ただし取り消すときは、先に止めて join してから結果(matches、cancelled)を書き換えてください。jthread に任せて return の後に join されると、
// [JA] その間にマッチングスレッドがまだ result.matches を増やしているかもしれません(返した結果と競合します)。
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(--grade)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void as06_matchmaking_hidden()
{
    using namespace async6;
    const MatchResult empty = runMatchmaking({});
    const MatchResult big = runMatchmaking({ "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l" });
    const MatchResult odd = runMatchmaking({ "a", "b", "c", "d", "e" });
    expect("빈 대기열은 취소되었나?", true, empty.cancelled);
    expect("12명 대기열 매치 수", 3, big.matches);
    expect("5명 대기열은 취소되었나?", true, odd.cancelled);
    expect("5명 대기열 매치 수", 0, odd.matches);
}
#pragma endregion
