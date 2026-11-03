#include "../labs.h"
#include "lg06_save_stats.h"

// 체크포인트를 지날 때마다 자동 저장하고, 저장 화면에서 "저장 횟수"와 "마지막 슬롯"을 보여 준다.
// 통계는 헤더(lg06_save_stats.h)의 g_saveStats 하나로 관리한다.

// [JA] チェックポイントを通るたびにオートセーブし、セーブ画面で「セーブ回数」と「最後のスロット」を表示します。
// [JA] 統計はヘッダー(lg06_save_stats.h)の g_saveStats 一つで管理します。
namespace lang6
{
void passCheckpoint(const std::string& name)
{
    std::cout << "  체크포인트: " << name << " → 자동 저장\n";
    autosave("auto-" + name);
}

void showSaveScreen()
{
    std::cout << "  [저장 화면] 저장 횟수 " << g_saveStats.count
              << ", 마지막 슬롯 \"" << g_saveStats.lastSlot << "\"\n";
}
}

void lg06_save_screen()
{
    using namespace lang6;

    passCheckpoint("숲 입구");
    passCheckpoint("폐광");
    passCheckpoint("용의 둥지");
    showSaveScreen();

    expect("저장 횟수", 3, g_saveStats.count);
    expect("마지막 슬롯", std::string("auto-용의 둥지"), g_saveStats.lastSlot);
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(Ctrl+M, Ctrl+O = すべて折りたたむ)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// lg06_autosave.cpp의 ++g_saveStats.count 줄에서 멈추고 조사식에 &g_saveStats를 넣으세요.
// 다음에 showSaveScreen에서 멈췄을 때도 &g_saveStats를 보세요. 두 주소가 같나요?

// [JA] lg06_autosave.cpp の ++g_saveStats.count の行で止めて、ウォッチに &g_saveStats を入れてください。
// [JA] 次に showSaveScreen で止まったときも &g_saveStats を見てください。二つのアドレスは同じですか?
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// 네임스페이스 범위의 static 변수는 "이 번역 단위 안에서만 보인다"(내부 링크)는 뜻입니다.
// 헤더에 static 변수를 정의하면, 그 헤더를 포함한 .cpp마다 이름만 같은 별개의 변수가 하나씩 생깁니다. 자동 저장은 자기 복사본을 올리고, 저장 화면은 0인 자기 복사본을 봅니다.
// 컴파일러도 링커도 경고하지 않습니다. 각 파일에서 보면 모두 올바른 코드이기 때문입니다.

// [JA] 名前空間スコープの static 変数は「この翻訳単位の中でだけ見える」(内部リンケージ)という意味です。
// [JA] ヘッダーに static 変数を定義すると、そのヘッダーをインクルードした .cpp ごとに、名前だけが同じ別々の変数が一つずつできます。オートセーブは自分のコピーを増やし、セーブ画面は 0 のままの自分のコピーを見ます。
// [JA] コンパイラーもリンカーも警告しません。それぞれのファイルから見ると、どれも正しいコードだからです。
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// 프로그램 전체에서 하나여야 하는 변수는 C++17의 inline 변수로 헤더에 두세요: inline SaveStats g_saveStats;
// 또는 헤더에는 extern SaveStats g_saveStats; 선언만 두고, .cpp 한 곳에서 정의합니다.
// (헤더의 const/constexpr 변수도 기본으로 내부 링크입니다. 값만 쓰는 상수라면 문제없지만 주소를 비교하면 다릅니다)

// [JA] プログラム全体で一つでなければならない変数は、C++17 の inline 変数としてヘッダーに置いてください: inline SaveStats g_saveStats;
// [JA] または、ヘッダーには extern SaveStats g_saveStats; の宣言だけを置き、.cpp の一か所で定義します。
// [JA] (ヘッダーの const/constexpr 変数も既定で内部リンケージです。値だけを使う定数なら問題ありませんが、アドレスを比べると異なります)
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(grade.bat)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void lg06_save_screen_hidden()
{
    using namespace lang6;
    const int before = g_saveStats.count;
    for (const char* place : { "항구", "등대", "난파선", "인어의 동굴", "해적 요새" })
        autosave(std::string("auto-") + place);
    expect("숨은 저장: 늘어난 횟수", 5, g_saveStats.count - before);
    expect("숨은 저장: 마지막 슬롯", std::string("auto-해적 요새"), g_saveStats.lastSlot);
}
#pragma endregion
