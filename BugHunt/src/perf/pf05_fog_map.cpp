#include "../labs.h"
#include "perf_kit.h"
#include <vector>

// 전장의 안개. 맵의 칸마다 "밝힌 정도"(0~255)가 있고, 매 프레임 밝혀진 칸 수와 밝기 합을 센다.
// 큰 맵에서만 프레임이 뚝 떨어진다. 계산 결과는 맞다.
namespace perf5
{
class FogMap
{
public:
    FogMap(int rows, int cols) : rows_(rows), cols_(cols), cells_(static_cast<size_t>(rows) * cols) {}

    int rows() const { return rows_; }
    int cols() const { return cols_; }

    // 칸은 한 줄(row)씩 이어서 메모리에 놓여 있다: [0행 0열][0행 1열]...[0행 끝][1행 0열]...
    int& at(int row, int col)
    {
        const size_t index = static_cast<size_t>(row) * cols_ + col;
        if (index != lastIndex_ + 1)
            ++jumps_;   // 바로 다음 칸이 아닌 곳으로 건너뛴 횟수 (캐시가 싫어하는 접근)
        lastIndex_ = index;
        return cells_[index];
    }

    long long jumps() const { return jumps_; }
    void resetJumps()
    {
        jumps_ = 0;
        lastIndex_ = static_cast<size_t>(-1);
    }

private:
    int rows_;
    int cols_;
    std::vector<int> cells_;
    long long jumps_ = 0;
    size_t lastIndex_ = static_cast<size_t>(-1);
};

void reveal(FogMap& map)
{
    for (int r = 0; r < map.rows(); ++r)
        for (int c = 0; c < map.cols(); ++c)
            map.at(r, c) = (r * 7 + c * 3) % 256;
}

struct FogStats
{
    long long revealed;     // 밝기가 128 이상인 칸 수
    long long brightness;   // 밝기 합
};

FogStats countFog(FogMap& map)
{
    FogStats s{ 0, 0 };
    for (int c = 0; c < map.cols(); ++c)
    {
        for (int r = 0; r < map.rows(); ++r)
        {
            const int v = map.at(r, c);
            s.brightness += v;
            if (v >= 128)
                ++s.revealed;
        }
    }
    return s;
}
}

void pf05_fog_map()
{
    perf5::FogMap map(2000, 2000);
    perf5::reveal(map);

    map.resetJumps();
    const perf::Stopwatch sw;
    const perf5::FogStats s = perf5::countFog(map);
    std::cout << "  2000x2000 맵 세기: " << sw.ms() << " ms, 건너뛴 접근 " << map.jumps() << "번\n";

    expect("밝혀진 칸 수", 2000044LL, s.revealed);
    expect("밝기 합", 510006272LL, s.brightness);
    expect("건너뛴 접근이 행 수 이하인가?", true, map.jumps() <= map.rows());
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (편집기의 접기 기능으로 모두 접어 두세요)
// ============================================================================

#pragma region 힌트 1 (먼저 직접 추적해 보고 펼치세요)
// Release + 프로파일러(CPU 샘플링). 시간을 쓰는 함수가 countFog라는 것까지는 쉽게 나옵니다. 그런데 reveal도 같은 칸 수를 도는데요?
// 두 함수의 반복문은 무엇이 다른가요?
#pragma endregion

#pragma region 힌트 2
// at()에 조건부 중단점(조건: jumps_ < 5)을 걸고 row, col, index가 어떻게 변하는지 보세요.
// 연속으로 읽은 두 칸은 메모리에서 몇 바이트 떨어져 있나요?
#pragma endregion

#pragma region 채점용 숨은 테스트 (풀이에는 필요 없습니다. 고치지 마세요)
// 채점기(--grade)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void pf05_fog_map_hidden()
{
    perf5::FogMap map(37, 53);
    perf5::reveal(map);
    map.resetJumps();
    const perf5::FogStats s = perf5::countFog(map);
    expect("밝혀진 칸 수", 974LL, s.revealed);
    expect("밝기 합", 249004LL, s.brightness);
    expect("건너뛴 접근이 행 수 이하인가?", true, map.jumps() <= map.rows());
}
#pragma endregion
