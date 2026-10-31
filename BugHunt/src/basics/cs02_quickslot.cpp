#include "../labs.h"
#include <string>
#include <vector>

// 숫자 키 1~4를 누르면 퀵슬롯 아이템을 사용한다.
namespace lab2
{
class QuickSlotBar
{
public:
    QuickSlotBar()
        : slots_{ "Healing Potion", "Mana Potion", "Fire Scroll", "Return Scroll" }
    {
    }

    const std::string& itemAt(int slotIndex) const
    {
        return slots_[slotIndex];
    }

private:
    std::vector<std::string> slots_;
};

void useItem(const std::string& item)
{
    std::cout << "  사용: " << item << '\n';
}

void onNumberKey(const QuickSlotBar& bar, int key)
{
    useItem(bar.itemAt(key));
}

// 녹화해 둔 입력을 재생한다: 1번, 3번, 2번, 4번 키를 차례로 눌렀다.
void replayInput()
{
    QuickSlotBar bar;
    const int pressedKeys[] = { 1, 3, 2, 4 };
    for (int key : pressedKeys)
        onNumberKey(bar, key);
}
}

void cs02_quickslot()
{
    std::cout << "  (기대 출력: Healing Potion, Fire Scroll, Mana Potion, Return Scroll)\n";
    lab2::replayInput();
}
