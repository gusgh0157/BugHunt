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
