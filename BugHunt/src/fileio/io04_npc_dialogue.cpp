#include "../labs.h"
#include "io_kit.h"
#include <string>
#include <vector>

// NPC 대사 파일: 첫 줄은 대사 수, 그다음 줄부터 대사가 한 줄에 하나씩 있다.
// 대사에는 띄어쓰기가 있으므로 한 줄을 통째로 읽는다.

// [JA] NPCのセリフ ファイル: 1行目はセリフの数で、次の行から1行に1つずつセリフがあります。
// [JA] セリフには空白が含まれるので、1行をまるごと読み込みます。
namespace io4
{
namespace fs = std::filesystem;

std::vector<std::string> loadDialogue(const fs::path& file)
{
    std::vector<std::string> lines;
    std::ifstream in(file);
    int count = 0;
    in >> count;
    for (int i = 0; i < count; ++i)
    {
        std::string line;
        std::getline(in, line);
        lines.push_back(line);
    }
    return lines;
}
}

void io04_npc_dialogue()
{
    using namespace io4;

    iokit::LabDir lab("io04_npc_dialogue");
    const fs::path file = lab / "innkeeper.txt";
    iokit::writeBytes(file, i18n::L(
        "3\n"
        "어서 오세요, 모험가님.\n"
        "북쪽 숲에 늑대가 많아요.\n"
        "조심해서 다녀오세요!\n",
        "3\n"
        "いらっしゃいませ、冒険者さま。\n"
        "北の森にはオオカミが多いですよ。\n"
        "気をつけていってらっしゃい!\n"));

    const std::vector<std::string> lines = loadDialogue(file);
    for (const std::string& line : lines)
        std::cout << i18n::L("  여관 주인: \"", "  宿屋の主人: \"") << line << "\"\n";

    expect(i18n::L("첫 대사", "最初のセリフ"), std::string(i18n::L("어서 오세요, 모험가님.", "いらっしゃいませ、冒険者さま。")), lines.at(0));
    expect(i18n::L("마지막 대사", "最後のセリフ"), std::string(i18n::L("조심해서 다녀오세요!", "気をつけていってらっしゃい!")), lines.at(2));
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(Ctrl+M, Ctrl+O = すべて折りたたむ)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// lines.push_back 줄에 중단점을 걸고, 첫 바퀴에서 line을 보세요. line.size()는 얼마인가요?

// [JA] lines.push_back の行にブレークポイントを置き、1周目で line を見てください。line.size() はいくつですか?
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// in >> count 는 "3"까지만 읽고 멈춥니다. 바로 뒤의 줄바꿈 한 글자는 스트림에 남습니다.
// getline은 줄바꿈을 만날 때까지 읽습니다. 처음 만난 글자가 줄바꿈이면 무엇을 돌려주나요?

// [JA] in >> count は「3」までしか読まずに止まります。直後の改行1文字はストリームに残ります。
// [JA] getline は改行に出会うまで読みます。最初に出会った文字が改行だったら、何を返すでしょうか?
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// >> 다음에 getline을 쓸 때는 숫자 줄의 나머지(줄바꿈까지)를 버리세요:
//     in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');   // <limits>
// in >> std::ws 도 줄바꿈을 건너뛰지만, 다음 줄 맨 앞의 공백(들여쓴 대사)까지 먹어 버립니다.

// [JA] >> の後に getline を使うときは、数字の行の残り(改行まで)を捨ててください:
// [JA]     in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');   // <limits>
// [JA] in >> std::ws も改行を読み飛ばしますが、次の行の先頭の空白(字下げしたセリフ)まで食べてしまいます。
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(grade.bat)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
// [JA] 採点器(grade.bat)が、画面には表示されない別のデータで同じコードをもう一度検査します。
// [JA] 期待値をハードコードして [PASS] にしても、ここで引っかかります。
void io04_npc_dialogue_hidden()
{
    using namespace io4;

    iokit::LabDir lab("io04_npc_dialogue_hidden");
    iokit::writeBytes(lab / "smith.txt", i18n::L(
        "2  \n"                               // 숫자 뒤에 공백이 남아 있다 [JA] 数字の後ろに空白が残っている
        "   (망치를 내려놓으며) 뭘 고쳐 줄까?\n"   // 일부러 들여 쓴 대사 [JA] わざと字下げしたセリフ
        "칼날이 많이 상했군.\n",
        "2  \n"
        "   (ハンマーを置きながら) 何を直してほしい?\n"
        "刃がだいぶ傷んでいるな。\n"));
    const std::vector<std::string> lines = loadDialogue(lab / "smith.txt");
    expect(i18n::L("숨은 대사 수", "隠しセリフの数"), static_cast<size_t>(2), lines.size());
    if (lines.size() == 2)
    {
        expect(i18n::L("들여 쓴 첫 대사", "字下げした最初のセリフ"), std::string(i18n::L("   (망치를 내려놓으며) 뭘 고쳐 줄까?", "   (ハンマーを置きながら) 何を直してほしい?")), lines[0]);
        expect(i18n::L("둘째 대사", "2番目のセリフ"), std::string(i18n::L("칼날이 많이 상했군.", "刃がだいぶ傷んでいるな。")), lines[1]);
    }
}
#pragma endregion
