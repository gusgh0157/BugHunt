#include "../labs.h"
#include <set>
#include <string>
#include <vector>

// 던전의 잠긴 문을 차례로 연다.
// 맞는 열쇠가 있으면 열쇠로 열고, 없을 때만 자물쇠 따개를 하나 써서 연다.

// [JA] ダンジョンの鍵のかかった扉を順番に開けます。
// [JA] 合う鍵があれば鍵で開け、ないときだけ鍵開けツール(ロックピック)を1つ使って開けます。
namespace flow12
{
struct Thief
{
    std::set<std::string> keys;       // 가진 열쇠 (문 이름)
    int lockpicks = 0;
};

bool hasKey(const Thief& t, const std::string& door)
{
    return t.keys.count(door) > 0;
}

// 따개가 남아 있으면 하나 쓰고 true
bool useLockpick(Thief& t)
{
    if (t.lockpicks == 0)
        return false;
    --t.lockpicks;
    return true;
}

bool openDoor(Thief& t, const std::string& door)
{
    return hasKey(t, door) | useLockpick(t);
}
}

void ft12_lockpicks()
{
    using namespace flow12;

    Thief thief;
    thief.keys = { i18n::L("감옥", "牢屋"), i18n::L("창고", "倉庫"), i18n::L("보물실", "宝物庫") };
    thief.lockpicks = 3;

    const std::vector<std::string> doors = { i18n::L("감옥", "牢屋"), i18n::L("부엌", "台所"), i18n::L("창고", "倉庫"), i18n::L("보물실", "宝物庫"), i18n::L("서재", "書斎") };
    int opened = 0;
    for (const std::string& door : doors)
    {
        const bool ok = openDoor(thief, door);
        std::cout << "  " << door << ": " << (ok ? i18n::L("열림", "開いた") : i18n::L("잠김", "施錠中"))
                  << i18n::L("  (남은 따개 ", "  (残りのロックピック") << thief.lockpicks << ")\n";
        if (ok)
            ++opened;
    }

    expect(i18n::L("연 문 수", "開いた扉の数"), 5, opened);
    expect(i18n::L("남은 자물쇠 따개", "残りのロックピック"), 1, thief.lockpicks);
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(Ctrl+M, Ctrl+O = すべて折りたたむ)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// 따개는 열쇠가 없는 "부엌"과 "서재"에서만 줄어야 합니다.
// useLockpick의 --t.lockpicks 줄에 중단점을 걸고, 멈출 때마다 호출 스택에서 openDoor로 올라가 door를 보세요.

// [JA] ロックピックは鍵のない「台所」と「書斎」でだけ減るはずです。
// [JA] useLockpickの --t.lockpicks の行にブレークポイントを置き、止まるたびに呼び出し履歴でopenDoorに上がってdoorを見てください。
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// 감옥 문에서 hasKey는 true였습니다. 그런데도 useLockpick이 불렸나요?
// "앞이 참이면 뒤는 계산하지 않는다"는 어떤 연산자의 규칙인가요?

// [JA] 牢屋の扉ではhasKeyはtrueでした。それでもuseLockpickは呼ばれましたか?
// [JA] 「左が真なら右は評価しない」というのは、どの演算子の規則でしょうか?
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// |는 bool끼리도 양쪽을 모두 계산하는 비트 OR입니다. 단락 평가는 ||에만 있습니다.
// 랩 27(업적 해금)은 단락 평가 때문에 필요한 호출이 빠진 경우였고, 이번에는 그 반대입니다.

// [JA] | はbool同士でも両辺を必ず評価するビットORです。短絡評価は || にしかありません。
// [JA] ラボ27(実績解除)は短絡評価のせいで必要な呼び出しが抜けた例で、今回はその逆です。
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(grade.bat)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void ft12_lockpicks_hidden()
{
    using namespace flow12;

    Thief t;
    t.keys = { i18n::L("북문", "北門"), i18n::L("남문", "南門") };
    t.lockpicks = 1;
    expect(i18n::L("북문(열쇠) 열림", "北門(鍵)開く"), true, openDoor(t, i18n::L("북문", "北門")));
    expect(i18n::L("북문 뒤 따개", "北門の後のロックピック"), 1, t.lockpicks);
    expect(i18n::L("남문(열쇠) 열림", "南門(鍵)開く"), true, openDoor(t, i18n::L("남문", "南門")));
    expect(i18n::L("남문 뒤 따개", "南門の後のロックピック"), 1, t.lockpicks);
    expect(i18n::L("동문(따개) 열림", "東門(ロックピック)開く"), true, openDoor(t, i18n::L("동문", "東門")));
    expect(i18n::L("동문 뒤 따개", "東門の後のロックピック"), 0, t.lockpicks);
    expect(i18n::L("서문(없음) 잠김", "西門(なし)開かない"), false, openDoor(t, i18n::L("서문", "西門")));
    expect(i18n::L("북문 다시(열쇠) 열림", "北門ふたたび(鍵)開く"), true, openDoor(t, i18n::L("북문", "北門")));
}
#pragma endregion
