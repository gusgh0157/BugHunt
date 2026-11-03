#include "../labs.h"
#include <string>

// 불꽃놀이 카운트다운. 2초 간격으로 숫자를 센다.
namespace lab5
{
std::string countdown(int n)
{
    if (n == 0)
        return "발사!";
    return std::to_string(n) + " " + countdown(n - 2);
}

void startFireworks(int seconds)
{
    const std::string text = countdown(seconds);
    std::cout << "  " << text << '\n';
}

void fireworksShow()
{
    startFireworks(10);
    startFireworks(6);
    startFireworks(5);
    startFireworks(8);
}
}

void cs05_fireworks()
{
    lab5::fireworksShow();
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (편집기의 접기 기능으로 모두 접어 두세요)
// ============================================================================

#pragma region 힌트 1 (먼저 호출 스택을 보고 펼치세요)
// 호출 스택이 lab5::countdown으로 가득 차 있습니다.
// 맨 위 몇 줄에서 매개변수 n의 값을 읽어 보세요.
#pragma endregion

#pragma region 힌트 2
// n이 홀수인가요, 짝수인가요? 그럼 처음에 어떤 숫자로 시작했을까요?
// (스택이 너무 깊으면 디버거가 일부만 보여 줄 수 있습니다. 그럴 땐 값으로 추리하세요.)
#pragma endregion

#pragma region 힌트 3
// 2씩 빼다 보면 n == 0을 영원히 건너뛰는 경우가 있습니다.
#pragma endregion
