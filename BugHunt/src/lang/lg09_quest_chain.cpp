#include "../labs.h"
#include <cstddef>
#include <set>
#include <string>
#include <vector>

// 차례가 끝날 때 퀘스트 일지를 훑어, 조건을 채운 퀘스트를 완료한다. 완료하면 이어지는 퀘스트가 일지 끝에 붙는다.
// 이어진 퀘스트도 조건을 이미 채웠다면 같은 차례에 바로 완료되어야 한다(연계 퀘스트).
// 일지(QuestLog)는 퀘스트 목록을 직접 내주지 않고 entries()로 목록을 돌려준다.

// [JA] ターンの終わりにクエスト日誌を見渡し、条件を満たしたクエストを完了します。完了すると、続きのクエストが日誌の末尾に追加されます。
// [JA] 続きのクエストもすでに条件を満たしているなら、同じターンですぐに完了しなければなりません(連続クエスト)。
// [JA] 日誌(QuestLog)はクエストのリストを直接渡さず、entries() でリストを返します。
namespace lang9
{
struct Quest
{
    std::string name;
    std::string goal;   // 완료 조건 (플레이어가 이미 이룬 일 목록에 있으면 완료)
    std::string next;   // 이어지는 퀘스트 이름 (없으면 빈 문자열)
    bool done = false;
};

struct QuestBook
{
    std::vector<Quest> all;

    Quest find(const std::string& name) const
    {
        for (const Quest& q : all)
        {
            if (q.name == name)
                return q;
        }
        return {};
    }
};

class QuestLog
{
public:
    void add(const Quest& q) { entries_.push_back(q); }

    // 일지의 퀘스트 목록
    std::vector<Quest> entries() const { return entries_; }
    std::size_t size() const { return entries_.size(); }
    const Quest& at(std::size_t i) const { return entries_.at(i); }

    // 완료 표시를 하고, 이어지는 퀘스트를 일지 끝에 붙인다
    void complete(const std::string& name, const QuestBook& book)
    {
        for (std::size_t i = 0; i < entries_.size(); ++i)
        {
            if (entries_[i].name == name && !entries_[i].done)
            {
                entries_[i].done = true;
                const std::string next = entries_[i].next;
                if (!next.empty())
                    entries_.push_back(book.find(next));
                return;
            }
        }
    }

    int countDone() const
    {
        int n = 0;
        for (const Quest& q : entries_)
            n += q.done ? 1 : 0;
        return n;
    }

private:
    std::vector<Quest> entries_;
};

// 차례 끝 처리. 이번 차례에 완료한 퀘스트 수를 돌려준다.
int endTurn(QuestLog& log, const QuestBook& book, const std::set<std::string>& achieved)
{
    int completed = 0;
    for (const Quest& q : log.entries())
    {
        if (q.done || achieved.count(q.goal) == 0)
            continue;
        log.complete(q.name, book);   // 이어지는 퀘스트가 일지에 붙고, 이번 반복에서 이어서 확인된다
        ++completed;
    }
    return completed;
}

void print(const QuestLog& log)
{
    for (const Quest& q : log.entries())
        std::cout << "  " << (q.done ? i18n::L("[완료] ", "[完了] ") : i18n::L("[진행] ", "[進行中] ")) << q.name << '\n';
}

// 퀘스트 이름이 고른 언어를 따르도록 처음 쓸 때 만든다. (전역 변수로 두면 언어를 고르기 전에 만들어진다.)
// [JA] クエスト名が選んだ言語に従うように、最初に使うときに作ります。(グローバル変数にすると言語を選ぶ前に作られてしまいます。)
const QuestBook& questBook()
{
    static const QuestBook book = { {
        { i18n::L("마을의 부탁", "村の頼みごと"), i18n::L("촌장과 대화", "村長と話す"), i18n::L("잃어버린 반지", "なくした指輪"), false },
        { i18n::L("잃어버린 반지", "なくした指輪"), i18n::L("반지 획득", "指輪を入手"), i18n::L("반지의 주인", "指輪の持ち主"), false },
        { i18n::L("반지의 주인", "指輪の持ち主"), i18n::L("촌장과 대화", "村長と話す"), i18n::L("숲의 소문", "森のうわさ"), false },
        { i18n::L("숲의 소문", "森のうわさ"), i18n::L("엘프와 대화", "エルフと話す"), "", false },
        { i18n::L("약초 채집", "薬草採集"), i18n::L("약초 10개", "薬草10個"), "", false },
    } };
    return book;
}
}

