#include "../labs.h"
#include <string>
#include <vector>

namespace
{
struct Item
{
    std::string name;
    int count;
};

class Inventory
{
public:
    // 아이템을 추가하고, 바로 수정할 수 있도록 참조를 돌려준다.
    Item& add(const std::string& name, int count)
    {
        items_.push_back({ name, count });
        return items_.back();
    }

    int countOf(const std::string& name) const
    {
        for (const Item& item : items_)
            if (item.name == name)
                return item.count;
        return 0;
    }

private:
    std::vector<Item> items_;
};
}

void lab02_inventory()
{
    Inventory inv;
    Item& sword = inv.add("Sword of the Northern Watch", 1);
    inv.add("Oak Shield", 1);
    inv.add("Healing Potion", 5);
    inv.add("Iron Arrow", 30);

    // 퀘스트 보상: 검을 하나 더 준다.
    sword.count += 1;

    expect("검 개수", 2, inv.countOf("Sword of the Northern Watch"));
    expect("포션 개수", 5, inv.countOf("Healing Potion"));
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (편집기의 접기 기능으로 모두 접어 두세요)
// ============================================================================

#pragma region 힌트 1 (먼저 직접 추적해 보고 펼치세요)
// sword를 받은 직후 조사식에 &sword와 &inv.items_[0]을 넣고,
// 한 줄씩 실행(step over)으로 add를 하나씩 진행하면서 두 주소를 비교하세요.
#pragma endregion

#pragma region 힌트 2
// std::vector의 capacity()는 언제 바뀌나요? 바뀌면 원소들은 어디로 가나요?
#pragma endregion

#pragma region 힌트 3
// ASan 구성으로 실행해 보세요. 리포트의 "freed by thread T0 here" 스택을 읽어 보세요.
#pragma endregion
