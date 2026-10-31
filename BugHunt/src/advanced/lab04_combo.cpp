#include "../labs.h"

namespace
{
class ComboCounter
{
public:
    void hit()
    {
        ++combo_;
        if (combo_ > best_)
            best_ = combo_;
    }

    void miss() { combo_ = 0; }

    int best() const { return best_; }

private:
    int combo_;
    int best_ = 0;
};
}

void lab04_combo()
{
    // H = 명중, M = 빗나감
    const char* log = "HHHHMHHMH";

    ComboCounter counter;
    for (const char* p = log; *p != '\0'; ++p)
    {
        if (*p == 'H')
            counter.hit();
        else
            counter.miss();
    }

    expect("최고 콤보", 4, counter.best());
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// ============================================================================

#pragma region 힌트 1 (먼저 직접 추적해 보고 펼치세요)
// ComboCounter counter; 다음 줄에 중단점을 걸고 지역 창에서 멤버 값을 보세요.
#pragma endregion

#pragma region 힌트 2
// -858993460을 16진수로 보면? README의 매직 넘버 표를 보세요.
#pragma endregion
