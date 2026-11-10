#include "../labs.h"
#include <string>
#include <utility>
#include <vector>

// 두 사람이 거래 창에 아이템을 올리고 거래를 확정한다.
// trade()는 내 거래 창의 아이템을 상대가 받은 목록으로 "옮긴다". 복사되면 아이템이 늘어나는 복제 버그가 된다.
// Item은 자기가 복사된 횟수를 센다.

// [JA] 二人がトレード画面にアイテムを置いてトレードを確定します。
// [JA] trade() は自分のトレード画面のアイテムを相手の受け取りリストへ「移動」します。コピーされるとアイテムが増える複製バグになります。
// [JA] Item は自分がコピーされた回数を数えます。
namespace lang1
{
int g_itemCopies = 0;

struct Item
{
    std::string name;

    explicit Item(std::string n) : name(std::move(n)) {}
    Item(const Item& other) : name(other.name) { ++g_itemCopies; }
    Item(Item&& other) noexcept = default;
    Item& operator=(const Item& other)
    {
        name = other.name;
        ++g_itemCopies;
        return *this;
    }
    Item& operator=(Item&& other) noexcept = default;
};

struct Trader
{
    std::string name;
    std::vector<Item> offer;      // 거래 창에 올린 아이템 [JA] 取引ウィンドウに載せたアイテム
    std::vector<Item> received;   // 거래로 받은 아이템 [JA] 取引で受け取ったアイテム
};

// from의 거래 창에 있는 아이템을 to에게 옮긴다. 거래가 끝나면 from의 거래 창은 비어야 한다.
// [JA] from の取引ウィンドウにあるアイテムを to に移します。取引が終わったら from の取引ウィンドウは空でなければなりません。
void trade(const Trader& from, Trader& to)
{
    for (const Item& item : from.offer)
        to.received.push_back(std::move(item));   // 복사하지 않고 옮긴다 [JA] コピーせずに移動する
}

void put(Trader& t, std::initializer_list<const char*> names)
{
    for (const char* n : names)
        t.offer.emplace_back(n);
}

void print(const Trader& t)
{
    std::cout << "  " << t.name << i18n::L(" 거래 창:", "のトレード画面:");
    for (const Item& item : t.offer)
        std::cout << " [" << item.name << "]";
    std::cout << i18n::L("  / 받은 것:", "  / 受け取ったもの:");
    for (const Item& item : t.received)
        std::cout << " [" << item.name << "]";
    std::cout << '\n';
}
}

