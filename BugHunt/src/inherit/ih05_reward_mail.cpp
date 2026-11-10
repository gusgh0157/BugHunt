#include "../labs.h"
#include <vector>

// 우편함으로 보상을 보낸다. give(골드)와 give(아이템) 두 가지가 있다.
// 이벤트 우편함은 아이템을 받을 때 이벤트 카운터만 하나 더 올린다.
// 이벤트 보상으로 골드 500을 보냈는데, 골드는 그대로고 500번 아이템이 들어왔다.

// [JA] メールボックスで報酬を送ります。give(ゴールド) と give(アイテム) の2種類があります。
// [JA] イベント用メールボックスは、アイテムを受け取るときにイベントカウンターを1つ増やすだけです。
// [JA] イベント報酬としてゴールド500を送ったのに、ゴールドはそのままで500番のアイテムが届きました。
namespace inh5
{
struct Item
{
    int id = 0;
    Item(int itemId) : id(itemId) {}       // 아이템 번호로 만든다 [JA] アイテム番号から作る
};

class Mailbox
{
public:
    virtual ~Mailbox() = default;

    void give(int gold) { gold_ += gold; }
    virtual void give(const Item& item) { items_.push_back(item.id); }

    int gold() const { return gold_; }
    const std::vector<int>& items() const { return items_; }

private:
    int gold_ = 0;
    std::vector<int> items_;
};

class EventMailbox : public Mailbox
{
public:
    void give(const Item& item) override
    {
        Mailbox::give(item);
        ++eventItems_;
    }

    int eventItems() const { return eventItems_; }

private:
    int eventItems_ = 0;
};
}

void ih05_reward_mail()
{
    using namespace inh5;

    EventMailbox box;
    box.give(Item(1203));      // 이벤트 한정 망토 [JA] イベント限定のマント
    box.give(500);             // 출석 보상 골드 [JA] 出席報酬のゴールド

    std::cout << i18n::L("  골드 ", "  ゴールド") << box.gold() << i18n::L(", 아이템 ", "、アイテム") << box.items().size() << i18n::L("개, 이벤트 아이템 ", "個、イベントアイテム")
              << box.eventItems() << i18n::L("개\n", "個\n");

    expect(i18n::L("받은 골드", "受け取ったゴールド"), 500, box.gold());
    expect(i18n::L("받은 아이템 수", "受け取ったアイテム数"), static_cast<size_t>(1), box.items().size());
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(Ctrl+M, Ctrl+O = すべて折りたたむ)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// box.give(500); 줄에 중단점을 걸고 한 단계씩 들어가 보세요. 처음 들어가는 함수는 무엇인가요?
// give(int)로 들어가나요, 아니면 Item 생성자로 들어가나요?

// [JA] box.give(500); の行にブレークポイントを置き、ステップインしてみてください。最初に入る関数は何ですか?
// [JA] give(int) に入りますか、それとも Item のコンストラクターに入りますか?
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// 파생 클래스에 give라는 이름을 하나라도 선언하면, 기반 클래스의 give 오버로드들은 전부 가려집니다(이름 숨김).
// EventMailbox에서 보이는 give는 give(const Item&) 하나뿐이고, 500은 Item(int) 생성자로 조용히 변환됩니다.

// [JA] 派生クラスで give という名前を1つでも宣言すると、基底クラスの give のオーバーロードはすべて隠されます(名前の隠蔽)。
// [JA] EventMailbox から見える give は give(const Item&) だけで、500 は Item(int) コンストラクターで黙って変換されます。
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// 파생 클래스에 using Mailbox::give; 를 넣어 기반의 오버로드를 다시 보이게 하세요.
// 숫자 하나로 만드는 생성자에는 explicit을 붙이세요. 그러면 이런 변환은 컴파일 오류가 됩니다.

// [JA] 派生クラスに using Mailbox::give; を入れて、基底のオーバーロードを再び見えるようにしてください。
// [JA] 数値1つから作るコンストラクターには explicit を付けてください。そうすればこの種の変換はコンパイルエラーになります。
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(grade.bat)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
// [JA] 採点器(grade.bat)が、画面には表示されない別のデータで同じコードをもう一度検査します。
// [JA] 期待値をハードコードして [PASS] にしても、ここで引っかかります。
void ih05_reward_mail_hidden()
{
    using namespace inh5;

    EventMailbox box;
    box.give(75);
    box.give(Item(42));
    box.give(1000);
    box.give(Item(7));
    expect(i18n::L("숨은 우편함: 골드", "隠しメールボックス: ゴールド"), 1075, box.gold());
    expect(i18n::L("숨은 우편함: 아이템 수", "隠しメールボックス: アイテム数"), static_cast<size_t>(2), box.items().size());
    expect(i18n::L("숨은 우편함: 이벤트 아이템 수", "隠しメールボックス: イベントアイテム数"), 2, box.eventItems());
}
#pragma endregion
