#include "../labs.h"
#include "io_kit.h"
#include <string>
#include <vector>

// 퀘스트를 끝낼 때마다 모험 일지 파일에 한 줄씩 적는다.
// 일지 창은 파일을 다시 읽어서 몇 줄인지 보여 준다.
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
    journal.record("늑대 다섯 마리 처치");
    journal.record("잃어버린 반지 찾기");
    journal.record("대장장이에게 광석 배달");

    const std::vector<std::string> shown = readJournal(file);
    std::cout << "  일지 창에 보이는 줄 " << shown.size() << "개\n";
    for (const std::string& line : shown)
        std::cout << "    - " << line << '\n';

    expect("일지 창의 줄 수", static_cast<size_t>(3), shown.size());
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// ============================================================================

#pragma region 힌트 1 (먼저 직접 추적해 보고 펼치세요)
// readJournal 호출 줄에 중단점을 걸고 멈추면, 탐색기나 다른 편집기로 journal.txt를 열어 보세요.
// (경로는 조사식에 file을 넣으면 보입니다.) 파일 크기는 몇 바이트인가요?
#pragma endregion

#pragma region 힌트 2
// out_ << ... 는 바로 디스크에 쓰지 않고 스트림 버퍼에 모아 둡니다.
// 버퍼는 언제 파일로 내려가나요? 버퍼가 가득 찰 때, flush할 때, 스트림을 닫을 때.
#pragma endregion

#pragma region 힌트 3
// 같은 프로그램 안에서 "쓰고 나서 다시 읽는" 곳이 있으면, 읽기 전에 쓴 내용이 파일에 있도록 보장해야 합니다.
// 기록할 때마다 flush하거나, 읽기 전에 flush/close하세요. std::endl은 줄마다 flush합니다(7부 랩 51의 반대 상황).
#pragma endregion

#pragma region 채점용 숨은 테스트 (풀이에는 필요 없습니다. 고치지 마세요)
// 채점기(grade.bat)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void io02_quest_journal_hidden()
{
    using namespace io2;

    iokit::LabDir lab("io02_quest_journal_hidden");
    const fs::path file = lab / "journal.txt";
    iokit::writeBytes(file, "지난 세션의 기록\n");   // 이전 실행이 남긴 줄

    Journal journal(file);
    journal.record("동굴 입구 발견");
    expect("한 줄 기록 뒤", static_cast<size_t>(2), readJournal(file).size());
    for (const char* entry : { "박쥐 소탕", "광부 구출", "보스 처치", "귀환" })
        journal.record(entry);
    const std::vector<std::string> lines = readJournal(file);
    expect("다섯 줄 기록 뒤", static_cast<size_t>(6), lines.size());
    expect("마지막 줄", std::string("귀환"), lines.empty() ? std::string() : lines.back());
}
#pragma endregion
