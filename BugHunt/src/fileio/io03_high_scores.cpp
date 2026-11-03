#include "../labs.h"
#include "io_kit.h"
#include <string>
#include <vector>

// 아케이드 점수판 파일(한 줄에 "이름 점수")을 읽어 순위표와 총점을 보여 준다.

// [JA] アーケードのスコアボード ファイル(1行に「名前 スコア」)を読み込み、ランキングと合計点を表示します。
namespace io3
{
namespace fs = std::filesystem;

struct Score
{
    std::string name;
    int points = 0;
};

std::vector<Score> loadScores(const fs::path& file)
{
    std::vector<Score> scores;
    std::ifstream in(file);
    std::string name;
    int points = 0;
    while (!in.eof())
    {
        in >> name >> points;
        scores.push_back({ name, points });
    }
    return scores;
}

int total(const std::vector<Score>& scores)
{
    int sum = 0;
    for (const Score& s : scores)
        sum += s.points;
    return sum;
}
}

void io03_high_scores()
{
    using namespace io3;

    iokit::LabDir lab("io03_high_scores");
    const fs::path file = lab / "scores.txt";
    iokit::writeBytes(file, "아린 9200\n보람 8700\n도윤 7100\n");

    const std::vector<Score> scores = loadScores(file);
    for (size_t i = 0; i < scores.size(); ++i)
        std::cout << "  " << (i + 1) << "위 " << scores[i].name << " " << scores[i].points << '\n';

    expect("순위표 줄 수", static_cast<size_t>(3), scores.size());
    expect("총점", 25000, total(scores));
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(Ctrl+M, Ctrl+O = すべて折りたたむ)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// scores.push_back 줄에 조건부 중단점을 거세요. 조건: scores.size() == 3
// 네 번째로 넣으려는 순간 멈춥니다. 그때 in.eof(), in.fail(), name, points를 조사식에 넣어 보세요.

// [JA] scores.push_back の行に条件付きブレークポイントを設定してください。条件: scores.size() == 3
// [JA] 4件目を入れようとした瞬間に止まります。そのとき in.eof()、in.fail()、name、points をウォッチに入れてみてください。
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// 세 번째 줄을 읽은 직후, 파일에는 아직 줄바꿈 한 글자가 남아 있습니다. 그때 eof()는 true인가요?
// eof()는 "끝에 닿았다"가 아니라 "끝을 넘어 읽으려다 실패했다"일 때 켜집니다.

// [JA] 3行目を読んだ直後、ファイルにはまだ改行が1文字残っています。そのとき eof() は true でしょうか?
// [JA] eof() は「末尾に着いた」ではなく、「末尾を越えて読もうとして失敗した」ときに立ちます。
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// 읽기가 성공했는지는 읽은 "뒤에" 확인해야 합니다. while (in >> name >> points) 처럼 읽기 자체를 조건으로 쓰세요.
// 마지막 줄을 pop_back으로 지우는 땜질은 줄바꿈 없이 끝나는 파일에서 진짜 기록을 지웁니다.

// [JA] 読み込みが成功したかどうかは、読んだ「後で」確認しなければなりません。while (in >> name >> points) のように読み込み自体を条件にしてください。
// [JA] 最後の行を pop_back で消すその場しのぎは、改行なしで終わるファイルで本物の記録を消してしまいます。
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(grade.bat)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void io03_high_scores_hidden()
{
    using namespace io3;

    iokit::LabDir lab("io03_high_scores_hidden");

    iokit::writeBytes(lab / "no_newline.txt", "가 500\n나 400");          // 마지막 줄바꿈 없음
    const std::vector<Score> a = loadScores(lab / "no_newline.txt");
    expect("줄바꿈 없이 끝남: 줄 수", static_cast<size_t>(2), a.size());
    expect("줄바꿈 없이 끝남: 총점", 900, total(a));

    iokit::writeBytes(lab / "blank_tail.txt", "다 30\n라 20\n마 10\n\n\n");   // 빈 줄이 여러 개
    const std::vector<Score> b = loadScores(lab / "blank_tail.txt");
    expect("빈 줄로 끝남: 줄 수", static_cast<size_t>(3), b.size());
    expect("빈 줄로 끝남: 총점", 60, total(b));

    iokit::writeBytes(lab / "empty.txt", "");
    expect("빈 파일: 줄 수", static_cast<size_t>(0), loadScores(lab / "empty.txt").size());
}
#pragma endregion
