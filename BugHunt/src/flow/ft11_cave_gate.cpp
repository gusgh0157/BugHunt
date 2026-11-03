#include "../labs.h"
#include <string>
#include <vector>

// 어둠의 동굴은 열쇠와 횃불을 "둘 다" 가진 사람만 들어갈 수 있다.
// 개수는 상관없고, 하나 이상씩만 있으면 된다.
namespace flow11
{
struct Adventurer
{
    std::string name;
    int keys = 0;
    int torches = 0;
};

bool canEnterCave(const Adventurer& a)
{
    return a.keys & a.torches;
}
}

void ft11_cave_gate()
{
    using namespace flow11;

    const std::vector<Adventurer> party = {
        { "아린", 1, 1 },
        { "보람", 2, 1 },     // 열쇠 2개, 횃불 1개
        { "도윤", 0, 3 },     // 열쇠가 없다
    };

    int entered = 0;
    for (const Adventurer& a : party)
    {
        const bool ok = canEnterCave(a);
        std::cout << "  " << a.name << " (열쇠 " << a.keys << ", 횃불 " << a.torches << ") → "
                  << (ok ? "입장" : "입장 불가") << '\n';
        if (ok)
            ++entered;
    }

    expect("보람은 들어갈 수 있는가?", true, canEnterCave(party[1]));
    expect("들어간 인원", 2, entered);
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// ============================================================================

#pragma region 힌트 1 (먼저 직접 추적해 보고 펼치세요)
// canEnterCave에 조건부 중단점을 거세요. 조건: a.keys > 0 && a.torches > 0
// 둘 다 가진 사람에서만 멈춥니다. 멈추면 조사식에 a.keys & a.torches 를 넣어 값을 보세요.
#pragma endregion

#pragma region 힌트 2
// 조사식에 2 & 1, 3 & 1, 2 && 1 을 각각 넣어 보세요. 2와 1을 2진수로 써 보면 왜 그런지 보입니다.
#pragma endregion

#pragma region 힌트 3
// &는 비트끼리 AND, &&는 "참/거짓"끼리 AND입니다. 개수(정수)를 참/거짓으로 쓰려면
// 무엇과 비교해야 하는지 써 주세요. 랩 20(상태 플래그)의 &와 비교해 보세요.
#pragma endregion

#pragma region 채점용 숨은 테스트 (풀이에는 필요 없습니다. 고치지 마세요)
// 채점기(grade.bat)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void ft11_cave_gate_hidden()
{
    using namespace flow11;

    expect("열쇠 4, 횃불 2", true, canEnterCave({ "가", 4, 2 }));
    expect("열쇠 1, 횃불 6", true, canEnterCave({ "나", 1, 6 }));
    expect("열쇠 3, 횃불 3", true, canEnterCave({ "다", 3, 3 }));
    expect("열쇠 5, 횃불 0", false, canEnterCave({ "라", 5, 0 }));
    expect("열쇠 0, 횃불 0", false, canEnterCave({ "마", 0, 0 }));
}
#pragma endregion
