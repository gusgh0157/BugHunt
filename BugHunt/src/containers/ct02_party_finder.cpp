#include "../labs.h"
#include <iterator>
#include <string>
#include <unordered_map>

// 파티 찾기 게시판. 등록한 플레이어를 이름으로 빨리 찾으려고 unordered_map에 둔다.
// 화면에는 게시판을 처음부터 순서대로 보여 주고, 플레이어는 목록에서 "몇 번째 줄"을 골라 초대한다.

// [JA] パーティー募集掲示板です。登録したプレイヤーを名前ですばやく探せるように unordered_map に入れておきます。
// [JA] 画面には掲示板を先頭から順番に表示し、プレイヤーは一覧から「何行目」かを選んで招待します。
namespace ct2
{
struct Applicant
{
    std::string role;
    int level;
};

using Board = std::unordered_map<std::string, Applicant>;

class PartyFinder
{
public:
    void registerPlayer(const std::string& name, const std::string& role, int level) { board_[name] = { role, level }; }

    // 화면에서 row번째 줄을 고른다. (초대 확인 창이 뜨는 동안 기억해 둔다)
    void select(size_t row) { selectedRow_ = row; }

    const std::string& selectedName() const { return std::next(board_.begin(), selectedRow_)->first; }

    // 확인 창에서 "초대"를 누르면 고른 사람을 데려온다.
    std::string confirmInvite()
    {
        auto it = std::next(board_.begin(), selectedRow_);
        std::string name = it->first;
        board_.erase(it);
        return name;
    }

    Board& board() { return board_; }

private:
    Board board_;
    size_t selectedRow_ = 0;
};

void registerWave(PartyFinder& finder, const char* prefix, int count)
{
    for (int i = 0; i < count; ++i)
        finder.registerPlayer(prefix + std::to_string(i), "DPS", 20 + i % 30);
}
}

void ct02_party_finder()
{
    ct2::PartyFinder finder;
    ct2::registerWave(finder, "Rookie", 6);
    finder.registerPlayer("Mira", "Healer", 42);

    // 게시판에서 Mira가 있는 줄을 찾아 고른다.
    size_t row = 0;
    for (const auto& [name, applicant] : finder.board())
    {
        if (name == "Mira")
            break;
        ++row;
    }
    finder.select(row);
    std::cout << "  " << row << i18n::L("번째 줄 선택: ", "行目を選択: ") << finder.selectedName() << '\n';

    // 확인 창이 떠 있는 동안 저녁 시간이라 플레이어가 대거 등록했다.
    ct2::registerWave(finder, "Evening", 60);

    const std::string invited = finder.confirmInvite();
    std::cout << i18n::L("  초대한 플레이어: ", "  招待したプレイヤー: ") << invited << '\n';
    expect(i18n::L("초대한 플레이어", "招待したプレイヤー"), std::string("Mira"), invited);
    expect(i18n::L("Mira가 게시판에서 빠졌나?", "Mira は掲示板から外れたか?"), static_cast<size_t>(0), finder.board().count("Mira"));
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (편집기의 접기 기능으로 모두 접어 두세요)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(エディターの折りたたみ機能ですべて折りたたんでおいてください)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// registerWave(finder, "Evening", 60) 줄 앞뒤에서 조사식에 finder.selectedName()과 finder.board().bucket_count()를 넣어 비교해 보세요.

// [JA] registerWave(finder, "Evening", 60) の行の前後で、ウォッチに finder.selectedName() と finder.board().bucket_count() を入れて比べてみてください。
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// unordered_map은 원소가 많아지면 버킷 수를 늘리고(rehash) 원소를 새 버킷에 다시 나눠 담습니다.
// 그때 "처음부터 순서대로 돌았을 때 몇 번째인가"는 보장될까요? 표준은 rehash가 이터레이터를 무효화한다고 하지만,
// 원소에 대한 참조와 포인터는 그대로 유효하다고 보장합니다.

// [JA] unordered_map は要素が増えるとバケット数を増やし(rehash)、要素を新しいバケットに分け直して入れます。
// [JA] そのとき「先頭から順番にたどったときに何番目か」は保証されるでしょうか? 標準は rehash がイテレーターを無効化するとしていますが、
// [JA] 要素への参照とポインターはそのまま有効だと保証しています。
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// 선택을 "줄 번호"가 아니라, rehash가 일어나도 변하지 않는 것으로 기억해 보세요.

// [JA] 選択を「行番号」ではなく、rehash が起きても変わらないもので覚えておくようにしてみてください。
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(--grade)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void ct02_party_finder_hidden()
{
    ct2::PartyFinder finder;
    ct2::registerWave(finder, "Tank", 3);
    finder.registerPlayer("Oren", "Tank", 55);
    ct2::registerWave(finder, "Mage", 4);

    size_t row = 0;
    for (const auto& entry : finder.board())
    {
        if (entry.first == "Oren")
            break;
        ++row;
    }
    finder.select(row);
    ct2::registerWave(finder, "Night", 120);

    expect(i18n::L("rehash 뒤 선택된 이름", "rehash 後に選ばれた名前"), std::string("Oren"), finder.selectedName());
    expect(i18n::L("초대한 플레이어", "招待したプレイヤー"), std::string("Oren"), finder.confirmInvite());
    expect(i18n::L("남은 인원", "残りの人数"), static_cast<size_t>(127), finder.board().size());
}
#pragma endregion
