#include "../labs.h"
#include <string>
#include <vector>

// 대장장이에게 장비를 수리받는다. 수리비는 잃은 내구도 1당 2골드.

// [JA] 鍛冶屋に装備を修理してもらいます。修理費は失った耐久度1につき2ゴールドです。
namespace flow6
{
struct Equipment
{
    std::string name;
    int durability;
    int maxDurability;
};

class Backpack
{
public:
    Backpack()
        : items_{ { "Iron Sword", 37, 100 }, { "Leather Armor", 60, 80 }, { "Torch", 10, 10 } }
    {
    }

    Equipment& get(const std::string& name)
    {
        for (Equipment& item : items_)
        {
            if (item.name == name)
                return item;
        }
        return items_.back();
    }

    void print() const
    {
        for (const Equipment& item : items_)
            std::cout << "    " << item.name << " " << item.durability << "/" << item.maxDurability << '\n';
    }

private:
    std::vector<Equipment> items_;
};

int repairCost(const Equipment& item)
{
    return (item.maxDurability - item.durability) * 2;
}

void repair(Equipment& item)
{
    item.durability = item.maxDurability;
}

void visitBlacksmith(Backpack& backpack, int& gold, const std::string& itemName)
{
    auto item = backpack.get(itemName);
    const int cost = repairCost(item);
    if (cost == 0 || gold < cost)
        return;

    gold -= cost;
    repair(item);
    std::cout << i18n::L("  수리 완료: ", "  修理完了: ") << item.name << " " << item.durability << "/" << item.maxDurability
              << " (-" << cost << i18n::L(" 골드)\n", " ゴールド)\n");
}
}

void ft06_repair()
{
    flow6::Backpack backpack;
    int gold = 500;

    flow6::visitBlacksmith(backpack, gold, "Iron Sword");

    std::cout << i18n::L("  배낭:\n", "  バックパック:\n");
    backpack.print();
    expect(i18n::L("남은 골드", "残りのゴールド"), 374, gold);
    expect(i18n::L("배낭 속 Iron Sword 내구도", "バックパック内の Iron Sword の耐久度"), 100, backpack.get("Iron Sword").durability);
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (편집기의 접기 기능으로 모두 접어 두세요)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(エディターの折りたたみ機能ですべて折りたたんでおいてください)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// "수리 완료" 메시지는 100/100이라고 합니다. 그런데 배낭에는 37입니다. 둘은 같은 칼일까요?
// visitBlacksmith의 repair(item); 줄에서 멈추고, 조사식에 넣어 보세요:
//     &item
//     &backpack.get("Iron Sword")

// [JA] 「修理完了」のメッセージは 100/100 だと言っています。ところがバックパックの中は37です。二つは同じ剣でしょうか?
// [JA] visitBlacksmith の repair(item); の行で止めて、ウォッチに入れてみてください:
// [JA]     &item
// [JA]     &backpack.get("Iron Sword")
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// 주소가 다르다면 item은 언제 만들어졌나요? get()은 무엇을 돌려주고, 받는 쪽은 무엇으로 받았나요?
// (Visual Studio라면 "개체 ID 만들기"로 $1 을 붙여 추적할 수도 있습니다.)

// [JA] アドレスが違うなら、item はいつ作られたのでしょうか? get() は何を返し、受け取る側は何で受け取りましたか?
// [JA] (Visual Studio なら「オブジェクト ID の作成」で $1 を付けて追跡することもできます。)
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(--grade)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
// [JA] 採点器(--grade)が、画面には表示されない別のデータで同じコードをもう一度検査します。
// [JA] 期待値をハードコードして [PASS] にしても、ここで引っかかります。
void ft06_repair_hidden()
{
    flow6::Backpack backpack;

    int gold = 100;
    flow6::visitBlacksmith(backpack, gold, "Leather Armor");
    expect(i18n::L("갑옷 수리 후 골드", "鎧の修理後のゴールド"), 60, gold);
    expect(i18n::L("배낭 속 Leather Armor 내구도", "バックパック内の Leather Armor の耐久度"), 80, backpack.get("Leather Armor").durability);

    int poorGold = 10;               // 골드가 모자라면 아무것도 바뀌지 않는다 [JA] ゴールドが足りなければ何も変わらない
    flow6::visitBlacksmith(backpack, poorGold, "Iron Sword");
    expect(i18n::L("골드 부족: 골드", "ゴールド不足: ゴールド"), 10, poorGold);
    expect(i18n::L("골드 부족: Iron Sword 내구도", "ゴールド不足: Iron Sword の耐久度"), 37, backpack.get("Iron Sword").durability);
}
#pragma endregion
