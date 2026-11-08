#include "../labs.h"
#include "perf_kit.h"
#include <string>
#include <vector>

// 장비 무게 표시. HUD는 매 프레임 "현재 무게 / 최대 무게"와 가장 무거운 장비를 보여 준다.
// 장비가 많은 플레이어일수록 프레임이 떨어진다는 제보가 있다. 숫자는 맞다.

// [JA] 装備の重さ表示です。HUD は毎フレーム「現在の重さ / 最大の重さ」といちばん重い装備を表示します。
// [JA] 装備の多いプレイヤーほどフレームレートが落ちるという報告があります。数字は正しいです。
namespace perf2
{
long long g_itemCopies = 0;

struct Item
{
    std::string name;
    std::string description;   // 툴팁에 쓰는 긴 설명
    int weight = 0;

    Item(std::string n, int w) : name(std::move(n)), description(std::string(200, '.')), weight(w) {}
    Item(const Item& other) : name(other.name), description(other.description), weight(other.weight) { ++g_itemCopies; }
    Item& operator=(const Item& other)
    {
        name = other.name;
        description = other.description;
        weight = other.weight;
        ++g_itemCopies;
        return *this;
    }
    Item(Item&&) noexcept = default;
    Item& operator=(Item&&) noexcept = default;
};

int totalWeight(std::vector<Item> items)
{
    int total = 0;
    for (Item item : items)
        total += item.weight;
    return total;
}

std::string heaviest(const std::vector<Item>& items)
{
    std::string name;
    int best = -1;
    for (const Item& item : items)
    {
        if (item.weight > best)
        {
            best = item.weight;
            name = item.name;
        }
    }
    return name;
}

struct HudResult
{
    int weight;
    std::string heaviest;
};

HudResult drawHud(const std::vector<Item>& inventory, int frames)
{
    HudResult r{};
    for (int f = 0; f < frames; ++f)
        r = { totalWeight(inventory), heaviest(inventory) };
    return r;
}

std::vector<Item> makeInventory(int count)
{
    std::vector<Item> items;
    items.reserve(count);
    for (int i = 0; i < count; ++i)
        items.emplace_back("Item" + std::to_string(i), 1 + (i * 7) % 23);
    return items;
}
}

void pf02_loadout()
{
    const std::vector<perf2::Item> inventory = perf2::makeInventory(120);

    perf2::g_itemCopies = 0;
    const perf::Stopwatch sw;
    const perf2::HudResult hud = perf2::drawHud(inventory, 600);
    std::cout << i18n::L("  HUD 600프레임: ", "  HUD 600フレーム: ") << sw.ms() << i18n::L(" ms, Item 복사 ", " ms、Item のコピー ") << perf2::g_itemCopies
              << i18n::L("번\n", "回\n");

    expect("무게 합", 1432, hud.weight);
    expect("가장 무거운 장비", std::string("Item13"), hud.heaviest);
    expect("HUD를 그리는 동안 Item 복사 횟수", 0LL, perf2::g_itemCopies);
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (편집기의 접기 기능으로 모두 접어 두세요)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(エディターの折りたたみ機能ですべて折りたたんでおいてください)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// Release + 프로파일러(CPU 샘플링). 핫 경로에 Item의 복사 생성자나 문자열 복사, 메모리 할당이 보이나요?
// 그 복사는 어느 함수에서 일어나나요?

// [JA] Release + プロファイラー(CPU サンプリング)。ホットパスに Item のコピーコンストラクターや文字列のコピー、メモリ確保が見えますか?
// [JA] そのコピーはどの関数で起きていますか?
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// Item의 복사 생성자에 중단점을 걸고 호출 스택을 보세요. 복사를 부른 줄은 어디인가요? 한 곳만이 아닐 수 있습니다.

// [JA] Item のコピーコンストラクターにブレークポイントを置いて呼び出し履歴を見てください。コピーを呼んだ行はどこですか? 1か所だけとは限りません。
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(--grade)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void pf02_loadout_hidden()
{
    const std::vector<perf2::Item> inventory = perf2::makeInventory(37);
    perf2::g_itemCopies = 0;
    expect("무게 합", 444, perf2::totalWeight(inventory));
    expect("HUD 무게", 444, perf2::drawHud(inventory, 50).weight);
    expect("Item 복사 횟수", 0LL, perf2::g_itemCopies);
}
#pragma endregion
