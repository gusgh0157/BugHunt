#include "../labs.h"

// 스킬 쿨다운. 게임은 0.1초마다 한 번씩 틱을 돌린다.
// 쿨다운이 끝날 때까지 몇 틱이 걸리는지 미리 계산해 UI의 원형 게이지를 그린다.

// [JA] スキルのクールダウンです。ゲームは0.1秒ごとに1回ティックを回します。
// [JA] クールダウンが終わるまで何ティックかかるかを前もって計算し、UI の円形ゲージを描きます。
namespace hang1
{
int ticksUntilReady(float cooldownSeconds, float tickSeconds)
{
    int ticks = 0;
    for (float t = 0.0f; t != cooldownSeconds; t += tickSeconds)
        ++ticks;
    return ticks;
}
}

void hg01_cooldown()
{
    int dash = 0;
    int fireball = 0;

    runWithWatchdog("쿨다운 계산", [&] {
        dash = hang1::ticksUntilReady(0.5f, 0.1f);
        std::cout << "  대시: " << dash << "틱\n";
        fireball = hang1::ticksUntilReady(1.5f, 0.1f);
        std::cout << "  파이어볼: " << fireball << "틱\n";
    });

    expect("대시 (0.5초 / 0.1초)", 5, dash);
    expect("파이어볼 (1.5초 / 0.1초)", 15, fireball);
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (편집기의 접기 기능으로 모두 접어 두세요)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(エディターの折りたたみ機能ですべて折りたたんでおいてください)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (README의 "따라 해 보기"를 먼저 하세요 / 先に README の"やってみよう"をやってください)
// 디버거로 실행하고 콘솔에 "대시: 5틱"이 나온 뒤 멈추면 모두 중단(break all).
// 스레드 목록에서 lab-worker(랩 작업 스레드)로 바꾸고, 호출 스택에서 내 코드 줄을 찾으세요.

// [JA] デバッガーで実行し、コンソールに「대시: 5틱」が出たあとで止まったら、すべて中断(break all)してください。
// [JA] スレッド ウィンドウで lab-worker(ラボの作業スレッド)に切り替え、呼び出し履歴で自分のコードの行を探してください。
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// 지역 변수에서 t와 cooldownSeconds를 비교해 보세요. t는 이미 cooldownSeconds를 지나쳤나요?
// ++ticks; 줄에 중단점을 걸고 조건 ticks == 15 를 넣은 뒤 다시 실행하면, 반복이 끝났어야 할 순간의 t를 볼 수 있습니다.

// [JA] ローカル変数で t と cooldownSeconds を比べてみてください。t はすでに cooldownSeconds を通り過ぎていますか?
// [JA] ++ticks; の行にブレークポイントを置き、条件 ticks == 15 を入れてから再実行すると、ループが終わっているはずの瞬間の t を見られます。
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// 0.1은 2진수로 정확히 나타낼 수 없습니다. 0.1f를 15번 더한 값이 정확히 1.5f일까요?
// 실수를 == 나 != 로 비교해서 반복을 끝내지 마세요. 몇 번 반복할지 정수로 셀 수 있지 않나요?

// [JA] 0.1 は2進数で正確に表せません。0.1f を15回足した値は正確に 1.5f でしょうか?
// [JA] 浮動小数点数を == や != で比べてループを終わらせないでください。何回繰り返すかは整数で数えられるのではないでしょうか?
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(--grade)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void hg01_cooldown_hidden()
{
    int a = 0, b = 0, c = 0;
    runWithWatchdog("쿨다운 계산 (숨은 케이스)", [&] {
        a = hang1::ticksUntilReady(2.0f, 0.25f);
        b = hang1::ticksUntilReady(1.2f, 0.05f);
        c = hang1::ticksUntilReady(0.8f, 0.1f);
    });
    expect("2.0초 / 0.25초", 8, a);
    expect("1.2초 / 0.05초", 24, b);
    expect("0.8초 / 0.1초", 8, c);
}
#pragma endregion
