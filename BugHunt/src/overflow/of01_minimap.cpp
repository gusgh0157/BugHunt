#include "../labs.h"
#include <cstddef>
#include <string>
#include <utility>
#include <vector>

// 미니맵을 열면 시작 칸에서 걸어서 갈 수 있는 칸을 모두 찾아 밝힌다(너비 우선 탐색).
// 지도는 최대 2048 x 2048칸까지 지원한다. 거리표(dist)는 최대 크기로 한 번에 잡아 둔다.

// [JA] ミニマップを開くと、開始マスから歩いて行けるマスをすべて探して明るくします(幅優先探索)。
// [JA] マップは最大 2048 x 2048 マスまで対応しています。距離表(dist)は最大サイズで一度に確保しておきます。
namespace ovf1
{
constexpr int kMaxSize = 2048;

struct Map
{
    int width = 0;
    int height = 0;
    std::vector<std::string> rows;   // '.' 길, '#' 벽 [JA] '.' 道、'#' 壁

    bool walkable(int x, int y) const
    {
        return x >= 0 && y >= 0 && x < width && y < height && rows[static_cast<size_t>(y)][static_cast<size_t>(x)] == '.';
    }
};

Map makeMap(std::vector<std::string> rows)
{
    Map m;
    m.height = static_cast<int>(rows.size());
    m.width = static_cast<int>(rows.front().size());
    m.rows = std::move(rows);
    return m;
}

// 시작 칸에서 갈 수 있는 칸 수
// [JA] 開始マスから行けるマスの数
int countReachable(const Map& map, int startX, int startY)
{
    int dist[kMaxSize][kMaxSize];   // 시작 칸에서 몇 걸음인지 (-1 = 아직 안 감) [JA] 開始マスから何歩か (-1 = まだ行っていない)
    for (int y = 0; y < map.height; ++y)
    {
        for (int x = 0; x < map.width; ++x)
            dist[y][x] = -1;
    }

    std::vector<std::pair<int, int>> queue;
    queue.emplace_back(startX, startY);
    dist[startY][startX] = 0;
    int reached = 0;
    for (size_t head = 0; head < queue.size(); ++head)
    {
        const auto [x, y] = queue[head];
        ++reached;
        const int step[4][2] = { { 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 } };
        for (const auto& s : step)
        {
            const int nx = x + s[0];
            const int ny = y + s[1];
            if (map.walkable(nx, ny) && dist[ny][nx] < 0)
            {
                dist[ny][nx] = dist[y][x] + 1;
                queue.emplace_back(nx, ny);
            }
        }
    }
    return reached;
}

void print(const Map& map)
{
    for (const std::string& row : map.rows)
        std::cout << "    " << row << '\n';
}
}

void of01_minimap()
{
    using namespace ovf1;

    const Map cave = makeMap({
        "..#.......",
        "..#.####..",
        "..#.#..#..",
        "....#..#..",
        "#####..###",
        "......#...",
    });
    print(cave);
    std::cout << i18n::L("  미니맵 여는 중...\n", "  ミニマップを開いています...\n") << std::flush;

    const int reached = countReachable(cave, 0, 0);
    std::cout << i18n::L("  밝힌 칸: ", "  照らしたマス: ") << reached << '\n';
    expect(i18n::L("시작 칸에서 갈 수 있는 칸 수", "開始マスから行けるマスの数"), 25, reached);
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(Ctrl+M, Ctrl+O = すべて折りたたむ)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// 디버거로 실행해 멈춘 곳의 호출 스택을 보세요. 재귀가 아닌데도 스택 오버플로입니다. 맨 위 함수는 무엇이고, countReachable의 몇 번째 줄인가요?
// countReachable의 여는 중괄호에 중단점을 걸어 보세요. 거기까지라도 가나요? 조사식에 sizeof(dist)를 넣어 보세요(크래시 전에 다른 곳에서 멈췄을 때 넣어도 됩니다).

// [JA] デバッガーで実行し、止まった場所の呼び出し履歴を見てください。再帰ではないのにスタックオーバーフローです。いちばん上の関数は何で、countReachable の何行目ですか?
// [JA] countReachable の開き波かっこにブレークポイントを置いてみてください。そこまででも行けますか? ウォッチに sizeof(dist) を入れてみてください(クラッシュ前に別の場所で止めたときに入れてもかまいません)。
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// dist는 2048 x 2048 x 4바이트 = 16 MB짜리 지역 변수입니다. 지역 변수는 스택에 놓이는데, 스레드 하나의 스택은 기본으로 Windows 1 MB, Linux 8 MB 정도입니다.
// 함수에 들어가자마자 16 MB를 잡으려다 스택 끝의 보호 페이지를 건드려 바로 죽습니다. 그래서 호출 스택이 짧고, 오류 위치가 함수의 첫 줄(또는 스택을 미리 건드려 보는 도우미 함수)입니다.
// 지도가 작아도 소용없습니다. 크기는 컴파일할 때 정해진 최대 크기입니다.

// [JA] dist は 2048 x 2048 x 4バイト = 16 MB のローカル変数です。ローカル変数はスタックに置かれますが、スレッド一つのスタックは既定で Windows が 1 MB、Linux が 8 MB 程度です。
// [JA] 関数に入った直後に 16 MB を確保しようとしてスタックの端のガードページに触れ、すぐに落ちます。そのため呼び出し履歴が短く、エラーの位置が関数の最初の行(またはスタックを事前に触って確かめる補助関数)になります。
// [JA] マップが小さくても関係ありません。サイズはコンパイル時に決まった最大サイズです。
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// 큰 데이터는 힙에 두세요. 실제 지도 크기만큼만: std::vector<int> dist(static_cast<size_t>(map.width) * map.height, -1); 그리고 dist[y * map.width + x]로 씁니다.
// (static 지역 배열로 바꿔도 죽지는 않지만 16 MB를 늘 붙잡고 있고, 여러 스레드에서 동시에 부를 수 없습니다)

// [JA] 大きなデータはヒープに置いてください。実際のマップの大きさの分だけ: std::vector<int> dist(static_cast<size_t>(map.width) * map.height, -1); とし、dist[y * map.width + x] で使います。
// [JA] (static のローカル配列に変えても落ちはしませんが、16 MB を常に抱えたままになり、複数のスレッドから同時に呼べません)
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(grade.bat)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
// [JA] 採点器(grade.bat)が、画面には表示されない別のデータで同じコードをもう一度検査します。
// [JA] 期待値をハードコードして [PASS] にしても、ここで引っかかります。
void of01_minimap_hidden()
{
    using namespace ovf1;
    const Map island = makeMap({
        "~~~~~~~",
        "~.....~",
        "~.###.~",
        "~.#.#.~",
        "~.###.~",
        "~.....~",
        "~~~~~~~",
    });
    expect(i18n::L("숨은 지도: 섬 둘레", "隠しマップ: 島の周り"), 16, countReachable(island, 1, 1));
    expect(i18n::L("숨은 지도: 갇힌 칸", "隠しマップ: 閉じ込められたマス"), 1, countReachable(island, 3, 3));

    std::vector<std::string> plain(300, std::string(400, '.'));
    plain[150] = std::string(399, '#') + ".";
    expect(i18n::L("숨은 지도: 넓은 들판", "隠しマップ: 広い野原"), 300 * 400 - 399, countReachable(makeMap(plain), 0, 0));
}
#pragma endregion
