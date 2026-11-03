#include "../labs.h"

namespace
{
struct Backpack
{
    unsigned maxSlots = 20;
    unsigned usedSlots = 0;
};

void loadSaveData(Backpack& bag)
{
    bag.usedSlots = 18;
}

// 저주받은 아이템은 장착 중인 동안 배낭 슬롯을 추가로 차지한다.
void applyCursedItem(Backpack& bag)
{
    bag.usedSlots += 4;
}

bool canPickUp(const Backpack& bag, unsigned itemSize)
{
    const unsigned freeSlots = bag.maxSlots - bag.usedSlots;
    return freeSlots >= itemSize;
}
}

void lab06_backpack()
{
    Backpack bag;
    loadSaveData(bag);
    applyCursedItem(bag);

    expect("크기 3짜리 아이템을 주울 수 있나?", false, canPickUp(bag, 3));
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (편집기의 접기 기능으로 모두 접어 두세요)
// ============================================================================

#pragma region 힌트 1 (먼저 직접 추적해 보고 펼치세요)
// canPickUp 안에서 freeSlots 값을 조사식으로 보세요.
#pragma endregion

#pragma region 힌트 2
// unsigned에서 20 - 22 는?
#pragma endregion
