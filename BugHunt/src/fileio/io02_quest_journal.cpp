#include "../labs.h"
#include "io_kit.h"
#include <string>
#include <vector>

// 퀘스트를 끝낼 때마다 모험 일지 파일에 한 줄씩 적는다.
// 일지 창은 파일을 다시 읽어서 몇 줄인지 보여 준다.

// [JA] クエストを終えるたびに、冒険日誌ファイルに1行ずつ書き込みます。
// [JA] 日誌ウィンドウはファイルを読み直して、何行あるかを表示します。
namespace io2
{
namespace fs = std::filesystem;

class Journal
{
public:
    explicit Journal(const fs::path& file)
        : out_(file, std::ios::app)
    {
    }

    void record(const std::string& entry)
    {
        out_ << entry << '\n';
    }

private:
    std::ofstream out_;
};

// 일지 창: 파일에 적힌 줄을 모두 읽는다
std::vector<std::string> readJournal(const fs::path& file)
{
    std::vector<std::string> lines;
    std::ifstream in(file);
    std::string line;
    while (std::getline(in, line))
        lines.push_back(line);
    return lines;
}
}

void io02_quest_journal()
{
    using namespace io2;

    iokit::LabDir lab("io02_quest_journal");
    const fs::path file = lab / "journal.txt";

    Journal journal(file);
    journal.record(i18n::L("늑대 다섯 마리 처치", "オオカミ5匹を討伐"));
    journal.record(i18n::L("잃어버린 반지 찾기", "なくした指輪を探す"));
    journal.record(i18n::L("대장장이에게 광석 배달", "鍛冶屋に鉱石を届ける"));

    const std::vector<std::string> shown = readJournal(file);
    std::cout << i18n::L("  일지 창에 보이는 줄 ", "  日誌ウィンドウ") << shown.size() << i18n::L("개\n", "行\n");
    for (const std::string& line : shown)
        std::cout << "    - " << line << '\n';

    expect(i18n::L("일지 창의 줄 수", "日誌ウィンドウの行数"), static_cast<size_t>(3), shown.size());
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(Ctrl+M, Ctrl+O = すべて折りたたむ)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// readJournal 호출 줄에 중단점을 걸고 멈추면, 탐색기나 다른 편집기로 journal.txt를 열어 보세요.
// (경로는 조사식에 file을 넣으면 보입니다.) 파일 크기는 몇 바이트인가요?

// [JA] readJournalを呼ぶ行にブレークポイントを置き、止まったらエクスプローラーや別のエディターで journal.txt を開いてみてください。
// [JA] (パスはウォッチに file を入れると見えます。) ファイルサイズは何バイトですか?
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// out_ << ... 는 바로 디스크에 쓰지 않고 스트림 버퍼에 모아 둡니다.
// 버퍼는 언제 파일로 내려가나요? 버퍼가 가득 찰 때, flush할 때, 스트림을 닫을 때.

// [JA] out_ << ... はすぐにはディスクに書かず、ストリームのバッファーにためておきます。
// [JA] バッファーはいつファイルに書き出されるでしょうか? バッファーがいっぱいのとき、flushしたとき、ストリームを閉じたときです。
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// 같은 프로그램 안에서 "쓰고 나서 다시 읽는" 곳이 있으면, 읽기 전에 쓴 내용이 파일에 있도록 보장해야 합니다.
// Journal 안(record)에서 기록할 때마다 flush하세요. 본문에서 읽기 전에만 flush/close하면 일지를 쓰고 바로 읽는 다른 곳에서 또 틀립니다.
// std::endl은 줄마다 flush합니다(12부 랩 96의 반대 상황).

// [JA] 同じプログラムの中で「書いてから読み直す」箇所があるなら、読む前に書いた内容がファイルにあることを保証しなければなりません。
// [JA] Journal の中(record)で記録するたびにflushしてください。本文で読む前にだけflush/closeすると、日誌を書いてすぐ読む別の場所でまた間違えます。
// [JA] std::endl は行ごとにflushします(第12部ラボ96とは逆の状況です)。
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(grade.bat)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void io02_quest_journal_hidden()
{
    using namespace io2;

    iokit::LabDir lab("io02_quest_journal_hidden");
    const fs::path file = lab / "journal.txt";
    iokit::writeBytes(file, i18n::L("지난 세션의 기록\n", "前回のセッションの記録\n"));   // 이전 실행이 남긴 줄

    Journal journal(file);
    journal.record(i18n::L("동굴 입구 발견", "洞窟の入口を発見"));
    expect(i18n::L("한 줄 기록 뒤", "1行記録した後"), static_cast<size_t>(2), readJournal(file).size());
    for (const char* entry : { i18n::L("박쥐 소탕", "コウモリ退治"), i18n::L("광부 구출", "鉱夫の救出"), i18n::L("보스 처치", "ボス討伐"), i18n::L("귀환", "帰還") })
        journal.record(entry);
    const std::vector<std::string> lines = readJournal(file);
    expect(i18n::L("다섯 줄 기록 뒤", "5行記録した後"), static_cast<size_t>(6), lines.size());
    expect(i18n::L("마지막 줄", "最後の行"), std::string(i18n::L("귀환", "帰還")), lines.empty() ? std::string() : lines.back());
}
#pragma endregion
