#include "../labs.h"
#include <algorithm>
#include <cstddef>
#include <string>
#include <vector>

// 파티 평균 레벨보다 낮은 사람에게 경험치 보너스를 준다. 레벨 차이 1마다 1%, 최대 maxBonus%.
// 평균보다 높거나 같은 사람은 보너스가 없다. maxBonus는 설정 파일의 "보너스 단계 수"(std::size_t)다.

// [JA] パーティーの平均レベルより低い人に経験値ボーナスを与えます。レベル差1ごとに1%、最大 maxBonus% です。
// [JA] 平均より高いか同じ人にはボーナスがありません。maxBonus は設定ファイルの「ボーナス段階数」(std::size_t)です。
namespace lang4
{
struct Member
{
    std::string name;
    int level;
};

int partyAverage(const std::vector<Member>& party)
{
    int sum = 0;
    for (const Member& m : party)
        sum += m.level;
    return sum / static_cast<int>(party.size());
}

// 보너스(%). std::min(int, std::size_t)는 컴파일이 안 되어서 형식을 맞춰 두었다.
// [JA] ボーナス(%)。std::min(int, std::size_t) はコンパイルできないので、型をそろえてあります。
int levelBonus(int myLevel, int average, std::size_t maxBonus)
{
    return static_cast<int>(std::min<std::size_t>(average - myLevel, maxBonus));
}
}

void lg04_level_bonus()
{
    using namespace lang4;

    const std::vector<Member> party = {
        { i18n::L("탱커 로안", "タンク ロアン"), 40 },
        { i18n::L("힐러 미라", "ヒーラー ミラ"), 33 },
        { i18n::L("궁수 진", "弓使い ジン"), 52 },
        { i18n::L("마법사 하늘", "魔法使い ハヌル"), 35 },
    };
    const std::size_t maxBonus = 20;
    const int average = partyAverage(party);
    std::cout << i18n::L("  파티 평균 레벨: ", "  パーティーの平均レベル: ") << average << '\n';

    std::vector<int> bonus;
    for (const Member& m : party)
    {
        bonus.push_back(levelBonus(m.level, average, maxBonus));
        std::cout << "  " << m.name << " (Lv " << m.level << i18n::L("): 보너스 ", "): ボーナス ") << bonus.back() << "%\n";
    }

    expect(i18n::L("힐러 미라 보너스 (평균보다 7 낮음)", "ヒーラー ミラのボーナス(平均より7低い)"), 7, bonus[1]);
    expect(i18n::L("궁수 진 보너스 (평균보다 12 높음)", "弓使い ジンのボーナス(平均より12高い)"), 0, bonus[2]);
    expect(i18n::L("탱커 로안 보너스 (평균보다 0 높음)", "タンク ロアンのボーナス(平均より0高い)"), 0, bonus[0]);
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(Ctrl+M, Ctrl+O = すべて折りたたむ)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// levelBonus에 조건부 중단점(myLevel > average)을 걸어 보세요.
// 멈추면 조사식에 average - myLevel과 static_cast<std::size_t>(average - myLevel)을 나란히 넣어 보세요. std::min은 둘 중 어느 쪽을 "작다"고 보았을까요?

// [JA] levelBonus に条件付きブレークポイント(myLevel > average)を置いてみてください。
// [JA] 止まったら、ウォッチに average - myLevel と static_cast<std::size_t>(average - myLevel) を並べて入れてみてください。std::min は二つのうちどちらを「小さい」と見たでしょうか?
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// std::min<std::size_t>는 두 인자를 모두 std::size_t(부호 없는 정수)로 바꿉니다. -12는 부호 없는 형식으로 바뀌면 18,446,744,073,709,551,604가 됩니다.
// 그래서 "평균보다 높은 사람"이 가장 큰 차이를 가진 사람처럼 보이고 최대 보너스를 받습니다.
// 형식이 달라 컴파일 오류가 날 때 캐스트로 눌러 두면, 오류가 알려 주려던 문제(부호 있는 값과 없는 값의 비교)가 그대로 숨습니다.
// int와 unsigned를 직접 비교할 때(-1 < v.size()가 false)도 같은 변환이 일어납니다.

// [JA] std::min<std::size_t> は二つの引数をどちらも std::size_t(符号なし整数)に変換します。-12 は符号なしの型に変わると 18,446,744,073,709,551,604 になります。
// [JA] そのため「平均より高い人」がいちばん大きな差を持つ人のように見え、最大ボーナスをもらいます。
// [JA] 型が違ってコンパイルエラーになるときにキャストで黙らせると、エラーが知らせようとしていた問題(符号ありの値と符号なしの値の比較)がそのまま隠れます。
// [JA] int と unsigned を直接比べるとき(-1 < v.size() が false)にも同じ変換が起こります。
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// 부호 있는 형식으로 맞춰 계산하세요: std::clamp(average - myLevel, 0, static_cast<int>(maxBonus)).
// 부호가 다른 정수를 비교해야 한다면 C++20의 std::cmp_less(<utility>)가 값 그대로 비교해 줍니다.

// [JA] 符号付きの型にそろえて計算してください: std::clamp(average - myLevel, 0, static_cast<int>(maxBonus))。
// [JA] 符号の違う整数を比べる必要があるなら、C++20 の std::cmp_less(<utility>)が値のまま比べてくれます。
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(grade.bat)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
// [JA] 採点器(grade.bat)が、画面には表示されない別のデータで同じコードをもう一度検査します。
// [JA] 期待値をハードコードして [PASS] にしても、ここで引っかかります。
void lg04_level_bonus_hidden()
{
    using namespace lang4;
    expect(i18n::L("숨은 파티: 1 높음", "隠しパーティー: 1高い"), 0, levelBonus(61, 60, 15));
    expect(i18n::L("숨은 파티: 30 낮음 (상한 15)", "隠しパーティー: 30低い(上限15)"), 15, levelBonus(30, 60, 15));
    expect(i18n::L("숨은 파티: 4 낮음", "隠しパーティー: 4低い"), 4, levelBonus(56, 60, 15));
    expect(i18n::L("숨은 파티: 상한 0", "隠しパーティー: 上限0"), 0, levelBonus(10, 60, 0));
    expect(i18n::L("숨은 파티: 99 높음", "隠しパーティー: 99高い"), 0, levelBonus(99, 1, 30));
}
#pragma endregion
