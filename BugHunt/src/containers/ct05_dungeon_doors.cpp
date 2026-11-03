#include "../labs.h"
#include <vector>

// 던전의 문. 스위치를 밟으면 연결된 문이 열리거나 닫힌다(토글).
// 퍼즐 점수: 닫혀 있던 문을 연 횟수만큼 1점.

// [JA] ダンジョンの扉です。スイッチを踏むと、つながった扉が開いたり閉じたりします(トグル)。
// [JA] パズルのスコア: 閉じていた扉を開けた回数だけ1点です。
namespace ct5
{
int pressSwitches(std::vector<bool>& doorsOpen, const std::vector<int>& presses)
{
    int score = 0;
    for (int door : presses)
    {
        auto wasOpen = doorsOpen[door];
        doorsOpen[door] = !doorsOpen[door];
        if (!wasOpen)
            ++score;
    }
    return score;
}
}

void ct05_dungeon_doors()
{
    std::vector<bool> doors = { false, true, false, false };   // 문 1만 열려 있다

    // 문 0 열기, 문 1 닫기, 문 2 열기, 문 0 닫기, 문 3 열기
    const int score = ct5::pressSwitches(doors, { 0, 1, 2, 0, 3 });

    expect("퍼즐 점수 (닫힌 문을 연 횟수)", 3, score);
    expect("문 2가 열려 있나?", true, static_cast<bool>(doors[2]));
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (편집기의 접기 기능으로 모두 접어 두세요)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(エディターの折りたたみ機能ですべて折りたたんでおいてください)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// for 안의 첫 줄에 중단점을 걸고 한 줄씩 실행하며 지역 변수 wasOpen의 타입과 값을 보세요.
// 문을 토글한 다음 줄에서 wasOpen의 값이 바뀌나요? 디버거가 보여 주는 형식(type)도 보세요.

// [JA] for の中の最初の行にブレークポイントを置き、1行ずつ実行しながらローカル変数 wasOpen の型と値を見てください。
// [JA] 扉をトグルした次の行で wasOpen の値は変わりますか? デバッガーが表示する型(type)も見てください。
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// vector<int>라면 auto x = v[i]; 는 값 복사입니다. vector<bool>도 그럴까요?
// vector<bool>은 비트로 압축해 저장해서, operator[]가 bool&를 돌려줄 수 없습니다. 대신 무엇을 돌려줄까요?

// [JA] vector<int> なら auto x = v[i]; は値のコピーです。vector<bool> もそうでしょうか?
// [JA] vector<bool> はビットに圧縮して保存するので、operator[] が bool& を返すことができません。代わりに何を返すのでしょうか?
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(--grade)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void ct05_dungeon_doors_hidden()
{
    std::vector<bool> doors(6, true);
    const int score = ct5::pressSwitches(doors, { 5, 5, 5, 2, 0, 2, 4 });   // 5: 닫기, 열기, 닫기 / 2: 닫기, 열기
    expect("퍼즐 점수", 2, score);
    expect("문 5가 닫혀 있나?", false, static_cast<bool>(doors[5]));
    expect("문 2가 열려 있나?", true, static_cast<bool>(doors[2]));
}
#pragma endregion
