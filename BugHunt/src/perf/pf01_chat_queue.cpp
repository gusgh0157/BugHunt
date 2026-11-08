#include "../labs.h"
#include "perf_kit.h"
#include <string>
#include <vector>

// 채팅 서버. 받은 메시지를 큐에 쌓아 두었다가, 프레임마다 앞에서부터 꺼내 처리한다.
// 이벤트가 끝나는 순간 메시지가 몰리면 서버가 몇 초씩 굳는다는 제보가 들어왔다. 결과는 맞다.

// [JA] チャットサーバーです。受け取ったメッセージをキューにためておき、フレームごとに先頭から取り出して処理します。
// [JA] イベントが終わる瞬間にメッセージが殺到すると、サーバーが数秒間固まるという報告がありました。結果は正しいです。
namespace perf1
{
long long g_moves = 0;   // Message가 옮겨진(이동 생성/대입) 횟수

struct Message
{
    int sender = 0;
    std::string text;

    Message(int s, std::string t) : sender(s), text(std::move(t)) {}
    Message(const Message&) = default;
    Message& operator=(const Message&) = default;
    Message(Message&& other) noexcept : sender(other.sender), text(std::move(other.text)) { ++g_moves; }
    Message& operator=(Message&& other) noexcept
    {
        sender = other.sender;
        text = std::move(other.text);
        ++g_moves;
        return *this;
    }
};

class ChatQueue
{
public:
    void push(Message m) { queue_.push_back(std::move(m)); }

    // 쌓인 메시지를 모두 처리하고, 처리한 글자 수의 합을 돌려준다.
    long long drain()
    {
        long long chars = 0;
        while (!queue_.empty())
        {
            chars += static_cast<long long>(queue_.front().text.size());
            queue_.erase(queue_.begin());
        }
        return chars;
    }

private:
    std::vector<Message> queue_;
};

long long runBurst(int messageCount)
{
    ChatQueue q;
    for (int i = 0; i < messageCount; ++i)
        q.push({ i % 50, "gg " + std::to_string(i) });
    g_moves = 0;
    return q.drain();
}
}

void pf01_chat_queue()
{
    constexpr int kMessages = 8000;

    const perf::Stopwatch sw;
    const long long chars = perf1::runBurst(kMessages);
    std::cout << i18n::L("  메시지 ", "  メッセージ ") << kMessages << i18n::L("개 처리: ", "件 処理: ") << sw.ms() << i18n::L(" ms, 원소 이동 ", " ms、要素の移動 ")
              << perf1::g_moves << i18n::L("번\n", "回\n");

    expect("처리한 글자 수", 54890LL, chars);
    expect("처리 중 원소 이동이 메시지 수의 2배 이하인가?", true, perf1::g_moves <= 2LL * kMessages);
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (편집기의 접기 기능으로 모두 접어 두세요)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(エディターの折りたたみ機能ですべて折りたたんでおいてください)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (README의 "따라 해 보기"를 먼저 하세요 / 先に README の"やってみよう"をやってください)
// Release 구성을 프로파일러(CPU 샘플링)로 실행하세요. 끝나면 시간을 가장 많이 쓴 호출 경로(핫 경로)를 펼쳐 보세요.
// 시간의 대부분을 쓰는 함수는 무엇이고, 그 함수는 내 코드의 어느 줄에서 불렸나요?

// [JA] Release 構成をプロファイラー(CPU サンプリング)で実行してください。終わったら、時間をいちばん多く使った呼び出し経路(ホットパス)を展開してみてください。
// [JA] 時間の大部分を使っている関数は何で、その関数は自分のコードのどの行から呼ばれましたか?
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// 메시지 수를 2배로 늘리면 원소 이동 횟수는 몇 배가 되나요? (kMessages를 바꿔 실행해 보세요)
// vector의 맨 앞을 지우면 뒤의 원소들은 어떻게 되나요?

// [JA] メッセージ数を2倍にすると、要素の移動回数は何倍になりますか?(kMessages を変えて実行してみてください)
// [JA] vector の先頭を消すと、後ろの要素はどうなりますか?
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// 앞에서 꺼내는 일이 잦다면 앞에서 지우기 싼 컨테이너를 쓰거나, 지우지 말고 끝까지 읽은 뒤 한 번에 비우세요.

// [JA] 先頭から取り出すことが多いなら、先頭の削除が安いコンテナーを使うか、消さずに最後まで読んでから一度に空にしてください。
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(--grade)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void pf01_chat_queue_hidden()
{
    const long long chars = perf1::runBurst(3000);
    expect("처리한 글자 수", 19890LL, chars);
    expect("원소 이동이 메시지 수의 2배 이하인가?", true, perf1::g_moves <= 2LL * 3000);

    perf1::ChatQueue q;
    q.push({ 1, "hello" });
    q.push({ 2, "world!" });
    expect("짧은 큐", 11LL, q.drain());
    expect("두 번째 drain은 0", 0LL, q.drain());
}
#pragma endregion