void lg01_trade_items()
{
    using namespace lang1;
    g_itemCopies = 0;

    Trader me{ i18n::L("나", "わたし"), {}, {} };
    Trader buyer{ i18n::L("상인", "商人"), {}, {} };
    put(me, { i18n::L("용의 비늘", "竜の鱗"), i18n::L("엘릭서", "エリクサー"), i18n::L("마법 두루마리", "魔法の巻物") });

    trade(me, buyer);
    print(me);
    print(buyer);

    expect(i18n::L("상인이 받은 아이템 수", "商人が受け取ったアイテム数"), static_cast<size_t>(3), buyer.received.size());
    expect(i18n::L("내 거래 창에 남은 아이템 수", "自分のトレード画面に残ったアイテム数"), static_cast<size_t>(0), me.offer.size());
    expect(i18n::L("거래 중 아이템 복사 횟수", "トレード中のアイテムのコピー回数"), 0, g_itemCopies);
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(Ctrl+M, Ctrl+O = すべて折りたたむ)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// Item의 복사 생성자 안에 중단점을 걸고 실행해 보세요. trade() 안에서 멈추나요?
// 멈추면 호출 스택을 한 칸 올라가 push_back의 어느 오버로드(const Item& / Item&&)가 불렸는지 보세요.
// 거래가 끝난 뒤 조사식에 me.offer를 넣어 보세요. 아이템이 사라졌나요, 이름이 비었나요, 그대로인가요?

// [JA] Item のコピーコンストラクターの中にブレークポイントを置いて実行してみてください。trade() の中で止まりますか?
// [JA] 止まったら呼び出し履歴を一つ上がって、push_back のどのオーバーロード(const Item& / Item&&)が呼ばれたかを見てください。
// [JA] トレードが終わった後、ウォッチに me.offer を入れてみてください。アイテムは消えましたか、名前が空になりましたか、そのままですか?
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// std::move는 아무것도 옮기지 않습니다. "이 값은 옮겨 가도 된다"는 형식 변환(rvalue 참조로 캐스트)일 뿐입니다.
// const Item&에 std::move를 하면 const Item&&가 되고, 이것은 이동 생성자(Item&&)에 맞지 않아 복사 생성자가 조용히 불립니다.
// 그리고 이동이 제대로 되더라도 원소를 옮긴 것이지 vector에서 지운 것이 아닙니다. 옮겨진 원소는 "유효하지만 정해지지 않은 상태"로 남습니다.

// [JA] std::move は何も移動しません。「この値は移動してよい」という型変換(右辺値参照へのキャスト)にすぎません。
// [JA] const Item& に std::move をすると const Item&& になり、これはムーブコンストラクター(Item&&)に合わないので、コピーコンストラクターが黙って呼ばれます。
// [JA] そして、ムーブが正しく行われても要素を移動しただけで vector から消したわけではありません。移動された要素は「有効だが未規定の状態」で残ります。
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// 옮기려면 원본을 바꿀 수 있어야 합니다: trade(Trader& from, Trader& to)로 바꾸고 for (Item& item : from.offer).
// 옮긴 뒤에는 원본의 상태에 기대지 말고 from.offer.clear()로 직접 비우세요. (옮겨진 객체에는 대입하거나 비우는 것만 안전합니다)

// [JA] 移動するには元を変更できなければなりません: trade(Trader& from, Trader& to) に変えて for (Item& item : from.offer) にします。
// [JA] 移動した後は元の状態に頼らず、from.offer.clear() で自分で空にしてください。(移動された後のオブジェクトには、代入するか空にすることだけが安全です)
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(grade.bat)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
// [JA] 採点器(grade.bat)が、画面には表示されない別のデータで同じコードをもう一度検査します。
// [JA] 期待値をハードコードして [PASS] にしても、ここで引っかかります。
void lg01_trade_items_hidden()
{
    using namespace lang1;
    g_itemCopies = 0;

    Trader a{ i18n::L("기사", "騎士"), {}, {} };
    Trader b{ i18n::L("궁수", "弓使い"), {}, {} };
    put(a, { i18n::L("강철 투구", "鋼の兜"), i18n::L("은화 주머니", "銀貨袋") });
    trade(a, b);
    put(a, { i18n::L("불꽃 반지", "炎の指輪"), i18n::L("치유 물약", "治癒の薬"), i18n::L("고대 지도", "古代の地図"), i18n::L("독 화살", "毒矢") });
    trade(a, b);
    put(b, { i18n::L("깃털 펜", "羽ペン") });
    trade(b, a);

    expect(i18n::L("숨은 거래: 궁수가 받은 아이템 수", "隠しトレード: 弓使いが受け取ったアイテム数"), static_cast<size_t>(6), b.received.size());
    expect(i18n::L("숨은 거래: 기사가 받은 아이템 수", "隠しトレード: 騎士が受け取ったアイテム数"), static_cast<size_t>(1), a.received.size());
    expect(i18n::L("숨은 거래: 기사 거래 창", "隠しトレード: 騎士のトレード画面"), static_cast<size_t>(0), a.offer.size());
    expect(i18n::L("숨은 거래: 궁수 거래 창", "隠しトレード: 弓使いのトレード画面"), static_cast<size_t>(0), b.offer.size());
    expect(i18n::L("숨은 거래: 마지막으로 받은 아이템", "隠しトレード: 最後に受け取ったアイテム"), std::string(i18n::L("독 화살", "毒矢")), b.received.back().name);
    expect(i18n::L("숨은 거래: 복사 횟수", "隠しトレード: コピー回数"), 0, g_itemCopies);
}
#pragma endregion
