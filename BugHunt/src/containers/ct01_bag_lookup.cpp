#include "../labs.h"
#include <map>
#include <string>
#include <vector>

// 가방. 아이템 이름 → 개수. 가방 창에는 "아이템 종류 수 / 최대 종류 수"가 표시된다.
// 상점 창을 열면 진열된 물건마다 "가방에 몇 개 있는지"를 옆에 보여 준다.

// [JA] バッグです。アイテム名 → 個数。バッグ画面には「アイテムの種類数 / 最大種類数」が表示されます。
// [JA] ショップ画面を開くと、並んでいる品物ごとに「バッグに何個あるか」を横に表示します。
namespace ct1
{
constexpr size_t kMaxKinds = 5;

using Bag = std::map<std::string, int>;

int countOf(Bag& bag, const std::string& item)
{
    return bag[item];
}

void openShop(Bag& bag, const std::vector<std::string>& shelf)
{
    for (const std::string& item : shelf)
        std::cout << i18n::L("  상점: ", "  ショップ: ") << item << i18n::L(" (보유 ", " (所持 ") << countOf(bag, item) << ")\n";
}

bool canLoot(const Bag& bag, const std::string& item)
{
    return bag.count(item) > 0 || bag.size() < kMaxKinds;
}
}

void ct01_bag_lookup()
{
    ct1::Bag bag = { { "Healing Potion", 3 }, { "Iron Arrow", 40 }, { "Bread", 2 } };

    ct1::openShop(bag, { "Healing Potion", "Mana Potion", "Fire Scroll", "Rope" });

    std::cout << i18n::L("  가방 종류 수: ", "  バッグの種類数: ") << bag.size() << " / " << ct1::kMaxKinds << '\n';
    expect("가방 종류 수", static_cast<size_t>(3), bag.size());
    expect("새 아이템 'Dragon Scale'을 주울 수 있나?", true, ct1::canLoot(bag, "Dragon Scale"));
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (편집기의 접기 기능으로 모두 접어 두세요)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(エディターの折りたたみ機能ですべて折りたたんでおいてください)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// openShop 호출 줄에 중단점을 걸고 지역 변수에서 bag을 펼쳐 두세요. 한 줄씩 실행(step over)으로 openShop을 넘긴 뒤 다시 보세요.
// 상점 창을 열었을 뿐인데 가방 안이 바뀌었나요?

// [JA] openShop の呼び出し行にブレークポイントを置き、ローカル変数で bag を展開しておいてください。ステップオーバー(step over)で openShop を越えたあと、もう一度見てください。
// [JA] ショップ画面を開いただけなのに、バッグの中身が変わりましたか?
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// countOf에 들어가(step into) bag[item] 줄을 지나기 전과 후의 bag.size()를 조사식으로 비교해 보세요.
// "Mana Potion"은 원래 가방에 없었습니다. operator[]는 없는 키를 만나면 무엇을 하나요?

// [JA] countOf にステップイン(step into)し、bag[item] の行を通る前と後の bag.size() をウォッチで比べてみてください。
// [JA] "Mana Potion" はもともとバッグにありませんでした。operator[] は存在しないキーに出会うと何をするでしょうか?
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// 읽기만 하는 함수라면 Bag& 대신 const Bag&를 받게 바꿔 보세요. 컴파일러가 무엇을 막아 주나요?

// [JA] 読むだけの関数なら、Bag& の代わりに const Bag& を受け取るように変えてみてください。コンパイラーは何を防いでくれるでしょうか?
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(--grade)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void ct01_bag_lookup_hidden()
{
    ct1::Bag bag = { { "Rope", 1 } };
    for (const char* item : { "Rope", "Torch", "Map", "Torch", "Compass", "Lantern" })
        ct1::countOf(bag, item);
    expect("조회 후 가방 종류 수", static_cast<size_t>(1), bag.size());
    expect("Rope 개수", 1, ct1::countOf(bag, "Rope"));
    expect("없는 아이템 개수", 0, ct1::countOf(bag, "Torch"));
    expect("조회 뒤에도 가방 종류 수 그대로", static_cast<size_t>(1), bag.size());
}
#pragma endregion
