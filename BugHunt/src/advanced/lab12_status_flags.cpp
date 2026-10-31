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