void lg09_quest_chain()
{
    using namespace lang9;

    const QuestBook& kBook = questBook();
    QuestLog log;
    log.add(kBook.find(i18n::L("약초 채집", "薬草採集")));
    log.add(kBook.find(i18n::L("마을의 부탁", "村の頼みごと")));
    const std::set<std::string> achieved = { i18n::L("촌장과 대화", "村長と話す"), i18n::L("반지 획득", "指輪を入手") };

    const int completed = endTurn(log, kBook, achieved);
    print(log);

    // 마을의 부탁 → 잃어버린 반지 → 반지의 주인까지 조건을 이미 채웠다. 숲의 소문은 아직.
    expect(i18n::L("이번 차례에 완료한 퀘스트 수", "今回のターンに完了したクエスト数"), 3, completed);
    expect(i18n::L("일지의 퀘스트 수", "日誌のクエスト数"), static_cast<size_t>(5), log.size());
    expect(i18n::L("일지에서 완료된 퀘스트 수", "日誌で完了したクエスト数"), 3, log.countDone());
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(Ctrl+M, Ctrl+O = すべて折りたたむ)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// endTurn의 for 줄에 중단점을 걸고 한 줄씩 실행하며 q.name을 보세요. 반복은 몇 번 도나요? 끝났을 때 log.size()는 몇인가요?
// complete에 들어가 push_back이 일어난 직후, 조사식에 log.size()와 &q를 넣고 &log.at(0)과 비교해 보세요. q는 일지 안의 퀘스트를 가리키고 있나요?

// [JA] endTurn の for の行にブレークポイントを置き、1行ずつ実行しながら q.name を見てください。ループは何回回りますか? 終わったとき log.size() はいくつですか?
// [JA] complete に入って push_back が起きた直後、ウォッチに log.size() と &q を入れ、&log.at(0) と比べてみてください。q は日誌の中のクエストを指していますか?
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// 범위 for는 시작할 때 범위 식(log.entries())을 한 번만 평가하고, 그 결과의 begin()과 end()를 한 번만 잡아 둡니다:
//   auto&& __range = log.entries(); auto __begin = __range.begin(); auto __end = __range.end(); ...
// entries()는 목록의 사본을 돌려주므로, 반복은 처음 사본의 원소 두 개만 돕니다. 반복 중에 일지에 붙은 퀘스트는 사본에도, 잡아 둔 끝 안에도 없습니다.
// entries()가 참조를 돌려주게 바꿔도 답이 아닙니다. 그러면 반복 중인 vector에 push_back하게 되어, 끝 이터레이터가 무효가 되고(정의되지 않은 동작)
// 재할당이 일어나면 q까지 해제된 메모리를 가리킵니다(랩 10). 표준 라이브러리의 디버그 검사가 이를 잡는지는 컴파일러마다 다릅니다.

// [JA] 範囲 for は開始時に範囲式(log.entries())を一度だけ評価し、その結果の begin() と end() を一度だけ取っておきます:
// [JA]   auto&& __range = log.entries(); auto __begin = __range.begin(); auto __end = __range.end(); ...
// [JA] entries() はリストのコピーを返すので、ループは最初のコピーの要素二つだけを回ります。ループ中に日誌に追加されたクエストは、コピーにも、取っておいた終端の内側にもありません。
// [JA] entries() が参照を返すように変えても答えではありません。そうするとループ中の vector に push_back することになり、終端イテレーターが無効になり(未定義動作)、
// [JA] 再割り当てが起これば q まで解放済みのメモリを指します(ラボ10)。標準ライブラリのデバッグチェックがこれを捕まえるかどうかはコンパイラーによって異なります。
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// 반복 중에 커지는 목록은 인덱스로 돌고, 조건에서 매번 size()를 다시 읽으세요:
//   for (std::size_t i = 0; i < log.size(); ++i) { const Quest q = log.at(i); ... log.complete(q.name, book); }
// q는 참조가 아니라 복사로 받아야 complete 안의 push_back(재할당) 뒤에도 안전합니다.

// [JA] ループ中に大きくなるリストはインデックスで回し、条件で毎回 size() を読み直してください:
// [JA]   for (std::size_t i = 0; i < log.size(); ++i) { const Quest q = log.at(i); ... log.complete(q.name, book); }
// [JA] q は参照ではなくコピーで受け取ってこそ、complete の中の push_back(再割り当て)の後でも安全です。
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(grade.bat)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void lg09_quest_chain_hidden()
{
    using namespace lang9;
    QuestBook book;
    QuestLog log;
    for (int i = 0; i < 6; ++i)
    {
        const std::string n = std::to_string(i);
        book.all.push_back({ i18n::L("현상금 ", "賞金首 ") + n, i18n::L("토벌", "討伐"), i18n::L("보고 ", "報告 ") + n, false });
        book.all.push_back({ i18n::L("보고 ", "報告 ") + n, i18n::L("토벌", "討伐"), i18n::L("보상 ", "報酬 ") + n, false });
        book.all.push_back({ i18n::L("보상 ", "報酬 ") + n, i18n::L("길드 방문", "ギルド訪問"), "", false });
        log.add(book.find(i18n::L("현상금 ", "賞金首 ") + n));
    }
    expect(i18n::L("숨은 일지: 완료 수", "隠し日誌: 完了数"), 12, endTurn(log, book, { i18n::L("토벌", "討伐") }));
    expect(i18n::L("숨은 일지: 일지 크기", "隠し日誌: 日誌の大きさ"), static_cast<size_t>(18), log.size());
    expect(i18n::L("숨은 일지: 남은 보상 퀘스트", "隠し日誌: 残った報酬クエスト"), 6, static_cast<int>(log.size()) - log.countDone());
    expect(i18n::L("숨은 일지: 다음 차례 (길드 방문)", "隠し日誌: 次のターン(ギルド訪問)"), 6, endTurn(log, book, { i18n::L("토벌", "討伐"), i18n::L("길드 방문", "ギルド訪問") }));
    expect(i18n::L("숨은 일지: 할 일 없는 차례", "隠し日誌: やることのないターン"), 0, endTurn(log, book, { i18n::L("토벌", "討伐"), i18n::L("길드 방문", "ギルド訪問") }));
}
#pragma endregion
