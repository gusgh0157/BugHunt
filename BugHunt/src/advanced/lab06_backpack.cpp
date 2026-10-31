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
