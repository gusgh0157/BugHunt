#include "../labs.h"
#include <chrono>
#include <condition_variable>
#include <future>
#include <mutex>
#include <string>
#include <vector>

// 맵 로딩. 맵을 청크로 나누고, 청크마다 비동기 작업을 하나씩 띄워 한꺼번에 불러온다.
// 순서: 로딩 시작(작업을 모두 띄움) → 로딩 화면 표시와 함께 디스크를 깨움 → 로딩 끝(작업을 모두 기다림)

// [JA] マップのロードです。マップをチャンクに分け、チャンクごとに非同期タスクを1つずつ起動して一斉に読み込みます。
// [JA] 順序: ロード開始(タスクをすべて起動) → ロード画面の表示とともにディスクを起こす → ロード終了(タスクをすべて待つ)
namespace async1
{
// 디스크를 흉내 낸다. 깨어나기(spinUp) 전에 들어온 읽기는 조금 기다려 보고,
// 그래도 깨어나지 않으면 아주 느린 경로(콜드 읽기)로 읽는다.
class ChunkDisk
{
public:
    std::string read(int chunk)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        if (!wokeUp_.wait_for(lock, std::chrono::milliseconds(300), [&] { return awake_; }))
            ++coldReads_;
        return "chunk" + std::to_string(chunk);
    }

    void spinUp()
    {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            awake_ = true;
        }
        wokeUp_.notify_all();
    }

    int coldReads()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return coldReads_;
    }

private:
    std::mutex mutex_;
    std::condition_variable wokeUp_;
    bool awake_ = false;
    int coldReads_ = 0;
};

class MapLoader
{
public:
    MapLoader(ChunkDisk& disk, int chunkCount) : disk_(disk), chunks_(chunkCount) {}

    // 청크마다 비동기 작업을 띄우고 바로 돌아온다.
    void startLoading()
    {
        for (int i = 0; i < static_cast<int>(chunks_.size()); ++i)
            std::async(std::launch::async, [this, i] { chunks_[i] = disk_.read(i); });
    }

    // 띄운 작업이 모두 끝날 때까지 기다린다.
    void finishLoading()
    {
        for (std::future<void>& job : jobs_)
            job.get();
        jobs_.clear();
    }

    int loadedCount() const
    {
        int n = 0;
        for (const std::string& c : chunks_)
            n += c.empty() ? 0 : 1;
        return n;
    }

private:
    ChunkDisk& disk_;
    std::vector<std::string> chunks_;
    std::vector<std::future<void>> jobs_;
};

// 맵 하나를 불러온다. 불러온 청크 수를 돌려준다.
int loadMap(ChunkDisk& disk, int chunkCount)
{
    MapLoader loader(disk, chunkCount);
    loader.startLoading();
    disk.spinUp();          // 로딩 화면을 띄우면서 디스크를 깨운다
    loader.finishLoading();
    return loader.loadedCount();
}
}

void as01_loading()
{
    using namespace async1;
    constexpr int kChunks = 6;

    ChunkDisk disk;
    const auto start = std::chrono::steady_clock::now();
    const int loaded = loadMap(disk, kChunks);
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
    std::cout << i18n::L("  청크 ", "  チャンク ") << kChunks << i18n::L("개 로딩: ", "個の読み込み: ") << ms << i18n::L(" ms, 콜드 읽기 ", " ms、コールド読み込み ") << disk.coldReads() << i18n::L("번\n", "回\n");

    expect(i18n::L("불러온 청크 수", "読み込んだチャンク数"), kChunks, loaded);
    expect(i18n::L("디스크가 깨기 전에 기다리다 느린 경로로 읽은 청크 수", "ディスクが目覚める前に待たされ、遅い経路で読んだチャンク数"), 0, disk.coldReads());
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (편집기의 접기 기능으로 모두 접어 두세요)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(エディターの折りたたみ機能ですべて折りたたんでおいてください)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (README의 "따라 해 보기"를 먼저 하세요 / 先に README の"やってみよう"をやってください)
// ChunkDisk::read에 중단점을 걸고, 멈출 때마다 스레드 목록을 보세요.
// read를 실행 중인 작업 스레드가 동시에 몇 개 있나요? 메인 스레드는 그동안 어디에서 기다리고 있나요?

// [JA] ChunkDisk::read にブレークポイントを置き、止まるたびにスレッド ウィンドウを見てください。
// [JA] read を実行中の作業スレッドは同時にいくつありますか? メインスレッドはその間どこで待っていますか?
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// 메인 스레드의 호출 스택을 보세요. startLoading의 for 문 안, 그런데 std::future의 소멸자 안입니다.
// std::async가 돌려준 값은 어디에 저장되나요? 저장하지 않은 임시 객체는 언제 파괴되나요?
// (컴파일러 경고도 이 줄을 가리킵니다)

// [JA] メインスレッドの呼び出し履歴を見てください。startLoading の for 文の中、しかも std::future のデストラクターの中です。
// [JA] std::async が返した値はどこに保存されていますか? 保存しなかった一時オブジェクトはいつ破棄されますか?
// [JA] (コンパイラーの警告もこの行を指しています)
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// std::launch::async로 만든 작업의 future는 파괴될 때 작업이 끝나기를 기다립니다.
// 그래서 버린 future는 "비동기"를 "하나씩 차례로"로 바꿉니다. finishLoading이 기다릴 수 있게 future를 모아 두세요.

// [JA] std::launch::async で作ったタスクの future は、破棄されるときにタスクが終わるのを待ちます。
// [JA] そのため、捨てた future は「非同期」を「1つずつ順番に」に変えてしまいます。finishLoading が待てるように future を集めておいてください。
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(--grade)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void as01_loading_hidden()
{
    using namespace async1;
    ChunkDisk disk;
    expect(i18n::L("불러온 청크 수", "読み込んだチャンク数"), 4, loadMap(disk, 4));
    expect(i18n::L("콜드 읽기 수", "コールド読み込み数"), 0, disk.coldReads());
    ChunkDisk disk2;
    expect(i18n::L("불러온 청크 수 (두 번째 맵)", "読み込んだチャンク数(2番目のマップ)"), 9, loadMap(disk2, 9));
    expect(i18n::L("콜드 읽기 수 (두 번째 맵)", "コールド読み込み数(2番目のマップ)"), 0, disk2.coldReads());
}
#pragma endregion
