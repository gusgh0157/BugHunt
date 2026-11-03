#include "../labs.h"
#include <string>
#include <vector>

// 숫자 키 1~4를 누르면 퀵슬롯 아이템을 사용한다.

// [JA] 数字キー1~4を押すとクイックスロットのアイテムを使います。
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
        return slots_.at(slotIndex);
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

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (편집기의 접기 기능으로 모두 접어 두세요)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(エディターの折りたたみ機能ですべて折りたたんでおいてください)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 호출 스택을 보고 펼치세요 / まず呼び出し履歴を見てから開いてください)
// 디버거는 처리되지 않은 예외(std::out_of_range)에서 멈춥니다.
// 호출 스택 맨 위의 std::vector<...>::at이나 표준 라이브러리, 런타임 줄은 건너뛰고,
// lab2:: 로 시작하는 첫 줄을 찾으세요.

// [JA] デバッガーはハンドルされない例外(std::out_of_range)で止まります。
// [JA] 呼び出し履歴のいちばん上にある std::vector<...>::at や標準ライブラリ、ランタイムの行は飛ばして、
// [JA] lab2:: で始まる最初の行を探してください。
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// itemAt 프레임의 slotIndex, onNumberKey 프레임의 key를 보세요.
// slots_에는 원소가 몇 개 있나요?

// [JA] itemAt フレームの slotIndex と、onNumberKey フレームの key を見てください。
// [JA] slots_ には要素がいくつありますか?
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// 콘솔 출력도 단서입니다. 1번 키를 눌렀는데 무엇이 나왔나요?
// 크래시가 나지 않은 줄들도 사실 틀렸던 겁니다.

// [JA] コンソール出力も手がかりです。1番キーを押したのに何が出ましたか?
// [JA] クラッシュしなかった行も、実は間違っていたのです。
#pragma endregion
