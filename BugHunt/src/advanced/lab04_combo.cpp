#include "../labs.h"
#include <new>

namespace
{
class ComboCounter
{
public:
    void hit()
    {
        ++combo_;
        if (combo_ > best_)
            best_ = combo_;
    }

    void miss() { combo_ = 0; }

    int best() const { return best_; }

private:
    int combo_;
    int best_ = 0;
};
}

void lab04_combo()
{
    // H = 명중, M = 빗나감
    // [JA] H = 命中、M = 外れ
    const char* log = "HHHHMHHMH";

    // 판이 끝날 때마다 새로 할당하지 않도록, 전 판에 쓰던 메모리 자리에 카운터를 다시 만든다. (오브젝트 풀)
    // [JA] 試合が終わるたびに新しく割り当てないよう、前の試合で使っていたメモリの場所にカウンターを作り直します。(オブジェクトプール)
    ComboCounter* counter = new (bughunt::recycledStorage(sizeof(ComboCounter))) ComboCounter;
    for (const char* p = log; *p != '\0'; ++p)
    {
        if (*p == 'H')
            counter->hit();
        else
            counter->miss();
    }

    expect(i18n::L("최고 콤보", "最高コンボ"), 4, counter->best());
    counter->~ComboCounter();
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (편집기의 접기 기능으로 모두 접어 두세요)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(エディターの折りたたみ機能ですべて折りたたんでおいてください)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// 카운터를 만든 다음 줄에 중단점을 걸고, 조사식(watch)에 *counter를 넣어 멤버 값을 보세요.

// [JA] カウンターを作った次の行にブレークポイントを置き、ウォッチ(watch)に *counter を入れてメンバーの値を見てください。
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// 그 이상한 값을 16진수로 보면? 앞 사용자가 남긴 값(0xA5A5A5A5)입니다.
// 생성자가 값을 정해 주지 않은 멤버는 그 자리에 있던 값을 그대로 가집니다. (표준 용어: 기본 초기화, 미정 값)

// [JA] その変な値を16進数で見ると? 前の使用者が残した値(0xA5A5A5A5)です。
// [JA] コンストラクターが値を決めなかったメンバーは、その場所にあった値をそのまま持ちます。(標準の用語: デフォルト初期化、不定値)
#pragma endregion
