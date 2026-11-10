#include "../labs.h"
#include <cstdint>

// 리플레이 녹화기: 녹화 길이(틱 수)만큼 프레임 배열을 잡고, 0틱부터 마지막 틱까지 입력을 기록한다.
// 녹화가 끝나면 체크섬을 계산해 저장하고 녹화기를 닫는다.

// [JA] リプレイ録画機: 録画の長さ(ティック数)の分だけフレーム配列を確保し、0ティックから最後のティックまで入力を記録します。
// [JA] 録画が終わるとチェックサムを計算して保存し、録画機を閉じます。
namespace ovf3
{
struct Frame
{
    int tick;
    std::uint32_t buttons;   // 눌린 버튼 비트 [JA] 押されたボタンのビット
};

class ReplayRecorder
{
public:
    explicit ReplayRecorder(int ticks) : ticks_(ticks), frames_(new Frame[ticks]) {}
    ~ReplayRecorder() { delete[] frames_; }
    ReplayRecorder(const ReplayRecorder&) = delete;
    ReplayRecorder& operator=(const ReplayRecorder&) = delete;

    // 입력 장치에서 tick마다 버튼 상태를 읽어 기록한다
    // [JA] 入力デバイスから tick ごとにボタンの状態を読んで記録します
    void recordAll(std::uint32_t seed)
    {
        for (int t = 0; t <= ticks_; ++t)
            frames_[t] = Frame{ t, buttonsAt(seed, t) };
    }

    int ticks() const { return ticks_; }
    const Frame& frame(int t) const { return frames_[t]; }

    std::uint32_t checksum() const
    {
        std::uint32_t sum = 0;
        for (int t = 0; t < ticks_; ++t)
            sum = sum * 31u + frames_[t].buttons;
        return sum;
    }

    static std::uint32_t buttonsAt(std::uint32_t seed, int t)
    {
        return (seed * 2654435761u + static_cast<std::uint32_t>(t) * 40503u) >> 24;
    }

private:
    int ticks_;
    Frame* frames_;
};

std::uint32_t expectedChecksum(std::uint32_t seed, int ticks)
{
    std::uint32_t sum = 0;
    for (int t = 0; t < ticks; ++t)
        sum = sum * 31u + ReplayRecorder::buttonsAt(seed, t);
    return sum;
}

// 녹화 한 판: 녹화하고, 체크섬을 돌려주고, 녹화기를 닫는다
// [JA] 録画1回分: 録画し、チェックサムを返し、録画機を閉じます
std::uint32_t recordMatch(int ticks, std::uint32_t seed, int& lastTick)
{
    ReplayRecorder recorder(ticks);
    recorder.recordAll(seed);
    lastTick = recorder.frame(recorder.ticks() - 1).tick;
    return recorder.checksum();
}
}

void of03_replay_buffer()
{
    using namespace ovf3;

    int lastTick = -1;
    const std::uint32_t sum = recordMatch(600, 77, lastTick);
    std::cout << i18n::L("  600틱 녹화: 마지막 프레임 ", "  600ティック録画: 最後のフレーム ") << lastTick << i18n::L("틱, 체크섬 ", "ティック、チェックサム ") << sum << '\n';
    expect(i18n::L("마지막 프레임의 틱", "最後のフレームのティック"), 599, lastTick);
    expect(i18n::L("체크섬", "チェックサム"), expectedChecksum(77, 600), sum);

    std::cout << i18n::L("  다음 판 녹화...\n", "  次の試合を録画...\n") << std::flush;
    const std::uint32_t sum2 = recordMatch(120, 5, lastTick);
    expect(i18n::L("두 번째 판 체크섬", "2試合目のチェックサム"), expectedChecksum(5, 120), sum2);
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(Ctrl+M, Ctrl+O = すべて折りたたむ)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// recordAll은 아무 일 없이 끝납니다. 호출 스택을 보면 죽은 곳은 기록하는 곳이 아니라, 녹화기를 닫는 ~ReplayRecorder의 delete[]입니다. 메시지를 읽어 보세요.
// "블록 바로 뒤 경계"는 어디일까요? recordAll의 첫 줄에서 멈추고 조사식에 &frames_[ticks_]를 넣어 그 주소에 데이터 중단점(값이 바뀔 때 멈춤)을 걸고 계속 실행해 보세요.

// [JA] recordAll は何事もなく終わります。呼び出し履歴を見ると、落ちた場所は記録している場所ではなく、録画機を閉じる ~ReplayRecorder の delete[] です。メッセージを読んでみてください。
// [JA] 「ブロックの直後の境界」とはどこでしょうか? recordAll の最初の行で止め、ウォッチに &frames_[ticks_] を入れて、そのアドレスにデータ ブレークポイント(値が変わったら止まる)を置いて実行を続けてみてください。
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// new Frame[600]의 원소는 frames_[0] ~ frames_[599]입니다. t <= ticks_는 frames_[600]까지, 할당한 블록 바로 뒤에 한 칸 더 씁니다.
// 쓰는 순간에는 아무 일도 없습니다. 그 자리는 다른 할당의 관리 정보이거나(여기서는 BugHunt 메모리 검사기의 경계 바이트 0xBD) 아무도 안 쓰는 곳이라서, 망가진 것이 드러나는 것은 나중에 그 블록을 해제하거나 다른 할당이 그 자리를 쓸 때입니다.
// 그래서 크래시 위치(해제)와 원인(쓰기)이 멀리 떨어져 있습니다. ASan 구성에서는 쓰는 순간 heap-buffer-overflow로 멈춥니다.

// [JA] new Frame[600] の要素は frames_[0] ~ frames_[599] です。t <= ticks_ は frames_[600] まで、確保したブロックの直後にもう一つ書き込みます。
// [JA] 書き込んだ瞬間には何も起きません。その場所は別の割り当ての管理情報か(ここでは BugHunt メモリチェッカーの境界バイト 0xBD)、誰も使っていない場所なので、壊れたことが表に出るのは、後でそのブロックを解放するときか、別の割り当てがその場所を使うときです。
// [JA] そのため、クラッシュの位置(解放)と原因(書き込み)が遠く離れています。ASan 構成では、書き込んだ瞬間に heap-buffer-overflow で止まります。
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// 반복 조건을 t < ticks_로 고치세요. 원소가 n개인 배열의 마지막 인덱스는 n - 1입니다.
// std::vector<Frame> frames_(ticks)와 범위 for(또는 at())를 쓰면 이런 실수가 들어설 자리가 줄어듭니다.

// [JA] ループの条件を t < ticks_ に直してください。要素が n 個の配列の最後のインデックスは n - 1 です。
// [JA] std::vector<Frame> frames_(ticks) と範囲 for(または at())を使えば、このようなミスが入り込む余地が減ります。
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(grade.bat)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
// [JA] 採点器(grade.bat)が、画面には表示されない別のデータで同じコードをもう一度検査します。
// [JA] 期待値をハードコードして [PASS] にしても、ここで引っかかります。
void of03_replay_buffer_hidden()
{
    using namespace ovf3;
    for (int ticks : { 1, 2, 37, 1800 })
    {
        int lastTick = -1;
        const std::uint32_t seed = static_cast<std::uint32_t>(ticks) * 13u + 1u;
        const std::uint32_t sum = recordMatch(ticks, seed, lastTick);
        expect(i18n::L("숨은 녹화: 마지막 프레임의 틱", "隠し録画: 最後のフレームのティック"), ticks - 1, lastTick);
        expect(i18n::L("숨은 녹화: 체크섬", "隠し録画: チェックサム"), expectedChecksum(seed, ticks), sum);
    }
}
#pragma endregion
