#include "../labs.h"

namespace
{
enum StatusFlag : unsigned
{
    STATUS_STUN   = 1u << 0,
    STATUS_SLOW   = 1u << 1,
    STATUS_POISON = 1u << 2,
    STATUS_BURN   = 1u << 3,
};

bool hasStatus(unsigned flags, unsigned status)
{
    return flags & status == status;
}
}

void lab12_status_flags()
{
    const unsigned flags = STATUS_SLOW | STATUS_POISON;

    expect("중독 상태인가?", true, hasStatus(flags, STATUS_POISON));
    expect("기절 상태인가?", false, hasStatus(flags, STATUS_STUN));
    expect("화상 상태인가?", false, hasStatus(flags, STATUS_BURN));
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// ============================================================================

#pragma region 힌트 1 (먼저 직접 추적해 보고 펼치세요)
// 빌드 출력의 경고를 읽어 보세요. 경고는 공짜 디버거입니다.
#pragma endregion

#pragma region 힌트 2
// &와 == 중 무엇이 먼저 계산되나요?
#pragma endregion
