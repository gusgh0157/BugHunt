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
// 힌트: 막힐 때 하나씩만 펼치세요. (편집기의 접기 기능으로 모두 접어 두세요)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(エディターの折りたたみ機能ですべて折りたたんでおいてください)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// 빌드 출력의 경고를 읽어 보세요. 경고는 공짜 디버거입니다.

// [JA] ビルド出力の警告を読んでみてください。警告は無料のデバッガーです。
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// &와 == 중 무엇이 먼저 계산되나요?

// [JA] & と == のどちらが先に計算されますか?
#pragma endregion
