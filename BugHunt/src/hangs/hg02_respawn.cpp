#include "../labs.h"

// 부활 대기 화면. 남은 시간을 1초씩 세면서 5초 단위와 마지막 3초(3, 2, 1, 0)에 알림을 띄운다.
// 알림 하나마다 효과음을 미리 불러와야 해서, 알림이 몇 번 뜰지 먼저 센다.

// [JA] 復活待ち画面です。残り時間を1秒ずつ数えながら、5秒ごとと最後の3秒(3, 2, 1, 0)に通知を出します。
// [JA] 通知1つごとに効果音を前もって読み込む必要があるので、通知が何回出るかを先に数えます。
namespace hang2
{
int countAnnouncements(unsigned int waitSeconds)
{
    int count = 0;
    for (unsigned int s = waitSeconds; s >= 0; --s)
    {
        if (s % 5 == 0 || s <= 3)
            ++count;
    }
    return count;
}
}

void hg02_respawn()
{
    int announcements = 0;

    runWithWatchdog(i18n::L("부활 알림 횟수 계산", "復活通知の回数計算"), [&] {
        announcements = hang2::countAnnouncements(12);
    });

    // 12초: 10, 5, 3, 2, 1, 0 → 6번
    expect(i18n::L("12초 대기 알림 횟수", "12秒待機の通知回数"), 6, announcements);
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (편집기의 접기 기능으로 모두 접어 두세요)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(エディターの折りたたみ機能ですべて折りたたんでおいてください)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// 멈추면 모두 중단(break all). 스레드 목록에서 lab-worker(랩 작업 스레드)로 바꾸고 지역 변수에서 s를 보세요.
// 12보다 작아야 할 s가 어떤 값인가요? 16진수로 보면(디버거의 16진수 표시, gdb: p/x)?

// [JA] 止まったらすべて中断(break all)してください。スレッド ウィンドウで lab-worker(ラボの作業スレッド)に切り替え、ローカル変数で s を見てください。
// [JA] 12より小さいはずの s はどんな値ですか? 16進数で見ると(デバッガーの16進数表示、gdb: p/x)?
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// for 줄에 조건부 중단점 s == 0 을 걸고 다시 실행하세요. 거기서 한 줄씩 몇 번 진행하면 s는 어떻게 되나요?
// 반복이 끝나려면 s >= 0 이 거짓이 되어야 합니다. unsigned int에서 그럴 수 있나요?

// [JA] for の行に条件付きブレークポイント s == 0 を置いて再実行してください。そこから1行ずつ何回か進めると、s はどうなりますか?
// [JA] ループが終わるには s >= 0 が偽にならなければなりません。unsigned int でそうなることはありえますか?
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// 이 실수를 알려 주는 경고는 컴파일러마다 이름이 다르고 기본으로 꺼져 있기도 합니다.
// GCC: -Wtype-limits(-Wextra에 포함), Clang: -Wtautological-unsigned-zero-compare, MSVC: /w14296. 켜고 다시 빌드해 보세요.
// 0까지 포함해서 거꾸로 세는 반복을 unsigned로 쓰는 안전한 방법은 무엇일까요?

// [JA] このミスを知らせてくれる警告は、コンパイラーごとに名前が違い、既定でオフになっていることもあります。
// [JA] GCC: -Wtype-limits(-Wextra に含まれます)、Clang: -Wtautological-unsigned-zero-compare、MSVC: /w14296。オンにして再ビルドしてみてください。
// [JA] 0まで含めて逆に数えるループを unsigned で書く安全な方法は何でしょうか?
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(--grade)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void hg02_respawn_hidden()
{
    int a = 0, b = 0, c = 0;
    runWithWatchdog(i18n::L("부활 알림 횟수 계산 (숨은 케이스)", "復活通知の回数計算(隠しケース)"), [&] {
        a = hang2::countAnnouncements(7);
        b = hang2::countAnnouncements(20);
        c = hang2::countAnnouncements(0);
    });
    expect(i18n::L("7초 대기 (5, 3, 2, 1, 0)", "7秒待機(5, 3, 2, 1, 0)"), 5, a);
    expect(i18n::L("20초 대기 (20, 15, 10, 5, 3, 2, 1, 0)", "20秒待機(20, 15, 10, 5, 3, 2, 1, 0)"), 8, b);
    expect(i18n::L("0초 대기 (0)", "0秒待機(0)"), 1, c);
}
#pragma endregion
