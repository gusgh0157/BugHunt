#include "../labs.h"
#include <memory>
#include <string>
#include <vector>

// 길드와 길드원. 길드는 길드원 목록을, 길드원은 자기 길드를 안다.
// 스마트 포인터를 쓰니 메모리 관리는 자동... 일까?

// [JA] ギルドとギルドメンバーです。ギルドはメンバーの一覧を、メンバーは自分のギルドを知っています。
// [JA] スマートポインターを使っているので、メモリ管理は自動... でしょうか?
namespace ptr6
{
int g_aliveGuilds = 0;
int g_aliveMembers = 0;

struct Guild;

struct Member
{
    explicit Member(const std::string& n) : name(n) { ++g_aliveMembers; }
    ~Member() { --g_aliveMembers; }

    std::string name;
    std::shared_ptr<Guild> guild;
};

struct Guild
{
    explicit Guild(const std::string& n) : name(n) { ++g_aliveGuilds; }
    ~Guild() { --g_aliveGuilds; }

    std::string name;
    std::vector<std::shared_ptr<Member>> members;
};

void join(const std::shared_ptr<Guild>& guild, const std::shared_ptr<Member>& member)
{
    guild->members.push_back(member);
    member->guild = guild;
}

// 길드원이 속한 길드 이름 (길드가 없으면 "(없음)")
std::string guildNameOf(const Member& member)
{
    return member.guild ? member.guild->name : i18n::L("(없음)", "(なし)");
}

// 길드를 하나 만들어 길드원을 모으고, 공지를 보낸 뒤 서버에서 내린다(= 모든 참조가 사라진다).
void runGuildSession(const std::string& guildName, const std::vector<std::string>& memberNames)
{
    auto guild = std::make_shared<Guild>(guildName);
    for (const std::string& name : memberNames)
        join(guild, std::make_shared<Member>(name));

    for (const auto& m : guild->members)
        std::cout << "  [" << guildNameOf(*m) << "] " << m->name << i18n::L("님, 길드전이 곧 시작됩니다\n", "さん、まもなくギルド戦が始まります\n");
}
}

void pt06_guild()
{
    ptr6::g_aliveGuilds = 0;
    ptr6::g_aliveMembers = 0;

    ptr6::runGuildSession("Northwind Rangers", { "Kael", "Mira", "Doran" });

    expect("세션이 끝난 뒤 메모리에 남은 길드", 0, ptr6::g_aliveGuilds);
    expect("세션이 끝난 뒤 메모리에 남은 길드원", 0, ptr6::g_aliveMembers);
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (편집기의 접기 기능으로 모두 접어 두세요)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(エディターの折りたたみ機能ですべて折りたたんでおいてください)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// runGuildSession의 마지막 } 에 중단점을 걸고 지역 변수에서 guild를 펼쳐 보세요.
// shared_ptr 옆에 [strong ref=?] 가 보입니다. 이 함수가 끝나면 guild 지역 변수 하나가 사라집니다. 그러면 0이 되나요?

// [JA] runGuildSession の最後の } にブレークポイントを置き、ローカル変数で guild を展開してみてください。
// [JA] shared_ptr の横に [strong ref=?] が見えます。この関数が終わると、ローカル変数 guild が一つ消えます。それで0になりますか?
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// guild->members[0]도 펼쳐서 그 길드원의 guild 멤버를 보세요. 그것도 같은 길드를 붙잡고 있습니다.
// 길드는 길드원을, 길드원은 길드를 붙잡고 있다면 누가 먼저 놓아 줄까요?
// ~Guild()와 ~Member()에 중단점을 걸어 두면 한 번이라도 걸리는지 확인할 수 있습니다.

// [JA] guild->members[0] も展開して、そのメンバーの guild メンバーを見てください。それも同じギルドをつかんでいます。
// [JA] ギルドはメンバーを、メンバーはギルドをつかんでいるとしたら、どちらが先に手を放すでしょうか?
// [JA] ~Guild() と ~Member() にブレークポイントを置いておけば、一度でも止まるかどうか確認できます。
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// "소유하지 않고 바라보기만 하는" 스마트 포인터가 있습니다. 둘 중 누가 누구를 소유해야 자연스러운가요?
// 바꾼 뒤에도 guildNameOf가 길드 이름을 제대로 보여 줘야 합니다.

// [JA] 「所有せずに見ているだけの」スマートポインターがあります。二つのうち、どちらがどちらを所有するのが自然でしょうか?
// [JA] 変更したあとも、guildNameOf がギルド名を正しく表示しなければなりません。
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(--grade)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void ptr6_hidden_check()
{
    using namespace ptr6;
    auto guild = std::make_shared<Guild>("Deepforge Clan");
    auto lead = std::make_shared<Member>("Bram");
    join(guild, lead);
    join(guild, std::make_shared<Member>("Ivo"));
    expect("길드원이 보는 길드 이름", std::string("Deepforge Clan"), guildNameOf(*lead));

    lead.reset();
    guild.reset();
}

void pt06_guild_hidden()
{
    ptr6::g_aliveGuilds = 0;
    ptr6::g_aliveMembers = 0;

    ptr6_hidden_check();
    ptr6::runGuildSession("Order of the Ember", { "Selene", "Nell", "Pia", "Oren", "Rin" });

    expect("남은 길드", 0, ptr6::g_aliveGuilds);
    expect("남은 길드원", 0, ptr6::g_aliveMembers);
}
#pragma endregion
