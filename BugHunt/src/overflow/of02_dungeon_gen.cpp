#include "../labs.h"
#include <cstddef>
#include <cstdint>

// 던전 생성기: 방 하나를 만들고 복도로 이어진 다음 방을 재귀로 만든다.
// 방마다 배치도(layout)를 최대 크기 버퍼에 그린 뒤, 다음 방들을 만들고 나서 자기 방의 보물 상자를 센다.
// 방이 몇 개 안 되는 시험 던전에서는 잘 동작했다.

// [JA] ダンジョン生成器: 部屋を一つ作り、通路でつながった次の部屋を再帰で作ります。
// [JA] 部屋ごとに配置図(layout)を最大サイズのバッファーに描き、次の部屋を作り終えてから自分の部屋の宝箱を数えます。
// [JA] 部屋が数個しかない試験用ダンジョンではうまく動いていました。
namespace ovf2
{
constexpr std::size_t kLayoutBytes = 96 * 1024;   // 방 배치도의 최대 크기

std::uint32_t nextSeed(std::uint32_t seed)
{
    return seed * 1664525u + 1013904223u;
}

// index번 방부터 마지막 방까지 만들고, 그 방들의 보물 상자 수를 돌려준다.
int buildRooms(int index, int roomCount, std::uint32_t seed)
{
    char layout[kLayoutBytes];                        // 이 방의 배치도
    const std::size_t size = 2048 + seed % 2048;      // 실제 방 크기
    std::uint32_t s = seed;
    for (std::size_t i = 0; i < size; ++i)
    {
        s = nextSeed(s);
        layout[i] = (s >> 16) % 64 == 0 ? '$' : '.';
    }
    if (index % 8 == 0)
        std::cout << "  방 " << index << "번 생성\n" << std::flush;

    int chests = 0;
    if (index + 1 < roomCount)
        chests += buildRooms(index + 1, roomCount, nextSeed(seed ^ 0x9E3779B9u));
    for (std::size_t i = 0; i < size; ++i)
        chests += layout[i] == '$' ? 1 : 0;
    return chests;
}
}

void of02_dungeon_gen()
{
    using namespace ovf2;

    const int small = buildRooms(0, 5, 7);
    std::cout << "  시험 던전 (방 5개): 보물 상자 " << small << "개\n";

    const int big = buildRooms(0, 160, 2024);
    std::cout << "  대형 던전 (방 160개): 보물 상자 " << big << "개\n";

    expect("시험 던전 보물 상자", 251, small);
    expect("대형 던전 보물 상자", 7'586, big);
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(Ctrl+M, Ctrl+O = すべて折りたたむ)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// 디버거로 실행해 멈췄을 때 호출 스택을 보세요. buildRooms가 몇 겹 쌓여 있나요? 방 160개를 다 만들기 전에 죽었습니다.
// 호출 스택에서 맨 위 buildRooms와 그보다 한 칸 아래 buildRooms를 각각 골라 조사식에 &layout을 넣어 보세요. 두 주소의 차이가 한 번의 호출이 쓰는 스택(프레임) 크기입니다.

// [JA] デバッガーで実行して止まったとき、呼び出し履歴を見てください。buildRooms は何段積み重なっていますか? 部屋160個を作り終える前に落ちました。
// [JA] 呼び出し履歴でいちばん上の buildRooms と、その一つ下の buildRooms をそれぞれ選び、ウォッチに &layout を入れてみてください。二つのアドレスの差が、一回の呼び出しが使うスタック(フレーム)の大きさです。
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// 재귀가 끝나지 않는 것이 아닙니다. 160겹이면 끝나는 재귀입니다. 문제는 "프레임 크기 x 깊이"입니다.
// 프레임마다 96 KB 버퍼가 있으므로 160겹이면 15 MB가 필요한데, 스택은 기본으로 Windows 1 MB, Linux 8 MB 정도입니다. Windows에서는 열 겹 남짓에서 넘칩니다.
// 프레임 크기는 구성마다 조금씩 다릅니다(Debug는 검사용 공간이 더 붙고, ASan은 변수 사이에 경계를 둡니다). 그래서 버퍼가 작은 재귀는 Debug에서만, 또는 ASan에서만 죽기도 합니다.
// 아래 layout은 다음 방들을 만드는 동안에도 살아 있어야 하므로(재귀 호출 뒤에 읽음) 컴파일러가 줄일 수 없습니다.

// [JA] 再帰が終わらないのではありません。160段で終わる再帰です。問題は「フレームの大きさ x 深さ」です。
// [JA] フレームごとに 96 KB のバッファーがあるので、160段なら 15 MB が必要ですが、スタックは既定で Windows が 1 MB、Linux が 8 MB 程度です。Windows では十段ほどであふれます。
// [JA] フレームの大きさは構成ごとに少しずつ違います(Debug は検査用の領域がさらに付き、ASan は変数の間に境界を置きます)。そのため、バッファーの小さい再帰は Debug でだけ、あるいは ASan でだけ落ちることもあります。
// [JA] layout は次の部屋を作っている間も生きていなければならないので(再帰呼び出しの後で読む)、コンパイラーは小さくできません。
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// 큰 버퍼는 힙으로 옮기세요: std::vector<char> layout(size); 이제 프레임에는 vector 하나(수십 바이트)만 남습니다.
// 또는 자기 방의 보물 상자를 재귀 호출 "전에" 세어 버퍼를 일찍 끝내거나, 재귀를 반복문으로 바꿉니다. 깊이가 데이터에 따라 정해지는 재귀는 늘 스택 한도를 생각해야 합니다.

// [JA] 大きなバッファーはヒープへ移してください: std::vector<char> layout(size); これでフレームには vector 一つ(数十バイト)だけが残ります。
// [JA] または、自分の部屋の宝箱を再帰呼び出しの「前に」数えてバッファーを早めに終わらせるか、再帰をループに変えます。深さがデータによって決まる再帰は、常にスタックの上限を考える必要があります。
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(grade.bat)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void of02_dungeon_gen_hidden()
{
    using namespace ovf2;
    expect("숨은 던전: 방 1개", 29, buildRooms(0, 1, 11));
    expect("숨은 던전: 방 40개", 1'904, buildRooms(0, 40, 31337));
    expect("숨은 던전: 방 300개", 14'399, buildRooms(0, 300, 99));
}
#pragma endregion
