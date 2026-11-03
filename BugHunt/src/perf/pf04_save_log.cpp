#include "../labs.h"
#include "perf_kit.h"
#include <ostream>
#include <string>
#include <vector>

// 세이브 파일 쓰기. 월드의 모든 오브젝트를 한 줄씩 기록한다.
// 오브젝트가 많은 맵에서 저장이 몇 초씩 걸린다. 저장된 내용은 맞다.

// [JA] セーブファイルの書き込みです。ワールドのすべてのオブジェクトを1行ずつ記録します。
// [JA] オブジェクトの多いマップでは保存に数秒かかります。保存された内容は正しいです。
namespace perf4
{
struct WorldObject
{
    int id;
    int x;
    int y;
};

void writeSave(std::ostream& out, const std::vector<WorldObject>& objects)
{
    out << "SAVE v3 " << objects.size() << std::endl;
    for (const WorldObject& o : objects)
        out << o.id << ' ' << o.x << ' ' << o.y << std::endl;
    out.flush();   // 다 쓴 뒤 한 번 디스크로 내보낸다
}

std::vector<WorldObject> makeWorld(int count)
{
    std::vector<WorldObject> objects;
    for (int i = 0; i < count; ++i)
        objects.push_back({ 1000 + i, (i * 13) % 500, (i * 29) % 500 });
    return objects;
}

struct SaveStats
{
    long long bytes;
    long long flushes;
};

SaveStats save(int count)
{
    perf::FakeLogFile file;
    std::ostream out(&file);
    writeSave(out, makeWorld(count));
    return { file.bytes(), file.flushes() };
}
}

void pf04_save_log()
{
    const perf::Stopwatch sw;
    const perf4::SaveStats s = perf4::save(5000);
    std::cout << "  오브젝트 5000개 저장: " << sw.ms() << " ms, " << s.bytes << " 바이트, 디스크 쓰기(flush) " << s.flushes
              << "번\n";

    expect("저장한 바이트 수", 62813LL, s.bytes);
    expect("디스크 쓰기(flush) 횟수", 1LL, s.flushes);
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (편집기의 접기 기능으로 모두 접어 두세요)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(エディターの折りたたみ機能ですべて折りたたんでおいてください)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// Release + 프로파일러(CPU 샘플링). 핫 경로 맨 아래에 있는 함수는 무엇이고, 누가 그 함수를 부르나요?
// (perf_kit.h의 FakeLogFile::sync가 "디스크에 쓰는" 흉내를 냅니다.)

// [JA] Release + プロファイラー(CPU サンプリング)。ホットパスのいちばん下にある関数は何で、誰がその関数を呼んでいますか?
// [JA] (perf_kit.h の FakeLogFile::sync が「ディスクに書き込む」ふりをします。)
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// FakeLogFile::sync에 중단점을 걸고 몇 번 계속(continue)해 보세요. 매번 호출 스택의 어느 줄에서 불리나요?
// std::endl은 줄바꿈 말고 무엇을 더 하나요?

// [JA] FakeLogFile::sync にブレークポイントを置いて、何回か続行(continue)してみてください。毎回、呼び出し履歴のどの行から呼ばれていますか?
// [JA] std::endl は改行のほかに何をしていますか?
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(--grade)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void pf04_save_log_hidden()
{
    const perf4::SaveStats s = perf4::save(321);
    expect("저장한 바이트 수", 4034LL, s.bytes);
    expect("flush 횟수", 1LL, s.flushes);

    const perf4::SaveStats empty = perf4::save(0);
    expect("빈 월드 바이트 수", 10LL, empty.bytes);
    expect("빈 월드 flush 횟수", 1LL, empty.flushes);
}
#pragma endregion
