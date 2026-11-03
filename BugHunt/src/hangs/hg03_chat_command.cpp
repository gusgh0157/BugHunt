#include "../labs.h"
#include <string>
#include <vector>

// 채팅 명령어. "/party invite Kael" 같은 입력을 공백으로 잘라 단어 목록으로 만든다.
// 공백이 여러 개 연달아 있어도 빈 단어는 만들지 않는다.

// [JA] チャットコマンドです。"/party invite Kael" のような入力を空白で区切って単語のリストにします。
// [JA] 空白がいくつも続いていても、空の単語は作りません。
namespace hang3
{
std::vector<std::string> splitCommand(const std::string& line)
{
    std::vector<std::string> words;
    std::string current;

    size_t i = 0;
    while (i < line.size())
    {
        const char c = line[i];
        if (c == ' ')
        {
            if (!current.empty())
            {
                words.push_back(current);
                current.clear();
            }
            continue;
        }
        current += c;
        ++i;
    }
    if (!current.empty())
        words.push_back(current);
    return words;
}
}

void hg03_chat_command()
{
    std::vector<std::string> words;

    runWithWatchdog("채팅 명령어 분석", [&] {
        words = hang3::splitCommand("/party invite Kael");
    });

    expect("단어 수", 3, static_cast<int>(words.size()));
    expect("초대할 이름", std::string("Kael"), words.size() == 3 ? words[2] : std::string());
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (편집기의 접기 기능으로 모두 접어 두세요)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(エディターの折りたたみ機能ですべて折りたたんでおいてください)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// 멈추면 모두 중단(break all) → 스레드 목록에서 lab-worker(랩 작업 스레드). 지역 변수에서 i, c, current, words를 보세요.
// 한 줄씩 여러 번 진행해도 i가 바뀌나요?

// [JA] 止まったらすべて中断(break all) → スレッド ウィンドウで lab-worker(ラボの作業スレッド)へ。ローカル変数で i, c, current, words を見てください。
// [JA] 1行ずつ何回か進めても i は変わりますか?
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// 반복이 한 바퀴 돌 때 i가 커지는 줄은 어디인가요? 공백을 만났을 때 현재 줄이 그 줄을 지나가나요?
// 실행 위치를 ++i; 줄로 옮긴 뒤(VS: 다음 문 설정, gdb: jump) 계속 실행해 보세요. 무엇이 달라지나요?

// [JA] ループが1周するとき、i が増える行はどこですか? 空白に出会ったとき、現在の行はその行を通りますか?
// [JA] 実行位置を ++i; の行に移してから(VS: 次のステートメントの設定、gdb: jump)実行を続けてみてください。何が変わりますか?
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(--grade)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void hg03_chat_command_hidden()
{
    std::vector<std::string> a, b;
    runWithWatchdog("채팅 명령어 분석 (숨은 케이스)", [&] {
        a = hang3::splitCommand("  /w   Mira  see you there ");
        b = hang3::splitCommand("/logout");
    });
    expect("단어 수 (공백 여러 개)", 5, static_cast<int>(a.size()));
    expect("두 번째 단어", std::string("Mira"), a.size() > 1 ? a[1] : std::string());
    expect("마지막 단어", std::string("there"), a.empty() ? std::string() : a.back());
    expect("한 단어 명령", 1, static_cast<int>(b.size()));
}
#pragma endregion
