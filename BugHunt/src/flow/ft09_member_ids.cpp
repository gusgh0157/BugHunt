#include "../labs.h"
#include <set>
#include <string>
#include <vector>

// 세이브에서 길드를 불러온 뒤 새 회원 세 명을 받는다.
// 회원 번호는 길드 안에서 겹치면 안 된다. lastId는 "마지막으로 발급한 번호"다.

// [JA] セーブからギルドを読み込んだあと、新メンバーを3人受け入れます。
// [JA] メンバー番号はギルド内で重複してはいけません。lastIdは「最後に発行した番号」です。
namespace flow9
{
struct Member
{
    int id = 0;
    std::string name;
};

struct Guild
{
    int lastId = 0;                 // 마지막으로 발급한 회원 번호
    std::vector<Member> members;
};

// 새 회원 번호를 발급한다. 발급한 번호는 lastId에 남는다.
int issueId(Guild& guild)
{
    return guild.lastId++;
}

void join(Guild& guild, const std::string& name)
{
    guild.members.push_back({ issueId(guild), name });
}

bool allIdsUnique(const Guild& guild)
{
    std::set<int> seen;
    for (const Member& m : guild.members)
    {
        if (!seen.insert(m.id).second)
            return false;
    }
    return true;
}
}

void ft09_member_ids()
{
    using namespace flow9;

    // 세이브에서 불러온 길드: 지금까지 1007번까지 발급했다.
    Guild guild;
    guild.lastId = 1007;
    guild.members = { { 1001, "아린" }, { 1004, "보람" }, { 1007, "도윤" } };

    join(guild, "새봄");
    join(guild, "가온");
    join(guild, "하람");

    for (const Member& m : guild.members)
        std::cout << "  " << m.id << "  " << m.name << '\n';

    expect("회원 번호가 모두 다른가?", true, allIdsUnique(guild));
    expect("새봄의 번호", 1008, guild.members[3].id);
    expect("마지막으로 발급한 번호", 1010, guild.lastId);
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(Ctrl+M, Ctrl+O = すべて折りたたむ)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// issueId 안에 중단점을 걸고, 첫 번째 join에서 멈추면 guild.lastId를 조사식에 넣으세요.
// 한 줄씩 실행(프로시저 단위)으로 return을 넘긴 뒤, 함수가 돌려준 값과 lastId를 나란히 보세요.
// (호출한 쪽으로 나오면 자동/지역 창에 "issueId 반환값"이 잠깐 보이는 디버거도 있습니다.)

// [JA] issueIdの中にブレークポイントを置き、最初のjoinで止まったらguild.lastIdをウォッチに追加してください。
// [JA] ステップオーバーでreturnを越えたあと、関数が返した値とlastIdを並べて見てください。
// [JA] (呼び出し元に戻ると、自動変数/ローカル ウィンドウに「issueIdの戻り値」が一瞬表示されるデバッガーもあります。)
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// x++ 식의 값은 "올리기 전" 값이고, ++x 식의 값은 "올린 뒤" 값입니다.
// lastId가 "마지막으로 발급한 번호"라면, 새 번호는 올리기 전 값과 올린 뒤 값 중 어느 쪽이어야 하나요?

// [JA] x++ という式の値は「増やす前」の値で、++x の値は「増やした後」の値です。
// [JA] lastIdが「最後に発行した番号」なら、新しい番号は増やす前と後のどちらの値であるべきでしょうか?
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// 반환값을 +1 해서 맞추면 lastId와 실제로 준 번호가 어긋나지 않는지도 확인하세요.
// 숨은 테스트는 다른 시작 번호와 다른 인원으로 "준 번호 == lastId"를 검사합니다.

// [JA] 戻り値に+1して合わせる場合は、lastIdと実際に渡した番号がずれていないかも確認してください。
// [JA] 隠しテストは別の開始番号と人数で「渡した番号 == lastId」を検査します。
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(grade.bat)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void ft09_member_ids_hidden()
{
    using namespace flow9;

    Guild guild;
    guild.lastId = 52;
    guild.members = { { 52, "창립자" } };
    for (const char* name : { "가", "나", "다", "라" })
    {
        join(guild, name);
        expect("발급한 번호 == lastId", guild.lastId, guild.members.back().id);
    }
    expect("숨은 길드: 번호가 모두 다른가?", true, allIdsUnique(guild));
    expect("숨은 길드: 첫 신입 번호", 53, guild.members[1].id);

    Guild fresh;                      // 아무도 없는 새 길드: 첫 번호는 1
    join(fresh, "첫 회원");
    expect("새 길드: 첫 번호", 1, fresh.members[0].id);
}
#pragma endregion
