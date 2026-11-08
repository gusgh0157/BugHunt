#include "../labs.h"
#include <set>
#include <string>
#include <vector>

// 시즌 랭킹. 점수 순으로 정렬된 set에 플레이어 포인터를 넣어 두고, 항상 정렬된 상태로 보여 준다.
// 경기가 끝나면 점수를 올리고, 시즌에서 탈퇴하면 set에서 뺀다.

// [JA] シーズンランキングです。スコア順に並ぶ set にプレイヤーのポインターを入れておき、常に並べ替えた状態で表示します。
// [JA] 試合が終わるとスコアを上げ、シーズンから脱退すると set から取り除きます。
namespace ct7
{
struct Player
{
    std::string name;
    int score;
};

struct ByScoreDesc
{
    bool operator()(const Player* a, const Player* b) const
    {
        if (a->score != b->score)
            return a->score > b->score;
        return a->name < b->name;
    }
};

class SeasonRanking
{
public:
    void join(Player* p) { ranked_.insert(p); }

    void recordMatch(Player* p, int points) { p->score += points; }

    bool leave(Player* p) { return ranked_.erase(p) == 1; }

    const Player* top() const { return ranked_.empty() ? nullptr : *ranked_.begin(); }

    size_t size() const { return ranked_.size(); }

private:
    std::set<Player*, ByScoreDesc> ranked_;
};
}

void ct07_season_ranking()
{
    using namespace ct7;

    Player kael{ "Kael", 1200 }, mira{ "Mira", 1500 }, doran{ "Doran", 900 }, selene{ "Selene", 1350 };
    SeasonRanking season;
    for (Player* p : { &kael, &mira, &doran, &selene })
        season.join(p);

    season.recordMatch(&doran, 800);    // 도란이 대회에서 우승했다 → 1700
    season.recordMatch(&mira, -50);

    std::cout << i18n::L("  1위: ", "  1位: ") << season.top()->name << " " << season.top()->score << '\n';
    expect("1위", std::string("Doran"), season.top()->name);

    const bool left = season.leave(&selene);
    expect("Selene 탈퇴 처리", true, left);
    expect("남은 인원", static_cast<size_t>(3), season.size());
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (편집기의 접기 기능으로 모두 접어 두세요)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(エディターの折りたたみ機能ですべて折りたたんでおいてください)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// season.top() 줄에 중단점을 걸고 조사식에 season.ranked_를 넣어 펼쳐 보세요. set 안의 순서는 지금 점수 순서와 맞나요?

// [JA] season.top() の行にブレークポイントを置き、ウォッチに season.ranked_ を入れて展開してみてください。set の中の順番は、今のスコアの順番と合っていますか?
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// set은 원소를 "넣을 때" 비교해서 자리를 정하고, 그 뒤로는 다시 비교하지 않습니다.
// recordMatch는 set 안에 들어 있는 원소의 정렬 기준(score)을 바꿉니다. 그러면 트리는 어떻게 될까요?
// leave(&selene)에 들어가(step into) erase가 트리를 따라 내려가는 길을 상상해 보세요.

// [JA] set は要素を「入れるとき」に比較して位置を決め、そのあとは二度と比較しません。
// [JA] recordMatch は set の中に入っている要素の並べ替えの基準(score)を変えます。すると、木はどうなるでしょうか?
// [JA] leave(&selene) にステップイン(step into)し、erase が木をたどって下りていく道筋を想像してみてください。
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// set의 키는 const입니다. 포인터를 키로 쓰면 그 const가 가리키는 대상까지는 지켜 주지 않습니다.
// 정렬 기준을 바꿔야 한다면, set에서 꺼낸 뒤 바꾸고 다시 넣으세요.

// [JA] set のキーは const です。ポインターをキーにすると、その const は指している対象までは守ってくれません。
// [JA] 並べ替えの基準を変える必要があるなら、set から取り出してから変更し、もう一度入れてください。
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(--grade)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void ct07_season_ranking_hidden()
{
    using namespace ct7;

    std::vector<Player> players;
    for (int i = 0; i < 12; ++i)
        players.push_back({ "P" + std::to_string(i), 1000 + i * 10 });
    SeasonRanking season;
    for (Player& p : players)
        season.join(&p);

    season.recordMatch(&players[0], 500);    // 1000 → 1500 (1위)
    season.recordMatch(&players[11], -300);  // 1110 → 810 (꼴찌)
    expect("1위", std::string("P0"), season.top()->name);

    int removed = 0;
    for (int i = 1; i < 12; i += 2)
        removed += season.leave(&players[i]) ? 1 : 0;
    expect("탈퇴 성공 수", 6, removed);
    expect("남은 인원", static_cast<size_t>(6), season.size());
}
#pragma endregion
